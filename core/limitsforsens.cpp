#include <QSettings>

#include "limitsforsens.h"

const int DEFAULT_LIMITS_FOR_SENS = 50;

LimitsForSens::LimitsForSens(QObject* parent)
    : QObject(parent)
{
    qRegisterMetaType<eInspectionMethod>("eInspectionMethod");
    restore();
}

LimitsForSens::~LimitsForSens() {}

int LimitsForSens::sens(quint16 angle, eInspectionMethod method)
{
    KeyForLimitsForSens key;
    key.angle = angle;
    key.method = static_cast<quint16>(method);
    if (_limitsForSens.contains(key.key)) {
        return _limitsForSens[key.key];
    }
    return DEFAULT_LIMITS_FOR_SENS;
}

void LimitsForSens::onSetSens(quint16 angle, eInspectionMethod method, int value)
{
    KeyForLimitsForSens key;
    key.angle = angle;
    key.method = static_cast<quint16>(method);
    if (_limitsForSens.contains(key.key) == false) {
        _limitsForSens[key.key] = value;
    }

    QSettings settings;
    settings.beginGroup("limitsForSens");
    settings.setValue(QString("%1,%2").arg(angle).arg(method), value);
    settings.endGroup();
}

void LimitsForSens::restore()
{
    QSettings settings;
    settings.beginGroup("limitsForSens");
    const QStringList& keys = settings.childKeys();
    for (const QString& key : keys) {
        KeyForLimitsForSens k;
        const QStringList& l = key.split(',');
        if (l.count() < 2) {
            settings.remove(key);
            continue;
        }
        k.angle = static_cast<quint16>(l[0].toInt());
        k.method = static_cast<quint16>(l[1].toInt());
        _limitsForSens[k.key] = settings.value(key).toInt();
    }
    settings.endGroup();
}
