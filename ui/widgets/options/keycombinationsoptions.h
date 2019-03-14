#ifndef KEYCOMBINATIONSOPTIONS_H
#define KEYCOMBINATIONSOPTIONS_H

#include <QWidget>
#include <QMap>

#include "coredefinitions.h"

class QComboBox;

namespace Ui {
class KeyCombinationsOptions;
}

class KeyCombinationsOptions : public QWidget
{
    Q_OBJECT

public:
    explicit KeyCombinationsOptions(QWidget *parent = 0);
    ~KeyCombinationsOptions();

    Actions checkKey(QKeyEvent* ke);

protected:
    bool event(QEvent* e);

signals:
    void doHotKeyReleased(Actions action);

private slots:
    void onComboBoxChanged(const QString& text);

private:
    void initializeKeysMap();
    void initializeActionsMap();
    void updateComboboxes();
    void setupUi();
    void deleteItems();

private:
    Ui::KeyCombinationsOptions *ui;
    QMap<int,int> _keyCombinationsMap;
    QVector<QComboBox*> _items;
};

#endif // KEYCOMBINATIONSOPTIONS_H
