#ifndef SCANNERDEFECTREGISTRATIONPAGE_H
#define SCANNERDEFECTREGISTRATIONPAGE_H

#include <QWidget>

class ScannerDefectRegistrationPage : public QWidget
{
    Q_OBJECT
public:
    explicit ScannerDefectRegistrationPage(QWidget* parent = Q_NULLPTR);

    virtual void switchState(int state) = 0;
    virtual int getWorkRail() = 0;
    virtual int getEdge() = 0;
    virtual int getHSize() = 0;
    virtual int getLSize() = 0;
    virtual int getDSize() = 0;
    virtual int getSpeedLimit() = 0;
    virtual QString getDefectCode() = 0;
    virtual QString getLink() = 0;
    virtual QString getComment() = 0;
    virtual void resetPage() = 0;
signals:
    void startWork();
    void nextHead();
    void saveResults();

public slots:
};

#endif  // SCANNERDEFECTREGISTRATIONPAGE_H
