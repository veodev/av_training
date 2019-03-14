#include "pathencoderemulatoroptions.h"
#include "ui_pathencoderemulatoroptions.h"
#include "core.h"
#include "debug.h"

PathEncoderEmulatorOptions::PathEncoderEmulatorOptions(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PathEncoderEmulatorOptions)
{
    ui->setupUi(this);
    ui->emulationSpinBox->addItem(tr("Off"), false);
    ui->emulationSpinBox->addItem(tr("On"), true);
    ui->emulationSpinBox->setIndex(0);
    ASSERT(connect(ui->emulationSpinBox, &SpinBoxList::valueChanged, this, &PathEncoderEmulatorOptions::onEnabledChanged));
}

PathEncoderEmulatorOptions::~PathEncoderEmulatorOptions()
{
    delete ui;
}

void PathEncoderEmulatorOptions::onEnabledChanged(int index, const QString& value, const QVariant& userData)
{
    bool state = userData.toBool();
    Core::instance().usePathEncoderEmulator(state);
}
