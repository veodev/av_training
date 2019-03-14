#include "accessories.h"

QStringList convertToQStringList(const QListWidget& listWidget)
{
    QStringList stringList;

    for (int i = 0; i < listWidget.count(); ++i) {
        stringList.append(listWidget.item(i)->text());
    }

    return stringList;
}

QStringList convertToQStringList(const QList<QListWidgetItem*>& list)
{
    QStringList stringList;

    for (const QListWidgetItem* item : list) {
        stringList.append(item->text());
    }

    return stringList;
}

void delay(int msToWait)
{
    QTime stopTime = QTime::currentTime().addMSecs(msToWait);
    while (QTime::currentTime() < stopTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

QString convertBytesToView(double bytes)
{
    QString sizeStr;
    if (bytes > 0 && bytes < 1024) {
        sizeStr = QString::number(bytes) + QString(QObject::tr(" B"));
    }
    else if (bytes >= 1024 && bytes <= 1048575) {
        sizeStr = QString::number(bytes / 1024, 'f', 1) + QString(QObject::tr(" kB"));
    }
    else if (bytes > 1048575 && bytes <= 1073741823) {
        sizeStr = QString::number(bytes / 1048576, 'f', 1) + QString(QObject::tr(" MB"));
    }
    else if (bytes > 1073741823 && bytes <= 1099511627775) {
        sizeStr = QString::number(bytes / 1073741824, 'f', 1) + QString(QObject::tr(" GB"));
    }
    else {
        sizeStr = QString::number(bytes);
    }
    return sizeStr;
}

int createKeyFromSchemeAndSideAndChannelAndGateIndex(const int scheme, const int side, const int channelId, const int gateIndex)
{
    Q_ASSERT(channelId >= 0);
    Q_ASSERT(gateIndex == 0 || gateIndex == 1);
    Q_ASSERT(scheme == 0 || scheme == 1);
    Q_ASSERT(side == 0 || side == 1 || side == 2);
    return ((((((scheme << 8) | side) << 8) | channelId) << 8) | gateIndex);
}

int createKeyFromSideAndChannel(const int side, const int channelId)
{
    Q_ASSERT(side == 0 || side == 1 || side == 2);
    Q_ASSERT(channelId >= 0);
    return (((side & 0xff) << 8) | channelId);
}

int createKeyFromSideAndChannelAndGateIndex(const int side, const int channelId, const int gateIndex)
{
    Q_ASSERT(side == 0 || side == 1 || side == 2);
    Q_ASSERT(channelId >= 0);
    Q_ASSERT(gateIndex == 0 || gateIndex == 1);
    return (((((side << 8) | channelId)) << 8) | gateIndex);
}
