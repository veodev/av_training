#ifndef DEFECTPAGE_H
#define DEFECTPAGE_H

#include <QWidget>

class DefectPage : public QWidget
{
    Q_OBJECT
public:
    explicit DefectPage(QWidget* parent = Q_NULLPTR);

    virtual int getSide() = 0;
    virtual int getDefectLength() = 0;
    virtual int getDefectWidth() = 0;
    virtual int getDefectDepth() = 0;
    virtual int getSpeedLimit() = 0;
    virtual QString getDefectCode() = 0;
    virtual QString getLink() = 0;
    virtual QString getComment() = 0;
    virtual void clearFields() = 0;
signals:

    void defectCreated();
public slots:
};

#endif  // DEFECTPAGE_H
