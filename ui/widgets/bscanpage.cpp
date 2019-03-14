#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>
#include "bscanpage.h"
#include "ui_bscanpage.h"
#include "bscanplot.h"

#include "spinboxnumber.h"
#include "spinboxlist.h"
#include "debug.h"
#include "roles.h"
#include "channelscontroller.h"
#include "settings.h"
#include "styles.h"
#include "bscantape.h"
#include "bottomsignalampl.h"
#include "accessories.h"
#include "core.h"
#include "colorscheme.h"
#include "registration.h"


static const int replotInterval = 30;
static const QVector<int> AMPLITUDE_TO_DB_ARRAY = {-12, -10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18};


BScanPage::BScanPage(QWidget* parent)
    : QWidget(parent)
    , _isMarkersTextVisible(true)
    , _isIncreaseTracks(restoreIncreaseTracks())
    , _viewMode(false)
    , _isProbePulseFromAbove(false)
    , _importaintAreaIndication(restoreImportantAreaIndicateEnable())
    , _isParallelVideoBrowsing(restoreParallelVideoBrowsingStatus())
    , _oldDisCoord(0)
    , _lastDisCoord(0)
    , _screensMovementOffset(0)
    , _rectID(0)
    , _additiveForSens(restoreAdditiveForSens())
    , _currentMagnifierFilter(0)
    , _bscanScale(static_cast<float>(restoreBscanScale()))
    , _encoderCorrection(encoderCorrection())
    , _cid(-1)
    , _currentRailFilter(NoneDeviceSide)
    , _deviceSide(NoneDeviceSide)
    , _currentDirection(0)
    , _currentEdge(0)
    , ui(new Ui::BScanPage)
    , _displayThresholdBscan(new SpinBoxNumber(this))
    , _scaleBscan(new SpinBoxNumber(this))
    , _railBscan(new SpinBoxList(this))
    , _channelsByDirectionSpinBox(new SpinBoxList(this))
    , _edgeRailBscan(new SpinBoxList(this))
    , _magnifierBscan(new SpinBoxList(this))
    , _navigationBscan(new SpinBoxList(this))
    , _movementControls(new BScanMovementControls(this))
    , _drawModeSpinBox(new SpinBoxList(this))
    , _colorSchemeSpinBox(new SpinBoxList(this))
    , _model(nullptr)
    , _replotTimer(new QTimer(this))
    , _labelColor(QColor(25, 227, 49))
    , _autoMarkColor(QColor(0, 0, 0))
    , _fileOpened(false)
{
    ui->setupUi(this);
    setupUi();

    _replotTimer->setInterval(replotInterval);
    ASSERT(connect(_replotTimer, &QTimer::timeout, this, &BScanPage::replotTimeout));
    ASSERT(connect(Core::instance().channelsController(), &ChannelsController::doTapeSelected, this, &BScanPage::onTapeSelected));
}

BScanPage::~BScanPage()
{
    _replotTimer->stop();
    disconnect(_replotTimer, &QTimer::timeout, this, &BScanPage::replotTimeout);
    delete ui;
    qDebug() << "BScanPage deleted!";
}

void BScanPage::setupUi()
{
    Core& core = Core::instance();
    ASSERT(connect(&core, &Core::doStartBoltJoint, this, &BScanPage::onStartBoltJoint));
    ASSERT(connect(&core, &Core::doStopBoltJoint, this, &BScanPage::onStopBoltJoint));

    _displayThresholdBscan->enableCaption();
    _displayThresholdBscan->setMinMaxStepValue(-12, 18, 2, 0);
    ASSERT(connect(_displayThresholdBscan, &SpinBoxNumber::valueChanged, this, &BScanPage::onThresholdBscanChanged));
    ui->controlsArea->addWidget(_displayThresholdBscan);

    ASSERT(connect(_scaleBscan, &SpinBoxNumber::valueChanged, this, &BScanPage::on_changeBscanScale));
    _scaleBscan->enableCaption();
    _scaleBscan->setPrecision(3);
    _scaleBscan->setMinimum(0.5);
    _scaleBscan->setStepBy(0.5);
    if (restoreDeviceType() == Avicon31Estonia) {
        _scaleBscan->enableMultiStep();
        _scaleBscan->setMaximum(50.0);
    }
    else {
        _scaleBscan->setMaximum(5.0);
    }
    _scaleBscan->setValue(restoreBscanScale(), false);
    ui->controlsArea->addWidget(_scaleBscan);

    ASSERT(connect(_railBscan, &SpinBoxList::valueChanged, this, &BScanPage::onChangeRailsBscan));
    _railBscan->enableCaption();
    ui->controlsArea->addWidget(_railBscan);

    ASSERT(connect(_channelsByDirectionSpinBox, &SpinBoxList::valueChanged, this, &BScanPage::onChannelsByDirectionChanged));
    _channelsByDirectionSpinBox->enableCaption();
    ui->controlsArea->addWidget(_channelsByDirectionSpinBox);

    ASSERT(connect(_edgeRailBscan, &SpinBoxList::valueChanged, this, &BScanPage::onEdgeRailBscan));
    _edgeRailBscan->enableCaption();
    ui->controlsArea->addWidget(_edgeRailBscan);

    ASSERT(connect(_magnifierBscan, &SpinBoxList::valueChanged, this, &BScanPage::onMagnifierChanged));
    _magnifierBscan->enableCaption();
    ui->controlsArea->addWidget(_magnifierBscan);

    ASSERT(connect(&core, &Core::doSetRegistrationThreshold, this, &BScanPage::onSetMinRegistrationThreshold));

    ASSERT(connect(_drawModeSpinBox, &SpinBoxList::valueChanged, this, &BScanPage::onDrawModeChanged));
    _drawModeSpinBox->enableCaption();
    ui->controlsArea->addWidget(_drawModeSpinBox);

    ASSERT(connect(_colorSchemeSpinBox, &SpinBoxList::valueChanged, this, &BScanPage::onColorSchemeChanged));
    _colorSchemeSpinBox->enableCaption();
    ui->controlsArea->addWidget(_colorSchemeSpinBox);

    ASSERT(connect(_navigationBscan, &SpinBoxList::valueChanged, this, &BScanPage::OnNavigationChanged));
    _navigationBscan->enableCaption();
    ui->controlsArea->addWidget(_navigationBscan);

    ASSERT(connect(&core, &Core::doBottomSignalAmpl, this, &BScanPage::onBottomSignalAmpl));

    ui->controlsArea->addWidget(_movementControls);

    ASSERT(connect(_movementControls, &BScanMovementControls::modeChanged, this, &BScanPage::onMovementModeChanged));
    ASSERT(connect(_movementControls, &BScanMovementControls::movementLeft, this, &BScanPage::onMovementLeft));
    ASSERT(connect(_movementControls, &BScanMovementControls::movementRight, this, &BScanPage::onMovementRight));

    retranslateUi();
}

void BScanPage::retranslateUi()
{
    _displayThresholdBscan->setCaption(tr("Level"));
    _displayThresholdBscan->setSuffix(tr(" dB"));
    _scaleBscan->setCaption(tr("Scale"));
    _scaleBscan->setSuffix(tr(" m"));
    _magnifierBscan->setCaption(tr("Magnifier"));
    _navigationBscan->hide();
    retranslateRailBscan();
    retranslateEdgeBscan();
    retranslateDirectionBscan();
    retranslateDrawModes();
    retranslateColorSchemes();
}

void BScanPage::retranslateRailBscan()
{
    _railBscan->setCaption(tr("Rail"));
    if (_railFilters.contains(LeftDeviceSide) && _railFilters.contains(RightDeviceSide)) {
        int currentIndex = _railBscan->index();
        _railBscan->clear();
        _railBscan->setVisible(true);
        _railBscan->addItem(tr("Both"), NoneDeviceSide);
        if (_railFilters.contains(LeftDeviceSide)) {
            _railBscan->addItem(tr("Left"), LeftDeviceSide);
        }
        if (_railFilters.contains(RightDeviceSide)) {
            _railBscan->addItem(tr("Right"), RightDeviceSide);
        }
        if (currentIndex >= 0) {
            _railBscan->setIndex(currentIndex);
        }
    }
    else {
        _railBscan->setVisible(false);
    }
}

void BScanPage::retranslateEdgeBscan()
{
    _edgeRailBscan->setCaption(tr("Edge"));
    int currentIndex = _edgeRailBscan->index();
    _edgeRailBscan->clear();
    _edgeRailBscan->addItem(tr("All"), CurveByDirectionAndEdge::NoEdgeType);
    _edgeRailBscan->addItem(tr("Gauge"), CurveByDirectionAndEdge::WorkEdgeType);
    _edgeRailBscan->addItem(tr("Field"), CurveByDirectionAndEdge::UnworkEdgeType);
    if (currentIndex >= 0) {
        _edgeRailBscan->setIndex(currentIndex);
    }

    for (CurveByDirectionAndEdge item : _curvesByDirectionAndEdge) {
        if (item.edge == CurveByDirectionAndEdge::WorkEdgeType || item.edge == CurveByDirectionAndEdge::UnworkEdgeType) {
            _edgeRailBscan->setVisible(true);
            break;
        }
        else {
            _edgeRailBscan->setVisible(false);
        }
    }
}

void BScanPage::retranslateDirectionBscan()
{
    _channelsByDirectionSpinBox->setCaption(tr("Channels"));
    int currentIndex = _edgeRailBscan->index();
    _channelsByDirectionSpinBox->clear();
    _channelsByDirectionSpinBox->addItem(tr("All"), UnknownDirection);
    _channelsByDirectionSpinBox->addItem(tr("Forward"), ForwardDirection);
    _channelsByDirectionSpinBox->addItem(tr("Backward"), BackwardDirection);
    if (currentIndex >= 0) {
        _channelsByDirectionSpinBox->setIndex(currentIndex);
    }
}

void BScanPage::retranslateDrawModes()
{
    _drawModeSpinBox->setCaption(tr("Mode"));
    _drawModeSpinBox->clear();
    _drawModeSpinBox->addItem(tr("Normal"), 0);
    _drawModeSpinBox->addItem(tr("C-Scan"), 1);
    _drawModeSpinBox->addItem(tr("Gradient"), 2);
    _drawModeSpinBox->addItem(tr("Individual"), 3);
    _drawModeSpinBox->setIndex(0);
}

void BScanPage::retranslateColorSchemes()
{
    _colorSchemeSpinBox->setCaption(tr("Scheme"));
    _colorSchemeSpinBox->clear();
    const auto& schemes = Core::instance().getColorSchemes();
    int index = 0;
    for (const auto& scheme : schemes) {
        _colorSchemeSpinBox->addItem(scheme.getName(), index);
        ++index;
    }
    _colorSchemeSpinBox->setIndex(0);
}

bool BScanPage::retranslateNavigation()
{
    _navigationBscan->clear();
    _navigationBscan->setCaption(QObject::tr("Event"));
    _navigationBscan->addItem(QObject::tr("Navigate"), 0);
    eventSpan span;
    Core::instance().registration().getEventSpan(span);
    if (span.empty()) {
        return false;
    }
    int i = 1;
    for (eventFileItem event : span) {
        switch (event.id) {
        case EID_ChangeOperatorName:
            _navigationBscan->addItem(QObject::tr("Operator"), i);
            break;
        case EID_Switch:
            _navigationBscan->addItem(QObject::tr("Switch"), i);
            break;
        case EID_TextLabel:
            _navigationBscan->addItem(QObject::tr("Text mark"), i);
            break;
        case EID_HandScan:
            _navigationBscan->addItem(QObject::tr("Hand"), i);
            break;
        case EID_RailHeadScaner:
            _navigationBscan->addItem(QObject::tr("Rail head scan"), i);
            break;
        case EID_StartSwitchShunter:
            _navigationBscan->addItem(QObject::tr("Start switch shunter"), i);
            break;
        case EID_EndSwitchShunter:
            _navigationBscan->addItem(QObject::tr("End switch shunter"), i);
            break;
        case EID_StBoltStyk:
            _navigationBscan->addItem(QObject::tr("BJ(on)"), i);
            break;
        case EID_EndBoltStyk:
            _navigationBscan->addItem(QObject::tr("BJ(off)"), i);
            break;
        case EID_SetRailType:
            _navigationBscan->addItem(QObject::tr("Rail type"), i);
            break;
        case EID_Media:
            _navigationBscan->addItem(QObject::tr("Media"), i);
            break;
        case EID_Stolb:
            _navigationBscan->addItem(QObject::tr("Track mark"), i);
            break;
        case EID_QualityCalibrationRec:
            _navigationBscan->addItem(QObject::tr("Calibration quality"), i);
            break;
        case EID_StolbChainage:
            _navigationBscan->addItem(QObject::tr("Chainage coordinate"), i);
            break;
        case EID_DefLabel:
            _navigationBscan->addItem(QObject::tr("Defect"), i);
            break;
        default:
            QDEBUG << "Unknown event with data!";
            break;
        }
        i++;
    }
    return true;
}


void BScanPage::setDotSize(int value)
{
    Q_ASSERT(value > 0 && value < 10);

    for (BScanTape* tape : _bScanTapes) {
        tape->setBScanDotSize(value);
    }
}

bool BScanPage::controlsAreaVisible()
{
    return ui->controlsArea->isVisible();
}

void BScanPage::setControlsAreaVisible(bool isVisible)
{
    ui->controlsArea->setVisible(isVisible);
}

void BScanPage::setModel(QAbstractItemModel* model)
{
    Q_ASSERT(model != nullptr);

    if (_model != nullptr) {
        detachModel();
    }
    _model = model;
    ASSERT(connect(_model, &QAbstractItemModel::destroyed, this, &BScanPage::detachModel));

    _channelsMap.clear();
    _tapesForBoltJoinMap.clear();
    _curvesByDirectionAndEdge.clear();

    _magnifierBscan->clear();
    *_magnifierBscan << tr("Off");
    _magnifierFilters.clear();
    QList<bool> list;
    for (int i = 0; i < _model->rowCount(); ++i) {
        list << true;
    }
    _magnifierFilters.push_back(list);
    _bScanTapes.reserve(10);

    if (_model->rowCount() != 0) {
        const QModelIndex& topLeft = _model->index(0, 0);
        const QModelIndex& bottomRight = _model->index(_model->rowCount() - 1, 0);

        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        DeviceSide prevSide = NoneDeviceSide;
        for (int tapeNumber = topLeft.row(); tapeNumber <= bottomRight.row(); ++tapeNumber) {
            const QModelIndex& tapeIndex = _model->index(tapeNumber, 0);
            if (!tapeIndex.isValid()) {
                continue;
            }

            DeviceSide side = static_cast<DeviceSide>(_model->data(tapeIndex, SideRole).toInt());
            if ((tapeNumber > 0) && (prevSide != side)) {
                QWidget* widget = new QWidget(this);
                widget->setSizePolicy(sizePolicy);
                widget->setMinimumHeight(1);
                widget->setMaximumHeight(1);
                _separators.append(widget);
                ui->layout->addWidget(widget);
            }
            prevSide = side;

            std::array<std::array<unsigned short, 4>, 3> colorScheme;
            BScanTape* item = new BScanTape(this);
            item->setSizePolicy(sizePolicy);
            item->setSide(side);
            _bScanTapes.push_back(item);
            ui->layout->addWidget(item);

            if (_railFilters.contains(side) == false) {
                QList<bool> list;
                for (int i = 0; i < _model->rowCount(); ++i) {
                    list << false;
                }
                _railFilters[side] = list;
            }
            QList<bool>& list = _railFilters[side];
            list[tapeNumber] = true;

            int angle = _model->data(tapeIndex, AngleRole).toInt();
            if (side == NoneDeviceSide || side == LeftDeviceSide) {
                QList<bool> list;
                for (int i = 0; i < _model->rowCount(); ++i) {
                    list << false;
                }
                list[tapeNumber] = true;
                if (side == LeftDeviceSide) {
                    int conformity = _model->data(tapeIndex, TapeConformityRole).toInt();
                    if (conformity >= 0) {
                        list[conformity] = true;
                    }
                }
                _magnifierFilters.push_back(list);
                _magnifierBscan->addItem(QString::number(angle), _magnifierFilters.size() - 1);
            }

            for (unsigned char row = 0; row < 2; ++row) {
                for (unsigned char column = 0; column < 2; ++column) {
                    const QModelIndex& childIndex = _model->index(row, column, tapeIndex);
                    if (childIndex.isValid() && _model->data(childIndex, FakedChannelRole).toBool() != true) {
                        int key = createKeyFromSideAndChannel(_model->data(childIndex, SideRole).toInt(), _model->data(childIndex, ChannelIdRole).toInt());

                        unsigned char colorIndex = row * 2 + column;

                        CurveByDirectionAndEdge curveByDirection;
                        curveByDirection.tapeIndex = tapeNumber;
                        curveByDirection.curveNumber = colorIndex;
                        curveByDirection.direction = static_cast<Direction>(_model->data(childIndex, DirectionRole).toInt());
                        eControlZone zone = static_cast<eControlZone>(_model->data(childIndex, ZoneRole).toInt());
                        switch (zone) {
                        case czHeadWork:
                        case czHeadLeftWork:
                        case czHeadAndWork:
                            curveByDirection.edge = CurveByDirectionAndEdge::WorkEdgeType;
                            break;
                        case czHeadUnWork:
                        case czHeadLeftUnWork:
                        case czHeadAndUnWork:
                            curveByDirection.edge = CurveByDirectionAndEdge::UnworkEdgeType;
                            break;
                        default:
                            curveByDirection.edge = CurveByDirectionAndEdge::NoEdgeType;
                            break;
                        }
                        _curvesByDirectionAndEdge.push_back(curveByDirection);
                        if (!_channelsMap.contains(key)) {
                            ItemOfMap element;
                            element.tapeNumber = tapeNumber;
                            element.color = colorIndex;
                            Q_ASSERT(_model->data(childIndex, DelayMultiplyRole).toInt() != 0);
                            element.delayMultiply = _model->data(childIndex, DelayMultiplyRole).toInt();
                            element.edge = curveByDirection.edge;
                            _channelsMap.insert(key, element);
                        }
                        colorScheme[0][colorIndex] = static_cast<unsigned short>(_model->data(childIndex, BScanNormalColorRole).toUInt());
                        colorScheme[1][colorIndex] = static_cast<unsigned short>(_model->data(childIndex, BScanSpecialColorRole).toUInt());
                        colorScheme[2][colorIndex] = static_cast<unsigned short>(_model->data(childIndex, BScanIndividualColorRole).toUInt());
                    }
                }
            }

            item->setCurrentColorScheme(colorScheme);
            item->setForwardColor(static_cast<unsigned short>(_model->data(tapeIndex, BScanForwardColorRole).toUInt()));
            item->setBackwardColor(static_cast<unsigned short>(_model->data(tapeIndex, BScanBackwardColorRole).toUInt()));

            _tapesForBoltJoinMap.push_back(_model->data(tapeIndex, IsTapeVisibleInBoltJointModeRole).toBool());
#if !defined TARGET_AVICON15
            QModelIndex proxyIndex = _model->index(0, 0, tapeIndex);
#else
            QModelIndex proxyIndex = _model->index(0, 1, tapeIndex);
            if (_model->data(tapeIndex, AngleRole).toInt() == 0) {
                proxyIndex = _model->index(0, 0, tapeIndex);
            }
#endif
            if (proxyIndex.isValid() == false) {
                continue;
            }

            QAbstractItemModel* proxyModel = const_cast<QAbstractItemModel*>(proxyIndex.model());
            Q_ASSERT(proxyModel != nullptr);

            item->setGatesScale(_model->data(proxyIndex, EndBscangateRole).toInt());
            item->setBScanVerticalScale(_model->data(proxyIndex, StartBscangateRole).toInt(), _model->data(proxyIndex, EndBscangateRole).toInt());
            item->setModel(proxyModel);
            ASSERT(connect(proxyModel, &QAbstractItemModel::dataChanged, item, &BScanTape::onDataChanged));
        }
    }
    setProbePulseLocation(restoreProbePulseLocation());
    retranslateEdgeBscan();
    retranslateDirectionBscan();

    ASSERT(connect(_model, &QAbstractItemModel::dataChanged, this, &BScanPage::onDataChanged, Qt::UniqueConnection));

    if (_railFilters.contains(NoneDeviceSide) == false) {
        QList<bool> list;
        for (int i = 0; i < _model->rowCount(); ++i) {
            list << true;
        }
        _railFilters[NoneDeviceSide] = list;
    }
    retranslateRailBscan();
    retranslateDrawModes();

    for (BScanTape* tape : _bScanTapes) {
        tape->clearTape();
        tape->setIsInvalid(false);
    }
    onThresholdBscanChanged(_displayThresholdBscan->value());
}

const BScanPlot& BScanPage::bScanPlotByTapeIndex(unsigned int tapeIndex) const
{
    Q_ASSERT(tapeIndex < _bScanTapes.size());
    return _bScanTapes[tapeIndex]->bScanPlot();
}

void BScanPage::onChangeRailsBscan(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    _currentRailFilter = static_cast<DeviceSide>(userData.toInt());
    Roles role = ShowSeparatorsRole;
    if (_currentRailFilter != NoneDeviceSide) {
        role = HideSeparatorsRole;
    }
    const QModelIndex& modelIndex = _model->index(0, 0);
    _model->setData(modelIndex, QVariant(), role);
    applyTapesVisibleFilters();
    if (!_fileOpened) {
        fillTapes();
    }
}

void BScanPage::onThresholdBscanChanged(qreal value)
{
    int index = AMPLITUDE_TO_DB_ARRAY.indexOf(static_cast<int>(value));
    uchar threshold = static_cast<uchar>(index);
    Core::instance().setBScanDisplayThresholdChanged(qRound(value));
    Q_ASSERT(threshold <= 0x0f);

    for (BScanTape* tape : _bScanTapes) {
        tape->setThreshold(threshold);
    }
    if (!_fileOpened) {
        fillTapes();
    }
}

void BScanPage::on_changeBscanScale(const qreal& value)
{
    for (BScanTape* tape : _bScanTapes) {
        tape->setBScanHorizontalScale(static_cast<float>(value));
    }
    double bscanScaleOld = static_cast<double>(_bscanScale);
    saveBscanScale(value);
    _bscanScale = static_cast<float>(value);

    if (!_fileOpened) {
        fillTapes();
    }
    else {
        double coef = bscanScaleOld / value;
        _screensMovementOffset = static_cast<int>(_screensMovementOffset * coef);
    }

    emit bscanScaleChanged(value);
}

void BScanPage::onChannelsByDirectionChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    _currentDirection = userData;
    applyDirectionAndEdgeFilter();
}

void BScanPage::onMagnifierChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    _currentMagnifierFilter = userData.toInt();
    applyTapesVisibleFilters();
}

void BScanPage::onEdgeRailBscan(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(index);
    Q_UNUSED(value);

    _currentEdge = userData;
    applyDirectionAndEdgeFilter();
}

void BScanPage::onDrawModeChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(userData);
    Q_UNUSED(value);

    Q_ASSERT(index < 4);

    for (BScanTape* tape : _bScanTapes) {
        tape->setDrawMode(index);
    }
    emit doChangeDrawMode(index);
    if (!_fileOpened) {
        fillTapes();
    }
}

void BScanPage::onColorSchemeChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(value);
    Q_UNUSED(userData);
    Core::instance().switchCurrentColorScheme(index);
}

void BScanPage::OnNavigationChanged(int index, const QString& value, const QVariant& userData)
{
    Q_UNUSED(value);
    Q_UNUSED(userData);
    eventSpan span;
    Core::instance().registration().getEventSpan(span);
    if (index - 1 < span.size()) {
        plotPart(span.at(index - 1).disCrd);
    }
}

bool BScanPage::event(QEvent* e)
{
    Q_ASSERT(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateUi();
    }
    return QWidget::event(e);
}

void BScanPage::mousePressEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    _isMarkersTextVisible = !_isMarkersTextVisible;
}

void BScanPage::setSeparatorVisible(bool value)
{
    for (QWidget* sparator : _separators) {
        sparator->setVisible(value);
    }
}

void BScanPage::applyTapesVisibleFilters()
{
    QList<bool>& magnifierFilter = _magnifierFilters[_currentMagnifierFilter];
    QList<bool>& sideFilter = _railFilters[_currentRailFilter];
    for (int i = 0; i < _model->rowCount(); ++i) {
        const QModelIndex& modelIndex = _model->index(i, 0);
        _model->setData(modelIndex, sideFilter[i] && magnifierFilter[i], VisibleRole);
    }
    if (!_fileOpened) {
        fillTapes();
    }
}

void BScanPage::applyDirectionAndEdgeFilter()
{
    bool isCurentDirectionValid = _currentDirection.isValid();
    bool isCurentEdgeValid = _currentEdge.isValid();
    Q_ASSERT(isCurentDirectionValid);
    Q_ASSERT(isCurentEdgeValid);
    Direction direction = UnknownDirection;
    if (isCurentDirectionValid) {
        direction = static_cast<Direction>(_currentDirection.toInt());
    }
    CurveByDirectionAndEdge::EdgesType edge = CurveByDirectionAndEdge::NoEdgeType;
    if (isCurentEdgeValid) {
        edge = static_cast<CurveByDirectionAndEdge::EdgesType>(_currentEdge.toInt());
    }
    for (const CurveByDirectionAndEdge& item : _curvesByDirectionAndEdge) {
        bool isVisible = ((item.direction == direction || direction == UnknownDirection) && (item.edge == edge || edge == CurveByDirectionAndEdge::NoEdgeType));
        _bScanTapes[item.tapeIndex]->setBscanCurveVisible(item.curveNumber, isVisible);
    }
    if (!_fileOpened) {
        fillTapes();
    }
}

void BScanPage::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        if (i >= static_cast<int>(_bScanTapes.size())) {
            break;
        }
        QVariant value;
        const QModelIndex& index = _model->index(i, 0);
        for (int role : roles) {
            switch (role) {
            case VisibleRole:
                value = _model->data(index, VisibleRole);
                Q_ASSERT(value.isValid());
                {
                    bool isVisible = value.toBool();
                    if (_bScanTapes[i]->isVisible() != isVisible) {
                        _bScanTapes[i]->setVisible(isVisible);
                    }
                }
                break;
            case ShowSeparatorsRole:
                setSeparatorVisible(true);
                break;
            case HideSeparatorsRole:
                setSeparatorVisible(false);
                break;
            case BScanForwardColorRole:
                value = _model->data(index, BScanForwardColorRole);
                if (value != _bScanTapes[i]->getForwardColor()) {
                    _bScanTapes[i]->setForwardColor(value.toUInt());
                }
                break;
            case BScanBackwardColorRole:
                value = _model->data(index, BScanBackwardColorRole);
                if (value != _bScanTapes[i]->getBackwardColor()) {
                    _bScanTapes[i]->setBackwardColor(value.toUInt());
                }
                break;
            default:
                break;
            }
        }
    }
}

void BScanPage::setVisible(bool visible)
{
    if (visible) {
        _replotTimer->start();
        for (BScanTape* tape : _bScanTapes) {
            tape->replotBScanPlot();
        }
    }
    else {
        _replotTimer->stop();
    }
    QWidget::setVisible(visible);
}

void BScanPage::onTapeSelected(int tapeIndex, int position, int side)
{
    Q_ASSERT(tapeIndex >= 0 && tapeIndex < static_cast<int>(_bScanTapes.size()));
    Q_ASSERT(side == 0 || side == 1 || side == 2);
    emit doBScanPlotSelected(tapeIndex, position * 2 + side);
}

void BScanPage::checkInvalidTapes()
{
    for (const BScanTape* tape : _bScanTapes) {
        if (tape->getIsInvalid()) {
            if (!_fileOpened) {
                fillTapes();
            }
            break;
        }
    }
}

void BScanPage::plotPart(int center)
{
    int to = 0;
    int from = 0;
    Registration& registration = Core::instance().registration();
    int lastDisCoord = registration.GetMaxFileCoord();
    int screenSizeInSteps = static_cast<int>(_bscanScale * 1000.0f / _encoderCorrection);
    int screensOffset = static_cast<int>(lastDisCoord / screenSizeInSteps * 4 / 3);
    for (int i = screensOffset; i != 0; i--) {
        to = qMax(0, static_cast<int>(lastDisCoord - screenSizeInSteps * 0.75f * i));
        if (to == 0) {
            Core::instance().videoSetCoord(0);
            for (BScanTape* tape : _bScanTapes) {
                tape->clearTape();
            }
            checkInvalidTapes();
            return;
        }
        from = to - screenSizeInSteps;
        if (from < 0) {
            from = 0;
            to = screenSizeInSteps;
        }
        if ((from <= center) && (center <= to)) {
            _screensMovementOffset = i;
            break;
        }
    }
    for (BScanTape* tape : _bScanTapes) {
        tape->clearTape();
    }
    checkInvalidTapes();
}

void BScanPage::replotViewMode()
{
    int to = 0;
    int from = 0;
    int lastDisCoord = 0;
    Registration& registration = Core::instance().registration();
    Q_ASSERT(_encoderCorrection != 0.0f);
    int screenSizeInSteps = static_cast<int>(_bscanScale * 1000.0f / _encoderCorrection);
    if (!_fileOpened) {
        lastDisCoord = _lastDisCoord;
        to = qMax(0, static_cast<int>(lastDisCoord - screenSizeInSteps * 0.75f * _screensMovementOffset));
        from = qMax(0, (to - screenSizeInSteps));
    }
    else {
        lastDisCoord = registration.GetMaxFileCoord();
        to = qMax(0, static_cast<int>(lastDisCoord - screenSizeInSteps * 0.75f * _screensMovementOffset));
        if (to == 0) {
            qDebug() << "Seek: 0";
            Core::instance().videoSetCoord(0);
            for (BScanTape* tape : _bScanTapes) {
                tape->clearTape();
            }
            checkInvalidTapes();
            return;
        }
        from = to - screenSizeInSteps;
        if (from < 0) {
            from = 0;
            to = screenSizeInSteps;
        }
    }
    int between = from + ((to - from) / 2);
    qDebug() << "Seek:" << between;
    if (_isParallelVideoBrowsing) {
        Core::instance().videoSetCoord(between);
    }
    for (BScanTape* tape : _bScanTapes) {
        tape->clearTape();
    }
    if (from != to) {
        BScanDisplaySpan span;
        if (registration.getSpan(from, to, span)) {
            drawSpan(span);
            if (_fileOpened) {
                Core::instance().setCurrentTrackMark(span.at(0).DisCrd, span.at(0).SysCrd);
                Core::instance().setCurrentTapeModel(span.at(0).DisCrd);
            }
        }
    }
    checkInvalidTapes();
}

void BScanPage::replotRealtimeMode()
{
    Registration& registration = Core::instance().registration();
    Q_ASSERT(_encoderCorrection != 0.0f);
    int screenSizeInSteps = (_bscanScale * 1000.0f) / _encoderCorrection;
    int newDisCoord = registration.GetMaxDisCoord();
    if (newDisCoord - _oldDisCoord > screenSizeInSteps) {
        _oldDisCoord = newDisCoord - screenSizeInSteps;
    }
    if (_oldDisCoord != newDisCoord) {
        BScanDisplaySpan span;
        if (registration.getSpan(_oldDisCoord, newDisCoord, span)) {
            _oldDisCoord = newDisCoord;
            drawSpan(span);
            for (BScanTape* tape : _bScanTapes) {
                if (tape->isVisible()) {
                    if (tape->getIsInvalid()) {
                        if (!_fileOpened) {
                            fillTapes();
                        }
                        return;
                    }
                    tape->replotBScanPlot();
                }
            }
        }
        /*registration.LoadBscanData(_oldDisCoord, newDisCoord);
        _oldDisCoord = newDisCoord;
        for (BScanTape* tape : _bScanTapes) {
            if (tape->isVisible()) {
                if (tape->getIsInvalid()) {
                    fillTapes();
                    return;
                }
                tape->replotBScanPlot();
            }
        }*/
    }
    else {
        checkInvalidTapes();
    }
}

void BScanPage::replotTimeout()
{
    _replotTimer->stop();

    if (!_viewMode) {
        replotRealtimeMode();
    }
    else {
        replotViewMode();
    }

#ifdef TARGET_AVICONDB
    if (ui->bottomSignalAmplWgt->isVisible()) {
        ui->bottomSignalAmplWgt->update();
    }
#endif
    _replotTimer->start();
}

void BScanPage::onStartBoltJoint()
{
    QDEBUG << "onStartBoltJoint()";
    if (_isIncreaseTracks) {
        for (auto& tape : _bScanTapes) {
            tape->setPlotVisible(false);
        }
        for (int i = 0; i < _model->rowCount(); ++i) {
            const QModelIndex& modelIndex = _model->index(i, 0);
            _model->setData(modelIndex, _tapesForBoltJoinMap[i], VisibleRole);
        }

        for (auto& tape : _bScanTapes) {
            tape->setPlotVisible(true);
        }

        _railBscan->setEnabled(false);
        _magnifierBscan->setEnabled(false);
    }
}

void BScanPage::onStopBoltJoint()
{
    QDEBUG << "onStopBoltJoint()";
    resetTapeStates();
    _railBscan->setEnabled(true);
    _magnifierBscan->setEnabled(true);
}

void BScanPage::onSetMinRegistrationThreshold(int value)
{
    if (value == -6 && qRound(_displayThresholdBscan->value()) == -12) {
        for (BScanTape* tape : _bScanTapes) {
            tape->setThreshold(value);
        }
        Core::instance().setBScanDisplayThresholdChanged(value);
        if (!_fileOpened) {
            fillTapes();
        }
        _displayThresholdBscan->setValue(value);
    }
    _displayThresholdBscan->setMinimum(value);
}

void BScanPage::onDataContainerData(pDaCo_DateFileItem pDaCoItem)
{ /*
     Q_ASSERT(pDaCoItem);
     for (BScanTape* tape : _bScanTapes) {
         tape->nextStep(pDaCoItem->Dir);
     }
     for (const tDaCo_BScanSignals& sigs : pDaCoItem->Signals) {
         int key = createKeyFromSideAndChannel(sigs.Side, sigs.Channel);
         if (_channelsMap.contains(key)) {
             const ItemOfMap& itemOfMap = _channelsMap[key];

             if (isChannelZero(sigs.Channel)) {
 #if defined TARGET_AVICON31 || defined TARGET_AVICON31E
                 int startGateLeft, endGateLeft, startGateRight, endGateRight;
 #else
                 int startGate, endGate;
 #endif

 #if defined TARGET_AVICON31 || defined TARGET_AVICON31E
                 Core::instance().channelsController()->getGatesForMsmChannel(startGateLeft, endGateLeft, startGateRight, endGateRight);
 #elif defined TARGET_AVICON15 || defined TARGET_AVICONDB
                 Core::instance().channelsController()->getGatesForMsmChannel(startGate, endGate);
 #endif

                 _bScanTapes[itemOfMap.tapeNumber]->addSignalsN0EMS(sigs.Signals,
 #if defined TARGET_AVICON31 || defined TARGET_AVICON31E
                                                                    (sigs.Side == dsLeft) ? startGateLeft : startGateRight,
                                                                    (sigs.Side == dsLeft) ? endGateLeft : endGateRight,
 #else
                                                                    startGate,
                                                                    endGate,
 #endif
                                                                    sigs.Count,
                                                                    itemOfMap.delayMultiply,
                                                                    itemOfMap.color,
                                                                    getInversion(sigs.Channel));
             }
             else {
                 _bScanTapes[itemOfMap.tapeNumber]->addSignalsNORMAL(sigs.Signals, sigs.Count, itemOfMap.delayMultiply, itemOfMap.color, getInversion(sigs.Channel));
             }
         }
         else {
             qDebug() << "BScanPage: no channel" << sigs.Side << sigs.Channel;
             Q_ASSERT(false);
         }
     }
     for (const tDaCo_Label& label : pDaCoItem->Labels) {
         drawLabel(label, NoneDeviceSide);
     }
     for (unsigned char event : pDaCoItem->Events) {
         drawEvent(event);
     }
     for (const tDaCo_Data& data : pDaCoItem->Data) {
         drawData(data, pDaCoItem->DisCrd);
     }*/
}

void BScanPage::fillTapes()
{
    int disCoordRange = (_bscanScale * 1000.0f) / _encoderCorrection;
    Core& core = Core::instance();
    Registration& registration = core.registration();
    int maxCoord = 0;
    if (Core::instance().getFileName().isEmpty()) {
        maxCoord = registration.GetMaxDisCoord();
    }
    else {
        maxCoord = registration.GetMaxFileCoord();
    }
    int minCoord = qMax(maxCoord - disCoordRange, 0);
    qDebug() << "Filling from :" << minCoord << " to: " << maxCoord;
    for (BScanTape* tape : _bScanTapes) {
        tape->clearTape();
        tape->setIsInvalid(false);
    }
    if (maxCoord != minCoord) {
        BScanDisplaySpan span;
        if (registration.getSpan(minCoord, maxCoord, span)) {
            drawSpan(span);
        }
    }
}

void BScanPage::onMovementModeChanged(bool mode)
{
    qDebug() << "Mode changed to: " << mode;

    if (mode) {
        Core& core = Core::instance();
        _lastDisCoord = core.registration().GetMaxDisCoord();
        QString fileName = Core::instance().getFileName();
        _fileOpened = !fileName.isEmpty();
        if (_fileOpened) {
            _movementControls->setModeButtonTitle(QObject::tr("Close file: \n") + fileName);
            int screenSizeInSteps = static_cast<int>(_bscanScale * 1000.0f / _encoderCorrection);
            int lastDisCoord = Core::instance().registration().GetMaxFileCoord();
            _screensMovementOffset = static_cast<int>(lastDisCoord / screenSizeInSteps * 4 / 3);
            if (retranslateNavigation()) {
                _navigationBscan->show();
            }
        }
    }
    else {
        _screensMovementOffset = 0;
        _navigationBscan->hide();
        replotTimeout();
        if (!_fileOpened) {
            fillTapes();
        }
        if (!Core::instance().getFileName().isEmpty()) {
            Core::instance().closeFile();
            _fileOpened = false;
        }
    }
    _viewMode = mode;
}

void BScanPage::onMovementLeft()
{
    _replotTimer->start();
    _screensMovementOffset++;
}

void BScanPage::onMovementRight()
{
    _replotTimer->start();
    if (_screensMovementOffset > 0) {
        _screensMovementOffset--;
    }
}

void BScanPage::setImportaintAreaOpacity(float value)
{
    int opacity = qRound(255.0f * (value / 100.0f));
    for (BScanTape* tape : _bScanTapes) {
        tape->setOpacity(opacity);
    }
}

void BScanPage::setImportaintAreaColor(const QColor& color)
{
    for (BScanTape* tape : _bScanTapes) {
        tape->setAutoMarksColor(color);
    }
}

void BScanPage::enableParallelVideoBrowsing(bool isEnabled)
{
    _isParallelVideoBrowsing = isEnabled;
    _movementControls->enableParallelVideoBrowsing(isEnabled);
}

void BScanPage::setMode(bool mode)
{
    setControlsAreaVisible(true);
    _movementControls->setMode(mode);
}

void BScanPage::onBottomSignalAmpl(QSharedPointer<int> bottomSignalAmpl)
{
    int* newValue = bottomSignalAmpl.data();
    ui->bottomSignalAmplWgt->setSignalAmplValue(*newValue);
}

void BScanPage::drawSpan(BScanDisplaySpan& span)
{
    for (const BScanDisplayData& currentItem : span) {
        // Q_ASSERT(pDaCoItem);
        for (BScanTape* tape : _bScanTapes) {
            tape->nextStep(currentItem.Dir);
        }
        for (const tDaCo_BScanSignals& sigs : currentItem.Signals) {
            int key = createKeyFromSideAndChannel(sigs.Side, sigs.Channel);
            if (_channelsMap.contains(key)) {
                const ItemOfMap& itemOfMap = _channelsMap[key];

                if (isChannelZero(sigs.Channel)) {
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
                    int startGateLeft, endGateLeft, startGateRight, endGateRight;
#else
                    int startGate, endGate;
#endif

#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
                    Core::instance().channelsController()->getGatesForMsmChannel(startGateLeft, endGateLeft, startGateRight, endGateRight);
#elif defined TARGET_AVICON15 || defined TARGET_AVICONDB
                    Core::instance().channelsController()->getGatesForMsmChannel(startGate, endGate);
#endif

                    _bScanTapes[itemOfMap.tapeNumber]->addSignalsN0EMS(sigs.Signals,
#if defined TARGET_AVICON31 || defined TARGET_AVICON31E
                                                                       (sigs.Side == dsLeft) ? startGateLeft : startGateRight,
                                                                       (sigs.Side == dsLeft) ? endGateLeft : endGateRight,
#else
                                                                       startGate,
                                                                       endGate,
#endif
                                                                       sigs.Count,
                                                                       itemOfMap.delayMultiply,
                                                                       itemOfMap.color,
                                                                       getInversion(sigs.Channel));
                }
                else {
                    _bScanTapes[itemOfMap.tapeNumber]->addSignalsNORMAL(sigs.Signals, sigs.Count, itemOfMap.delayMultiply, itemOfMap.color, getInversion(sigs.Channel));
                }
            }
            else {
                qDebug() << "BScanPage: no channel" << sigs.Side << sigs.Channel;
                Q_ASSERT(false);
            }
        }
        if (currentItem.Metadata.get() != nullptr) {
            for (const tDaCo_Label& label : currentItem.Metadata.get()->Labels) {
                drawLabel(label, NoneDeviceSide);
            }
            for (unsigned char event : currentItem.Metadata.get()->Events) {
                drawEvent(event);
            }
            for (const tDaCo_Data& data : currentItem.Metadata.get()->Data) {
                drawData(data, currentItem.DisCrd);
            }
        }
    }
}

void BScanPage::drawLabel(const tDaCo_Label& label, DeviceSide side)
{
    switch (label.id) {
    case EID_ChangeOperatorName:
        addMarker(_labelColor, tr("Operator") + " " + label.Text, side, 0);
        break;
    case EID_Switch:
        addMarker(_labelColor, label.Text, side, 0);
        break;
    case MEID_LeftRail_DefLabel:
        addMarker(_labelColor, tr("Defect") + " " + label.Text, LeftDeviceSide, 0);
        break;
    case MEID_RightRail_DefLabel:
        addMarker(_labelColor, tr("Defect") + " " + label.Text, RightDeviceSide, 0);
        break;
    case MEID_DefLabel:
        addMarker(_labelColor, tr("Defect") + " " + label.Text, NoneDeviceSide, 0);
        break;
    case EID_TextLabel:
        addMarker(_labelColor, label.Text, side, 0);
        break;
    default:
        QDEBUG << "Unknown label!" << label.id;
        break;
    }
}

void BScanPage::drawEvent(unsigned char event)
{
    switch (event) {
    case EID_HandScan:
        addMarker(_labelColor, tr("Hand"), NoneDeviceSide, 0);
        break;
    case EID_RailHeadScaner:
        addMarker(_labelColor, tr("Rail head scan"), NoneDeviceSide, 0);
        break;
    case EID_StartSwitchShunter:
        addMarker(_labelColor, tr("Start switch shunter"), NoneDeviceSide, 0);
        break;
    case EID_EndSwitchShunter:
        addMarker(_labelColor, tr("End switch shunter"), NoneDeviceSide, 0);
        break;
    case EID_StBoltStyk:
        addMarker(_labelColor, tr("BJ(on)"), NoneDeviceSide, 0);
        break;
    case EID_EndBoltStyk:
        addMarker(_labelColor, tr("BJ(off)"), NoneDeviceSide, 0);
        break;
    case EID_SetRailType:
        addMarker(_labelColor, tr("Rail type"), NoneDeviceSide, 0);
        break;
    default:
        QDEBUG << "Unknown event!";
        break;
    }
}

void BScanPage::drawData(const tDaCo_Data& data, int disCoord)
{
    switch (data.id) {
    case EID_Media:
        addMarker(_labelColor, getMediaName(static_cast<tMediaType>(data.Value[0])), NoneDeviceSide, 0);
        break;
    case EID_Stolb:
        if (data.Value[0] == data.Value[1]) {
            addMarker(_labelColor, QString::number(data.Value[2]) + "/" + QString::number(data.Value[3]) + tr(" pk "), NoneDeviceSide, 0);
        }
        else {
            addMarker(_labelColor, QString::number(data.Value[0]) + "/" + QString::number(data.Value[1]) + tr(" KM "), NoneDeviceSide, 0);
        }
        break;
    case EID_QualityCalibrationRec:
        addMarker(_labelColor, getCalibrationQualityMsg(static_cast<tQualityCalibrationRecType>(data.Value[1])), static_cast<DeviceSide>(data.Value[0]), 20);
        break;
    case EID_AutomaticSearchRes: {
        if (_importaintAreaIndication) {
            int centr = data.Value[2];
            int width = data.Value[3];

            int posx = centr - width / 2;
            unsigned char posy1 = static_cast<unsigned char>(data.Value[4]);
            unsigned char posy2 = static_cast<unsigned char>(data.Value[5]);

            addRectangle(disCoord - posx, width, posy1, posy2, static_cast<DeviceSide>(data.Value[0]), data.Value[1]);
        }
    } break;
    case EID_StolbChainage:
        addMarker(_labelColor, getCoordMetric(data.Value[0], data.Value[1]), NoneDeviceSide, 0);
        break;
    default:
        QDEBUG << "Unknown event with data!";
        break;
    }
}

QString BScanPage::getMediaName(tMediaType type)
{
    switch (type) {
    case mtAudio:
        return tr("Audio");
    case mtPhoto:
        return tr("Photo");
    case mtVideo:
        return tr("Video");
    }
    return "Unknown";
}

QString BScanPage::getCoordMetric(int km, int m)
{
    QString xxx = QString::number(qAbs(km));

    if (xxx.size() < 3) {
        int znum = 3 - xxx.size();
        if (znum == 2) {
            xxx.prepend("0");
            xxx.prepend("0");
        }
        else if (znum == 1) {
            xxx.prepend("0");
        }
    }
    if (km < 0) {
        xxx.prepend("-");
    }

    QString yyy = QString::number(m);
    if (yyy.size() < 3) {
        int znum = 3 - yyy.size();
        if (znum == 2) {
            yyy.prepend("0");
            yyy.prepend("0");
        }
        else if (znum == 1) {
            yyy.prepend("0");
        }
    }

    return xxx + "." + yyy;
}

void BScanPage::resetTapeStates()
{
    QDEBUG << "resetTapeStates()";
    _railBscan->setIndex(0);
    _currentRailFilter = NoneDeviceSide;
    _magnifierBscan->setIndex(0);
    _currentMagnifierFilter = 0;
    if (_model == nullptr) {
        return;
    }
    for (auto& tape : _bScanTapes) {
        tape->setPlotVisible(false);
    }
    for (auto& tape : _bScanTapes) {
        tape->setVisible(true);
    }
    for (auto& tape : _bScanTapes) {
        tape->setPlotVisible(true);
    }
    for (int i = 0; i < _model->rowCount(); ++i) {
        const QModelIndex& modelIndex = _model->index(i, 0);
        _model->setData(modelIndex, true, VisibleRole);
    }
    resetBscans();
}

void BScanPage::addMarker(const QColor& color, const QString& markerText, DeviceSide side, int vOffset)
{
    bool isFirstChannel = true;
    unsigned char size = _bScanTapes.size();
    for (unsigned char i = 0; i < size; ++i) {
        if (side == NoneDeviceSide) {
            _bScanTapes[i]->addMarker(color, markerText, i == 0, vOffset);
        }
        else if (side == _bScanTapes[i]->getSide()) {
            _bScanTapes[i]->addMarker(color, markerText, isFirstChannel, vOffset);
            isFirstChannel = false;
        }
    }
}

void BScanPage::addRectangle(int offset, int width, unsigned char posy1, unsigned char posy2, DeviceSide side, CID channel)
{
    Q_ASSERT(channel >= 0);
    int key = createKeyFromSideAndChannel(side, channel);

    if (_channelsMap.contains(key)) {
        const ItemOfMap& itemOfMap = _channelsMap[key];

        CurveByDirectionAndEdge::EdgesType currentEdge = static_cast<CurveByDirectionAndEdge::EdgesType>(_currentEdge.toInt());
        CurveByDirectionAndEdge::EdgesType itemOfMapEdge = static_cast<CurveByDirectionAndEdge::EdgesType>(itemOfMap.edge);

        if ((currentEdge == CurveByDirectionAndEdge::WorkEdgeType && itemOfMapEdge == CurveByDirectionAndEdge::UnworkEdgeType) || (currentEdge == CurveByDirectionAndEdge::UnworkEdgeType && itemOfMapEdge == CurveByDirectionAndEdge::WorkEdgeType)) {
            return;
        }
        else {
            _bScanTapes[itemOfMap.tapeNumber]->addRectangle(offset, width, posy1, posy2, itemOfMap.delayMultiply);
            _rectID++;
        }
    }
    else {
        qDebug() << "Cannot add rect to channel:" << channel;
    }
}

void BScanPage::resetBscans()
{
    for (BScanTape* tape : _bScanTapes) {
        tape->clearTape();
        tape->setIsInvalid(true);
    }
}

void BScanPage::setProbePulseLocation(bool isProbePulseFromAbove)
{
    for (BScanTape* tape : _bScanTapes) {
        tape->setProbePulseLocation(isProbePulseFromAbove);
    }
    _isProbePulseFromAbove = isProbePulseFromAbove;
}

void BScanPage::plugTheBscan(bool isPlug)
{
    QDEBUG << "Plug BScan: " << isPlug;
    Core& core = Core::instance();
    _replotTimer->stop();
    _encoderCorrection = encoderCorrection();
    for (BScanTape* tape : _bScanTapes) {
        tape->clearTape();
        tape->setEncoderCorrection(_encoderCorrection);
    }
    if (isPlug) {
        _lastDisCoord = core.registration().GetMaxDisCoord();
        _replotTimer->start();
        replotTimeout();
        if (!_fileOpened) {
            fillTapes();
        }
    }
}

void BScanPage::setIncreaseTracks(bool isIncreaseTracks)
{
    _isIncreaseTracks = isIncreaseTracks;
}

void BScanPage::setAdditiveForSens(int additiveForSens)
{
    Q_ASSERT(additiveForSens < 100);
    _additiveForSens = additiveForSens;
}

void BScanPage::onHotKeyReleased(Actions action)
{
    if (this->isVisible() == false) {
        return;
    }

    switch (action) {
    case BScanLevelMinus:
        _displayThresholdBscan->setValue(_displayThresholdBscan->value() - _displayThresholdBscan->stepBy());
        break;
    case BScanLevelPlus:
        _displayThresholdBscan->setValue(_displayThresholdBscan->value() + _displayThresholdBscan->stepBy());
        break;
    default:
        break;
    }
}

void BScanPage::detachModel()
{
    QDEBUG << "detachModel()";
    _model->disconnect(this);
    _model = nullptr;
    for (BScanTape* tape : _bScanTapes) {
        tape->setParent(nullptr);
        delete tape;
    }
    _bScanTapes.clear();
    for (QWidget* item : _separators) {
        item->setParent(nullptr);
        delete item;
    }
    resetBscans();
    _separators.clear();
}

QString BScanPage::getCalibrationQualityMsg(tQualityCalibrationRecType type)
{
    switch (type) {
    case qrtNoJoint:
        return tr("Joint not found");
    case qrtFailByBoltedJoint:
        return tr("Invalid bolt joint");
    case qrtFailByAluminothermicJoint:
        return tr("Invalid aluminothermic weld");
    case qrtValidByBoltedJoint:
        return tr("Valid bolt joint");
    case qrtValidByAluminothermicJoint:
        return tr("Valid aluminothermic weld");
    case qrtAluminothermicJoint:
        return tr("Aluminothermic joint");
    }
    return QString();
}


bool BScanPage::getImportaintAreaIndication() const
{
    return _importaintAreaIndication;
}

void BScanPage::setImportaintAreaIndication(bool importaintAreaIndication)
{
    _importaintAreaIndication = importaintAreaIndication;
}

void BScanPage::setBottomSignalWigetVisible(bool state)
{
    ui->bottomSignalAmplWgt->setVisible(state);
}

void BScanPage::setMovementsMode(bool mode)
{
    Q_ASSERT(_movementControls);
    if (_movementControls != nullptr) {
        _movementControls->setMode(mode);
    }
}
