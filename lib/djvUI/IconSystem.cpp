//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUI/IconSystem.h>

#include <djvAV/IO.h>
#include <djvAV/Image.h>

#include <djvCore/Cache.h>
#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const size_t imageCacheMax = 1000;

            struct ImageRequest
            {
                ImageRequest()
                {}

                ImageRequest(ImageRequest&& other) :
                    path(other.path),
                    queue(std::move(other.queue)),
                    read(std::move(other.read)),
                    promise(std::move(other.promise))
                {}

                ImageRequest& operator = (ImageRequest&& other)
                {
                    if (this != &other)
                    {
                        path = other.path;
                        queue = std::move(other.queue);
                        read = std::move(other.read);
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                FileSystem::Path path;
                std::shared_ptr<AV::IO::Queue> queue;
                std::shared_ptr<AV::IO::IRead> read;
                std::promise<std::shared_ptr<AV::Image::Image> > promise;
            };

            size_t getImageCacheKey(const FileSystem::Path & path)
            {
                size_t out = 0;
                Memory::hashCombine(out, path.get());
                return out;
            }

        } // namespace

        struct IconSystem::Private
        {
            std::vector<int> dpiList;
            std::list<ImageRequest> imageQueue;
            std::condition_variable requestCV;
            std::mutex requestMutex;
            std::list<ImageRequest> newImageRequests;
            std::list<ImageRequest> pendingImageRequests;

            Memory::Cache<size_t, std::shared_ptr<AV::Image::Image> > imageCache;
            std::mutex cacheMutex;

            std::shared_ptr<Time::Timer> statsTimer;
            std::thread thread;
            std::atomic<bool> running;

            FileSystem::Path getPath(const std::string & name, int dpi, Context *) const;
            int findClosestDPI(int) const;
        };

        void IconSystem::_init(Context * context)
        {
            ISystem::_init("djv::AV::Image::IconSystem", context);

            DJV_PRIVATE_PTR();
            p.imageCache.setMax(imageCacheMax);



            p.statsTimer = Time::Timer::create(context);
            p.statsTimer->setRepeating(true);
            p.statsTimer->start(
                Time::Timer::getMilliseconds(Time::Timer::Value::VerySlow),
                [this](float)
            {
                DJV_PRIVATE_PTR();
                std::stringstream s;
                {
                    std::lock_guard<std::mutex> lock(p.cacheMutex);
                    s << "Image cache: " << p.imageCache.getPercentageUsed() << '%';
                }
                _log(s.str());
            });

            p.running = true;
            p.thread = std::thread(
                [this, context]
            {
                DJV_PRIVATE_PTR();
                try
                {
                    // Find the DPI values.
                    for (const auto& i : FileSystem::FileInfo::directoryList(context->getPath(FileSystem::ResourcePath::IconsDirectory)))
                    {
                        const std::string fileName = i.getFileName(Frame::Invalid, false);
                        const size_t size = fileName.size();
                        if (size > 3 &&
                            fileName[size - 3] == 'D' &&
                            fileName[size - 2] == 'P' &&
                            fileName[size - 1] == 'I')
                        {
                            p.dpiList.push_back(std::stoi(fileName.substr(0, size - 3)));
                        }
                    }
                    std::sort(p.dpiList.begin(), p.dpiList.end());
                    for (const auto & i : p.dpiList)
                    {
                        std::stringstream ss;
                        ss << "Found DPI: " << i;
                        _log(ss.str());
                    }

                    const auto timeout = Time::Timer::getValue(Time::Timer::Value::Medium);
                    while (p.running)
                    {
                        {
                            std::unique_lock<std::mutex> lock(p.requestMutex);
                            if (p.requestCV.wait_for(
                                lock,
                                std::chrono::milliseconds(timeout),
                                [this]
                            {
                                DJV_PRIVATE_PTR();
                                return p.imageQueue.size();
                            }))
                            {
                                p.newImageRequests = std::move(p.imageQueue);
                            }
                        }
                        if (p.newImageRequests.size() || p.pendingImageRequests.size())
                        {
                            _handleImageRequests();
                        }
                    }
                }
                catch (const std::exception & e)
                {
                    context->log("djv::AV::ThumbnailSystem", e.what(), LogLevel::Error);
                }
            });
        }

        IconSystem::IconSystem() :
            _p(new Private)
        {}

        IconSystem::~IconSystem()
        {
            DJV_PRIVATE_PTR();
            p.running = false;
            if (p.thread.joinable())
            {
                p.thread.join();
            }
        }

        std::shared_ptr<IconSystem> IconSystem::create(Context * context)
        {
            auto out = std::shared_ptr<IconSystem>(new IconSystem);
            out->_init(context);
            return out;
        }

        std::future<std::shared_ptr<AV::Image::Image> > IconSystem::getIcon(const std::string & name, int size)
        {
            DJV_PRIVATE_PTR();
            ImageRequest request;
            request.path = _p->getPath(name, _p->findClosestDPI(size), getContext());
            auto future = request.promise.get_future();
            {
                std::unique_lock<std::mutex> lock(p.requestMutex);
                p.imageQueue.push_back(std::move(request));
            }
            p.requestCV.notify_one();
            return future;
        }

        void IconSystem::_handleImageRequests()
        {
            DJV_PRIVATE_PTR();

            // Process new requests.
            for (auto & i : p.newImageRequests)
            {
                std::shared_ptr<AV::Image::Image> image;
                {
                    std::lock_guard<std::mutex> lock(p.cacheMutex);
                    const auto key = getImageCacheKey(i.path);
                    if (p.imageCache.contains(key))
                    {
                        image = p.imageCache.get(key);
                    }
                }
                if (!image)
                {
                    if (auto io = getContext()->getSystemT<AV::IO::System>().lock())
                    {
                        try
                        {
                            i.queue = AV::IO::Queue::create(1, 0);
                            i.read = io->read(i.path, i.queue);
                            p.pendingImageRequests.push_back(std::move(i));
                        }
                        catch (const std::exception& e)
                        {
                            try
                            {
                                i.promise.set_exception(std::current_exception());
                            }
                            catch (const std::exception & e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            _log(e.what(), LogLevel::Error);
                        }
                    }
                }
                if (image)
                {
                    i.promise.set_value(image);
                }
            }
            p.newImageRequests.clear();

            // Process pending requests.
            auto i = p.pendingImageRequests.begin();
            while (i != p.pendingImageRequests.end())
            {
                std::shared_ptr<AV::Image::Image> image;
                {
                    std::lock_guard<std::mutex> lock(i->queue->getMutex());
                    if (i->queue->hasVideo())
                    {
                        image = i->queue->getVideo().second;
                    }
                }
                if (image)
                {
                    p.imageCache.add(getImageCacheKey(i->path), image);
                    i->promise.set_value(image);
                    i = p.pendingImageRequests.erase(i);
                }
                else
                {
                    ++i;
                }
            }
        }

        FileSystem::Path IconSystem::Private::getPath(const std::string & name, int dpi, Context * context) const
        {
            auto out = context->getPath(FileSystem::ResourcePath::IconsDirectory);
            {
                std::stringstream ss;
                ss << dpi << "DPI";
                out = FileSystem::Path(out, ss.str());
            }
            {
                std::stringstream ss;
                ss << name << ".png";
                out = FileSystem::Path(out, ss.str());
            }
            return out;
        }

        int IconSystem::Private::findClosestDPI(int value) const
        {
            const int dpi = static_cast<int>(value / 24.f * 96.f);
            std::map<int, int> differenceToDPI;
            for (auto i : dpiList)
            {
                differenceToDPI[Math::abs(dpi - i)] = i;
            }
            return differenceToDPI.size() ? differenceToDPI.begin()->second : dpi;
        }

    } // namespace UI
} // namespace djv