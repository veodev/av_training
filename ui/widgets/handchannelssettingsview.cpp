#include <QAbstractItemModel>

#include "handchannelssettingsview.h"
#include "ui_handchannelssettingsview.h"
#include "handchannelssettingsitem.h"
#include "roles.h"
#include "debug.h"

HandChannelsSettingsView::HandChannelsSettingsView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HandChannelsSettingsView)
    , _model(nullptr)
{
    ui->setupUi(this);
    ui->itemsLayout->setAlignment(Qt::AlignTop);
}

HandChannelsSettingsView::~HandChannelsSettingsView()
{
    delete ui;
}

void HandChannelsSettingsView::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model);

    deleteItems();

    if (_model != nullptr) {
        _model->disconnect(this);
    }
    _model = model;
    ASSERT(connect(_model, &QAbstractItemModel::dataChanged, this, &HandChannelsSettingsView::onDataChanged));

    int modelSize = _model->rowCount();
    for (int i = 0; i < modelSize; ++i) {
        const QModelIndex& index = _model->index(i, 0);
        auto* item = new HandChannelsSettingsItem(ui->itemsWidget);
        item->setObjectName(QString::number(i));
        setChannelsParams(item, index);
        item->setItemBackgroundColor(i);
        ui->itemsLayout->addWidget(item);
    }
}

void HandChannelsSettingsView::deleteItems()
{
    QLayoutItem* layoutItem;
    while ((layoutItem = ui->itemsLayout->takeAt(0)) != 0) {
        layoutItem->widget()->deleteLater();
    }
}

void HandChannelsSettingsView::setChannelsParams(HandChannelsSettingsItem* item, const QModelIndex& index)
{
    item->setAngle(_model->data(index, AngleRole).toInt());
    item->setMethod(static_cast<InspectMethod>(_model->data(index, MethodRole).toInt()));
    item->setTimeDelay(_model->data(index, TimeDelayRole).toReal());
    item->setSensAndRecommendedSens(_model->data(index, SensRole).toInt(), _model->data(index, RecommendedSensRole).toInt());
    item->setGain(_model->data(index, GainRole).toInt());
    item->setTvg(_model->data(index, TvgRole).toInt());
    item->setGate(_model->data(index, StartGateRole).toInt(), _model->data(index, EndGateRole).toInt());

    switch (static_cast<ChannelFrequency>(_model->data(index, FrequencyChannelRole).toInt())) {
    case f2_5MHz:
        item->setFrequency(2.5);
        break;
    case f5MHz:
        item->setFrequency(5);
        break;
    }
}

bool HandChannelsSettingsView::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        deleteItems();
        ui->retranslateUi(this);
        setModel(_model);
    }
    return QWidget::event(e);
}

void HandChannelsSettingsView::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
        const QModelIndex& index = _model->index(row, 0);
        auto* tmpItem = ui->itemsWidget->findChild<HandChannelsSettingsItem*>(QString::number(row));
        if (index.isValid()) {
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
