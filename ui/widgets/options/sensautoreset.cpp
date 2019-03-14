#include "sensautoreset.h"
#include "ui_sensautoreset.h"
#include "settings.h"
#include "debug.h"
#include "core.h"

SensAutoReset::SensAutoReset(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SensAutoReset)
{
    ui->setupUi(this);
    ui->enableSpinBox->addItem(tr("On"), true);
    ui->enableSpinBox->addItem(tr("Off"), false);
    ui->enableSpinBox->setIndex(restoreAutoSensResetEnabled() ? 0 : 1);
    ASSERT(connect(ui->enableSpinBox, &SpinBoxList::valueChanged, this, &SensAutoReset::onAutoSensResetEnabledChanged));

    ui->delaySpinBox->setMinimum(0);
    ui->delaySpinBox->setMaximum(120);
    ui->delaySpinBox->setValue(restoreAutoSensResetDelay());
    ui->delaySpinBox->setSuffix(tr(" sec."));
    ASSERT(connect(ui->delaySpinBox, &SpinBoxNumber::valueChanged, this, &SensAutoReset::onAutoSensResetDelayChanged));
}

SensAutoReset::~SensAutoReset()
{
    delete ui;
}

bool SensAutoReset::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateUi();
    }
    return QWidget::event(e);
}

void SensAutoReset::retranslateUi()
{
    ui->enableSpinBox->clear();
    ui->enableSpinBox->addItem(tr("On"), true);
    ui->enableSpinBox->addItem(tr("Off"), false);
    ui->delaySpinBox->setSuffix(tr(" sec."));
}

void SensAutoReset::onAutoSensResetEnabledChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(value);
    Q_UNUSED(userData);
    saveAutoSensResetEnabled(index == 0);
    Core::instance().SetFixBadSensState(index == 0);
}

void SensAutoReset::onAutoSensResetDelayChanged(qreal value)
{
    saveAutoSensResetDelay(value);
    Core::instance().SetTimeToFixBadSens(qRound(value));
}
