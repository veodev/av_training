#include "dbmarksview.h"
#include "ui_dbmarksview.h"

DBMarksView::DBMarksView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DBMarksView)
{
    ui->setupUi(this);
    ui->listWidget->setStyleSheet("QScrollBar:vertical {width: 40;}");
}

DBMarksView::~DBMarksView()
{
    delete ui;
}

void DBMarksView::setMarks(QVector<DBM::tMark> marksList)
{
    ui->listWidget->clear();
    for (int i = 0; i < marksList.size(); i++) {
        QString mark;
        mark = QString(tr("km ")) + QString::number(marksList[i].Km) + QString(" ") + QString(tr("pk ")) + QString::number(marksList[i].Pk) + QString(" ") + QString(tr("m ")) + QString::number(marksList[i].Meter) + QString(", ")
               + ((marksList[i].Rail == 1) ? QString(tr("right")) : QString(tr("left"))) + QString(", ") + marksList[i].Code;

        ui->listWidget->addItem(mark);
    }
}

void DBMarksView::on_pushButton_released()
{
    this->hide();
}
