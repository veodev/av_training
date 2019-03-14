#include <QCoreApplication>
#include <QTranslator>
#include <QMessageBox>

#include "languageswitcher.h"
#include "debug.h"

static QString defaultLanguage("English");
static QString _currentLanguage(defaultLanguage);

Languages LanguageSwitcher::supportedLanguages()
{
    static Languages _supportedLanguages;
    static bool isFirstRun = true;

    if (isFirstRun) {
        _supportedLanguages[defaultLanguage] = defaultLanguage;
        _supportedLanguages["Russian"] = "Русский";
        _supportedLanguages["German"] = "Deutsche";
        isFirstRun = false;
    }

    return _supportedLanguages;
}

void LanguageSwitcher::switchLanguage(const QString & language)
{
    static QTranslator * _translator = 0;

    if (_translator != 0) {
        qApp->removeTranslator(_translator);
        delete _translator;
        _translator = 0;
    }

    if (language != defaultLanguage) {
        _translator = new QTranslator();
        Q_ASSERT(_translator);
        QString resource(":/");
        resource += language.toLower();
        if (_translator->load(resource) == false) {
            QString error = QObject::tr("Can't load '%1' language translator!").arg(resource);
            qWarning() << error;
            QMessageBox::warning(0, QObject::tr("Error of loading language"), error, QMessageBox::Ok);
            return;
        }
        _currentLanguage = language;
        qApp->installTranslator(_translator);
    }
    else {
        _currentLanguage = defaultLanguage;
    }
}

QString LanguageSwitcher::currentLanguage()
{
    return _currentLanguage;
}
