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

#include <djvUI/TextBlock.h>

#include <djvCore/Cache.h>
#include <djvCore/Math.h>
#include <djvCore/Memory.h>

#include <djvAV/FontSystem.h>
#include <djvAV/Render2DSystem.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct TextBlock::Private
        {
            std::string text;
            TextHAlign textHAlign = TextHAlign::Left;
            ColorRole textColorRole = ColorRole::Foreground;
            FontFace fontFace = FontFace::First;
            MetricsRole fontSizeRole = MetricsRole::FontMedium;
            float minimumWidth = 200.f;
            std::future<AV::FontMetrics> fontMetricsFuture;
            AV::FontMetrics fontMetrics;
            float heightForWidth = 0.f;
            size_t heightForWidthHash = 0;
            glm::vec2 textSize = glm::vec2(0.f, 0.f);
            size_t textSizeHash = 0;
            std::future<glm::vec2> textSizeFuture;
            std::vector<AV::FontLine> breakText;
            size_t breakTextHash = 0;
            std::future<std::vector<AV::FontLine> > breakTextFuture;
            BBox2f clipRect;
        };

        void TextBlock::_init(Context * context)
        {
            Widget::_init(context);
            
            setClassName("djv::UI::TextBlock");
        }
        
        TextBlock::TextBlock() :
            _p(new Private)
        {}

        TextBlock::~TextBlock()
        {}

        std::shared_ptr<TextBlock> TextBlock::create(Context * context)
        {
            auto out = std::shared_ptr<TextBlock>(new TextBlock);
            out->_init(context);
            return out;
        }

        std::shared_ptr<TextBlock> TextBlock::create(const std::string& text, Context * context)
        {
            auto out = TextBlock::create(context);
            out->setText(text);
            return out;
        }

        const std::string& TextBlock::getText() const
        {
            return _p->text;
        }

        void TextBlock::setText(const std::string& value)
        {
            _p->text = value;
            _p->heightForWidthHash = 0;
            _p->textSizeHash = 0;
            _p->breakTextHash = 0;
        }

        TextHAlign TextBlock::getTextHAlign() const
        {
            return _p->textHAlign;
        }
        
        void TextBlock::setTextHAlign(TextHAlign value)
        {
            _p->textHAlign = value;
        }

        ColorRole TextBlock::getTextColorRole() const
        {
            return _p->textColorRole;
        }

        void TextBlock::setTextColorRole(ColorRole value)
        {
            _p->textColorRole = value;
        }

        FontFace TextBlock::getFontFace() const
        {
            return _p->fontFace;
        }

        MetricsRole TextBlock::getFontSizeRole() const
        {
            return _p->fontSizeRole;
        }

        void TextBlock::setFontFace(FontFace value)
        {
            _p->fontFace = value;
        }

        void TextBlock::setFontSizeRole(MetricsRole value)
        {
            _p->fontSizeRole = value;
        }

        float TextBlock::getMinimumWidth() const
        {
            return _p->minimumWidth;
        }

        void TextBlock::setMinimumWidth(float value)
        {
            _p->minimumWidth = value;
            _p->textSizeHash = 0;
            _p->breakTextHash = 0;
        }

        float TextBlock::getHeightForWidth(float value) const
        {
            float out = 0.f;
            if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    const auto font = style->getFont(_p->fontFace, _p->fontSizeRole);
                    const float w = value - getMargin().getWidth(style);

                    size_t hash = 0;
                    Memory::hashCombine(hash, font.name);
                    Memory::hashCombine(hash, font.size);
                    Memory::hashCombine(hash, w);
                    if (!_p->heightForWidthHash || _p->heightForWidthHash != hash)
                    {
                        _p->heightForWidthHash = hash;
                        _p->heightForWidth = fontSystem->measure(_p->text, w, font).get().y;
                    }
                }
                out = _p->heightForWidth + getMargin().getHeight(style);
            }
            return out;
        }

        void TextBlock::updateEvent(UpdateEvent& event)
        {
            if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    auto font = style->getFont(_p->fontFace, _p->fontSizeRole);
                    const float w = _p->minimumWidth - getMargin().getWidth(style);

                    _p->fontMetricsFuture = fontSystem->getMetrics(font);

                    size_t hash = 0;
                    Memory::hashCombine(hash, font.name);
                    Memory::hashCombine(hash, font.size);
                    Memory::hashCombine(hash, w);
                    if (!_p->textSizeHash || _p->textSizeHash != hash)
                    {
                        _p->textSizeHash = hash;
                        _p->textSizeFuture = fontSystem->measure(_p->text, w, font);
                    }
                }
            }
        }

        void TextBlock::preLayoutEvent(PreLayoutEvent& event)
        {
            if (auto style = _getStyle().lock())
            {
                if (_p->textSizeFuture.valid())
                {
                    _p->textSize = _p->textSizeFuture.get();
                }
                glm::vec2 size = _p->textSize;
                size.x = std::max(size.x, _p->minimumWidth - getMargin().getWidth(style));
                _setMinimumSize(size + getMargin().getSize(style));
            }
        }

        void TextBlock::layoutEvent(LayoutEvent& event)
        {
            if (auto style = _getStyle().lock())
            {
                if (auto fontSystem = _getFontSystem().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    const auto font = style->getFont(_p->fontFace, _p->fontSizeRole);

                    size_t hash = 0;
                    Memory::hashCombine(hash, font.name);
                    Memory::hashCombine(hash, font.size);
                    Memory::hashCombine(hash, g.w());
                    if (!_p->breakTextHash || _p->breakTextHash != hash)
                    {
                        _p->breakTextHash = hash;
                        _p->breakTextFuture = fontSystem->breakLines(_p->text, g.w(), font);
                    }
                }
            }
        }

        void TextBlock::clipEvent(ClipEvent& event)
        {
            _p->clipRect = event.getClipRect();
        }

        void TextBlock::paintEvent(PaintEvent& event)
        {
            Widget::paintEvent(event);
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);

                    float ascender = 0.f;
                    if (_p->fontMetricsFuture.valid())
                    {
                        ascender = _p->fontMetricsFuture.get().ascender;
                    }
                    if (_p->breakTextFuture.valid())
                    {
                        _p->breakText = _p->breakTextFuture.get();
                    }
                    glm::vec2 pos = g.min;
                    render->setCurrentFont(style->getFont(_p->fontFace, _p->fontSizeRole));
                    render->setFillColor(style->getColor(_p->textColorRole));
                    for (const auto& line : _p->breakText)
                    {
                        if (pos.y >= _p->clipRect.min.y && pos.y + line.size.y <= _p->clipRect.max.y)
                        {
                            render->drawText(line.text, glm::vec2(pos.x, pos.y + ascender));
                        }
                        pos.y += line.size.y;
                    }
                }
            }
        }

    } // namespace UI
} // namespace djv
