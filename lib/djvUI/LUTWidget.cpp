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

#include <djvUI/LUTWidget.h>

#include <djvUI/UIContext.h>
#include <djvUI/PrefsGroupBox.h>
#include <djvUI/Style.h>

#include <djvGraphics/ImageIO.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        LUTWidget::LUTWidget(djvImageIO * plugin, UIContext * context) :
            ImageIOWidget(plugin, context),
            _typeWidget(0)
        {
            // Create the widgets.
            _typeWidget = new QComboBox;
            _typeWidget->addItems(djvLUT::typeLabels());
            _typeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);
            layout->setSpacing(context->style()->sizeMetric().largeSpacing);

            PrefsGroupBox * prefsGroupBox = new PrefsGroupBox(
                qApp->translate("djv::UI::LUTWidget", "Pixel Type"),
                qApp->translate("djv::UI::LUTWidget", "Set the pixel type used when loading LUTs."),
                context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::UI::LUTWidget", "Pixel type:"),
                _typeWidget);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            QStringList tmp;
            tmp = plugin->option(plugin->options()[djvLUT::TYPE_OPTION]);
            tmp >> _options.type;
            widgetUpdate();

            // Setup the callbacks.
            connect(
                plugin,
                SIGNAL(optionChanged(const QString &)),
                SLOT(pluginCallback(const QString &)));
            connect(
                _typeWidget,
                SIGNAL(activated(int)),
                SLOT(typeCallback(int)));
        }

        LUTWidget::~LUTWidget()
        {}

        void LUTWidget::resetPreferences()
        {
            _options = djvLUT::Options();
            pluginUpdate();
            widgetUpdate();
        }

        void LUTWidget::pluginCallback(const QString & option)
        {
            try
            {
                QStringList tmp;
                tmp = plugin()->option(option);
                if (0 == option.compare(
                    plugin()->options()[djvLUT::TYPE_OPTION], Qt::CaseInsensitive))
                    tmp >> _options.type;
            }
            catch (const QString &)
            {
            }
            widgetUpdate();
        }

        void LUTWidget::typeCallback(int in)
        {
            _options.type = static_cast<djvLUT::TYPE>(in);
            pluginUpdate();
        }

        void LUTWidget::pluginUpdate()
        {
            QStringList tmp;
            tmp << _options.type;
            plugin()->setOption(plugin()->options()[djvLUT::TYPE_OPTION], tmp);
        }

        void LUTWidget::widgetUpdate()
        {
            djvSignalBlocker signalBlocker(QObjectList() <<
                _typeWidget);
            _typeWidget->setCurrentIndex(_options.type);
        }

        LUTWidgetPlugin::LUTWidgetPlugin(djvCoreContext * context) :
            ImageIOWidgetPlugin(context)
        {}

        ImageIOWidget * LUTWidgetPlugin::createWidget(djvImageIO * plugin) const
        {
            return new LUTWidget(plugin, uiContext());
        }

        QString LUTWidgetPlugin::pluginName() const
        {
            return djvLUT::staticName;
        }

    } // namespace UI
} // namespace djv
