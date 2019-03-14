#include <QAudioProbe>
#include <QAudioRecorder>
#include <QDir>
#include <QDateTime>

#include "audiorecorderpage.h"
#include "ui_audiorecorderpage.h"
#include "audiolevel.h"
#include "appdatapath.h"
#include "debug.h"
#include "core.h"
#include "alarmtone.h"
#include "registration.h"

AudioRecorderPage::AudioRecorderPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AudioRecorderPage)
    , _audioRecorder(new QAudioRecorder())
    , _audioProbe(new QAudioProbe())
{
    ui->setupUi(this);

    ASSERT(connect(_audioProbe, &QAudioProbe::audioBufferProbed, this, &AudioRecorderPage::processBuffer));
    _audioProbe->setSource(_audioRecorder);

    ASSERT(connect(_audioRecorder, &QAudioRecorder::durationChanged, this, &AudioRecorderPage::updateProgress));
    ASSERT(connect(_audioRecorder, &QAudioRecorder::statusChanged, this, &AudioRecorderPage::updateStatus));
    ASSERT(connect(_audioRecorder, &QAudioRecorder::stateChanged, this, &AudioRecorderPage::updateState));
    ASSERT(connect(_audioRecorder, QOverload<QAudioRecorder::Error>::of(&QAudioRecorder::error), this, &AudioRecorderPage::displayErrorMessage));

    ui->recordButton->setIconSize(QSize(32, 32));
    ui->recordButton->setIcon(QPixmap(":/record_black_18x32.png"));
    ui->recordButton->setText("");

    ui->pauseButton->setIconSize(QSize(32, 32));
    ui->pauseButton->setIcon(QPixmap(":/media_pause_black_32x32.png"));
    ui->pauseButton->setText("");
}

AudioRecorderPage::~AudioRecorderPage()
{
    _audioRecorder->stop();

    QAudioRecorder* nullPtrTemp = Q_NULLPTR;
    _audioProbe->setSource(nullPtrTemp);
    disconnect(_audioProbe, &QAudioProbe::audioBufferProbed, this, &AudioRecorderPage::processBuffer);
    delete _audioProbe;
    delete _audioRecorder;

    delete ui;
}

void AudioRecorderPage::setRecordingEnabled(bool value)
{
    if (value == true) {
        ui->recordButton->setEnabled(true);
    }
    else {
        stopRecording();
        ui->recordButton->setDisabled(true);
    }
}
/*
void AudioRecorderPage::supports()
{
    // audio devices
    foreach (const QString& device, _audioRecorder->audioInputs()) {
        QDEBUG << "audio device:" << device;
    }

    // audio codecs
    foreach (const QString& codecName, _audioRecorder->supportedAudioCodecs()) {
        QDEBUG << "audio codec:" << codecName;
    }

    // containers
    foreach (const QString& containerName, _audioRecorder->supportedContainers()) {
        QDEBUG << "container:" << containerName;
    }

    // sample rate
    foreach (int sampleRate, _audioRecorder->supportedAudioSampleRates()) {
        QDEBUG << "sample rate" << sampleRate;
    }
}
*/
void AudioRecorderPage::setVisible(bool visible)
{
    if (visible == false) {
        _audioRecorder->stop();
        Core::instance().resumeAudioOutput();
    }
    else {
        Core::instance().stopAudioOutput();
    }

    QWidget::setVisible(visible);
}

void AudioRecorderPage::processBuffer(const QAudioBuffer& buffer)
{
    QVector<qreal> levels = getBufferLevels(buffer);

    for (int i = 0; i < levels.count(); ++i) {
        if (i < _audioLevels.count()) {
            _audioLevels.at(i)->setLevel(levels.at(i));
        }
    }
}

bool AudioRecorderPage::event(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    return QWidget::event(e);
}

void AudioRecorderPage::updateProgress(qint64 duration)
{
    if (_audioRecorder->error() != QMediaRecorder::NoError || duration < 2000) {
        return;
    }

    ui->status->setText(tr("Recorded %1 sec").arg(duration / 1000));
}

void AudioRecorderPage::updateStatus(QMediaRecorder::Status status)
{
    QString statusMessage;

    QDEBUG << Q_FUNC_INFO << "status:" << status;

    switch (status) {
    case QMediaRecorder::RecordingStatus:
    case QMediaRecorder::StartingStatus:
        if (_audioLevels.count() != _audioRecorder->audioSettings().channelCount()) {
            qDeleteAll(_audioLevels);
            _audioLevels.clear();
            for (int i = 0; i < _audioRecorder->audioSettings().channelCount(); ++i) {
                AudioLevel* level = new AudioLevel(this);
                _audioLevels.append(level);
                ui->levelsLayout->addWidget(level);
            }
        }

        ui->recordButton->setIcon(QPixmap(":/media_stop_black_32x32.png"));
        ui->pauseButton->setIcon(QPixmap(":/media_pause_black_32x32.png"));
        statusMessage = tr("Recording");
        break;
    case QMediaRecorder::PausedStatus:
        clearAudioLevels();
        ui->recordButton->setIcon(QPixmap(":/media_stop_black_32x32.png"));
        ui->pauseButton->setIcon(QPixmap(":/record_red_18x32.png"));
        statusMessage = tr("Paused");
        break;
    case QMediaRecorder::FinalizingStatus:
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        clearAudioLevels();
        ui->recordButton->setIcon(QPixmap(":/record_black_18x32.png"));
        ui->pauseButton->setIcon(QPixmap(":/media_pause_black_32x32.png"));
        statusMessage = tr("Stopped");
    default:
        break;
    }

    ui->pauseButton->setEnabled(_audioRecorder->state() != QMediaRecorder::StoppedState);

    if (_audioRecorder->error() == QMediaRecorder::NoError) {
        ui->status->setText(statusMessage);
    }
}

void AudioRecorderPage::updateState(QMediaRecorder::State state)
{
    QDEBUG << Q_FUNC_INFO << state;

    if (state == QMediaRecorder::StoppedState) {
        Core::instance().registration().addAudioComment(_audioRecorder->outputLocation().toString());
    }
}

void AudioRecorderPage::toggleRecord()
{
    if (_audioRecorder->state() == QMediaRecorder::StoppedState) {
        _audioRecorder->setAudioInput("alsa:sysdefault:CARD=imx3stack");

        QAudioEncoderSettings settings;
        settings.setCodec("audio/PCM");
        settings.setSampleRate(0);
        settings.setBitRate(32000);
        settings.setChannelCount(1);
        settings.setQuality(QMultimedia::NormalQuality);
        settings.setEncodingMode(QMultimedia::ConstantQualityEncoding);

        _audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), "wav");
        _audioRecorder->setOutputLocation(commentsPath() + QDateTime::currentDateTime().toString("yyMMdd-hhmmss") + QString(".wav"));
        QDEBUG << "outputLocation:" << _audioRecorder->outputLocation();
        _audioRecorder->record();
    }
    else {
        _audioRecorder->stop();
    }
}

void AudioRecorderPage::togglePause()
{
    if (_audioRecorder->state() != QMediaRecorder::PausedState) {
        _audioRecorder->pause();
    }
    else {
        _audioRecorder->record();
    }
}

void AudioRecorderPage::displayErrorMessage(QMediaRecorder::Error error)
{
    Q_UNUSED(error);
    ui->status->setText(_audioRecorder->errorString());
}

void AudioRecorderPage::stopRecording()
{
    _audioRecorder->stop();
    Core::instance().alarmTone()->resume();
}

void AudioRecorderPage::clearAudioLevels()
{
    for (int i = 0; i < _audioLevels.size(); ++i) {
        _audioLevels.at(i)->setLevel(0);
    }
}

void AudioRecorderPage::on_recordButton_released()
{
    toggleRecord();
}

void AudioRecorderPage::on_pauseButton_released()
{
    togglePause();
}
