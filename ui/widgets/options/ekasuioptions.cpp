#include "ekasuioptions.h"
#include "ui_ekasuioptions.h"
#include "core.h"
#include "settings.h"
#include "debug.h"
#include "virtualkeyboards.h"
#include "ekasui/EK_ASUIdata.h"
#include "appdatapath.h"

void EKASUIOptions::init()
{
    EK_ASUI* ekasui = Core::instance().getEkasui();
    QString rr = restoreEKASUIRailroad();
    QString pred = restoreEKASUIPRED();
    ui->railroadComboBox->addItems(ekasui->GetRRs());

    if (rr.size() == 0) {
        const QStringList& rrs = ekasui->GetRRs();
        if (rrs.size() > 0) {
            rr = rrs.first();
        }
        else {
            rr = "None";
        }
    }
    ui->railroadComboBox->setCurrentText(rr);
    ui->predComboBox->addItems(ekasui->GetPREDs_by_RR(rr));
    ui->predComboBox->setCurrentText(pred);
    ui->ekasuiMode->setIndex(restoreEKASUIMode());
    ui->carIdLineEdit->setText(restoreEKASUICarId());
}

EKASUIOptions::EKASUIOptions(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::EKASUIOptions)
{
    _inited = false;
    ui->setupUi(this);

    ui->IntegrateMode->addItem(tr("Off"), DefaultRegistration);
    ui->IntegrateMode->addItem(tr("On"), EKASUIRegistration);
    ui->IntegrateMode->setIndex(restoreRegistrationType());

    ui->ekasuiMode->addItem(tr("CDU"), 0);
    ui->ekasuiMode->addItem(tr("EK ASUI"), 1);
    ui->ekasuiMode->addItem(tr("CDU + EK ASUI"), 2);

    ui->updateProgressBar->hide();

    ASSERT(connect(ui->railroadComboBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &EKASUIOptions::onrr_currentIndexChanged));
    ASSERT(connect(ui->predComboBox, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &EKASUIOptions::onpred_currentIndexChanged));
    ASSERT(connect(ui->ekasuiMode, &SpinBoxList::valueChanged, this, &EKASUIOptions::onEKASUIModeChanged));
}

EKASUIOptions::~EKASUIOptions()
{
    delete ui;
}

void EKASUIOptions::setVisible(bool visible)
{
    if (visible) {
        if (!_inited) {
            init();
        }
    }

    QWidget::setVisible(visible);
}

void EKASUIOptions::setCarId()
{
    const QString& carid = VirtualKeyboards::instance()->value();
    ui->carIdLineEdit->setText(carid);
    saveEKASUICarId(carid);
    EK_ASUI* ekasui = Core::instance().getEkasui();
    ekasui->SetCarID(carid);
}

void EKASUIOptions::enableSettingsEditing(bool enable)
{
    ui->carIdLineEdit->setEnabled(enable);
    ui->carIdPushButton->setEnabled(enable);
}

void EKASUIOptions::onrr_currentIndexChanged(const QString& arg1)
{
    saveEKASUIRailroad(arg1);
    EK_ASUI* ekasui = Core::instance().getEkasui();
    ui->predComboBox->clear();
    ui->predComboBox->addItems(ekasui->GetPREDs_by_RR(arg1));
    ui->predComboBox->setCurrentIndex(0);
    qDebug() << "Railroad changed!" << restoreEKASUIRailroad();
    emit ekasuiChanged();
}

void EKASUIOptions::onpred_currentIndexChanged(const QString& arg1)
{
    saveEKASUIPRED(arg1);
    qDebug() << "Pred changed!" << restoreEKASUIPRED();
    emit ekasuiChanged();
}

void EKASUIOptions::on_updateDBButton_released()
{
    ui->updateProgressBar->setValue(0);
    ui->updateProgressBar->show();
    QString flashPath = getFlashPath() + "/";
    qDebug() << "Flash path:" << flashPath;
    if (flashPath.size() != 0) {
        QDir flash(flashPath + "ekasui");
        QFileInfoList list = flash.entryInfoList();
        int size = list.size();
        for (int i = 0; i < size; ++i) {
            ui->updateProgressBar->setValue(i * (100 / (list.size() - 1)));
            QFileInfo fileInfo = list.at(i);
            qDebug() << "Copying file:" << fileInfo.filePath() << "to" << ekasuiPath() + fileInfo.fileName();
            QFile(ekasuiPath() + fileInfo.fileName()).remove();
            QFile::copy(fileInfo.filePath(), ekasuiPath() + fileInfo.fileName());
            qApp->processEvents();
        }
        ui->updateProgressBar->setValue(100);
    }
    qDebug() << "Update done!";
    ui->updateProgressBar->hide();
}

void EKASUIOptions::onEKASUIModeChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    saveEKASUIMode(userData.toInt());
}

QString EKASUIOptions::getFlashPath()
{
    QProcess process;
    process.start("sh");
    process.waitForStarted();
    process.write("df | grep sda | tr -s \" \" \" \" | cut -d \" \" -f1");
    process.closeWriteChannel();
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput();
    process.close();
    if (!output.isEmpty()) {
        QProcess process3;
        process3.start("sh");
        process3.waitForStarted();
        process3.write("fdisk -l | grep sda");
        process3.closeWriteChannel();
        process3.waitForFinished();
        QByteArray output3 = process3.readAllStandardOutput();
        process3.close();
        output3 = output3.remove(output3.size() - 1, 1);
        if (Q_LIKELY(static_cast<QString>(output3).contains("FAT") == true)) {
            QProcess process2;
            process2.start("sh");
            process2.waitForStarted();
            process2.write("df | grep sda | tr -s \" \" \" \" | cut -d \" \" -f6");
            process2.closeWriteChannel();
            process2.waitForFinished();
            QByteArray output2 = process2.readAllStandardOutput();
            process2.close();
            output2 = output2.remove(output2.size() - 1, 1);
            qDebug() << "mount" << output2;
            ui->updateProgressBar->setValue(20);
            return output2;
        }
    }
    return "";
}

void EKASUIOptions::on_IntegrateMode_valueChanged()
{
    saveRegistrationType(ui->IntegrateMode->userData().toInt());
}

void EKASUIOptions::on_carIdPushButton_released()
{
    VirtualKeyboards* virtualKeyboards = VirtualKeyboards::instance();
    virtualKeyboards->setCallback(this, &EKASUIOptions::setCarId);
    virtualKeyboards->setInputWidgetType(VirtualKeyboards::LineEdit);
    virtualKeyboards->setValidator(VirtualKeyboards::None);
    virtualKeyboards->setValue(ui->carIdLineEdit->text());
    virtualKeyboards->show();
}
