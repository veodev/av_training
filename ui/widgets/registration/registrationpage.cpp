#include "registrationpage.h"

RegistrationPage::RegistrationPage(QWidget* parent)
    : QWidget(parent)
    , _operatorSelectionWidget(Q_NULLPTR)
    , _lineSelectionWidget(Q_NULLPTR)
    , _filePlacementWidget(Q_NULLPTR)
    , _selectOperatorNumber(1)
    , _isRegistrationStarted(false)
    , _fileName(QString())
{
}
