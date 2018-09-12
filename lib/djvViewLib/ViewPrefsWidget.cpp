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

#include <djvViewLib/ViewPrefsWidget.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/ViewPrefs.h>

#include <djvUI/ColorSwatch.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/PrefsGroupBox.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QListWidget>
#include <QVBoxLayout>

namespace
{
class SmallListWidget : public QListWidget
{
public:

    QSize sizeHint() const
    {
        const QSize size = QListWidget::sizeHint();
        return QSize(size.width(), size.height() / 2);
    }
};

} // namespace

namespace djv
{
    namespace ViewLib
    {
        struct ViewPrefsWidget::Private
        {
            Private() :
                zoomFactorWidget(0),
                backgroundColorWidget(0),
                gridWidget(0),
                gridColorWidget(0),
                hudEnabledWidget(0),
                hudInfoWidget(0),
                hudColorWidget(0),
                hudBackgroundWidget(0),
                hudBackgroundColorWidget(0)
            {}

            QComboBox * zoomFactorWidget;
            UI::ColorSwatch * backgroundColorWidget;
            QComboBox * gridWidget;
            UI::ColorSwatch * gridColorWidget;
            QCheckBox * hudEnabledWidget;
            QListWidget * hudInfoWidget;
            UI::ColorSwatch * hudColorWidget;
            QComboBox * hudBackgroundWidget;
            UI::ColorSwatch * hudBackgroundColorWidget;
        };

        ViewPrefsWidget::ViewPrefsWidget(Context * context) :
            AbstractPrefsWidget(qApp->translate("djv::ViewLib::ViewPrefsWidget", "Views"), context),
            _p(new Private)
        {
            // Create the general widgets.
            _p->zoomFactorWidget = new QComboBox;
            _p->zoomFactorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->zoomFactorWidget->addItems(Util::zoomFactorLabels());

            _p->backgroundColorWidget = new UI::ColorSwatch(context);
            _p->backgroundColorWidget->setSwatchSize(UI::ColorSwatch::SWATCH_SMALL);
            _p->backgroundColorWidget->setColorDialogEnabled(true);

            // Create the grid widgets.
            _p->gridWidget = new QComboBox;
            _p->gridWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->gridWidget->addItems(Util::gridLabels());

            _p->gridColorWidget = new UI::ColorSwatch(context);
            _p->gridColorWidget->setSwatchSize(UI::ColorSwatch::SWATCH_SMALL);
            _p->gridColorWidget->setColorDialogEnabled(true);

            // Create the HUD widgets.
            _p->hudEnabledWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "Enable the HUD"));

            _p->hudInfoWidget = new SmallListWidget;

            for (int i = 0; i < Util::HUD_COUNT; ++i)
            {
                QListWidgetItem * item = new QListWidgetItem(_p->hudInfoWidget);
                item->setText(Util::hudInfoLabels()[i]);
                item->setFlags(
                    Qt::ItemIsSelectable |
                    Qt::ItemIsUserCheckable |
                    Qt::ItemIsEnabled);
            }

            _p->hudColorWidget = new UI::ColorSwatch(context);
            _p->hudColorWidget->setSwatchSize(UI::ColorSwatch::SWATCH_SMALL);
            _p->hudColorWidget->setColorDialogEnabled(true);

            _p->hudBackgroundWidget = new QComboBox;
            _p->hudBackgroundWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->hudBackgroundWidget->addItems(Util::hudBackgroundLabels());

            _p->hudBackgroundColorWidget = new UI::ColorSwatch(context);
            _p->hudBackgroundColorWidget->setSwatchSize(UI::ColorSwatch::SWATCH_SMALL);
            _p->hudBackgroundColorWidget->setColorDialogEnabled(true);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);

            UI::PrefsGroupBox * prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "Views"), context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "Mouse wheel zoom factor:"),
                _p->zoomFactorWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "Background color:"),
                _p->backgroundColorWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "Grid"), context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "Size:"),
                _p->gridWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "Color:"),
                _p->gridColorWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "HUD (Heads Up Display)"), context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->hudEnabledWidget);
            formLayout->addRow(_p->hudInfoWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "Foreground color:"),
                _p->hudColorWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "Background style:"),
                _p->hudBackgroundWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::ViewPrefsWidget", "Background color:"),
                _p->hudBackgroundColorWidget);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->zoomFactorWidget,
                SIGNAL(currentIndexChanged(int)),
                SLOT(zoomFactorCallback(int)));
            connect(
                _p->backgroundColorWidget,
                SIGNAL(colorChanged(const djvColor &)),
                SLOT(backgroundCallback(const djvColor &)));
            connect(
                _p->gridWidget,
                SIGNAL(currentIndexChanged(int)),
                SLOT(gridCallback(int)));
            connect(
                _p->gridColorWidget,
                SIGNAL(colorChanged(const djvColor &)),
                SLOT(gridColorCallback(const djvColor &)));
            connect(
                _p->hudEnabledWidget,
                SIGNAL(toggled(bool)),
                SLOT(hudEnabledCallback(bool)));
            connect(
                _p->hudInfoWidget,
                SIGNAL(itemChanged(QListWidgetItem *)),
                SLOT(hudInfoCallback(QListWidgetItem *)));
            connect(
                _p->hudColorWidget,
                SIGNAL(colorChanged(const djvColor &)),
                SLOT(hudColorCallback(const djvColor &)));
            connect(
                _p->hudBackgroundWidget,
                SIGNAL(currentIndexChanged(int)),
                SLOT(hudBackgroundCallback(int)));
            connect(
                _p->hudBackgroundColorWidget,
                SIGNAL(colorChanged(const djvColor &)),
                SLOT(hudBackgroundColorCallback(const djvColor &)));
        }

        ViewPrefsWidget::~ViewPrefsWidget()
        {}

        void ViewPrefsWidget::resetPreferences()
        {
            context()->viewPrefs()->setZoomFactor(ViewPrefs::zoomFactorDefault());
            context()->viewPrefs()->setBackground(ViewPrefs::backgroundDefault());
            context()->viewPrefs()->setGrid(ViewPrefs::gridDefault());
            context()->viewPrefs()->setGridColor(ViewPrefs::gridColorDefault());
            context()->viewPrefs()->setHudEnabled(ViewPrefs::hudEnabledDefault());
            context()->viewPrefs()->setHudInfo(ViewPrefs::hudInfoDefault());
            context()->viewPrefs()->setHudColor(ViewPrefs::hudColorDefault());
            context()->viewPrefs()->setHudBackground(ViewPrefs::hudBackgroundDefault());
            context()->viewPrefs()->setHudBackgroundColor(ViewPrefs::hudBackgroundColorDefault());
            widgetUpdate();
        }

        void ViewPrefsWidget::zoomFactorCallback(int in)
        {
            context()->viewPrefs()->setZoomFactor(static_cast<Util::ZOOM_FACTOR>(in));
        }

        void ViewPrefsWidget::backgroundCallback(const djvColor & in)
        {
            _p->backgroundColorWidget->setColor(in);
            context()->viewPrefs()->setBackground(in);
        }

        void ViewPrefsWidget::gridCallback(int in)
        {
            context()->viewPrefs()->setGrid(static_cast<Util::GRID>(in));
        }

        void ViewPrefsWidget::gridColorCallback(const djvColor & in)
        {
            _p->gridColorWidget->setColor(in);
            context()->viewPrefs()->setGridColor(in);
        }

        void ViewPrefsWidget::hudEnabledCallback(bool in)
        {
            context()->viewPrefs()->setHudEnabled(in);
        }

        void ViewPrefsWidget::hudInfoCallback(QListWidgetItem * item)
        {
            const int row = _p->hudInfoWidget->row(item);
            QVector<bool> info = context()->viewPrefs()->hudInfo();
            info[row] = !info[row];
            context()->viewPrefs()->setHudInfo(info);
        }

        void ViewPrefsWidget::hudColorCallback(const djvColor & in)
        {
            _p->hudColorWidget->setColor(in);
            context()->viewPrefs()->setHudColor(in);
        }

        void ViewPrefsWidget::hudBackgroundCallback(int in)
        {
            context()->viewPrefs()->setHudBackground(static_cast<Util::HUD_BACKGROUND>(in));
        }

        void ViewPrefsWidget::hudBackgroundColorCallback(const djvColor & in)
        {
            _p->hudBackgroundColorWidget->setColor(in);
            context()->viewPrefs()->setHudBackgroundColor(in);
        }

        void ViewPrefsWidget::widgetUpdate()
        {
            djvSignalBlocker signalBlocker(QObjectList() <<
                _p->zoomFactorWidget <<
                _p->backgroundColorWidget <<
                _p->gridWidget <<
                _p->gridColorWidget <<
                _p->hudEnabledWidget <<
                _p->hudInfoWidget <<
                _p->hudColorWidget <<
                _p->hudBackgroundWidget <<
                _p->hudBackgroundColorWidget);
            _p->zoomFactorWidget->setCurrentIndex(context()->viewPrefs()->zoomFactor());
            _p->backgroundColorWidget->setColor(context()->viewPrefs()->background());
            _p->gridWidget->setCurrentIndex(context()->viewPrefs()->grid());
            _p->gridColorWidget->setColor(context()->viewPrefs()->gridColor());
            _p->hudEnabledWidget->setChecked(context()->viewPrefs()->isHudEnabled());
            QVector<bool> hudInfo = context()->viewPrefs()->hudInfo();
            for (int i = 0; i < Util::HUD_COUNT; ++i)
            {
                QListWidgetItem * item = _p->hudInfoWidget->item(i);
                item->setCheckState(hudInfo[i] ? Qt::Checked : Qt::Unchecked);
            }
            _p->hudColorWidget->setColor(context()->viewPrefs()->hudColor());
            _p->hudBackgroundWidget->setCurrentIndex(context()->viewPrefs()->hudBackground());
            _p->hudBackgroundColorWidget->setColor(context()->viewPrefs()->hudBackgroundColor());
        }

    } // namespace ViewLib
} // namespace djv
