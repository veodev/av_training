#include <QAbstractItemModel>
#include <QDebug>

#include "Definitions.h"
#include "tablesettingsview.h"
#include "ui_tablesettingsview.h"

#include "tablesettingsitem.h"
#include "modeltypes.h"
#include "roles.h"
#include "debug.h"
#include "Definitions.h"
#include "coredefinitions.h"

TableSettingsView::TableSettingsView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TableSettingsView)
    , _model(nullptr)
    , _controledRail(None)
{
    ui->setupUi(this);
    ui->headerSingleRail->hide();
    ui->headerBothRail->hide();
    ui->itemsVerticalLayout->setAlignment(Qt::AlignTop);
}

TableSettingsView::~TableSettingsView()
{
    delete ui;
}

void TableSettingsView::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model);

    deleteItems();

    if (_model != 0) {
        _model->disconnect(this);
    }
    _model = model;
    ASSERT(connect(_model, &QAbstractItemModel::dataChanged, this, &TableSettingsView::onDataChanged));

    (_controledRail == Both) ? ui->headerBothRail->show() : ui->headerSingleRail->show();

    int itemIndexOfTableSettings = 0;
    if (_controledRail == Both) {
        for (int i = 0; i < _model->rowCount(); ++i) {
            QModelIndex iIndex = _model->index(i, 0);
            if (iIndex.isValid() == false) {
                continue;
            }
            DeviceSide iSide = static_cast<DeviceSide>(_model->data(iIndex, SideRole).toInt());
            QString iChannelName = _model->data(iIndex, ChannelTitleRole).toString();
            eControlZone iZone = static_cast<eControlZone>(_model->data(iIndex, ZoneRole).toInt());
            int iMethod = _model->data(iIndex, MethodRole).toInt();

            DeviceSide jSide = LeftDeviceSide;
            QString jChannelName = 0;
            eControlZone jZone = czAll;
            int jMethod = 0;

            for (int j = i + 1; j < _model->rowCount(); ++j) {
                QModelIndex jIndex = _model->index(j, 0);
                if (jIndex.isValid() == false) {
                    continue;
                }
                jSide = static_cast<DeviceSide>(_model->data(jIndex, SideRole).toInt());
                jChannelName = _model->data(jIndex, ChannelTitleRole).toString();
                jZone = static_cast<eControlZone>(_model->data(jIndex, ZoneRole).toInt());
                jMethod = _model->data(jIndex, MethodRole).toInt();

                if (jChannelName == iChannelName && jZone == iZone && jMethod == iMethod && jSide != iSide) {
                    TableSettingsItem* item = new TableSettingsItem(_controledRail, ui->viewWidget);
                    item->setObjectName(QString::number(itemIndexOfTableSettings));
                    _channelsMap.insert(i, itemIndexOfTableSettings);
                    _channelsMap.insert(j, itemIndexOfTableSettings);

                    switch (jSide) {
                    case RightDeviceSide: {
                        setItemRightRailParams(item, jIndex);
                        break;
                    }
                    case LeftDeviceSide: {
                        setItemLeftRailParams(item, jIndex);
                        break;
                    }
                    default:
                        break;
                    }

                    switch (iSide) {
                    case RightDeviceSide: {
                        setItemRightRailParams(item, iIndex);
                        break;
                    }
                    case LeftDeviceSide: {
                        setItemLeftRailParams(item, iIndex);
                        break;
                    }
                    default:
                        break;
                    }
                    item->setItemBackgroundColor(itemIndexOfTableSettings);
                    ui->itemsVerticalLayout->addWidget(item);
                    itemIndexOfTableSettings++;
                }
                continue;
            }
        }
    }
    else {
        for (int i = 0; i < _model->rowCount(); ++i) {
            QModelIndex index = _model->index(i, 0);
            TableSettingsItem* item = new TableSettingsItem(_controledRail, ui->viewWidget);
            item->setObjectName(QString::number(itemIndexOfTableSettings));
            _channelsMap.insert(i, itemIndexOfTableSettings);
            setItemRailParams(item, index);
            item->setItemBackgroundColor(itemIndexOfTableSettings);
            ui->itemsVerticalLayout->addWidget(item);
            itemIndexOfTableSettings++;
        }
    }
}

void TableSettingsView::deleteItems()
{
    QLayoutItem* layoutItem;
    while ((layoutItem = ui->itemsVerticalLayout->takeAt(0)) != 0) {
        layoutItem->widget()->deleteLater();
    }
}

void TableSettingsView::setItemLeftRailParams(TableSettingsItem* item, QModelIndex& index)
{
    correctChannelName(item, index);
    item->setLeftGain(_model->data(index, GainRole).toInt());
    item->setLeftSensAndRecommendedSens(_model->data(index, SensRole).toInt(), _model->data(index, RecommendedSensRole).toInt());
    item->setLeftGate(_model->data(index, StartGateRole).toInt(), _model->data(index, EndGateRole).toInt());
    item->setLeftTimeDelay(_model->data(index, TimeDelayRole).toReal());
    item->setLeftTvg(_model->data(index, TvgRole).toInt());
}

void TableSettingsView::setItemRightRailParams(TableSettingsItem* item, QModelIndex& index)
{
    correctChannelName(item, index);
    item->setRightGain(_model->data(index, GainRole).toInt());
    item->setRightSensAndRecommendedSens(_model->data(index, SensRole).toInt(), _model->data(index, RecommendedSensRole).toInt());
    item->setRightGate(_model->data(index, StartGateRole).toInt(), _model->data(index, EndGateRole).toInt());
    item->setRightTimeDelay(_model->data(index, TimeDelayRole).toReal());
    item->setRightTvg(_model->data(index, TvgRole).toInt());
}

void TableSettingsView::setItemRailParams(TableSettingsItem* item, QModelIndex& index)
{
    correctChannelName(item, index, false);
    item->setGain(_model->data(index, GainRole).toInt());
    item->setSensAndRecommendedSens(_model->data(index, SensRole).toInt(), _model->data(index, RecommendedSensRole).toInt());
    item->setGate(_model->data(index, StartGateRole).toInt(), _model->data(index, EndGateRole).toInt());
    item->setTimeDelay(_model->data(index, TimeDelayRole).toReal());
    item->setTvg(_model->data(index, TvgRole).toInt());
}

void TableSettingsView::correctChannelName(TableSettingsItem* item, QModelIndex& index, bool isBoth)
{
    int currentAngle = _model->data(index, AngleRole).toInt();
    eControlZone currentZone = static_cast<eControlZone>(_model->data(index, ZoneRole).toInt());
    InspectMethod currentMethod = static_cast<InspectMethod>(_model->data(index, MethodRole).toInt());
    QString correctedChannelName = "";
    QString tmpMethod = "";
    QString tmpZone = "";
    QString tmpDir = "";

    switch (currentMethod) {
    case Echo:
        tmpMethod = QObject::tr("ECHO");
        break;
    case MirrorShadow:
        tmpMethod = QObject::tr("MSM");
        break;
    case Mirror:
        tmpMethod = QObject::tr("M");
        break;
    case Shadow:
        tmpMethod = QObject::tr("S");
        break;
    default:
        break;
    }

    switch (currentZone) {
    case czWeb:
        tmpZone = QObject::tr("W-");
        break;
    case czBase:
        tmpZone = QObject::tr("B-");
        break;
    case czHeadAndWork:
    case czHeadLeftWork:
    case czHeadWork:
        tmpZone = QObject::tr("Gauge-");
        break;
    case czHeadAndUnWork:
    case czHeadLeftUnWork:
    case czHeadUnWork:
        tmpZone = QObject::tr("Field-");
        break;
    default:
        break;
    }

    switch (static_cast<Direction>(_model->data(index, DirectionRole).toInt())) {
    case ForwardDirection:
        tmpDir = QObject::tr("F");
        break;
    case BackwardDirection:
        tmpDir = QObject::tr("B");
        break;
    case UnknownDirection:
        break;
    }

    switch (currentAngle) {
    case 0:
    case 55:
        correctedChannelName = QString::number(currentAngle) + QString(0x00B0) + " " + tmpMethod;
        break;
    case 58:
        if (currentMethod != Echo) {
            currentZone == czHeadBoth ? correctedChannelName = QString::number(currentAngle) + QString(0x00B0) + " " + tmpMethod + QString("-") + tmpDir : correctedChannelName = QString::number(currentAngle) + QString(0x00B0) + " " + tmpMethod + tmpZone + tmpDir;
        }
        else {
            correctedChannelName = QString::number(currentAngle) + QString(0x00B0) + " " + tmpZone + tmpDir;
        }
        break;
    default:
        correctedChannelName = QString::number(currentAngle) + QString(0x00B0) + " " + tmpZone + tmpDir;
        break;
    }

    (isBoth == true) ? item->setChannelName(correctedChannelName) : item->setChannelNameSingle(correctedChannelName);
}

void TableSettingsView::setContoledRail(ControledRail value)
{
    _controledRail = value;
}

bool TableSettingsView::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        deleteItems();
        ui->retranslateUi(this);
        setModel(_model);
    }
    return QWidget::event(e);
}

void TableSettingsView::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    TableSettingsItem* tmpItem = 0;

    for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
        QModelIndex index = _model->index(row, 0);
        if (_channelsMap.contains(row)) {
            QMap<int, int>::iterator it = _channelsMap.find(row);
            tmpItem = ui->viewWidget->findChild<TableSettingsItem*>(QString::number(it.value()));
        }

        if (tmpItem == 0) {
            continue;
        }

        if (_model->data(index, SideRole) == LeftDeviceSide) {
            for (int role : roles) {
                switch (role) {
                case TimeDelayRole:
                    tmpItem->setLeftTimeDelay(_model->data(index, TimeDelayRole).toReal());
                    break;
                case SensRole:
                    tmpItem->setLeftSensAndRecommendedSens(_model->data(index, SensRole).toInt(), _model->data(index, RecommendedSensRole).toInt());
                    break;
                case GainRole:
                    tmpItem->setLeftGain(_model->data(index, GainRole).toInt());
                    break;
                case TvgRole:
                    tmpItem->setLeftTvg(_model->data(index, TvgRole).toInt());
                    break;
                case StartGateRole:
                    tmpItem->setLeftGate(_model->data(index, StartGateRole).toInt(), _model->data(index, EndGateRole).toInt());
                    break;
                case EndGateRole:
                    tmpItem->setLeftGate(_model->data(index, StartGateRole).toInt(), _model->data(index, EndGateRole).toInt());
                    break;
                default:
                    break;
                }
            }
        }
        else if (_model->data(index, SideRole) == RightDeviceSide) {
            for (int role : roles) {
                switch (role) {
                case TimeDelayRole:
                    tmpItem->setRightTimeDelay(_model->data(index, TimeDelayRole).toReal());
                    break;
                case SensRole:
                    tmpItem->setRightSensAndRecommendedSens(_model->data(index, SensRole).toInt(), _model->data(index, RecommendedSensRole).toInt());
                    break;
                case GainRole:
                    tmpItem->setRightGain(_model->data(index, GainRole).toInt());
                    break;
                case TvgRole:
                    tmpItem->setRightTvg(_model->data(index, TvgRole).toInt());
                    break;
                case StartGateRole:
                    tmpItem->setRightGate(_model->data(index, StartGateRole).toInt(), _model->data(index, EndGateRole).toInt());
                    break;
                case EndGateRole:
                    tmpItem->setRightGate(_model->data(index, StartGateRole).toInt(), _model->data(index, EndGateRole).toInt());
                    break;
                default:
                    break;
                }
            }
        }
        else if (_model->data(index, SideRole) == NoneDeviceSide) {
            for (int role : roles) {
                switch (role) {
                case TimeDelayRole:
                    tmpItem->setTimeDelay(_model->data(index, TimeDelayRole).toReal());
                    break;
                case SensRole:
                    tmpItem->setSensAndRecommendedSens(_model->data(index, SensRole).toInt(), _model->data(index, RecommendedSensRole).toInt());
                    break;
                case GainRole:
                    tmpItem->setGain(_model->data(index, GainRole).toInt());
                    break;
                case TvgRole:
                    tmpItem->setTvg(_model->data(index, TvgRole).toInt());
                    break;
                case StartGateRole:
                    tmpItem->setGate(_model->data(index, StartGateRole).toInt(), _model->data(index, EndGateRole).toInt());
                    break;
                case EndGateRole:
                    tmpItem->setGate(_model->data(index, StartGateRole).toInt(), _model->data(index, EndGateRole).toInt());
                    break;
                default:
                    break;
                }
            }
        }
    }
}
