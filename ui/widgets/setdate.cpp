#include <QProcess>
#include <QMessageBox>
#include <QDateTime>

#include "setdate.h"
#include "ui_setdate.h"
#include "debug.h"

SetDate::SetDate(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SetDate)
{
    ui->setupUi(this);

    ASSERT(connect(ui->calendarWidget, &QCalendarWidget::currentPageChanged, this, &SetDate::onCurrentPageChanged));
}

SetDate::~SetDate()
{
    delete ui;
}

void SetDate::setVisible(bool visible)
{
    if (visible == true) {
        onCurrentPageChanged(ui->calendarWidget->yearShown(), ui->calendarWidget->monthShown());
    }

    QWidget::setVisible(visible);
}

bool SetDate::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

bool SetDate::runProcess(const QString& program)
{
    QProcess process;

    QDEBUG << "program:" << program;
    process.start(program);
    process.waitForFinished();
    if (process.exitCode()) {
        QByteArray error = process.readAllStandardError();
        qWarning() << error;
        QMessageBox::critical(this, QObject::tr("Error of setting date"), QString(error), QMessageBox::Ok);
        return false;
    }

    return true;
}

void SetDate::on_okButton_released()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    dateTime.setDate(ui->calendarWidget->selectedDate());
    QString sudoCommand;
#if defined TARGET_AVICONDB || defined TARGET_AVICONDBHS
    sudoCommand = "sudo ";
#endif
    if (runProcess(sudoCommand + QString("/bin/date -s \"%1\"").arg(dateTime.toUTC().toString("yyyy-MM-dd hh:mm:ss"))) == true) {
        // Set the Hardware Clock to the current System Time.
        runProcess(sudoCommand + "hwclock -w");
    }
    emit dateChanged();
    emit leaveTheWidget();
}

void SetDate::on_prevMonthPushButton_released()
{
    ui->calendarWidget->showPreviousMonth();
}

void SetDate::on_nextMonthPushButton_released()
{
    ui->calendarWidget->showNextMonth();
}

void SetDate::on_prevYearPushButton_released()
{
    ui->calendarWidget->showPreviousYear();
}

void SetDate::on_nextYearPushButton_released()
{
    ui->calendarWidget->showNextYear();
}

void SetDate::onCurrentPageChanged(int year, int month)
{
    QStringList months;
    months << tr("January") << tr("February") << tr("March") << tr("April") << tr("May") << tr("June") << tr("July") << tr("August") << tr("September") << tr("October") << tr("November") << tr("December");

    ui->yearLabel->setText(QString::number(year));
    ui->monthLabel->setText(months.at(month - 1));
}
