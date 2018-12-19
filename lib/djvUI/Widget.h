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

#include <djvUI/Margin.h>
#include <djvUI/Style.h>

#include <djvCore/BBox.h>
#include <djvCore/Context.h>
#include <djvCore/Event.h>
#include <djvCore/IObject.h>

#include <functional>
#include <memory>

namespace djv
{
    namespace AV
    {
        class FontSystem;
        class IconSystem;
        class Render2DSystem;
    
    } // namespace AV

    namespace UI
    {
        class Action;
        class IWindowSystem;
        class Window;

        //! This class provides the base widget functionality.
        class Widget : public Core::IObject
        {
            DJV_NON_COPYABLE(Widget);

        protected:
            void _init(Core::Context *);
            Widget();

        public:
            virtual ~Widget();

            //! Create a new widget.
            static std::shared_ptr<Widget> create(Core::Context *);

            //! Get the number of widgets that currently exist.
            static size_t getCurrentWidgetCount();

            //! Get the top-level window.
            std::weak_ptr<Window> getWindow();

            //! \name Widget Visibility
            ///@{

            inline bool isVisible(bool parents = false) const;
            inline bool isClipped() const;
            inline float getOpacity(bool parents = false) const;
            virtual void setVisible(bool);
            void show();
            void hide();
            void setOpacity(float);

            ///@}

            //! \name Widget Layout
            ///@{

            inline const glm::vec2& getMinimumSize() const;
            inline const Core::BBox2f& getGeometry() const;
            inline glm::vec2 getSize() const;
            inline float getWidth() const;
            inline float getHeight() const;
            void setGeometry(const Core::BBox2f&);
            void move(const glm::vec2&);
            void resize(const glm::vec2&);

            //! This function is called during the layout event to determine the height of a
            //! widget based upon the given width. This is used for example by FlowLayout
            //! and TextBlock.
            inline virtual float getHeightForWidth(float) const;

            inline const Margin& getMargin() const;
            void setMargin(const Margin&);

            inline HAlign getHAlign() const;
            inline VAlign getVAlign() const;
            void setHAlign(HAlign);
            void setVAlign(VAlign);

            static Core::BBox2f getAlign(const Core::BBox2f&, const glm::vec2& minimumSize, HAlign, VAlign);

            ///@}

            //! \name Widget Style
            ///@{

            inline ColorRole getBackgroundRole() const;
            void setBackgroundRole(ColorRole);

            ///@}

            //! \name Widget Input
            ///@{

            inline bool isPointerEnabled() const;
            void setPointerEnabled(bool);

            ///@}

            //! \name Actions
            ///@{

            void addAction(const std::shared_ptr<Action>&);
            void removeAction(const std::shared_ptr<Action>&);
            void clearActions();

            ///@}

            void setParent(const std::shared_ptr<IObject>&, int insert = -1) override;

            virtual void preLayoutEvent(Core::PreLayoutEvent&) {}
            virtual void layoutEvent(Core::LayoutEvent&) {}
            virtual void clipEvent(Core::ClipEvent&) {}
            virtual void paintEvent(Core::PaintEvent&);
            virtual void scrollEvent(Core::ScrollEvent&) {}
            virtual void dropEvent(Core::DropEvent&) {}
            virtual void keyboardFocusEvent(Core::KeyboardFocusEvent&) {}
            virtual void keyboardFocusLostEvent(Core::KeyboardFocusLostEvent&) {}
            virtual void keyPressEvent(Core::KeyPressEvent&);
            virtual void keyReleaseEvent(Core::KeyReleaseEvent&) {}
            virtual void textEvent(Core::TextEvent&) {}

            void pointerEnterEvent(Core::PointerEnterEvent&) override;
            void pointerLeaveEvent(Core::PointerLeaveEvent&) override;
            void pointerMoveEvent(Core::PointerMoveEvent&) override;
            bool event(Core::IEvent&) override;

        protected:
            inline const std::weak_ptr<AV::IconSystem>& _getIconSystem() const;
            inline const std::weak_ptr<AV::FontSystem>& _getFontSystem() const;
            inline const std::weak_ptr<AV::Render2DSystem>& _getRenderSystem() const;
            inline const std::weak_ptr<Style>& _getStyle() const;

            AV::Color _getColorWithOpacity(const AV::Color &) const;

            void _setMinimumSize(const glm::vec2&);

        private:
            bool _visible = true;
            bool _parentsVisible = true;
            bool _clipped = false;
            float _opacity = 1.f;
            float _parentsOpacity = 1.f;
            Core::BBox2f _geometry = Core::BBox2f(0.f, 0.f, 0.f, 0.f);
            glm::vec2 _minimumSize = glm::vec2(0.f, 0.f);
            Margin _margin;
            HAlign _hAlign = HAlign::Fill;
            VAlign _vAlign = VAlign::Fill;
            ColorRole _backgroundRole = ColorRole::None;
            bool _pointerEnabled = false;
            std::weak_ptr<AV::IconSystem> _iconSystem;
            std::weak_ptr<AV::FontSystem> _fontSystem;
            std::weak_ptr<AV::Render2DSystem> _renderSystem;
            std::weak_ptr<Style> _style;
            std::vector<std::shared_ptr<Action> > _actions;

            friend class IWindowSystem;
        };

    } // namespace UI
} // namespace djv

#include <djvUI/WidgetInline.h>