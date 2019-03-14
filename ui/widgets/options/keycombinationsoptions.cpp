#include "keycombinationsoptions.h"
#include "ui_keycombinationsoptions.h"

#include "settings.h"

#include <QKeyEvent>
#include <QLayout>
#include <QList>
#include <QComboBox>

static QMap<int, QString> _keysMap;
static QMap<int, QString> _actionsMap;

KeyCombinationsOptions::KeyCombinationsOptions(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::KeyCombinationsOptions)
{
    ui->setupUi(this);
    _keyCombinationsMap = restoreKeyCombinations();
    setupUi();
}

KeyCombinationsOptions::~KeyCombinationsOptions()
{
    delete ui;
}

Actions KeyCombinationsOptions::checkKey(QKeyEvent* ke)
{
    int key = ke->key();
    if (_keysMap.contains(key)) {
        Actions action = static_cast<Actions>(*_keyCombinationsMap.find(key));
        emit doHotKeyReleased(action);
        return action;
    }
    return NoAction;
}

bool KeyCombinationsOptions::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        deleteItems();
        setupUi();
    }
    return QWidget::event(e);
}

void KeyCombinationsOptions::onComboBoxChanged(const QString& text)
{
    QString comboBoxName = sender()->objectName();
    int key = _keysMap.key(comboBoxName);
    int action = _actionsMap.key(text);

    if (text.isEmpty()) {
        _keyCombinationsMap.remove(key);
    }
    else {
        _keyCombinationsMap.insert(key, action);
    }
    saveKeyCombinations(_keyCombinationsMap);
}

void KeyCombinationsOptions::initializeKeysMap()
{
    _keysMap.clear();
    _keysMap.insert(Qt::Key_Escape, "Escape");
    _keysMap.insert(Qt::Key_F1, "F1");
    _keysMap.insert(Qt::Key_F2, "F2");
    _keysMap.insert(Qt::Key_F3, "F3");
    _keysMap.insert(Qt::Key_F4, "F4");
    _keysMap.insert(Qt::Key_F5, "F5");
    _keysMap.insert(Qt::Key_F6, "F6");
    _keysMap.insert(Qt::Key_F7, "F7");
    _keysMap.insert(Qt::Key_F8, "F8");
    _keysMap.insert(Qt::Key_F9, "F9");
    _keysMap.insert(Qt::Key_F10, "F10");
    _keysMap.insert(Qt::Key_F11, "F11");
    _keysMap.insert(Qt::Key_F12, "F12");
    _keysMap.insert(Qt::Key_PageUp, "Page Up");
    _keysMap.insert(Qt::Key_PageDown, "Page Down");
    _keysMap.insert(Qt::Key_Up, "Up");
    _keysMap.insert(Qt::Key_Down, "Down");
    _keysMap.insert(Qt::Key_Left, "Left");
    _keysMap.insert(Qt::Key_Right, "Right");
    _keysMap.insert(Qt::Key_Plus, "Plus");
    _keysMap.insert(Qt::Key_Minus, "Minus");
}

void KeyCombinationsOptions::initializeActionsMap()
{
    _keysMap.clear();
    _actionsMap.insert(Back, tr("Back button"));
    _actionsMap.insert(Menu, tr("Menu button"));
    _actionsMap.insert(Menu, tr("Menu button"));
    _actionsMap.insert(ServiceMarks, tr("Service Marks Button"));
    _actionsMap.insert(RailType, tr("Rail Type button"));
    _actionsMap.insert(BScan, tr("B-Scan button"));
    _actionsMap.insert(KyPlus, tr("Ky +"));
    _actionsMap.insert(KyMinus, tr("Ky -"));
    _actionsMap.insert(TvgPlus, tr("Tvg +"));
    _actionsMap.insert(TvgMinus, tr("Tvg -"));
    _actionsMap.insert(BScanLevelPlus, tr("B-Scan Level +"));
    _actionsMap.insert(BScanLevelMinus, tr("B-Scan Level -"));
}

void KeyCombinationsOptions::updateComboboxes()
{
    for (auto it = _keyCombinationsMap.begin(); it != _keyCombinationsMap.end(); ++it) {
        int key = it.key();
        int action = it.value();
        QString text;
        QString objName;
        if (_actionsMap.contains(action)) {
            text = *_actionsMap.find(action);
        }
        if (_keysMap.contains(key)) {
            objName = *_keysMap.find(key);
        }
        for (auto item : _items) {
            if (item->objectName() == objName) {
                item->setCurrentText(text);
            }
        }
    }
}

void KeyCombinationsOptions::setupUi()
{
    initializeActionsMap();
    initializeKeysMap();
    for (auto item : _keysMap.values()) {
        QHBoxLayout* layout = new QHBoxLayout();
        layout->setDirection(QBoxLayout::LeftToRight);
        QComboBox* comboBox = new QComboBox();
        _items.push_back(comboBox);
        comboBox->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        comboBox->setObjectName(item);
        comboBox->addItem("");
        comboBox->addItems(_actionsMap.values());
        QLabel* label = new QLabel(item);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Preferred);
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        comboBox->setFont(font);
        font.setWeight(50);
        label->setFont(font);
        layout->addWidget(label);
        layout->addWidget(comboBox);
        connect(comboBox, &QComboBox::currentTextChanged, this, &KeyCombinationsOptions::onComboBoxChanged);
        ui->widgetsLayout->addLayout(layout);
    }
    updateComboboxes();
}

void KeyCombinationsOptions::deleteItems()
{
    for (auto item : _items) {
        disconnect(item, &QComboBox::currentTextChanged, this, &KeyCombinationsOptions::onComboBoxChanged);
    }
    _items.clear();

    QLayoutItem* layoutItem;
    while ((layoutItem = ui->widgetsLayout->takeAt(0)) != 0) {
        QLayoutItem* interiorLayoutItem;
        while ((interiorLayoutItem = layoutItem->layout()->takeAt(0)) != 0) {
            interiorLayoutItem->widget()->deleteLater();
        }
    }
}
