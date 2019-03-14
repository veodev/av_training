#include <QtEndian>
#include <QLabel>
#include <QFile>

#include "report.h"
#include "ui_report.h"
#include "debug.h"
#include "dc_definitions.h"

Report::Report(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Report)
    , _scanStep(0)
    , _direction(0)
    , _forwardSystemCoordinate(0)
    , _startSystemCoordinate(0)
    , _currentSystemCoordinate(0)
    , _startDate(QDate())
    , _startTime(QTime())
    , _currentTime(QTime())
    , _startOverSpeedSystemCoordinate(0)
    , _stopOverSpeedSystemCoordinate(0)
    , _maxSpeed(0)
    , _overSpeedDistance(0)
    , _countBoltJoints(0)
    , _countOfGoodBoltJoints(0)
    , _lineName(QString(""))
    , _firstOperator(QString(""))
    , _secondOperator(QString(""))
    , _thirdOperator(QString(""))
    , _countChannelsWithWrongSens(0)
    , _distanceWithWrongSens(0)
    , _startWrongSensSystemCoordinate(0)
    , _stopWrongSensSystemCoordinate(0)
    , _isBadKy(false)
    , _isBackWard(false)
    , _acousticContactState(true)
    , _isZoneForwardStart(false)
    , _isZoneBackwardStart(false)
    , _fullDistanceWithoutBackZone(0)
    , _fullDistanceWithBackZone(0)
    , _isAcousticContactReg(false)
    , _isNeedRecalibrate(false)
{
    ui->setupUi(this);
    ui->secondColumnLine->hide();
    ui->thirdColumnLine->hide();
    ui->firstOperatorWidget->hide();
    ui->secondOperatorWidget->hide();
    ui->thirdOperatorWidget->hide();
}

Report::~Report()
{
    delete ui;
}

void Report::makeReport(const QString& fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly) == false) {
        qDebug() << "Cannot open file for report!";
        return;
    }

    clearReportWidget();
    resetVariables();
    QByteArray data;

    //Чтение направления движения
    file.seek(18);
    data.clear();
    data = file.read(4);
    _direction = qFromLittleEndian<int>(reinterpret_cast<const uchar*>(data.data()));

    //Чтение шага датчика пути
    data.clear();
    quint16 scanStep = 0;
    file.seek(22);
    data = file.read(2);
    scanStep = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
    _scanStep = scanStep / 100.0;

    //Чтение состояния контроля акустического контакта
    file.seek(51);
    uchar stateOfAcousticContactSystem = '\0';
    file.getChar(reinterpret_cast<char*>(&stateOfAcousticContactSystem));
    _isAcousticContactReg = stateOfAcousticContactSystem;

    //Чтение названия перегона
    data.clear();
    file.seek(2970);
    data = file.read(130);
    _lineName = QString::fromUtf16(reinterpret_cast<const ushort*>(data.constData()));

    //Чтение первого оператора
    data.clear();
    file.seek(3100);
    data = file.read(130);
    _firstOperator = QString::fromUtf16(reinterpret_cast<const ushort*>(data.constData()));

    //Чтение даты
    data.clear();
    file.seek(3236);
    quint16 year = 0;
    quint16 month = 0;
    quint16 day = 0;
    data = file.read(2);
    year = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
    data.clear();
    data = file.read(2);
    month = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
    data.clear();
    data = file.read(2);
    day = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
    _startDate.setDate(year, month, day);

    //Чтение часов и минут
    data.clear();
    quint16 hours = 0;
    quint16 minutes = 0;
    file.seek(3242);
    data = file.read(2);
    hours = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
    data.clear();
    file.seek(3244);
    data = file.read(2);
    minutes = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
    _startTime.setHMS(hours, minutes, 0);

    //Чтение КМ, ПК, М
    data.clear();
    data = file.read(4);
    _startCoordinateMarker.km = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
    data.clear();
    data = file.read(4);
    _startCoordinateMarker.pk = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
    data.clear();
    data = file.read(4);
    _startCoordinateMarker.m = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
    _currentCoordinateMarker = _startCoordinateMarker;

    file.seek(5310);  //пропускаем заголовок файла

    while (file.atEnd() == false) {
        unsigned char id = '\0';
        file.getChar(reinterpret_cast<char*>(&id));
        if ((id >> 7) == 0) {  // B-Scan
            int countOfSignals = (id & 3) + 1;
            switch (countOfSignals) {
            case 1:
                file.seek(file.pos() + 2);
                break;
            case 2:
                file.seek(file.pos() + 3);
                break;
            case 3:
                file.seek(file.pos() + 5);
                break;
            case 4:
                file.seek(file.pos() + 6);
                break;
            default:
                break;
            }
        }
        else if (id == EID_HandScan) {
            quint32 offset = file.pos();
            data.clear();
            data = file.read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
            file.seek(offset + 11 + dataSize);
        }
        else if (id == EID_Ku || id == EID_Att || id == EID_TVG || id == EID_StStr || id == EID_EndStr || id == EID_HeadPh) {
            file.seek(file.pos() + 3);
        }
        else if (id == EID_Mode) {
            file.seek(file.pos() + 7);
        }
        else if (id == EID_SetRailType) {
        }
        else if (id == EID_PrismDelay) {
            file.seek(file.pos() + 4);
        }
        else if (id == EID_Stolb) {
            file.seek(file.pos() + 4);
            data.clear();
            data = file.read(4);
            quint32 kmRight = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
            file.seek(file.pos() + 4);
            data.clear();
            data = file.read(4);
            quint32 pkRight = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
            _currentCoordinateMarker.km = kmRight;
            _currentCoordinateMarker.pk = pkRight;
            _currentCoordinateMarker.systemCoordinate = _currentSystemCoordinate;
            file.seek(file.pos() + 128);
        }
        else if (id == EID_Switch || id == EID_TextLabel) {
            quint32 offset = file.pos();
            uchar length = '\0';
            file.getChar(reinterpret_cast<char*>(&length));
            file.seek(offset + 1 + (length * 2));
        }
        else if (id == EID_DefLabel) {
            quint32 offset = file.pos();
            uchar length = '\0';
            file.getChar(reinterpret_cast<char*>(&length));
            file.seek(offset + 2 + (length * 2));
        }
        else if (id == EID_StBoltStyk || id == EID_EndBoltStyk || id == EID_StartSwitchShunter || id == EID_EndSwitchShunter) {
        }
        else if (id == EID_Time) {
            uchar hoursByte, minutesByte = '\0';
            file.getChar(reinterpret_cast<char*>(&hoursByte));
            file.getChar(reinterpret_cast<char*>(&minutesByte));
            if ((hoursByte >> 5) == 0) {
                _currentTime.setHMS(hoursByte & 31, minutesByte, 0);
            }
        }
        else if (id == EID_StolbChainage) {
            file.seek(file.pos() + 136);
        }
        else if (id == EID_ZerroProbMode) {
            file.seek(file.pos() + 1);
        }
        else if (id == EID_LongLabel) {
            file.seek(file.pos() + 24);
        }
        else if (id == EID_SpeedState) {
            data.clear();
            data = file.read(4);
            quint32 speed = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
            _vectorOfSpeedValues.append(speed);
            uchar speedState = '\0';
            file.getChar(reinterpret_cast<char*>(&speedState));
            if (_isBackWard == false) {
                if (speedState == 0) {  // normal speed
                    if (_startOverSpeedSystemCoordinate != 0 && _stopOverSpeedSystemCoordinate == 0) {
                        _stopOverSpeedSystemCoordinate = _currentSystemCoordinate;
                        _overSpeedDistance += _stopOverSpeedSystemCoordinate - _startOverSpeedSystemCoordinate;
                        _startOverSpeedSystemCoordinate = 0;
                        _stopOverSpeedSystemCoordinate = 0;
                    }
                }
                if (speedState == 1 && _startOverSpeedSystemCoordinate == 0) {  // over speed
                    _startOverSpeedSystemCoordinate = _currentSystemCoordinate;
                    _stopOverSpeedSystemCoordinate = 0;
                }
            }
            if (_maxSpeed < (speed / 10.0)) {
                _maxSpeed = speed / 10.0;
            }
        }
        else if (id == EID_ChangeOperatorName) {
            uchar strSize = '\0';
            file.getChar(reinterpret_cast<char*>(&strSize));
            data.clear();
            data = file.read(128);
            if (_secondOperator.isEmpty()) {
                _secondOperator = QString::fromUtf16(reinterpret_cast<const ushort*>(data.constData()));
            }
            else {
                if (_thirdOperator.isEmpty()) {
                    _thirdOperator = QString::fromUtf16(reinterpret_cast<const ushort*>(data.constData()));
                    if (_thirdOperator == _firstOperator) {
                        _thirdOperator = "";
                    }
                }
            }
        }
        else if (id == EID_AutomaticSearchRes) {
            file.seek(file.pos() + 9);
        }
        else if (id == EID_TestRecordFile) {
            quint32 offset = file.pos();
            data.clear();
            data = file.read(4);
            quint32 fileSize = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
            file.seek(offset + 4 + fileSize);
        }
        else if (id == EID_OperatorRemindLabel) {
            quint32 offset = file.pos();
            uchar textLen = '\0';
            file.getChar(reinterpret_cast<char*>(&textLen));
            file.seek(offset + 11 + (textLen * 2));
        }
        else if (id == EID_QualityCalibrationRec) {
            file.seek(file.pos() + 1);
            uchar markerType = '\0';
            file.getChar(reinterpret_cast<char*>(&markerType));
            if (markerType == 1) {
                ++_countBoltJoints;
            }
            if (markerType == 3) {
                ++_countOfGoodBoltJoints;
                ++_countBoltJoints;
            }
            file.seek(file.pos() + 13);
        }
        else if (id == EID_Sensor1) {
            file.seek(file.pos() + 2);
        }
        else if (id == EID_PaintSystemRes) {
            file.seek(file.pos() + 182);
        }
        else if (id == EID_PaintMarkRes || id == EID_SatelliteCoord) {
            file.seek(file.pos() + 8);
        }
        else if (id == EID_PaintSystemTempOff || id == EID_AlarmTempOff) {
            file.seek(file.pos() + 5);
        }
        else if (id == EID_PaintSystemRes_Debug) {
            file.seek(file.pos() + 246);
        }
        else if (id == EID_Temperature) {
            file.seek(file.pos() + 5);
        }
        else if (id == EID_DEBUG) {
            file.seek(file.pos() + 128);
        }
        else if (id == EID_PaintSystemParams) {
            file.seek(file.pos() + 2048);
        }
        else if (id == EID_UMUPaintJob || id == EID_SCReceiverStatus) {
            file.seek(file.pos() + 9);
        }
        else if (id == EID_SmallDate) {
            quint32 offset = file.pos();
            uchar dataSize, dataType = '\0';
            file.getChar(reinterpret_cast<char*>(&dataSize));
            file.getChar(reinterpret_cast<char*>(&dataType));
            _isNeedRecalibrate = dataType;
            file.seek(offset + 2 + dataSize);
        }
        else if (id == EID_RailHeadScaner) {
            quint32 offset = file.pos();
            data.clear();
            data = file.read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
            file.seek(offset + 4 + dataSize);
        }
        else if (id == EID_SensAllowedRanges) {
            quint32 offset = file.pos();
            uchar count = '\0';
            file.getChar(reinterpret_cast<char*>(&count));
            file.seek(offset + 1 + (count * 3));
        }
        else if (id == EID_Media) {
            quint32 offset = file.pos();
            file.seek(file.pos() + 1);
            data.clear();
            data = file.read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
            file.seek(offset + 1 + 4 + dataSize);
        }
        else if (id == EID_SatelliteCoordAndSpeed) {
            file.seek(file.pos() + 12);
        }
        else if (id == EID_NORDCO_Rec) {
            file.seek(file.pos() + 2015);
        }
        else if (id == EID_SensFault) {
            uchar rail, channel = '\0';
            int kU = 0;
            file.getChar(reinterpret_cast<char*>(&rail));
            file.getChar(reinterpret_cast<char*>(&channel));
            int key = keyFromChannelIdAndRail(channel, rail);
            data.clear();
            data = file.read(4);
            kU = qFromLittleEndian<int>(reinterpret_cast<const uchar*>(data.data()));
            if (kU != 0 && _startWrongSensSystemCoordinate == 0) {
                _startWrongSensSystemCoordinate = _currentSystemCoordinate;
                _listOfBadKyChannels.insert(key, 0);
                _isBadKy = true;
            }
            else {
                if (_isBadKy == true) {
                    _listOfBadKyChannels.remove(key);
                    _stopWrongSensSystemCoordinate = _currentSystemCoordinate;
                    _distanceWithWrongSens += _stopWrongSensSystemCoordinate - _startWrongSensSystemCoordinate;
                    _startWrongSensSystemCoordinate = 0;
                    _stopWrongSensSystemCoordinate = 0;
                    _isBadKy = false;
                }
            }
        }
        else if (id == EID_AScanFrame || id == EID_BigDate) {
            quint32 offset = file.pos();
            data.clear();
            data = file.read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
            file.seek(offset + 5 + dataSize);
        }
        else if (id == EID_MediumDate) {
            quint32 offset = file.pos();
            data.clear();
            data = file.read(2);
            quint16 dataSize = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
            file.seek(offset + 3 + dataSize);
        }
        else if (id == EID_SysCrd_NS) {
            uchar byte = '\0';
            file.getChar((reinterpret_cast<char*>(&byte)));
            int coordinate = _currentSystemCoordinate;
            coordinate = (coordinate & 0xffffff00) + byte;
            (_currentSystemCoordinate <= coordinate) ? _isBackWard = false : _isBackWard = true;
            if (_isAcousticContactReg == true) {
                checkAcousticContact();
            }
            checkSystemCoordinate(_currentSystemCoordinate, coordinate);
        }
        else if (id == EID_SysCrd_NF) {
            data.clear();
            data = file.read(4);
            int coordinate = qFromLittleEndian<int>(reinterpret_cast<const uchar*>(data.data()));
            (_currentSystemCoordinate <= coordinate) ? _isBackWard = false : _isBackWard = true;
            if (_isAcousticContactReg == true) {
                checkAcousticContact();
            }
            checkSystemCoordinate(_currentSystemCoordinate, coordinate);
        }
        else if (id == EID_ACState) {
            if (_isAcousticContactReg == false) {
                file.seek(file.pos() + 1);
            }
            uchar data, state, rail, channelId = '\0';
            file.getChar(reinterpret_cast<char*>(&data));
            state = data >> 7;
            rail = (data & 64) >> 6;
            channelId = (data & 60) >> 2;
            int key = keyFromChannelIdAndRail(channelId, rail);
            if (state == false) {
                _listOfBadAcChannels.insert(key, 0);
            }
            else {
                _listOfBadAcChannels.remove(key);
            }
            _listOfBadAcChannels.isEmpty() ? _acousticContactState = true : _acousticContactState = false;
        }
        else if (id == EID_CheckSum) {
            file.seek(file.pos() + 4);
        }
        else if (id == EID_EndFile) {
            data.clear();
            data = file.read(13);
            if (data == QByteArray().fill(0xFF, 13)) {
                break;
            }
        }
    }

    file.close();
    if (_startOverSpeedSystemCoordinate != 0 && _stopOverSpeedSystemCoordinate == 0) {
        _stopOverSpeedSystemCoordinate = _currentSystemCoordinate;
        _overSpeedDistance += _stopOverSpeedSystemCoordinate - _startOverSpeedSystemCoordinate;
        _startOverSpeedSystemCoordinate = 0;
        _stopOverSpeedSystemCoordinate = 0;
    }

    QString str = QString("");


    ui->lineName->setText(_lineName + QString("   "));
    if (_startDate.isNull() == false && _startTime.isNull() == false && _currentTime.isNull() == false) {
        ui->time->setText(_startTime.toString("hh:mm") + QString(" - ") + _currentTime.toString("hh:mm"));
        ui->date->setText(_startDate.toString("dd.MM.yyyy") + QString("   "));
    }

    if (_firstOperator.isEmpty() == false) {
        ui->firstOperatorWidget->show();
        ui->firstOperatorName->setText(_firstOperator);
        if (_secondOperator.isEmpty() == false && _thirdOperator.isEmpty() == false) {
            ui->firstOperatorName->setText(_firstOperator + QString(", ") + _secondOperator + QString(", ") + _thirdOperator);
        }
        if (_secondOperator.isEmpty() == false && _thirdOperator.isEmpty() == true) {
            ui->firstOperatorName->setText(_firstOperator + QString(", ") + _secondOperator);
        }
        if (_thirdOperator.isEmpty() == false && _secondOperator.isEmpty() == true) {
            ui->firstOperatorName->setText(_firstOperator + QString(", ") + _thirdOperator);
        }
        ui->firstBeginCoordinate->setText(QString::number(_startCoordinateMarker.km) + QString(tr(" Km ")) + QString::number(_startCoordinateMarker.pk) + QString(tr(" Pk ")) + QString::number(_startCoordinateMarker.m) + QString(tr(" m ")));
        calculateRemainingMeters();
        ui->firstEndCoordinate->setText(QString::number(_currentCoordinateMarker.km) + QString(tr(" Km ")) + QString::number(_currentCoordinateMarker.pk) + QString(tr(" Pk ")) + QString::number(_currentCoordinateMarker.m) + QString(tr(" m ")));
        ui->firstDistance->setText(QString::number(calculateDistanceInMeters()) + QString(tr(" m")));
        str.clear();
        travelTime().isNull() ? str = "---" : str = QString(travelTime().toString("hh:mm"));
        ui->firstElapsedTime->setText(str);
        str.clear();
        meanSpeed() == -1 ? str = "---" : str = QString::number(meanSpeed(), 'f', 1) + QString(tr(" km/h"));
        ui->firstMeanSpeed->setText(str);
        str.clear();
        _maxSpeed == 0 ? str = "---" : str = QString::number(_maxSpeed) + QString(tr(" km/h"));
        ui->firstMaxSpeed->setText(str);
        str.clear();
        double percent = percentOfDistanceWithOverSpeed();
        percent == -1 ? str = "---" : str = QString::number(percent, 'f', 1) + QString(" %");
        ui->firstOverSpeed->setText(str);
        str.clear();
        percent = percentOfDistanceWithWrongSens();
        percent == -1 ? str = "---" : str = QString::number(percent, 'f', 1) + QString(" %");
        ui->firstFaultSens->setText(str);
        str.clear();
        percent = percentOfBadAcousticContact();
        percent == -1 ? str = "---" : str = QString::number(percent, 'f', 1) + QString(" %");
        ui->firstBadAc->setText(str);
        str.clear();
        percent = percentOfSatisfactoryBoltJoints();
        percent == -1 ? str = "---" : str = QString::number(percent, 'f', 1) + QString(" %");
        ui->firstGoodBoltJoints->setText(str);
        str.clear();
        _isNeedRecalibrate == false ? str = tr("No") : str = tr("Yes");
        ui->firstNeedRecalibrate->setText(str);
    }
}

void Report::mousePressEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    this->hide();
}

bool Report::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

double Report::meanSpeed()
{
    if (_vectorOfSpeedValues.isEmpty()) {
        return -1;
    }
    else {
        int sum = 0;
        double count = _vectorOfSpeedValues.size();
        foreach (quint32 speed, _vectorOfSpeedValues) {
            sum += speed;
        }
        return sum / (count * 10.0);
    }
}

QTime Report::travelTime()
{
    if (_currentTime.isNull() || _startTime.isNull()) {
        return QTime();
    }
    else {
        int second = _startTime.msecsTo(_currentTime) / 1000;
        int hours = second / 3600;
        int minutes = (second % 3600) / 60;
        return QTime(hours, minutes, 0);
    }
}

double Report::percentOfSatisfactoryBoltJoints()
{
    if (_countBoltJoints == 0) {
        return -1;
    }
    else {
        return (_countOfGoodBoltJoints / static_cast<double>(_countBoltJoints)) * 100.0;
    }
}

double Report::percentOfDistanceWithOverSpeed()
{
    if (_fullDistanceWithBackZone != 0) {
        return (_overSpeedDistance / static_cast<double>(_fullDistanceWithBackZone)) * 100.0;
    }
    else {
        return 0;
    }
}

double Report::percentOfDistanceWithWrongSens()
{
    if (_fullDistanceWithBackZone != 0) {
        return (_distanceWithWrongSens / static_cast<double>(_fullDistanceWithBackZone)) * 100;
    }
    else {
        return 0;
    }
}

double Report::percentOfBadAcousticContact()
{
    if (_fullDistanceWithBackZone == 0 || _isAcousticContactReg == false) {
        return -1;
    }
    return ((_fullDistanceWithBackZone - calculateFullAcousticContactArea()) / static_cast<double>(_fullDistanceWithBackZone)) * 100.0;
}

void Report::calculateRemainingMeters()
{
    if (_currentCoordinateMarker.systemCoordinate <= _currentSystemCoordinate && _direction == 1) {
        _currentCoordinateMarker.m += (((_forwardSystemCoordinate - _currentCoordinateMarker.systemCoordinate) * _scanStep) / 1000);
    }
    else if (_currentCoordinateMarker.systemCoordinate <= _currentSystemCoordinate && _direction == -1) {
        _currentCoordinateMarker.m -= (((_forwardSystemCoordinate - _currentCoordinateMarker.systemCoordinate) * _scanStep) / 1000);
    }
}

int Report::calculateDistanceInMeters()
{
    return ((_forwardSystemCoordinate - _startSystemCoordinate) * _scanStep) / 1000.0;
}

void Report::resetVariables()
{
    ui->secondColumnLine->hide();
    ui->thirdColumnLine->hide();
    ui->firstOperatorWidget->hide();
    ui->secondOperatorWidget->hide();
    ui->thirdOperatorWidget->hide();

    _startCoordinateMarker = CoordinateMarker();
    _currentCoordinateMarker = CoordinateMarker();
    _startSystemCoordinate = 0;
    _currentSystemCoordinate = 0;
    _forwardSystemCoordinate = 0;

    _startDate = QDate();
    _startTime = QTime();
    _currentTime = QTime();

    _vectorOfSpeedValues.clear();
    _overSpeedDistance = 0;

    _countBoltJoints = 0;
    _countOfGoodBoltJoints = 0;

    _startOverSpeedSystemCoordinate = 0;
    _stopOverSpeedSystemCoordinate = 0;
    _maxSpeed = 0;

    _countChannelsWithWrongSens = 0;
    _startWrongSensSystemCoordinate = 0;
    _stopWrongSensSystemCoordinate = 0;
    _distanceWithWrongSens = 0;

    _direction = 0;
    _scanStep = 0;

    _firstOperator = QString("");
    _secondOperator = QString("");
    _thirdOperator = QString("");

    _isBadKy = false;
    _isBackWard = false;
    _acousticContactState = true;
    _isZoneForwardStart = false;
    _isZoneBackwardStart = false;

    _vectorAcSections.clear();
    _listOfBadAcChannels.clear();

    _listOfBadKyChannels.clear();

    _fullDistanceWithBackZone = 0;
    _fullDistanceWithoutBackZone = 0;

    _isAcousticContactReg = false;
    _isNeedRecalibrate = false;
}

void Report::clearReportWidget()
{
    ui->lineName->setText("");
    ui->date->setText("");
    ui->time->setText("");

    ui->firstOperatorName->setText("");
    ui->firstBeginCoordinate->setText("");
    ui->firstEndCoordinate->setText("");
    ui->firstDistance->setText("");
    ui->firstElapsedTime->setText("");
    ui->firstMeanSpeed->setText("");
    ui->firstMaxSpeed->setText("");
    ui->firstOverSpeed->setText("");
    ui->firstFaultSens->setText("");
    ui->firstBadAc->setText("");
    ui->firstGoodBoltJoints->setText("");
    ui->firstNeedRecalibrate->setText("");

    ui->secondOperatorName->setText("");
    ui->secondBeginCoordinate->setText("");
    ui->secondEndCoordinate->setText("");
    ui->secondDistance->setText("");
    ui->secondElapsedTime->setText("");
    ui->secondMeanSpeed->setText("");
    ui->secondMaxSpeed->setText("");
    ui->secondOverSpeed->setText("");
    ui->secondFaultSens->setText("");
    ui->secondBadAc->setText("");
    ui->secondGoodBoltJoints->setText("");
    ui->secondNeedRecalibrate->setText("");

    ui->thirdOperatorName->setText("");
    ui->thirdBeginCoordinate->setText("");
    ui->thirdEndCoordinate->setText("");
    ui->thirdDistance->setText("");
    ui->thirdElapsedTime->setText("");
    ui->thirdMeanSpeed->setText("");
    ui->thirdMaxSpeed->setText("");
    ui->thirdOverSpeed->setText("");
    ui->thirdFaultSens->setText("");
    ui->thirdBadAc->setText("");
    ui->thirdGoodBoltJoints->setText("");
    ui->thirdNeedRecalibrate->setText("");
}

int Report::keyFromChannelIdAndRail(int channelId, int rail)
{
    return (channelId << 8) | rail;
}

void Report::completeZone()
{
    if (_vectorAcSections.isEmpty()) {
        return;
    }

    int index = _vectorAcSections.count() - 1;
    if (_vectorAcSections[index].isBackWard == false) {
        _vectorAcSections[index].stopSysCoordinate = _currentSystemCoordinate;
    }
    else if (_vectorAcSections[index].isBackWard == true) {
        _vectorAcSections[index].startSysCoordinate = _currentSystemCoordinate;
    }
    _vectorAcSections[index].isComplete = true;
}

void Report::checkAcousticContact()
{
    //если начали двигаться вперед и есть контакт и ни одной зоны
    if (_isBackWard == false && _acousticContactState == true && _vectorAcSections.isEmpty() == true) {
        AcousticContactArea item;
        item.startSysCoordinate = _currentSystemCoordinate;
        item.isBackWard = false;
        item.isComplete = false;
        _vectorAcSections.append(item);
        _isZoneForwardStart = true;
        _isZoneBackwardStart = false;
    }
    //если начали двигаться назад и есть контакт и ни одной зоны
    else if (_isBackWard == true && _acousticContactState == true && _vectorAcSections.isEmpty() == true) {
        AcousticContactArea item;
        item.stopSysCoordinate = _currentSystemCoordinate;
        item.isBackWard = false;
        item.isComplete = false;
        _vectorAcSections.append(item);
        _isZoneForwardStart = false;
        _isZoneBackwardStart = true;
    }
    //если стали двигаться назад, есть контакт и есть открытая зона вперед
    else if (_isBackWard == true && _acousticContactState == true && _isZoneForwardStart == true) {
        completeZone();
        AcousticContactArea item;
        item.stopSysCoordinate = _currentSystemCoordinate;
        item.isBackWard = true;
        item.isComplete = false;
        _vectorAcSections.append(item);
        _isZoneForwardStart = false;
        _isZoneBackwardStart = true;
    }
    //если стали двигаться вперед, есть контакт и есть открытая зона назад
    else if (_isBackWard == false && _acousticContactState == true && _isZoneBackwardStart == true) {
        completeZone();
        AcousticContactArea item;
        item.startSysCoordinate = _currentSystemCoordinate;
        item.isBackWard = false;
        item.isComplete = false;
        _vectorAcSections.append(item);
        _isZoneForwardStart = true;
        _isZoneBackwardStart = false;
    }
    //если двигались вперед, пропал контакт и есть открытая зона вперед
    else if (_isBackWard == false && _acousticContactState == false && _isZoneForwardStart == true) {
        completeZone();
        _isZoneForwardStart = false;
    }
    //если двигались вперед, появился контакт и нет открытой зоны вперед
    else if (_isBackWard == false && _acousticContactState == true && _isZoneForwardStart == false) {
        AcousticContactArea item;
        item.startSysCoordinate = _currentSystemCoordinate;
        item.isBackWard = false;
        item.isComplete = false;
        _vectorAcSections.append(item);
        _isZoneForwardStart = true;
    }
    //если двигались назад, пропал контакт и есть открытая зона назад
    else if (_isBackWard == true && _acousticContactState == false && _isZoneBackwardStart == true) {
        completeZone();
        _isZoneBackwardStart = false;
    }
    //если двигались назад, появился контакт и нет открытой зоны назад
    else if (_isBackWard == true && _acousticContactState == true && _isZoneBackwardStart == false) {
        AcousticContactArea item;
        item.stopSysCoordinate = _currentSystemCoordinate;
        item.isBackWard = true;
        item.isComplete = false;
        _vectorAcSections.append(item);
        _isZoneBackwardStart = true;
    }
}

void Report::checkSystemCoordinate(int prevCoord, int nextCoord)
{
    int diff = 0;
    _currentSystemCoordinate = nextCoord;
    if (_startSystemCoordinate == 0 || _currentSystemCoordinate < _startSystemCoordinate) {
        _startSystemCoordinate = _currentSystemCoordinate;
    }
    if ((_currentSystemCoordinate > _forwardSystemCoordinate) || _forwardSystemCoordinate == 0) {
        _forwardSystemCoordinate = _currentSystemCoordinate;
        diff = abs(_currentSystemCoordinate - _forwardSystemCoordinate);
        if (diff > 1) {
            _fullDistanceWithoutBackZone += diff;
        }
        else {
            ++_fullDistanceWithoutBackZone;
        }
    }
    diff = abs(nextCoord - prevCoord);
    if (diff > 1) {
        _fullDistanceWithBackZone += diff;
    }
    else {
        ++_fullDistanceWithBackZone;
    }
}

int Report::calculateFullAcousticContactArea()
{
    if (_vectorAcSections.size() <= 0) {
        return 0;
    }

    QVector<AcousticContactArea> resultVector;
    resultVector.append(_vectorAcSections[0]);
    for (int i = 1; i < _vectorAcSections.size(); ++i) {
        AcousticContactArea interval = _vectorAcSections[i];
        for (QVector<AcousticContactArea>::iterator iter = resultVector.begin(); iter != resultVector.end();) {
            int start = iter->startSysCoordinate;
            int stop = iter->stopSysCoordinate;
            if (interval.startSysCoordinate < start && interval.stopSysCoordinate > stop) {
                iter = resultVector.erase(iter);
                continue;
            }
            if (interval.startSysCoordinate >= start && interval.startSysCoordinate <= stop) {
                interval.startSysCoordinate = stop;
            }
            if (interval.stopSysCoordinate >= start && interval.stopSysCoordinate <= stop) {
                interval.stopSysCoordinate = start;
            }
            ++iter;
        }
        if (interval.stopSysCoordinate > interval.startSysCoordinate) {
            resultVector.append(interval);
        }
    }
    int rez = 0;
    for (const AcousticContactArea& interval : resultVector) {
        rez += interval.stopSysCoordinate - interval.startSysCoordinate;
    }
    return rez;
}
