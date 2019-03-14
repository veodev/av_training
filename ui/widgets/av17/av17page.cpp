//Раскомментировать в файле batterywidget.cpp  rows 56,57

#include "av17page.h"
#include "av17plot.h"
#include "ui_av17page.h"
#include "Definitions.h"
#include "Device_definitions.h"
#include "DataFileUnit.h"
#include "debug.h"
#include "core.h"
#include "roles.h"
#include "channelscontroller.h"
#include "settings.h"
#include <QPainter>
#include "TickCount.h"

//#define DP_BUG

Av17Page::Av17Page(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Av17Page)
    , XSys(0)
    , YSys(0)
    , XIndex(0)
    , YIndex(0)
    , _xDefOffset(0)
    , _timer(nullptr)
    , _startTime(0)
    , _headScanChannelsModel(nullptr)
{
    ui->setupUi(this);
    ui->A17ProgressWidget->setYindexParentPtr(&YIndex);
    ui->A17ProgressWidget->setYsysParentPtr(&YSys);
    _timer.setInterval(45);
    _timer.setSingleShot(false);
    _timer.setTimerType(Qt::CoarseTimer);
    _timer.stop();
    retranslateUi();
}

Av17Page::~Av17Page()
{
    _timer.stop();
    delete ui;
}

bool Av17Page::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateUi();
    }
    return QWidget::event(e);
}

void Av17Page::retranslateUi()
{
    ui->MinButton->setText(tr("Min."));
    ui->MaxButton->setText(tr("Max."));
    ui->ResetButton->setText(tr("Reset"));
    ui->FinishButton->setText(tr("Finish"));
}

int Av17Page::yindex()
{
    return YIndex;
}

int Av17Page::ysys()
{
    return YSys;
}

void Av17Page::on_ResetButton_released()
{
    Core& core = Core::instance();

    XSys = 0;
    core.SetAv17CoordX(XSys);
    //  XIndex= 0;
    ui->A17ProgressWidget->update();
}

void Av17Page::on_MinButton_released()
{
    Core& core = Core::instance();

    YSys = 0;
    core.SetAv17CoordY(YSys);
    YIndex = 0;
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    av17config.CurrentY = av17config.StartY;
    //  av17config.Save;
    ui->A17ProgressWidget->update();
}

void Av17Page::on_MaxButton_released()
{
    Core& core = Core::instance();
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    A17Data& av17data = HeadScannerData::instance().getData();

    YSys = av17config.ScanHeight - 1;
    YIndex = av17data.Y_Sys_to_Idx(YSys);
    // !!!Oleg YSys= av17config.ScanHeight;
    // !!!Oleg  YIndex= av17data.Y_Sys_to_Idx(YSys);

    //  sprintf(Label_H.Caption,'YSys: %d; YIndex: %d; mm: %3.1f', [YSys, YIndex, Data.Y_Idx_to_mm(YIndex)]);
    core.SetAv17CoordY(YSys);
    av17config.CurrentY = av17config.StartY;
    if (!av17data.Y_Idx_Test(YIndex)) YIndex = av17config.ScanHeight - 1;
    //  av17config.Save;
    ui->A17ProgressWidget->update();
}

void Av17Page::on_FinishButton_released()
{
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    A17Data& av17data = HeadScannerData::instance().getData();
    _timer.stop();
    /* Modyfied AK 08.07.2015 Добавил 6 строчек ниже. Необходимы для задания
    начальных значений конца и начала слоев сразу после проверки.
    До этого при пустой болванки в первый раз брались нули, а во второй раз - значений границ
    предыдущего примера.*/
    av17data.SetLayerBegin(aX, -38);
    av17data.SetLayerEnd(aX, 38);
    av17data.SetLayerBegin(aY, -11);
    av17data.SetLayerEnd(aY, 29);
    av17data.SetLayerBegin(aZ, -138);
    av17data.SetLayerEnd(aZ, 138);

    emit doStartAv17DefView();
}

void Av17Page::StartScan()
{
    Core& core = Core::instance();
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    A17Data& av17data = HeadScannerData::instance().getData();

    restoreHeadScannerParams(av17config.XPathStep, av17config.YPathStep, av17config.StartY, av17config.ScanWidth, av17config.ScanHeight, av17config.CurrentY, av17config.ScannerOrientation, av17config.WorkRail, av17config.threshold);

    av17data.InitScanData(av17config.ScanWidth, std::ceil(av17config.ScanHeight * av17config.YPathStep / 100), av17config.StartY, av17config.XPathStep, av17config.YPathStep, 50, 100);

    XSys = 0;
    XIndex = 0;
    YIndex = av17data.Y_mm_to_Idx(av17config.CurrentY);  //!!!
    YSys = av17data.Y_Idx_to_Sys(YIndex);                //!!!

    core.SetAv17CoordX(0);
    core.SetAv17CoordY(YSys);

    //получить расстояние от колеса телеги до центра дефекта
    av17data.XDefOffset = _xDefOffset;
    emit doLoadBScan0Chan();

    av17data.Head._scanTime = 0;
}

void Av17Page::setVisible(bool visible)
{
    Core& core = Core::instance();
    A17Data& av17data = HeadScannerData::instance().getData();
    if (visible == true) {
        ASSERT(connect(&core, &Core::doBScan2Data, this, &Av17Page::onBScanData));
        disconnect(&core, &Core::doBScan2Data, this, &Av17Page::CountCenterOfDefect);

        ASSERT(connect(&core, &Core::doPathStepData, this, &Av17Page::onPathStepData));
        ASSERT(connect(&_timer, &QTimer::timeout, this, &Av17Page::RedrawOnTimer));

        if (av17data.getNeedInit()) {
            StartScan();
        }
        _startTime = GetTickCount_();
        _timer.start();
        av17data.setNeedInit(false);
    }
    else {
        disconnect(&core, &Core::doBScan2Data, this, &Av17Page::onBScanData);
        disconnect(&core, &Core::doPathStepData, this, &Av17Page::onPathStepData);

        disconnect(&_timer, &QTimer::timeout, this, &Av17Page::RedrawOnTimer);

        _timer.stop();
        av17data.Head._scanTime = GetTickCount_() - _startTime + av17data.Head._scanTime;
    }
    QWidget::setVisible(visible);
}

void Av17Page::AutoTuning(bool isEnable)
{
    emit Core::instance().doEnableSMChSensAutoCalibration(isEnable);
}

void Av17Page::CountCenterOfDefect(QSharedPointer<tDEV_BScan2Head> head)
{
    Q_ASSERT(head.data() != nullptr);
    _xDefOffset = head.data()->XSysCrd[1];
}

void Av17Page::onPathStepData(QSharedPointer<tDEV_PathStepData> encoder)
{
    if (encoder.data()->Dir[2] != 0) {
        A17Data& av17data = HeadScannerData::instance().getData();
        int tmpY = encoder.data()->XSysCrd[2];
        int tmpYIndex = av17data.Y_Sys_to_Idx(tmpY);
        if (av17data.Y_Idx_Test(tmpYIndex)) {
            YSys = tmpY;
            YIndex = tmpYIndex;
            ui->A17ProgressWidget->update();
        }
    }
}

void Av17Page::onBScanData(QSharedPointer<tDEV_BScan2Head> head)
{
    struct TScanDataItem_2* p;

    Q_ASSERT(head.data() != 0);

    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    A17Data& av17data = HeadScannerData::instance().getData();

    //установить координату: XSys
    XSys = head.data()->XSysCrd[1];  // 0
    XIndex = av17data.X_Sys_to_Idx(XSys);

    if (av17data.X_Idx_Test(XIndex) && av17data.Y_Idx_Test(YIndex)) {
        p = &(av17data.FScan[YIndex][XIndex]);
        p->Exist = true;

        for (int probe = BP11_BP23; probe <= BP13_BP21; ++probe) {
            p->MaxAmpl[probe] = 0;

            // get attenuator
            Q_ASSERT(_headScanChannelsModel);
            QModelIndex modelIndex = _headScanChannelsModel->index(probe, 0);
            p->Att[probe] = _headScanChannelsModel->data(modelIndex, GainRole).toInt();
            int stgate = _headScanChannelsModel->data(modelIndex, StartGateRole).toInt();
            int endgate = _headScanChannelsModel->data(modelIndex, EndGateRole).toInt();
            int delayMultiply = _headScanChannelsModel->data(modelIndex, DelayMultiplyRole).toInt();
            Q_ASSERT(delayMultiply != 0);

            int maxa = -1;
            for (const tDEV_BScan2HeadItem& item : head.data()->Items) {
                //выбрать макс. из сигналов В-разв для кажд. канала
                int ich;
                CID Ch = item.Channel;
                switch (Ch) {  // CID
                case 0xB1:
                    ich = BP11_BP23;  // 0xB4
                    break;
                case 0xAF:
                    ich = BP12_BP22;  // 0xB2
                    break;
                case 0xB0:
                    ich = BP13_BP21;  // 0xB6
                    break;
                default:
                    ich = -1;
                }
                if (ich != probe) continue;

                for (const auto& sign : item.Signals) {
                    int delay = sign.Delay;
                    if (delayMultiply != 0) delay /= delayMultiply;
                    int amplitude = sign.Ampl[MaxAmpl];
                    if (delay >= stgate && delay <= endgate)
                        if (maxa < amplitude) {
                            maxa = amplitude;
                        }
                }
            }
            //записать в массив FScan[] на YIndex,XIndex .MaxAmpl[]=  ; .Att[]=; .Exist=
            if (maxa != -1) {
                p->MaxAmpl[probe] = maxa;
            }
        };  // for probe
    };      // if
    ui->A17ProgressWidget->update();
}

void Av17Page::RedrawOnTimer()
{
    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    A17Data& av17data = HeadScannerData::instance().getData();
    _timer.stop();
    if (av17data.X_Idx_AttZone(XIndex)) {  // if в зоне автоподстройки чувствительности
        AutoTuning(true);                  //настроить каналы
    }
    else {
        AutoTuning(false);
    }

    points1.clear();
    points2.clear();
    points3.clear();

    if (av17data.Y_Idx_Test(YIndex)) {
        size_t fscanSize = av17data.FScan[YIndex].size();
        for (size_t I = 0; I < fscanSize; ++I) {  // av17data.Head.ScanWidth
            if (av17data.FScan[YIndex][I].Exist || (I > 0 && !av17data.FScan[YIndex][I - 1].Exist)) {
                float xMM = av17data.X_Idx_to_mm(I);
                points1.push_back(QPointF(xMM, BASELINEMINUS45 + av17data.FScan[YIndex][I].MaxAmpl[BP11_BP23]));
                points2.push_back(QPointF(xMM, BASELINE0 + av17data.FScan[YIndex][I].MaxAmpl[BP12_BP22]));
                points3.push_back(QPointF(xMM, BASELINEPLUS45 + av17data.FScan[YIndex][I].MaxAmpl[BP13_BP21]));
            }
        };
    }

    ui->widget_plot->curveminus45->setSamples(points1);
    ui->widget_plot->curve0->setSamples(points2);
    ui->widget_plot->curveplus45->setSamples(points3);

    float f = av17data.X_Idx_to_mm(XIndex);
    ui->widget_plot->markerX->setXValue(f);

    //отрисовка усиления по каналам
    if (av17data.Y_Idx_Test(YIndex) && av17data.X_Idx_Test(XIndex)) {
        ui->labelplus45->setText(QString("+45° (%1 " + tr("dB") + ")").arg(av17data.FScan[YIndex][XIndex].Att[BP11_BP23]));
        ui->label0->setText(QString("0° (%1 " + tr("dB") + ")").arg(av17data.FScan[YIndex][XIndex].Att[BP12_BP22]));
        ui->labelminus45->setText(QString("-45° (%1 " + tr("dB") + ")").arg(av17data.FScan[YIndex][XIndex].Att[BP13_BP21]));
    }

    // Draw Labels
    av17config.CurrentY = qRound(av17data.Y_Sys_to_mm(YSys));

    f = av17data.X_Sys_to_mm(XSys);
    ui->XLabel->setText(QString("L: %1 " + tr("mm")).arg(std::floor(f)));  //"L: %3.0f мм"

    if (av17data.X_Idx_Test(XIndex)) {
        if (av17data.X_Idx_AttZone(XIndex)) {
            ui->XLabel->setStyleSheet("color: rgb(0, 175, 0)");  // rgb(0, 100, 200)//lime rgb(50, 150, 50) yellow")65280;//Qt::green;rgb(50, 0, 70)
        }
        else {
            ui->XLabel->setStyleSheet("color: black");
        }
    }
    else {
        ui->XLabel->setStyleSheet("color: red");
    }

    if (av17data.Y_Idx_Test(YIndex))
        ui->YLabel->setStyleSheet("color: black");
    else
        ui->YLabel->setStyleSheet("color: red");

    ui->YLabel->setText(QString("H: %1 " + tr("mm")).arg(av17data.Y_Sys_to_mm(YSys)));

    ui->widget_plot->replot();
    _timer.start();
}

void Av17Page::setModel(QAbstractItemModel* headScanChannelsModel)
{
    Q_ASSERT(headScanChannelsModel != nullptr);
    _headScanChannelsModel = headScanChannelsModel;
}

Av17ScanProgress::Av17ScanProgress(QWidget* parent)
{
    Q_UNUSED(parent);
    Border = 2;
    ysysParentPtr = nullptr;
    yindexParentPtr = nullptr;
}
Av17ScanProgress::~Av17ScanProgress() {}
QPoint Av17ScanProgress::MMtoPoint(float mmX, float mmY)
{
    QPoint Res;
    Res.setX(qRound(width() * (mmX - MMRect.left()) / (MMRect.right() - MMRect.left())));
    Res.setY(Border + qRound((height() - Border) * (mmY - MMRect.top()) / (MMRect.bottom() - MMRect.top())));
    return (Res);
}

void Av17ScanProgress::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    TAv17Config& av17config = HeadScannerData::instance().getConfig();
    A17Data& av17data = HeadScannerData::instance().getData();

    QPainter painter(this);
    QRect r = QRect(0, Border, width() - 1, height() - 1 - Border);  // contentsRect();

    MMRect.setLeft(-av17config.ScanWidth);
    MMRect.setRight(av17config.ScanWidth);
    MMRect.setTop(av17config.StartY);
    MMRect.setBottom(av17config.StartY + av17data.Head._scanHeight_mm);

    painter.eraseRect(r);
    painter.setPen(Qt::black);
    painter.drawRect(r);
    for (int Y = 0; Y < av17data.Head._scanHeight; ++Y) {
        QColor color;
        if (Y == yindex()) {
            color = Qt::green;
        }
        else {
            color = Qt::red;
        }

        int Start = -1;
        for (int X = 0; X < av17data.Head._scanWidth; ++X) {
            if (av17data.FScan[Y][X].Exist) {
                Start = X;
                break;
            }
        }

        int Stop = -1;
        for (int X = av17data.Head._scanWidth - 1; X >= 0; --X) {
            if (av17data.FScan[Y][X].Exist) {
                Stop = X;
                break;
            }
        }

        if ((Stop != -1) && (Start != -1)) {
            Pt1 = MMtoPoint(av17data.X_Idx_to_mm(Start), av17data.Y_Idx_to_mm(Y));
            Pt2 = MMtoPoint(av17data.X_Idx_to_mm(Stop), av17data.Y_Idx_to_mm(Y + 1));
            painter.fillRect(Pt1.x(), Pt1.y(), Pt2.x() - Pt1.x(), Pt2.y() - Pt1.y(), color);
        }
    }

    Pt1 = MMtoPoint(MMRect.left(), av17data.Y_Sys_to_mm(ysys()));
    Pt2 = MMtoPoint(MMRect.right(), av17data.Y_Sys_to_mm(ysys() + 1));  // !!!for sys step 1mm
    painter.fillRect(Pt1.x(), (Pt1.y() + Pt2.y()) / 2, Pt2.x() - Pt1.x(), 2, Qt::black);
}

void Av17ScanProgress::setYsysParentPtr(int* value)
{
    ysysParentPtr = value;
}

void Av17ScanProgress::setYindexParentPtr(unsigned int* value)
{
    yindexParentPtr = value;
}

int Av17ScanProgress::ysys()
{
    if (ysysParentPtr != nullptr) {
        return *ysysParentPtr;
    }
    else {
        return 0;
    }
}

int Av17ScanProgress::yindex()
{
    if (yindexParentPtr != nullptr) {
        return static_cast<int>(*yindexParentPtr);
    }
    else {
        return 0;
    }
}
