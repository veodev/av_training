#ifndef LANGUAGESWITCHER_H
#define LANGUAGESWITCHER_H

#include <QMap>

// key:"Russian", value:"Русский"
typedef QMap<QString, QString> Languages;

class LanguageSwitcher
{
public:
    static Languages supportedLanguages();
    static void switchLanguage(const QString & language);
    static QString currentLanguage();
};

#endif // LANGUAGESWITCHER_H
