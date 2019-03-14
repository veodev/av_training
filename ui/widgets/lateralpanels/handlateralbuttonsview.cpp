#include <QVBoxLayout>
#include <QAbstractItemModel>

#include "handlateralbuttonsview.h"
#include "lateralbuttonitem.h"
#include "debug.h"

HandLateralButtonsView::HandLateralButtonsView(QWidget* parent)
    : QWidget(parent)
    , _model(nullptr)
    , _layout(new QVBoxLayout(this))
{
    _layout->setSpacing(1);
    _layout->setAlignment(Qt::AlignTop);
    _layout->setContentsMargins(0, 0, 3, 0);
}

HandLateralButtonsView::~HandLateralButtonsView()
{
    Q_ASSERT(_layout);
    delete _layout;
}

void HandLateralButtonsView::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model);

    if (_model != nullptr) {
        detachModel();
    }
    _model = model;
    ASSERT(connect(_model, &QAbstractItemModel::destroyed, this, &HandLateralButtonsView::detachModel));

    if (_model != nullptr && _model->rowCount()) {
        const QModelIndex& topLeft = _model->index(0, 0);
        const QModelIndex& bottomRight = _model->index(_model->rowCount() - 1, 0);

        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
            const QModelIndex& index = _model->index(i, 0);
            if (index.isValid() == true) {
                LateralButtonItem* button = new LateralButtonItem(i, this);
                button->setSizePolicy(sizePolicy);
                button->setFocusPolicy(Qt::NoFocus);
                ASSERT(connect(button, &LateralButtonItem::clicked, this, &HandLateralButtonsView::clicked));
                _items.append(button);
                _layout->addWidget(button);
#if defined(TARGET_AVICON31) || defined (TARGET_AVICON31E)
                if (i == 4) {
                    QWidget* widget = new QWidget(this);
                    widget->setSizePolicy(sizePolicy);
                    widget->setMinimumHeight(3);
                    widget->setMaximumHeight(3);
                    _emptyWidgets.append(widget);
                    _layout->addWidget(widget);
                }
#endif
            }
        }
#if defined(TARGET_AVICON31) || defined (TARGET_AVICON31E)
        for (int i = bottomRight.row() + 1; i <= 9; ++i) {
            QWidget* widget = new QWidget(this);
            widget->setSizePolicy(sizePolicy);
            _emptyWidgets.append(widget);
            _layout->addWidget(widget);
        }
#endif
    }
}

void HandLateralButtonsView::blockButtons(bool isBlock)
{
    for (LateralButtonItem* item : _items) {
        Q_ASSERT(item);
        item->setDisabled(isBlock);
    }
}

void HandLateralButtonsView::detachModel()
{
    _model = nullptr;
    for (LateralButtonItem* item : _items) {
        Q_ASSERT(item);
        item->setParent(Q_NULLPTR);
        delete item;
    }
    _items.clear();
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
    for (QWidget* item : _emptyWidgets) {
        Q_ASSERT(item);
        item->setParent(Q_NULLPTR);
        delete item;
    }
    _emptyWidgets.clear();
#endif
}
