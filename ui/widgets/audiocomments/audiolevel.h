#ifndef AUDIOLEVEL_H
#define AUDIOLEVEL_H

#include <QWidget>
#include <QAudioBuffer>

QVector<qreal> getBufferLevels(const QAudioBuffer & buffer);

class AudioLevel : public QWidget
{
    Q_OBJECT
public:
    explicit AudioLevel(QWidget * parent = 0);

    // Using [0; 1.0] range
    void setLevel(qreal level);

protected:
    void paintEvent(QPaintEvent * event);

private:
    qreal _level;
};

#endif // AUDIOLEVEL_H
