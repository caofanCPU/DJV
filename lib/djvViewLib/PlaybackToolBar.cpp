//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvViewLib/PlaybackToolBar.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/MiscWidget.h>
#include <djvViewLib/PlaybackActions.h>
#include <djvViewLib/PlaybackWidgets.h>
#include <djvUI/ToolButton.h>

#include <djvUI/IconLibrary.h>

#include <QApplication>
#include <QHBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackToolBar::Private
        {
            Util::LAYOUT layout = static_cast<Util::LAYOUT>(0);
            QWidget * widget = nullptr;
            QHBoxLayout * widgetLayout = nullptr;
            PlaybackButtons * playbackButtons = nullptr;
            LoopWidget * loopWidget = nullptr;
            SpeedWidget * speedWidget = nullptr;
            SpeedDisplay * realSpeedDisplay = nullptr;
            UI::ToolButton * everyFrameButton = nullptr;
            FrameWidget * frameWidget = nullptr;
            FrameSlider * frameSlider = nullptr;
            FrameWidget * startWidget = nullptr;
            FrameWidget * endWidget = nullptr;
            FrameButtons * frameButtons = nullptr;
            FrameDisplay * durationDisplay = nullptr;
            UI::ToolButton * inOutEnabledButton = nullptr;
            UI::ToolButton * markInPointButton = nullptr;
            UI::ToolButton * markOutPointButton = nullptr;
            UI::ToolButton * resetInPointButton = nullptr;
            UI::ToolButton * resetOutPointButton = nullptr;
        };

        PlaybackToolBar::PlaybackToolBar(
            AbstractActions * actions,
            Context *         context,
            QWidget *         parent) :
            AbstractToolBar(actions, context, parent),
            _p(new Private)
        {
            // Create the playback widgets.
            _p->playbackButtons = new PlaybackButtons(
                actions->group(PlaybackActions::PLAYBACK_GROUP),
                context);

            _p->loopWidget = new LoopWidget(
                actions->group(PlaybackActions::LOOP_GROUP),
                context);

            // Create the speed widgets.
            _p->speedWidget = new SpeedWidget(context);
            _p->speedWidget->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Playback speed"));

            _p->realSpeedDisplay = new SpeedDisplay;
            _p->realSpeedDisplay->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Real playback speed"));

            _p->everyFrameButton = new UI::ToolButton;
            _p->everyFrameButton->setDefaultAction(
                actions->action(PlaybackActions::EVERY_FRAME));
            _p->everyFrameButton->setIconSize(QSize(20, 20));
            _p->everyFrameButton->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Playback every frame"));

            // Create the frame widgets.
            _p->frameWidget = new FrameWidget(context);
            _p->frameWidget->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Current frame"));

            _p->frameSlider = new FrameSlider(context);
            _p->frameSlider->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Frame slider"));

            _p->startWidget = new FrameWidget(context);
            _p->startWidget->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Start frame / in point"));

            _p->frameButtons = new FrameButtons(
                actions->group(PlaybackActions::FRAME_GROUP),
                context);

            _p->endWidget = new FrameWidget(context);
            _p->endWidget->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "End frame / out point"));

            _p->durationDisplay = new FrameDisplay(context);
            _p->durationDisplay->setToolTip(
                qApp->translate("djv::ViewLib::PlaybackToolBar", "Playback duration"));

            // Create the in/out point widgets.
            _p->inOutEnabledButton = new UI::ToolButton;
            _p->inOutEnabledButton->setDefaultAction(
                actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[
                    Util::IN_OUT_ENABLE]);
            _p->inOutEnabledButton->setIconSize(QSize(20, 20));

            _p->markInPointButton = new UI::ToolButton;
            _p->markInPointButton->setDefaultAction(
                actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[
                    Util::MARK_IN]);
            _p->markInPointButton->setIconSize(QSize(20, 20));

            _p->markOutPointButton = new UI::ToolButton;
            _p->markOutPointButton->setDefaultAction(
                actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[
                    Util::MARK_OUT]);
            _p->markOutPointButton->setIconSize(QSize(20, 20));

            _p->resetInPointButton = new UI::ToolButton;
            _p->resetInPointButton->setDefaultAction(
                actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[
                    Util::RESET_IN]);
            _p->resetInPointButton->setIconSize(QSize(20, 20));

            _p->resetOutPointButton = new UI::ToolButton;
            _p->resetOutPointButton->setDefaultAction(
                actions->group(PlaybackActions::IN_OUT_GROUP)->actions()[
                    Util::RESET_OUT]);
            _p->resetOutPointButton->setIconSize(QSize(20, 20));

            // Layout the widgets.
            _p->widget = new QWidget;
            addWidget(_p->widget);

            // Initialize.
            setAllowedAreas(Qt::BottomToolBarArea);
            setFloatable(false);
            setMovable(false);
            setIconSize(context->iconLibrary()->defaultSize());
            layoutUpdate();

            // Setup callbacks.
            connect(
                _p->playbackButtons,
                SIGNAL(shuttlePressed(bool)),
                SIGNAL(playbackShuttlePressed(bool)));
            connect(
                _p->playbackButtons,
                SIGNAL(shuttleChanged(int)),
                SIGNAL(playbackShuttleValue(int)));
            connect(
                _p->speedWidget,
                SIGNAL(speedChanged(const djvSpeed &)),
                SIGNAL(speedChanged(const djvSpeed &)));
            connect(
                _p->frameWidget,
                SIGNAL(frameChanged(qint64)),
                SIGNAL(frameChanged(qint64)));
            connect(
                _p->frameSlider,
                SIGNAL(pressed(bool)),
                SIGNAL(frameSliderPressed(bool)));
            connect(
                _p->frameSlider,
                SIGNAL(frameChanged(qint64)),
                SIGNAL(frameSliderChanged(qint64)));
            connect(
                _p->frameSlider,
                SIGNAL(inPointChanged(qint64)),
                SIGNAL(inPointChanged(qint64)));
            connect(
                _p->frameSlider,
                SIGNAL(outPointChanged(qint64)),
                SIGNAL(outPointChanged(qint64)));
            connect(
                _p->startWidget,
                SIGNAL(frameChanged(qint64)),
                _p->frameSlider,
                SLOT(setInPoint(qint64)));
            connect(
                _p->endWidget,
                SIGNAL(frameChanged(qint64)),
                _p->frameSlider,
                SLOT(setOutPoint(qint64)));
            connect(
                _p->frameButtons,
                SIGNAL(shuttlePressed(bool)),
                SIGNAL(frameShuttlePressed(bool)));
            connect(
                _p->frameButtons,
                SIGNAL(shuttleChanged(int)),
                SIGNAL(frameShuttleValue(int)));
            connect(
                _p->frameButtons,
                SIGNAL(pressed()),
                SIGNAL(frameButtonsPressed()));
            connect(
                _p->frameButtons,
                SIGNAL(released()),
                SIGNAL(frameButtonsReleased()));
        }

        PlaybackToolBar::~PlaybackToolBar()
        {}

        void PlaybackToolBar::setSpeed(const djvSpeed & speed)
        {
            _p->speedWidget->setSpeed(speed);
            _p->frameWidget->setSpeed(speed);
            _p->frameSlider->setSpeed(speed);
            _p->startWidget->setSpeed(speed);
            _p->endWidget->setSpeed(speed);
            _p->durationDisplay->setSpeed(speed);
        }

        void PlaybackToolBar::setDefaultSpeed(const djvSpeed & speed)
        {
            _p->speedWidget->setDefaultSpeed(speed);
        }

        void PlaybackToolBar::setRealSpeed(float speed)
        {
            _p->realSpeedDisplay->setSpeed(speed);
        }

        void PlaybackToolBar::setDroppedFrames(bool in)
        {
            _p->realSpeedDisplay->setDroppedFrames(in);
        }

        void PlaybackToolBar::setFrameList(const djvFrameList & in)
        {
            _p->frameWidget->setFrameList(in);
            _p->frameSlider->setFrameList(in);
            _p->startWidget->setFrameList(in);
            _p->endWidget->setFrameList(in);
        }

        void PlaybackToolBar::setFrame(qint64 in)
        {
            _p->frameWidget->setFrame(in);
            _p->frameSlider->setFrame(in);
        }

        void PlaybackToolBar::setCachedFrames(const djvFrameList & in)
        {
            _p->frameSlider->setCachedFrames(in);
        }

        void PlaybackToolBar::setStart(qint64 in)
        {
            _p->startWidget->setFrame(in);
        }

        void PlaybackToolBar::setEnd(qint64 in)
        {
            _p->endWidget->setFrame(in);
        }

        void PlaybackToolBar::setDuration(qint64 in, bool inOutEnabled)
        {
            _p->durationDisplay->setFrame(in);
            _p->durationDisplay->setInOutEnabled(inOutEnabled);
        }

        void PlaybackToolBar::setInOutEnabled(bool in)
        {
            _p->frameSlider->setInOutEnabled(in);
        }

        void PlaybackToolBar::setInPoint(qint64 in)
        {
            _p->frameSlider->setInPoint(in);
        }

        void PlaybackToolBar::setOutPoint(qint64 in)
        {
            _p->frameSlider->setOutPoint(in);
        }

        void PlaybackToolBar::markInPoint()
        {
            _p->frameSlider->markInPoint();
        }

        void PlaybackToolBar::markOutPoint()
        {
            _p->frameSlider->markOutPoint();
        }

        void PlaybackToolBar::resetInPoint()
        {
            _p->frameSlider->resetInPoint();
        }

        void PlaybackToolBar::resetOutPoint()
        {
            _p->frameSlider->resetOutPoint();
        }

        void PlaybackToolBar::setLayout(Util::LAYOUT layout)
        {
            if (layout == _p->layout)
                return;
            _p->layout = layout;
            layoutUpdate();
        }

        void PlaybackToolBar::layoutUpdate()
        {
            //DJV_DEBUG("PlaybackToolBar::layoutUpdate");
            delete _p->widgetLayout;
            _p->widgetLayout = new QHBoxLayout(_p->widget);
            _p->widgetLayout->setMargin(0);

            switch (_p->layout)
            {
            case Util::LAYOUT_DEFAULT:
            case Util::LAYOUT_LEFT:
            {
                QVBoxLayout * leftLayout = new QVBoxLayout;
                leftLayout->setMargin(0);

                QHBoxLayout * hLayout = new QHBoxLayout;
                hLayout->setMargin(0);
                hLayout->addWidget(_p->playbackButtons);
                hLayout->addWidget(_p->frameButtons);
                leftLayout->addLayout(hLayout);

                hLayout = new QHBoxLayout;
                hLayout->setMargin(0);
                QHBoxLayout * hLayout2 = new QHBoxLayout;
                hLayout2->setMargin(0);
                hLayout2->setSpacing(0);
                hLayout2->addWidget(_p->speedWidget);
                hLayout2->addWidget(_p->realSpeedDisplay);
                hLayout2->addWidget(_p->everyFrameButton);
                hLayout->addLayout(hLayout2);
                hLayout->addStretch(1000);
                hLayout->addWidget(_p->loopWidget);
                hLayout->addWidget(_p->inOutEnabledButton);
                leftLayout->addLayout(hLayout);

                _p->widgetLayout->addLayout(leftLayout);

                QVBoxLayout * rightLayout = new QVBoxLayout;
                rightLayout->setMargin(0);

                hLayout = new QHBoxLayout;
                hLayout->setMargin(0);
                hLayout->addWidget(_p->frameSlider, 1);
                rightLayout->addLayout(hLayout);

                hLayout = new QHBoxLayout;
                hLayout->setMargin(0);
                hLayout->addWidget(_p->frameWidget);
                hLayout2 = new QHBoxLayout;
                hLayout2->setSpacing(0);
                hLayout2->setMargin(0);
                hLayout2->addWidget(_p->markInPointButton);
                hLayout2->addWidget(_p->resetInPointButton);
                hLayout2->addWidget(_p->startWidget);
                hLayout->addLayout(hLayout2);
                if (Util::LAYOUT_DEFAULT == _p->layout)
                    hLayout->addStretch(1000);
                hLayout2 = new QHBoxLayout;
                hLayout2->setSpacing(0);
                hLayout2->setMargin(0);
                hLayout2->addWidget(_p->endWidget);
                hLayout2->addWidget(_p->resetOutPointButton);
                hLayout2->addWidget(_p->markOutPointButton);
                hLayout->addLayout(hLayout2);
                hLayout->addWidget(_p->durationDisplay);
                if (Util::LAYOUT_LEFT == _p->layout)
                    hLayout->addStretch(1000);
                rightLayout->addLayout(hLayout);

                _p->widgetLayout->addLayout(rightLayout, 1);
            }
            break;
            case Util::LAYOUT_CENTER:
            {
                QVBoxLayout * layout = new QVBoxLayout;
                layout->setMargin(0);

                layout->addWidget(_p->frameSlider);

                QHBoxLayout * hLayout = new QHBoxLayout;
                hLayout->setMargin(0);
                hLayout->setSpacing(0);
                hLayout->addStretch(1000);
                hLayout->addWidget(_p->frameWidget);
                hLayout->addWidget(_p->markInPointButton);
                hLayout->addWidget(_p->resetInPointButton);
                hLayout->addWidget(_p->startWidget);
                hLayout->addWidget(_p->playbackButtons);
                hLayout->addWidget(_p->frameButtons);
                hLayout->addWidget(_p->endWidget);
                hLayout->addWidget(_p->resetOutPointButton);
                hLayout->addWidget(_p->markOutPointButton);
                hLayout->addWidget(_p->durationDisplay);
                hLayout->addStretch(1000);
                layout->addLayout(hLayout);

                _p->widgetLayout->addLayout(layout, 1);
            }
            break;
            case Util::LAYOUT_MINIMAL:
            {
                _p->widgetLayout->addWidget(_p->playbackButtons);
                _p->widgetLayout->addWidget(_p->frameSlider, 1);
            }
            break;
            default: break;
            }

            switch (_p->layout)
            {
            case Util::LAYOUT_DEFAULT:
            case Util::LAYOUT_LEFT:
                _p->speedWidget->show();
                _p->realSpeedDisplay->show();
                _p->everyFrameButton->show();
                _p->loopWidget->show();
                _p->inOutEnabledButton->show();
                _p->frameSlider->show();
                _p->frameWidget->show();
                _p->frameButtons->show();
                _p->startWidget->show();
                _p->endWidget->show();
                _p->markInPointButton->show();
                _p->resetInPointButton->show();
                _p->markOutPointButton->show();
                _p->resetOutPointButton->show();
                _p->durationDisplay->show();
                break;

            case Util::LAYOUT_CENTER:
                _p->speedWidget->hide();
                _p->realSpeedDisplay->hide();
                _p->everyFrameButton->hide();
                _p->loopWidget->hide();
                _p->inOutEnabledButton->hide();
                _p->frameSlider->show();
                _p->frameWidget->show();
                _p->frameButtons->show();
                _p->startWidget->show();
                _p->endWidget->show();
                _p->markInPointButton->show();
                _p->resetInPointButton->show();
                _p->markOutPointButton->show();
                _p->resetOutPointButton->show();
                _p->durationDisplay->show();
                break;

            case Util::LAYOUT_MINIMAL:
                _p->speedWidget->hide();
                _p->realSpeedDisplay->hide();
                _p->everyFrameButton->hide();
                _p->loopWidget->hide();
                _p->inOutEnabledButton->hide();
                _p->frameWidget->hide();
                _p->frameButtons->hide();
                _p->startWidget->hide();
                _p->endWidget->hide();
                _p->markInPointButton->hide();
                _p->resetInPointButton->hide();
                _p->markOutPointButton->hide();
                _p->resetOutPointButton->hide();
                _p->durationDisplay->hide();
                break;
            default: break;
            }
        }

    } // namespace ViewLib
} // namespace djv
