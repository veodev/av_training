#include "testexternalkeyboardwidget.h"
#include "ui_testexternalkeyboardwidget.h"

TestExternalKeyboardWidget::TestExternalKeyboardWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestExternalKeyboardWidget)
{
    ui->setupUi(this);
    setWindowTitle("Remote Control");
}

TestExternalKeyboardWidget::~TestExternalKeyboardWidget()
{
    delete ui;
}

void TestExternalKeyboardWidget::on_trackMarkButton_released()
{
    emit testTrackMarkButtonPressed();
}

void TestExternalKeyboardWidget::on_serviceMarkButton_released()
{
    emit testServiceMarkButtonPressed();
}

void TestExternalKeyboardWidget::on_boltJointButton_released()
{
    emit testBoltJointButtonReleased();
}

void TestExternalKeyboardWidget::on_boltJointButton_pressed()
{
    emit testBoltJointButtonPressed();
}
