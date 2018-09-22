//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvViewLib/FilePrefsWidget.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/FilePrefs.h>
#include <djvViewLib/MiscWidget.h>

#include <djvUI/Prefs.h>
#include <djvUI/PrefsGroupBox.h>

#include <djvCore/FileInfoUtil.h>
#include <djvCore/ListUtil.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct FilePrefsWidget::Private
        {
            QCheckBox *       autoSequenceWidget = nullptr;
            QComboBox *       proxyWidget = nullptr;
            QCheckBox *       u8ConversionWidget = nullptr;
            QCheckBox *       cacheWidget = nullptr;
            CacheSizeWidget * cacheSizeWidget = nullptr;
            QCheckBox *       preloadWidget = nullptr;
            QCheckBox *       displayCacheWidget = nullptr;
        };

        FilePrefsWidget::FilePrefsWidget(Context * context) :
            AbstractPrefsWidget(qApp->translate("djv::ViewLib::FilePrefsWidget", "Files"), context),
            _p(new Private)
        {
            // Create the options widgets.
            _p->autoSequenceWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Automatically detect sequences when opening files"));

            // Create the proxy scale widgets.
            _p->proxyWidget = new QComboBox;
            _p->proxyWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->proxyWidget->addItems(Graphics::PixelDataInfo::proxyLabels());

            // Create the convert to 8-bits widgets.
            _p->u8ConversionWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Enable 8-bit conversion"));

            // Create the file cache widgets.
            _p->cacheWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Enable the memory cache"));

            _p->cacheSizeWidget = new CacheSizeWidget(context);

            _p->preloadWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Pre-load cache frames"));

            _p->displayCacheWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Display cached frames in timeline"));

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);

            UI::PrefsGroupBox * prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Files"), context);
            QFormLayout * formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->autoSequenceWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Proxy Scale"),
                qApp->translate("djv::ViewLib::FilePrefsWidget",
                    "Set proxy scaling to reduce the resolution when loading images. This "
                    "allows more images to be stored in the memory cache at the expense of "
                    "image quality. Proxy scaling can also improve playback speed since the "
                    "images are smaller."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Proxy scale:"),
                _p->proxyWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "8-bit Conversion"),
                qApp->translate("djv::ViewLib::FilePrefsWidget",
                    "Set 8-bit conversion to allow more images to be stored in the memory "
                    "cache at the expense of image quality."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->u8ConversionWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Memory Cache"),
                qApp->translate("djv::ViewLib::FilePrefsWidget",
                    "The memory cache allows the application to store images in memory "
                    "which can improve playback performance. When the memory cache is "
                    "disabled the images are streamed directly from disk."),
                context);
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->cacheWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::FilePrefsWidget", "Cache size (gigabytes):"),
                _p->cacheSizeWidget);
            formLayout->addRow(_p->preloadWidget);
            formLayout->addRow(_p->displayCacheWidget);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->autoSequenceWidget,
                SIGNAL(toggled(bool)),
                SLOT(autoSequenceCallback(bool)));
            connect(
                _p->proxyWidget,
                SIGNAL(activated(int)),
                SLOT(proxyCallback(int)));
            connect(
                _p->u8ConversionWidget,
                SIGNAL(toggled(bool)),
                SLOT(u8ConversionCallback(bool)));
            connect(
                _p->cacheWidget,
                SIGNAL(toggled(bool)),
                SLOT(cacheCallback(bool)));
            connect(
                _p->cacheSizeWidget,
                SIGNAL(cacheSizeChanged(float)),
                SLOT(cacheSizeCallback(float)));
            connect(
                _p->preloadWidget,
                SIGNAL(toggled(bool)),
                SLOT(preloadCallback(bool)));
            connect(
                _p->displayCacheWidget,
                SIGNAL(toggled(bool)),
                SLOT(displayCacheCallback(bool)));
        }

        FilePrefsWidget::~FilePrefsWidget()
        {}

        void FilePrefsWidget::resetPreferences()
        {
            context()->filePrefs()->setAutoSequence(FilePrefs::autoSequenceDefault());
            context()->filePrefs()->setProxy(FilePrefs::proxyDefault());
            context()->filePrefs()->setU8Conversion(FilePrefs::u8ConversionDefault());
            context()->filePrefs()->setCache(FilePrefs::cacheDefault());
            context()->filePrefs()->setCacheSize(FilePrefs::cacheSizeDefault());
            context()->filePrefs()->setPreload(FilePrefs::preloadDefault());
            context()->filePrefs()->setDisplayCache(FilePrefs::displayCacheDefault());
            widgetUpdate();
        }

        void FilePrefsWidget::autoSequenceCallback(bool in)
        {
            context()->filePrefs()->setAutoSequence(in);
        }

        void FilePrefsWidget::proxyCallback(int in)
        {
            context()->filePrefs()->setProxy(static_cast<Graphics::PixelDataInfo::PROXY>(in));
        }

        void FilePrefsWidget::u8ConversionCallback(bool in)
        {
            context()->filePrefs()->setU8Conversion(in);
        }

        void FilePrefsWidget::cacheCallback(bool in)
        {
            context()->filePrefs()->setCache(in);
        }

        void FilePrefsWidget::cacheSizeCallback(float in)
        {
            context()->filePrefs()->setCacheSize(in);
        }

        void FilePrefsWidget::preloadCallback(bool in)
        {
            context()->filePrefs()->setPreload(in);
        }

        void FilePrefsWidget::displayCacheCallback(bool in)
        {
            context()->filePrefs()->setDisplayCache(in);
        }

        void FilePrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->autoSequenceWidget <<
                _p->proxyWidget <<
                _p->u8ConversionWidget <<
                _p->cacheWidget <<
                _p->cacheSizeWidget <<
                _p->preloadWidget <<
                _p->displayCacheWidget);
            _p->autoSequenceWidget->setChecked(context()->filePrefs()->hasAutoSequence());
            _p->proxyWidget->setCurrentIndex(context()->filePrefs()->proxy());
            _p->u8ConversionWidget->setChecked(context()->filePrefs()->hasU8Conversion());
            _p->cacheWidget->setChecked(context()->filePrefs()->hasCache());
            _p->cacheSizeWidget->setCacheSize(context()->filePrefs()->cacheSize());
            _p->preloadWidget->setChecked(context()->filePrefs()->hasPreload());
            _p->displayCacheWidget->setChecked(context()->filePrefs()->hasDisplayCache());
        }

    } // namespace ViewLib
} // namespace djv