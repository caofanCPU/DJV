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

#include <djvViewLib/PlaybackWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackWidget::Private
        {
            std::shared_ptr<ValueSubject<Playback> > playback;
            std::shared_ptr<UI::Button::Group> buttonGroup;
            std::shared_ptr<UI::Layout::HorizontalLayout> layout;
        };
        
        void PlaybackWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            p.playback = ValueSubject<Playback>::create();

            auto stopButton = UI::Button::Tool::create(context);
            stopButton->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, "djvIconPlaybackStop90DPI.png"));
            auto forwardButton = UI::Button::Tool::create(context);
            forwardButton->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, "djvIconPlaybackForward90DPI.png"));
            auto reverseButton = UI::Button::Tool::create(context);
            reverseButton->setIcon(context->getPath(FileSystem::ResourcePath::IconsDirectory, "djvIconPlaybackReverse90DPI.png"));

            p.buttonGroup = UI::Button::Group::create(UI::ButtonType::Radio);
            p.buttonGroup->addButton(stopButton);
            p.buttonGroup->addButton(forwardButton);
            p.buttonGroup->addButton(reverseButton);

            p.layout = UI::Layout::HorizontalLayout::create(context);
            p.layout->setSpacing(UI::Style::MetricsRole::None);
            p.layout->addWidget(reverseButton);
            p.layout->addWidget(stopButton);
            p.layout->addWidget(forwardButton);
            p.layout->setParent(shared_from_this());

            _updateWidget();

            p.buttonGroup->setRadioCallback(
                [this](int index)
            {
                _p->playback->setIfChanged(static_cast<Playback>(index));
            });
        }

        PlaybackWidget::PlaybackWidget() :
            _p(new Private)
        {}

        PlaybackWidget::~PlaybackWidget()
        {}

        std::shared_ptr<PlaybackWidget> PlaybackWidget::create(Context * context)
        {
            auto out = std::shared_ptr<PlaybackWidget>(new PlaybackWidget);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::IValueSubject<Playback> > PlaybackWidget::getPlayback() const
        {
            return _p->playback;
        }

        void PlaybackWidget::setPlayback(Playback value)
        {
            if (_p->playback->setIfChanged(value))
            {
                _updateWidget();
            }
        }

        void PlaybackWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void PlaybackWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }
        
        void PlaybackWidget::_updateWidget()
        {
            DJV_PRIVATE_PTR();
            p.buttonGroup->setChecked(static_cast<int>(p.playback->get()));
        }

    } // namespace ViewLib
} // namespace djv
