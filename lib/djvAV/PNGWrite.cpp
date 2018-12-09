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

#include <djvAV/PNG.h>

#include <djvAV/PixelProcess.h>

#include <djvCore/Context.h>
#include <djvCore/FileIO.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PNG
            {
                struct Write::Private
                {
                    std::shared_ptr<Pixel::Convert> convert;
                };

                Write::Write() :
                    _p(new Private)
                {}

                Write::~Write()
                {}

                std::shared_ptr<Write> Write::create(const std::string & fileName, const Info & info, const std::shared_ptr<Queue> & queue, const std::shared_ptr<Core::Context> & context)
                {
                    auto out = std::shared_ptr<Write>(new Write);
                    out->_init(fileName, info, queue, context);
                    return out;
                }

                void Write::_start()
                {
                    if (auto context = _context.lock())
                    {
                        _p->convert = Pixel::Convert::create(context);
                    }
                }

                namespace
                {
                    struct File
                    {
                        FILE * f = nullptr;
                        png_structp png = nullptr;
                        png_infop pngInfo = nullptr;
                        ErrorStruct pngError;
                    };

                    bool pngOpen(
                        FILE *              f,
                        png_structp         png,
                        png_infop *         pngInfo,
                        const Pixel::Info & info)
                    {
                        if (setjmp(png_jmpbuf(png)))
                        {
                            return false;
                        }
                        *pngInfo = png_create_info_struct(png);
                        if (!*pngInfo)
                        {
                            return false;
                        }
                        png_init_io(png, f);

                        int colorType = 0;
                        switch (info.getGLFormat())
                        {
                        case GL_R: colorType = PNG_COLOR_TYPE_GRAY; break;
                        case GL_RG: colorType = PNG_COLOR_TYPE_GRAY_ALPHA; break;
                        case GL_RGB: colorType = PNG_COLOR_TYPE_RGB; break;
                        case GL_RGBA: colorType = PNG_COLOR_TYPE_RGB_ALPHA; break;
                        default: break;
                        }

                        png_set_IHDR(
                            png,
                            *pngInfo,
                            info.size.x,
                            info.size.y,
                            static_cast<int>(Pixel::getBitDepth(info.type)),
                            colorType,
                            PNG_INTERLACE_NONE,
                            PNG_COMPRESSION_TYPE_DEFAULT,
                            PNG_FILTER_TYPE_DEFAULT);
                        png_write_info(png, *pngInfo);
                        return true;
                    }

                    bool pngScanline(png_structp png, const uint8_t* in)
                    {
                        if (setjmp(png_jmpbuf(png)))
                            return false;
                        png_write_row(png, (png_byte *)in);
                        return true;
                    }

                    bool pngEnd(png_structp png, png_infop pngInfo)
                    {
                        if (setjmp(png_jmpbuf(png)))
                            return false;
                        png_write_end(png, pngInfo);
                        return true;
                    }

                } // namespace

                void Write::_write(const std::string & fileName, const std::shared_ptr<Image> & image)
                {
                    Pixel::Type pixelType = Pixel::Type::None;
                    switch (image->getType())
                    {
                    case Pixel::Type::L_U8:
                    case Pixel::Type::L_U16:
                    case Pixel::Type::LA_U8:
                    case Pixel::Type::LA_U16:
                    case Pixel::Type::RGB_U8:
                    case Pixel::Type::RGB_U16:
                    case Pixel::Type::RGBA_U8:
                    case Pixel::Type::RGBA_U16: pixelType = image->getType(); break;
                    case Pixel::Type::L_U32:
                    case Pixel::Type::L_F16:
                    case Pixel::Type::L_F32:    pixelType = Pixel::Type::L_U16; break;
                    case Pixel::Type::LA_U32:
                    case Pixel::Type::LA_F16:
                    case Pixel::Type::LA_F32:   pixelType = Pixel::Type::LA_U16; break;
                    case Pixel::Type::RGB_U32:
                    case Pixel::Type::RGB_F16:
                    case Pixel::Type::RGB_F32:  pixelType = Pixel::Type::RGB_U16; break;
                    case Pixel::Type::RGBA_U32:
                    case Pixel::Type::RGBA_F16:
                    case Pixel::Type::RGBA_F32: pixelType = Pixel::Type::RGBA_U16; break;
                    default: break;
                    }
                    if (Pixel::Type::None == pixelType)
                    {
                        std::stringstream s;
                        s << pluginName << ": " << DJV_TEXT("cannot write") << ": " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    const auto info = Pixel::Info(image->getSize(), pixelType);

                    std::shared_ptr<Pixel::Data> pixelData = image;
                    if (pixelData->getInfo() != info)
                    {
                        auto tmp = Pixel::Data::create(info);
                        _p->convert->process(*pixelData, info, *tmp);
                        pixelData = tmp;
                    }

                    File f;
                    f.png = png_create_write_struct(
                        PNG_LIBPNG_VER_STRING,
                        &f.pngError,
                        djvPngError,
                        djvPngWarning);
                    if (!f.png)
                    {
                        std::stringstream s;
                        s << pluginName << ": " << DJV_TEXT("cannot open") << ": " << fileName << ": " << f.pngError.msg;
                        throw std::runtime_error(s.str());
                    }
                    f.f = Core::fopen(fileName.c_str(), "wb");
                    if (!f.f)
                    {
                        std::stringstream s;
                        s << pluginName << ": " << DJV_TEXT("cannot open") << ": " << fileName;
                        throw std::runtime_error(s.str());
                    }
                    if (!pngOpen(f.f, f.png, &f.pngInfo, info))
                    {
                        std::stringstream s;
                        s << pluginName << ": " << DJV_TEXT("cannot open") << ": " << fileName << ": " << f.pngError.msg;
                        throw std::runtime_error(s.str());
                    }
                    if (Pixel::getBitDepth(info.type) > 8 && Core::Memory::Endian::LSB == Core::Memory::getEndian())
                    {
                        png_set_swap(f.png);
                    }

                    const auto & size = pixelData->getSize();
                    for (int y = 0; y < size.y; ++y)
                    {
                        if (!pngScanline(f.png, pixelData->getData(y)))
                        {
                            std::stringstream s;
                            s << pluginName << ": " << DJV_TEXT("cannot write") << ": " << fileName << ": " << f.pngError.msg;
                            throw std::runtime_error(s.str());
                        }
                    }
                    if (!pngEnd(f.png, f.pngInfo))
                    {
                        std::stringstream s;
                        s << pluginName << ": " << DJV_TEXT("cannot write") << ": " << fileName << ": " << f.pngError.msg;
                        throw std::runtime_error(s.str());
                    }
                }

                void Write::_exit()
                {
                    _p->convert.reset();
                }

            } // namespace PNG
        } // namespace IO
    } // namespace AV
} // namespace djv

