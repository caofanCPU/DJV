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

#pragma once

#include <djvUI/Enum.h>

#include <djvAV/Color.h>
#include <djvAV/FontSystem.h>

#include <djvCore/ValueObserver.h>

#include <glm/vec2.hpp>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            class Style;

        } // namespace Settings

        //! This namespace provides styling.
        namespace Style
        {
            //! This enumeration provides the color roles.
            enum class ColorRole
            {
                None,
                Background,
                BackgroundHeader,
                BackgroundText,
                BackgroundTooltip,
                Foreground,
                ForegroundDim,
                ForegroundTooltip,
                Border,
                Trough,
                Button,
                Checked,
                CheckedForeground,
                Hover,
                Disabled,
                Overlay,
                Shadow,

                Count,
                First = None
            };
            DJV_ENUM_HELPERS(ColorRole);

            //! This enumeration provides the metrics roles.
            enum class MetricsRole
            {
                None,
                Border,
                Margin,
                MarginSmall,
                MarginLarge,
                Spacing,
                SpacingSmall,
                SpacingLarge,
                Drag,
                Icon,
                IconLarge,
                FontSmall,
                FontMedium,
                FontLarge,
                FontExtraLarge,
                FontExtraExtraLarge,
                Swatch,
                ThumbnailSmall,
                ThumbnailLarge,
                Shadow,
                ScrollArea,
                TextColumn,
                Dialog,
                TooltipOffset,

                Count,
                First = Border
            };
            DJV_ENUM_HELPERS(MetricsRole);

            //! This class provides a color palette.
            class Palette
            {
            public:
                Palette();
                Palette(const Palette&);
                ~Palette();
                Palette& operator = (const Palette&);

                const AV::Image::Color& getColor(ColorRole) const;
                void setColor(ColorRole, const AV::Image::Color&);

                bool operator == (const Palette&) const;

            private:
                DJV_PRIVATE();
            };

            //! This class provides size metrics.
            class Metrics
            {
            public:
                Metrics();
                Metrics(const Metrics&);
                ~Metrics();
                Metrics& operator = (const Metrics&);

                float getMetric(MetricsRole role) const;
                void setMetric(MetricsRole, float);

                bool operator == (const Metrics&) const;

            private:
                DJV_PRIVATE();
            };

            //! This struct provides text metrics.
            struct TextMetrics
            {
                float ascender = 0.f;
                float descender = 0.f;
                float lineHeight = 0.f;
            };

            //! This class provides the UI style.
            class Style : public Core::ISystem
            {
                DJV_NON_COPYABLE(Style);
                void _init(Core::Context *);
                Style();

            public:
                virtual ~Style();

                //! Create a new style.
                static std::shared_ptr<Style> create(Core::Context *);

                //! Get the style settings.
                const std::shared_ptr<Settings::Style>& getSettings() const;

                //! \name Color Palette
                ///@{

                const Palette & getPalette() const;
                const AV::Image::Color & getColor(ColorRole) const;

                ///@}

                //! \name Size Metrics
                ///@{

                int getDPI() const;
                float getScale() const;

                float getMetric(MetricsRole) const;

                ///@}

                //! \name Fonts
                ///@{

                AV::Font::Info getFont(const std::string &, MetricsRole) const;

                ///@}

                std::shared_ptr<Core::IValueSubject<bool> > observeStyleChanged() const;

            private:
                void _updateCurrentFont();

                DJV_PRIVATE();
            };

        } // namespace Style
    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(UI::Style::ColorRole);
    DJV_ENUM_SERIALIZE_HELPERS(UI::Style::MetricsRole);

} // namespace djv
