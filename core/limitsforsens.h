#ifndef LIMITSFORSENS_H
#define LIMITSFORSENS_H

#include <QtCore/QObject>
#include <QMap>

#include "Definitions.h"

class LimitsForSens : public QObject
{
    Q_OBJECT

public:
    explicit LimitsForSens(QObject* parent = nullptr);
    ~LimitsForSens();

    int sens(quint16 angle, eInspectionMethod method);

public slots:
    void onSetSens(quint16 angle, eInspectionMethod method, int value);

private:
    void restore();

private:
    union KeyForLimitsForSens
    {
        quint16 angle;
        quint16 method;
        quint32 key;
    };
    QMap<quint32, int> _limitsForSens;
};

#endif  // LIMITSFORSENS_H
