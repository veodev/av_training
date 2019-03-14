#include "boltjointoptions.h"
#include "ui_boltjointoptions.h"
#include "settings.h"
#include "debug.h"

BoltJointOptions::BoltJointOptions(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BoltJointOptions)
{
    ui->setupUi(this);
    int additiveForSens = restoreAdditiveForSens();
    ui->additiveSensSpinBox->setMinimum(0);
    ui->additiveSensSpinBox->setMaximum(6);
    ui->additiveSensSpinBox->setStepBy(2);
    ui->additiveSensSpinBox->setValue(additiveForSens);
    ASSERT(connect(ui->additiveSensSpinBox, &SpinBoxNumber::valueChanged, this, &BoltJointOptions::onAdditiveSensChanged));

    bool increaseTracks = restoreIncreaseTracks();
    ui->increaseTracksSpinBox->addItem(tr("Off"), false);
    ui->increaseTracksSpinBox->addItem(tr("On"), true);
    ui->increaseTracksSpinBox->setIndex(increaseTracks);
    ASSERT(connect(ui->increaseTracksSpinBox, &SpinBoxList::valueChanged, this, &BoltJointOptions::onIncreaseTracks));
}

BoltJointOptions::~BoltJointOptions()
{
    delete ui;
}

void BoltJointOptions::retranslateUi()
{
    ui->increaseTracksSpinBox->clear();
    ui->increaseTracksSpinBox->addItem(tr("Off"), false);
    ui->increaseTracksSpinBox->addItem(tr("On"), true);
    ui->increaseTracksSpinBox->setIndex(restoreIncreaseTracks());
}

bool BoltJointOptions::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateUi();
    }
    return QWidget::event(e);
}

void BoltJointOptions::onAdditiveSensChanged(qreal value)
{
    qDebug() << "BJ Additive:" << value;
    saveAdditiveForSens(static_cast<int>(value));
    emit additiveSensChanged(static_cast<int>(value));
}

void BoltJointOptions::onIncreaseTracks(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    qDebug() << "BJ increase tracks:" << userData.toBool();

    saveIncreaseTracks(userData.toBool());
    emit increaseTracks(userData.toBool());
}
