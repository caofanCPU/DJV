//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvUIComponents/FileBrowserItemView.h>

#include <djvUI/IconSystem.h>

#include <djvAV/IO.h>
#include <djvAV/Render2D.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            namespace
            {
                //! \todo [1.0 S] Should this be configurable?
                const float thumbnailFadeTime = .2f;

                const size_t invalid = static_cast<size_t>(-1);

            } // namespace

            struct ItemView::Private
            {
                ViewType viewType = ViewType::First;
                std::vector<FileSystem::FileInfo> items;
                AV::Font::Metrics nameFontMetrics;
                std::future<AV::Font::Metrics> nameFontMetricsFuture;
                std::map<size_t, BBox2f> itemGeometry;
                std::map<size_t, std::vector<AV::Font::TextLine> > nameLines;
                std::map<size_t, std::future<std::vector<AV::Font::TextLine> > > nameLinesFutures;
                std::map<size_t, AV::IO::Info> ioInfo;
                std::map<size_t, AV::ThumbnailSystem::InfoFuture> ioInfoFutures;
                glm::ivec2 thumbnailSize = glm::ivec2(100, 50);
                std::map<size_t, std::shared_ptr<AV::Image::Image> > thumbnails;
                std::map<size_t, AV::ThumbnailSystem::ImageFuture> thumbnailFutures;
                std::map<size_t, float> thumbnailTimers;
                std::map<FileSystem::FileType, std::shared_ptr<AV::Image::Image> > icons;
                std::map<FileSystem::FileType, std::future<std::shared_ptr<AV::Image::Image> > > iconsFutures;
                std::map<size_t, std::string> sizeText;
                std::map<size_t, std::string> timeText;
                std::vector<float> split = { .7f, .8f, 1.f };

                size_t hover = invalid;
                size_t grab = invalid;
                Event::PointerID pressedId = Event::InvalidID;
                glm::vec2 pressedPos = glm::vec2(0.f, 0.f);
                std::function<void(const FileSystem::FileInfo &)> callback;

                std::string getTooltip(const FileSystem::FileInfo &, Context * context) const;
                std::string getTooltip(const FileSystem::FileInfo &, const AV::IO::Info &, Context *) const;
            };

            void ItemView::_init(Context * context)
            {
                UI::Widget::_init(context);
                DJV_PRIVATE_PTR();
                setClassName("djv::UI::FileBrowser::ItemView");
            }

            ItemView::ItemView() :
                _p(new Private)
            {}

            ItemView::~ItemView()
            {}

            std::shared_ptr<ItemView> ItemView::create(Context * context)
            {
                auto out = std::shared_ptr<ItemView>(new ItemView);
                out->_init(context);
                return out;
            }

            void ItemView::setViewType(ViewType value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.viewType)
                    return;
                p.viewType = value;
                _iconsUpdate();
                _itemsUpdate();
            }

            void ItemView::setThumbnailSize(const glm::ivec2 & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.thumbnailSize)
                    return;
                p.thumbnailSize = value;
                _iconsUpdate();
                _thumbnailsSizeUpdate();
                _resize();
            }

            void ItemView::setSplit(const std::vector<float> & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.split)
                    return;
                p.split = value;
                _redraw();
            }

            void ItemView::setItems(const std::vector<FileSystem::FileInfo> & value)
            {
                _p->items = value;
                _itemsUpdate();
            }

            void ItemView::setCallback(const std::function<void(const FileSystem::FileInfo &)> & value)
            {
                _p->callback = value;
            }

            float ItemView::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.f;
                if (const size_t itemCount = p.items.size())
                {
                    auto style = _getStyle();
                    const float m = style->getMetric(MetricsRole::MarginSmall);
                    const float s = style->getMetric(MetricsRole::SpacingSmall);
                    switch (p.viewType)
                    {
                    case ViewType::Tiles:
                    {
                        size_t columns = 1;
                        float x = p.thumbnailSize.x + m * 2.f;
                        while (x < value - (p.thumbnailSize.x + m * 2.f))
                        {
                            ++columns;
                            x += p.thumbnailSize.x + m * 2.f;
                        }
                        const size_t rows = itemCount / columns + (itemCount % columns ? 1 : 0);
                        out = (p.thumbnailSize.y + s + p.nameFontMetrics.lineHeight * 2.f + m * 2.f) * rows;
                        break;
                    }
                    case ViewType::List:
                        out = (std::max(static_cast<float>(p.thumbnailSize.y), p.nameFontMetrics.lineHeight) + m * 2.f) * itemCount;
                        break;
                    default: break;
                    }
                }
                return out;
            }

            void ItemView::_styleEvent(Event::Style &)
            {
                _iconsUpdate();
                _itemsUpdate();
            }

            void ItemView::_preLayoutEvent(Event::PreLayout & event)
            {
                DJV_PRIVATE_PTR();
                if (p.nameFontMetricsFuture.valid() &&
                    p.nameFontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        p.nameFontMetrics = p.nameFontMetricsFuture.get();
                    }
                    catch (const std::exception & e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }
            }

            void ItemView::_layoutEvent(Event::Layout & event)
            {
                DJV_PRIVATE_PTR();
                const BBox2f & g = getGeometry();
                auto style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float s = style->getMetric(MetricsRole::SpacingSmall);
                p.itemGeometry.clear();
                glm::vec2 pos = g.min;
                auto item = p.items.begin();
                size_t i = 0;
                for (; item != p.items.end(); ++item, ++i)
                {
                    switch (p.viewType)
                    {
                    case ViewType::Tiles:
                    {
                        const float itemHeight = p.thumbnailSize.y + s + p.nameFontMetrics.lineHeight * 2.f + m * 2.f;
                        p.itemGeometry[i] = BBox2f(pos.x, pos.y, p.thumbnailSize.x + m * 2.f, itemHeight);
                        pos.x += p.thumbnailSize.x + m * 2.f;
                        if (pos.x > g.max.x - (p.thumbnailSize.x + m * 2.f))
                        {
                            pos.x = g.min.x;
                            pos.y += itemHeight;
                        }
                        break;
                    }
                    case ViewType::List:
                    {
                        const float itemHeight = std::max(static_cast<float>(p.thumbnailSize.y), p.nameFontMetrics.lineHeight) + m * 2.f;
                        p.itemGeometry[i] = BBox2f(pos.x, pos.y, g.w(), itemHeight);
                        pos.y += itemHeight;
                        break;
                    }
                    default: break;
                    }
                }
            }

            void ItemView::_clipEvent(Event::Clip & event)
            {
                DJV_PRIVATE_PTR();
                if (isClipped())
                    return;
                auto fontSystem = _getFontSystem();
                auto style = _getStyle();
                auto context = getContext();
                const auto & clipRect = event.getClipRect();
                for (const auto & i : p.itemGeometry)
                {
                    if (i.first < p.items.size() && i.second.intersects(clipRect))
                    {
                        const auto & fileInfo = p.items[i.first];
                        {
                            const auto j = p.nameLines.find(i.first);
                            if (j == p.nameLines.end())
                            {
                                const auto k = p.nameLinesFutures.find(i.first);
                                if (k == p.nameLinesFutures.end())
                                {
                                    const float m = style->getMetric(MetricsRole::MarginSmall);
                                    const auto fontInfo = style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium);
                                    p.nameLinesFutures[i.first] = fontSystem->textLines(
                                        fileInfo.getFileName(Frame::Invalid, false), p.thumbnailSize.x - m * 2.f, fontInfo);
                                }
                            }
                        }
                        if (p.ioInfo.find(i.first) == p.ioInfo.end())
                        {
                            if (p.ioInfoFutures.find(i.first) == p.ioInfoFutures.end())
                            {
                                auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                                auto ioSystem = context->getSystemT<AV::IO::System>();
                                if (thumbnailSystem && ioSystem)
                                {
                                    const auto & path = fileInfo.getPath();
                                    if (ioSystem->canRead(path))
                                    {
                                        p.ioInfoFutures[i.first] = thumbnailSystem->getInfo(path);
                                    }
                                }
                            }
                        }
                        if (p.thumbnails.find(i.first) == p.thumbnails.end())
                        {
                            if (p.thumbnailFutures.find(i.first) == p.thumbnailFutures.end())
                            {
                                const auto & path = fileInfo.getPath();
                                auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                                auto ioSystem = context->getSystemT<AV::IO::System>();
                                if (thumbnailSystem && ioSystem && ioSystem->canRead(path))
                                {
                                    p.thumbnailFutures[i.first] = thumbnailSystem->getImage(
                                        path,
                                        glm::ivec2(static_cast<int>(p.thumbnailSize.x), static_cast<int>(p.thumbnailSize.y)));
                                }
                            }
                        }
                        {
                            const auto j = p.sizeText.find(i.first);
                            if (j == p.sizeText.end())
                            {
                                p.sizeText[i.first] = Memory::getSizeLabel(fileInfo.getSize());
                            }
                        }
                        {
                            const auto j = p.timeText.find(i.first);
                            if (j == p.timeText.end())
                            {
                                p.timeText[i.first] = Time::getLabel(fileInfo.getTime());
                            }
                        }
                    }
                    else if (auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>())
                    {
                        {
                            const auto j = p.ioInfoFutures.find(i.first);
                            if (j != p.ioInfoFutures.end())
                            {
                                thumbnailSystem->cancelInfo(j->second.uid);
                                p.ioInfoFutures.erase(j);
                            }
                        }
                        {
                            const auto j = p.thumbnailFutures.find(i.first);
                            if (j != p.thumbnailFutures.end())
                            {
                                thumbnailSystem->cancelImage(j->second.uid);
                                p.thumbnailFutures.erase(j);
                            }
                        }
                    }
                }
            }

            void ItemView::_paintEvent(Event::Paint & event)
            {
                DJV_PRIVATE_PTR();
                const BBox2f & g = getGeometry();
                auto style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const float s = style->getMetric(MetricsRole::SpacingSmall);
                const float b = style->getMetric(MetricsRole::Border);

                auto render = _getRender();
                {
                    const auto i = p.itemGeometry.find(p.grab);
                    if (i != p.itemGeometry.end())
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Pressed)));
                        render->drawRect(i->second);
                    }
                }

                {
                    const auto i = p.itemGeometry.find(p.hover);
                    const auto j = p.itemGeometry.find(p.grab);
                    if (i != p.itemGeometry.end() && i != j)
                    {
                        render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hovered)));
                        render->drawRect(i->second);
                    }
                }

                const float ut = _getUpdateTime();
                auto item = p.items.begin();
                size_t index = 0;
                for (; item != p.items.end(); ++item, ++index)
                {
                    const auto i = p.itemGeometry.find(index);
                    if (i != p.itemGeometry.end())
                    {
                        const BBox2f & itemGeometry = i->second;
                        if (ViewType::List == p.viewType)
                        {
                            render->pushClipRect(BBox2f(
                                itemGeometry.min.x,
                                itemGeometry.min.y,
                                itemGeometry.w() * p.split[0],
                                itemGeometry.h()));
                        }
                        float opacity = 0.f;
                        {
                            const auto j = p.thumbnails.find(index);
                            if (j != p.thumbnails.end())
                            {
                                if (j->second)
                                {
                                    opacity = 1.f;
                                    const auto k = p.thumbnailTimers.find(index);
                                    if (k != p.thumbnailTimers.end())
                                    {
                                        opacity = std::min((ut - k->second) / thumbnailFadeTime, 1.f);
                                    }
                                    const auto & size = j->second->getSize();
                                    float x = 0.f;
                                    float y = 0.f;
                                    switch (p.viewType)
                                    {
                                    case ViewType::Tiles:
                                        x = floor(i->second.min.x + m + p.thumbnailSize.x / 2.f - size.x / 2.f);
                                        y = floor(i->second.min.y + m + p.thumbnailSize.y - size.y);
                                        break;
                                    case ViewType::List:
                                        x = floor(i->second.min.x + m);
                                        y = floor(i->second.min.y + i->second.h() / 2.f - size.y / 2.f);
                                        break;
                                    default: break;
                                    }
                                    render->setFillColor(_getColorWithOpacity(AV::Image::Color(1.f, 1.f, 1.f, opacity)));
                                    render->drawImage(
                                        j->second,
                                        BBox2f(x, y, static_cast<float>(size.x), static_cast<float>(size.y)));
                                }
                            }
                        }
                        if (opacity < 1.f)
                        {
                            const auto j = p.icons.find(item->getType());
                            if (j != p.icons.end())
                            {
                                const auto & size = j->second->getSize();
                                float x = 0.f;
                                float y = 0.f;
                                switch (p.viewType)
                                {
                                case ViewType::Tiles:
                                    x = floor(i->second.min.x + m + p.thumbnailSize.x / 2.f - size.x / 2.f);
                                    y = floor(i->second.min.y + m + p.thumbnailSize.y - size.y);
                                    break;
                                case ViewType::List:
                                    x = floor(i->second.min.x + m);
                                    y = floor(i->second.min.y + i->second.h() / 2.f - size.y / 2.f);
                                    break;
                                default: break;
                                }
                                auto c = style->getColor(ColorRole::Button).convert(AV::Image::Type::RGBA_F32);
                                c.setF32(1.f - opacity, 3);
                                render->setFillColor(_getColorWithOpacity(c));
                                render->drawFilledImage(
                                    j->second,
                                    BBox2f(x, y, static_cast<float>(size.x), static_cast<float>(size.y)));
                            }
                        }
                        {
                            render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Foreground)));
                            render->setCurrentFont(style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium));
                            switch (p.viewType)
                            {
                            case ViewType::Tiles:
                            {
                                const auto j = p.nameLines.find(index);
                                if (j != p.nameLines.end())
                                {
                                    float x = i->second.min.x + m;
                                    float y = i->second.max.y - p.nameFontMetrics.lineHeight * 2.f - m;
                                    size_t line = 0;
                                    auto k = j->second.begin();
                                    for (; k != j->second.end() && line < 2; ++k, ++line)
                                    {
                                        //! \bug Why the extra subtract by one here?
                                        render->drawText(
                                            k->text,
                                            glm::vec2(
                                                floorf(x + p.thumbnailSize.x / 2.f - k->size.x / 2.f),
                                                floorf(y + p.nameFontMetrics.ascender - 1.f)));
                                        y += p.nameFontMetrics.lineHeight;
                                    }
                                }
                                break;
                            }
                            case ViewType::List:
                            {
                                float x = i->second.min.x + m + p.thumbnailSize.x + s;
                                float y = i->second.min.y + i->second.h() / 2.f - p.nameFontMetrics.lineHeight / 2.f;
                                //! \bug Why the extra subtract by one here?
                                render->drawText(
                                    item->getFileName(Frame::Invalid, false),
                                    glm::vec2(
                                        floorf(x),
                                        floorf(y + p.nameFontMetrics.ascender - 1.f)));

                                render->popClipRect();

                                x = i->second.min.x + i->second.w() * p.split[0] + m;
                                auto j = p.sizeText.find(index);
                                if (j != p.sizeText.end())
                                {
                                    render->pushClipRect(BBox2f(
                                        itemGeometry.min.x + itemGeometry.w() * p.split[0],
                                        itemGeometry.min.y,
                                        itemGeometry.w() * (p.split[1] - p.split[0]),
                                        itemGeometry.h()));

                                    //! \bug Why the extra subtract by one here?
                                    render->drawText(
                                        j->second,
                                        glm::vec2(
                                            floorf(x),
                                            floorf(y + p.nameFontMetrics.ascender - 1.f)));

                                    render->popClipRect();
                                }

                                x = i->second.min.x + i->second.w() * p.split[1] + m;
                                j = p.timeText.find(index);
                                if (j != p.timeText.end())
                                {
                                    render->pushClipRect(BBox2f(
                                        itemGeometry.min.x + itemGeometry.w() * p.split[1],
                                        itemGeometry.min.y,
                                        itemGeometry.w() * (p.split[2] - p.split[1]),
                                        itemGeometry.h()));

                                    //! \bug Why the extra subtract by one here?
                                    render->drawText(
                                        j->second,
                                        glm::vec2(
                                            floorf(x),
                                            floorf(y + p.nameFontMetrics.ascender - 1.f)));

                                    render->popClipRect();
                                }
                                break;
                            }
                            default: break;
                            }
                        }
                    }
                }
            }

            void ItemView::_pointerEnterEvent(Event::PointerEnter & event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto & pointerInfo = event.getPointerInfo();
                for (const auto & i : p.itemGeometry)
                {
                    if (i.second.contains(pointerInfo.pos))
                    {
                        p.hover = i.first;
                        _redraw();
                        break;
                    }
                }
            }

            void ItemView::_pointerLeaveEvent(Event::PointerLeave & event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                if (p.hover != invalid)
                {
                    p.hover = invalid;
                    _redraw();
                }
            }

            void ItemView::_pointerMoveEvent(Event::PointerMove & event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto & pointerInfo = event.getPointerInfo();
                if (p.grab != invalid && pointerInfo.id == p.pressedId)
                {
                    const float distance = glm::length(pointerInfo.projectedPos - p.pressedPos);
                    auto style = _getStyle();
                    const bool accepted = distance < style->getMetric(MetricsRole::Drag);
                    event.setAccepted(accepted);
                    if (!accepted)
                    {
                        p.grab = invalid;
                        p.pressedId = Event::InvalidID;
                        _redraw();
                    }
                }
                else
                {
                    for (const auto & i : p.itemGeometry)
                    {
                        if (i.second.contains(pointerInfo.pos))
                        {
                            p.hover = i.first;
                            _redraw();
                            break;
                        }
                    }
                }
            }

            void ItemView::_buttonPressEvent(Event::ButtonPress & event)
            {
                DJV_PRIVATE_PTR();
                if (p.pressedId)
                    return;
                const auto & pointerInfo = event.getPointerInfo();
                for (const auto & i : p.itemGeometry)
                {
                    if (i.second.contains(pointerInfo.pos))
                    {
                        event.accept();
                        p.grab = i.first;
                        p.pressedId = pointerInfo.id;
                        p.pressedPos = pointerInfo.pos;
                        _redraw();
                        break;
                    }
                }
            }

            void ItemView::_buttonReleaseEvent(Event::ButtonRelease & event)
            {
                DJV_PRIVATE_PTR();
                const auto & pointerInfo = event.getPointerInfo();
                if (p.grab != invalid && pointerInfo.id == p.pressedId)
                {
                    event.accept();
                    p.grab = invalid;
                    p.pressedId = Event::InvalidID;
                    const auto & hover = _getPointerHover();
                    const auto i = hover.find(pointerInfo.id);
                    if (p.callback && i != hover.end())
                    {
                        for (const auto & j : p.itemGeometry)
                        {
                            if (j.second.contains(i->second))
                            {
                                if (j.first < p.items.size())
                                {
                                    p.callback(p.items[j.first]);
                                }
                            }
                        }
                    }
                    _redraw();
                }
            }

            std::shared_ptr<Widget> ItemView::_createTooltip(const glm::vec2 & pos)
            {
                DJV_PRIVATE_PTR();
                std::string text;
                for (const auto & i : p.itemGeometry)
                {
                    if (i.second.contains(pos))
                    {
                        if (i.first < p.items.size())
                        {
                            auto context = getContext();
                            const auto & fileInfo = p.items[i.first];
                            const auto j = p.ioInfo.find(i.first);
                            if (j != p.ioInfo.end())
                            {
                                text = p.getTooltip(fileInfo, j->second, context);
                            }
                            else
                            {
                                text = p.getTooltip(fileInfo, context);
                            }
                        }
                        break;
                    }
                }
                return !text.empty() ? _createTooltipDefault(text) : nullptr;
            }

            void ItemView::_updateEvent(Event::Update & event)
            {
                DJV_PRIVATE_PTR();
                {
                    auto i = p.nameLinesFutures.begin();
                    while (i != p.nameLinesFutures.end())
                    {
                        if (i->second.valid() &&
                            i->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                p.nameLines[i->first] = i->second.get();
                                _redraw();
                            }
                            catch (const std::exception & e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.nameLinesFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                {
                    auto i = p.ioInfoFutures.begin();
                    while (i != p.ioInfoFutures.end())
                    {
                        if (i->second.future.valid() &&
                            i->second.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                p.ioInfo[i->first] = i->second.future.get();
                            }
                            catch (const std::exception & e)
                            {
                                p.ioInfo[i->first] = AV::IO::Info();
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.ioInfoFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                {
                    auto i = p.thumbnailFutures.begin();
                    while (i != p.thumbnailFutures.end())
                    {
                        if (i->second.future.valid() &&
                            i->second.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            p.thumbnails[i->first] = nullptr;
                            try
                            {
                                if (const auto image = i->second.future.get())
                                {
                                    p.thumbnails[i->first] = image;
                                    p.thumbnailTimers[i->first] = _getUpdateTime();
                                    _redraw();
                                }
                            }
                            catch (const std::exception & e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.thumbnailFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                if (p.thumbnailTimers.size())
                {
                    const float ut = _getUpdateTime();
                    auto i = p.thumbnailTimers.begin();
                    while (i != p.thumbnailTimers.end())
                    {
                        if (ut - i->second > thumbnailFadeTime)
                        {
                            i = p.thumbnailTimers.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                    _redraw();
                }
                {
                    auto i = p.iconsFutures.begin();
                    while (i != p.iconsFutures.end())
                    {
                        if (i->second.valid() &&
                            i->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                p.icons[i->first] = i->second.get();
                                _redraw();
                            }
                            catch (const std::exception & e)
                            {
                                _log(e.what(), LogLevel::Error);
                            }
                            i = p.iconsFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
            }

            void ItemView::_localeEvent(Event::Locale &)
            {
                _itemsUpdate();
            }

            void ItemView::_iconsUpdate()
            {
                DJV_PRIVATE_PTR();
                p.icons.clear();
                for (size_t i = 0; i < static_cast<size_t>(FileSystem::FileType::Count); ++i)
                {
                    const auto type = static_cast<FileSystem::FileType>(i);
                    std::string name;
                    switch (type)
                    {
                    case FileSystem::FileType::Directory:
                        name = "djvIconDirectory";
                        break;
                    default:
                        name = "djvIconFile";
                        break;
                    }
                    auto iconSystem = _getIconSystem();
                    _p->iconsFutures[type] = iconSystem->getIcon(name, p.thumbnailSize.y);
                }
            }

            void ItemView::_thumbnailsSizeUpdate()
            {
                DJV_PRIVATE_PTR();
                p.thumbnails.clear();
                auto context = getContext();
                auto thumbnailSystem = context->getSystemT<AV::ThumbnailSystem>();
                auto fontSystem = _getFontSystem();
                auto style = _getStyle();
                p.nameLines.clear();
                p.nameLinesFutures.clear();
                for (const auto & i : p.itemGeometry)
                {
                    const auto j = p.thumbnailFutures.find(i.first);
                    if (j != p.thumbnailFutures.end())
                    {
                        thumbnailSystem->cancelImage(j->second.uid);
                        p.thumbnailFutures.erase(j);
                    }
                }
                p.thumbnailFutures.clear();

                const auto & clipRect = getClipRect();
                for (const auto & i : p.itemGeometry)
                {
                    if (i.first < p.items.size() && i.second.intersects(clipRect))
                    {
                        if (p.thumbnails.find(i.first) == p.thumbnails.end())
                        {
                            const auto & fileInfo = p.items[i.first];
                            {
                                const auto j = p.nameLines.find(i.first);
                                if (j == p.nameLines.end())
                                {
                                    const auto k = p.nameLinesFutures.find(i.first);
                                    if (k == p.nameLinesFutures.end())
                                    {
                                        const float m = style->getMetric(MetricsRole::MarginSmall);
                                        const auto fontInfo = style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium);
                                        p.nameLinesFutures[i.first] = fontSystem->textLines(
                                            fileInfo.getFileName(Frame::Invalid, false), p.thumbnailSize.x - m * 2.f, fontInfo);
                                    }
                                }
                            }
                            if (p.thumbnailFutures.find(i.first) == p.thumbnailFutures.end())
                            {
                                const auto & path = fileInfo.getPath();
                                auto ioSystem = context->getSystemT<AV::IO::System>();
                                if (ioSystem && ioSystem->canRead(path))
                                {
                                    p.thumbnailFutures[i.first] = thumbnailSystem->getImage(
                                        path,
                                        glm::ivec2(static_cast<int>(p.thumbnailSize.x), static_cast<int>(p.thumbnailSize.y)));
                                }
                            }
                        }
                    }
                }
            }

            void ItemView::_itemsUpdate()
            {
                DJV_PRIVATE_PTR();
                auto style = _getStyle();
                auto fontSystem = _getFontSystem();
                auto thumbnailSystem = getContext()->getSystemT<AV::ThumbnailSystem>();
                p.nameFontMetricsFuture = fontSystem->getMetrics(
                    style->getFontInfo(AV::Font::Info::faceDefault, MetricsRole::FontMedium));
                p.nameLines.clear();
                p.nameLinesFutures.clear();
                p.ioInfo.clear();
                p.ioInfoFutures.clear();
                p.thumbnails.clear();
                for (const auto & i : p.itemGeometry)
                {
                    const auto j = p.thumbnailFutures.find(i.first);
                    if (j != p.thumbnailFutures.end())
                    {
                        thumbnailSystem->cancelImage(j->second.uid);
                        p.thumbnailFutures.erase(j);
                    }
                }
                p.thumbnailFutures.clear();
                p.sizeText.clear();
                p.timeText.clear();
                _resize();
            }

            std::string ItemView::Private::getTooltip(const FileSystem::FileInfo & fileInfo, Context * context) const
            {
                std::stringstream ss;
                ss << fileInfo.getFileName(Frame::Invalid, false) << '\n';
                ss << '\n';
                ss << context->getText(DJV_TEXT("Size")) << ": " << Memory::getSizeLabel(fileInfo.getSize()) << '\n';
                ss << context->getText(DJV_TEXT("Last modification time")) << ": " << Time::getLabel(fileInfo.getTime());
                return ss.str();
            }

            std::string ItemView::Private::getTooltip(const FileSystem::FileInfo & fileInfo, const AV::IO::Info & avInfo, Context * context) const
            {
                std::stringstream ss;
                ss << getTooltip(fileInfo, context);
                size_t track = 0;
                for (const auto & videoInfo : avInfo.video)
                {
                    ss << '\n' << '\n';
                    ss << context->getText(DJV_TEXT("Video track")) << " #" << track << '\n';
                    ss << context->getText(DJV_TEXT("Size")) << ": " << videoInfo.info.size << '\n';
                    ss << context->getText(DJV_TEXT("Type")) << ": " << videoInfo.info.type << '\n';
                    ss.precision(2);
                    ss << context->getText(DJV_TEXT("Speed")) << ": " <<
                        Time::Speed::speedToFloat(videoInfo.speed) <<
                        context->getText(DJV_TEXT("FPS")) << '\n';
                    ss << context->getText(DJV_TEXT("Duration")) << ": " <<
                        Time::getLabel(Time::durationToSeconds(videoInfo.duration));
                    ++track;
                }
                track = 0;
                for (const auto & audioInfo : avInfo.audio)
                {
                    ss << '\n' << '\n';
                    ss << context->getText(DJV_TEXT("Audio track")) << " #" << track << '\n';
                    ss << context->getText(DJV_TEXT("Channels")) << ": " << audioInfo.info.channelCount << '\n';
                    ss << context->getText(DJV_TEXT("Type")) << ": " << audioInfo.info.type << '\n';
                    ss << context->getText(DJV_TEXT("Sample rate")) << ": " <<
                        audioInfo.info.sampleRate / 1000.f << DJV_TEXT("kHz") << '\n';
                    ss << context->getText(DJV_TEXT("Duration")) << ": " <<
                        Time::getLabel(Time::durationToSeconds(audioInfo.duration));
                    ++track;
                }
                return ss.str();
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv