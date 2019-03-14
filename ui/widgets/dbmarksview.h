#ifndef DBMARKSVIEW_H
#define DBMARKSVIEW_H

#include <QWidget>
#include <QVector>
#include "core/db/DataBaseManager.h"

namespace Ui
{
class DBMarksView;
}

class DBMarksView : public QWidget
{
    Q_OBJECT

public:
    explicit DBMarksView(QWidget* parent = 0);
    ~DBMarksView();

    void setMarks(QVector<DBM::tMark> marksList);
private slots:
    void on_pushButton_released();

private:
    Ui::DBMarksView* ui;
};

#endif  // DBMARKSVIEW_H
