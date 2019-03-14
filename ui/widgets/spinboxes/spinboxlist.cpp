#include "spinboxlist.h"
#include "ui_spinboxbase.h"

SpinBoxList::SpinBoxList(QWidget* parent)
    : SpinBoxBase(parent)
    , _index(-1)
{
}

SpinBoxList::~SpinBoxList()
{
    for (unsigned short i = 0; i < _list.size(); ++i) {
        delete _list[i];
    }
    _list.clear();
}

void SpinBoxList::setIndex(int index)
{
    if (index != _index && index >= 0 && index < _list.size()) {
        _index = index;
        displayValue();
        emitValueChanged();
    }
}

int SpinBoxList::index() const
{
    return _index;
}

QString SpinBoxList::value() const
{
    if (_index != -1) {
        return _list.at(_index)->value;
    }
    return QString();
}

QVariant SpinBoxList::userData() const
{
    if (_index != -1) {
        return _list.at(_index)->userData;
    }
    return QVariant();
}

SpinBoxList& SpinBoxList::operator<<(const QString& value)
{
    addItem(value, _list.count());
    return *this;
}

void SpinBoxList::addItem(const QString& value, const QVariant& userData)
{
    Item* item = new Item;
    item->value = value;
    item->userData = userData;
    _list.append(item);
    if (_index == -1) {
        _index = 0;
    }
    if (_index >= 0) {
        displayValue();
    }
}

void SpinBoxList::clear()
{
    for (unsigned short i = 0; i < _list.size(); ++i) {
        delete _list[i];
    }
    _list.clear();
    _index = -1;
}

void SpinBoxList::displayValue()
{
    if (_index != -1) {
        ui->value->setText(_list.at(_index)->value);
    }

    if (_index == 0) {
        ui->leftButton->setEnabled(false);
    }
    else {
        ui->leftButton->setEnabled(true);
    }

    if (_index + 1 == _list.size()) {
        ui->rightButton->setEnabled(false);
    }
    else {
        ui->rightButton->setEnabled(true);
    }
}

void SpinBoxList::nextValue()
{
    if (_index + 1 < _list.size()) {
        ++_index;
        displayValue();
        emitValueChanged();
    }
}

void SpinBoxList::prevValue()
{
    if (_index - 1 >= 0) {
        --_index;
        displayValue();
        emitValueChanged();
    }
}

bool SpinBoxList::isMinimumReached()
{
    return (_index == 0);
}

bool SpinBoxList::isMaximumReached()
{
    return ((_list.size() == 0) || (_index == _list.size() - 1));
}

void SpinBoxList::emitValueChanged()
{
    Item* item = _list.at(_index);
    Q_ASSERT(item);
    emit valueChanged(_index, item->value, item->userData);
}
