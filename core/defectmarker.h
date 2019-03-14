#ifndef DEFECTMARKER
#define DEFECTMARKER
#include <QObject>

#include "Definitions.h"

class Defcore;
class Registration;
class Core;

class DefectMarker : public QObject
{
    Q_OBJECT
public:
    explicit DefectMarker(QObject* parent = nullptr);
    ~DefectMarker();

    void setDefcore(Core* core, Defcore* defcore);
    void enableImportantAreaIndication(bool isEnabled);
    void setMinPacketSize(int value);
    void setMinSignals0dB(int value);
    void setSwitchShunterState(bool state);
    void setMinZTMSpacing(int value);

private slots:
    void onDeviceSignalsPacket(QSharedPointer<tDEV_Packet>);
    void onDeviceSignalSpacing(QSharedPointer<tDEV_SignalSpacing>);
    void onStartBoltJoint();
    void onStopBoltJoint();
    void onDefect53_1(QSharedPointer<tDEV_Defect53_1>);


private:
    Defcore* _defcore;
    Registration* _registration;
    bool _boltJointActive;
    int _minPacketSize;  // min packet size of important area to indicate in mm
    int _minSignals0dB;  // min signals quantity on 0 dB in packet of important area to in
    bool _swichShunterActive;
    int _minZTMSpacing;
};
#endif  // DEFECTMARKER
