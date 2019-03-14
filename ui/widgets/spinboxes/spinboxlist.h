#ifndef SPINBOXLIST_H
#define SPINBOXLIST_H

#include <QList>
#include <QVariant>

#include "spinboxbase.h"

class SpinBoxList : public SpinBoxBase
{
    Q_OBJECT

    Q_PROPERTY(int index READ index WRITE setIndex)
    Q_PROPERTY(QString value READ value)

public:
    explicit SpinBoxList(QWidget* parent = 0);
    virtual ~SpinBoxList();

    void setIndex(int index);
    int index() const;

    QString value() const;
    QVariant userData() const;

    SpinBoxList& operator<<(const QString& value);
    void addItem(const QString& value, const QVariant& userData);

    void clear();

signals:
    void valueChanged(int index, const QString& value, const QVariant& userData);

protected:
    virtual void displayValue();
    virtual void nextValue();
    virtual void prevValue();
    virtual bool isMinimumReached();
    virtual bool isMaximumReached();

protected:
    void timerStart(const char* slot = 0);
    void timerStop();
    void emitValueChanged();

private:
    int _index;
    struct Item
    {
        QString value;
        QVariant userData;
    };
    QList<Item*> _list;
};

#endif  // SPINBOXLIST_H
