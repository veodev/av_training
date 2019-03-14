#include <QDir>
#include <QProcess>
#include <QDebug>

#include "wifimanager.h"

static QString gOutput;
static bool runProcess(const QString& program, QString& output = gOutput)
{
    qDebug() << "program:[" << program << "]";

    QProcess process;

    process.start("sh");
    process.waitForStarted();
    process.write(program.toLocal8Bit());
    process.closeWriteChannel();
    process.waitForFinished();
    if (process.exitCode()) {
        const QByteArray& error = process.readAllStandardError();
        qWarning() << error;
        return false;
    }
    output = process.readAllStandardOutput();

    qDebug() << Q_FUNC_INFO << "[" << output << "]";

    return true;
}

// static bool runProcessPkExec(const QString & program, QString & output = gOutput)
//{
//    return runProcess(QString("pkexec %1").arg(program), output);
//}

#if defined(IMX_DEVICE)
static const QString IWCONFIG("/sbin/iwconfig");
static const QString IFCONFIG("/sbin/ifconfig");
static const QString IWLIST("/sbin/iwlist");
static const QString DHCPC("/sbin/udhcpc -n -f -q -i");

#define RUN_PROCESS runProcess
#else
static const QString IWCONFIG("/usr/sbin/iwconfig");
static const QString IFCONFIG("/sbin/ifconfig");
static const QString IWLIST("/usr/sbin/iwlist");
static const QString DHCPC("/sbin/udhcpc -n -f -q -i");
//    static const QString DHCPC("/sbin/dhclient");

static bool runProcessBySudoWithPassword(const QString& program, QString& output = gOutput)
{
    return runProcess(QString("echo '123456\n' | sudo -S %1").arg(program), output);
}

#define RUN_PROCESS runProcessBySudoWithPassword
#endif

struct WiFiAccessPoint
{
    QString address;
    QString ssid;
    double quality;
    int channel;
    float frequency;
    QString encryption;
};

WifiManager::WifiManager(QObject* parent)
    : QObject(parent)
    , _currentInterface(-1)
    , _isConnect(false)
{
}

void WifiManager::scanInterfaces()
{
    QString interfacesDir("/sys/class/net/");
    QDir dir(interfacesDir);
    const QStringList& files = dir.entryList();
    _ifNames.clear();
    bool found = false;
    for (const QString& file : files) {
        if (file == "." || file == ".." || file == "lo") {
            continue;
        }
        dir.setPath(interfacesDir + file + "/phy80211");
        if (dir.exists()) {
            found = true;
            _ifNames.append(file);
        }
    }

    if (!found) {
        qDebug() << "WiFi phy80211 not found!";
    }
}

QStringList WifiManager::availableInterfaces() const
{
    return _ifNames;
}

bool WifiManager::isInterfaceConnected(int interfaceIndex)
{
    if (correctInterfaceIndex(interfaceIndex) == -1) {
        return false;
    }

    QString operstateFilename = QString("/sys/class/net/%1/operstate").arg(_ifNames[interfaceIndex]);
    QFile operstate(operstateFilename);
    if (operstate.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
        qWarning() << "Can't open file" << operstateFilename;
        return false;
    }

    QString state(operstate.readAll());
    operstate.close();
    if (state.length() == 0) {
        qWarning() << "File is empty" << operstateFilename;
        return false;
    }

    if (state == "up\n") {
        return true;
    }

    return false;
}

QString WifiManager::getInetAddrForInterface(int interfaceIndex)
{
    QString inetAddr;

    if (correctInterfaceIndex(interfaceIndex) != -1) {
        RUN_PROCESS(QString("/sbin/ifconfig %1 | awk '/inet/{print $2}'").arg(_ifNames[interfaceIndex]), inetAddr);
        inetAddr.remove(QRegExp("[\n\t\r]"));
    }

    return inetAddr;
}

QString WifiManager::getEssidForInterface(int interfaceIndex)
{
    QString essid;

    if (correctInterfaceIndex(interfaceIndex) != -1) {
        RUN_PROCESS(QString("%1 %2 | awk '/ESSID/{print $4}' | awk -F\":\" '{print $2}' | sed 's/\\\"//g'").arg(IWCONFIG).arg(_ifNames[interfaceIndex]), essid);
        essid.remove(QRegExp("[\n\t\r]"));
    }

    return essid;
}


bool WifiManager::enableInterface(bool value, int interfaceIndex)
{
    if (correctInterfaceIndex(interfaceIndex) == -1) {
        return false;
    }

    return RUN_PROCESS(QString("%1 %2 %3").arg(IFCONFIG).arg(_ifNames[interfaceIndex]).arg(value ? "up" : "down"));
}

bool WifiManager::setInetAddress(const QString& addr, int interfaceIndex)
{
    if (correctInterfaceIndex(interfaceIndex) == -1) {
        return false;
    }

    return RUN_PROCESS(QString("%1 %2 %3").arg(IFCONFIG).arg(_ifNames[interfaceIndex]).arg(addr));
}

void WifiManager::scanAcessPoints(int interfaceIndex)
{
    static const QString addressPattern("Address: ");
    static const QString channelPattern("Channel:");
    static const QString frequencyPattern("Frequency:");
    static const QString qualityPattern("Quality=");
    static const QString encryptionKeyPattern("Encryption key:");
    static const QString essidPattern("ESSID:");

    apClear();

    if (correctInterfaceIndex(interfaceIndex) == -1) {
        return;
    }
    _currentInterface = interfaceIndex;

    if (RUN_PROCESS(QString("%1 %2 up").arg(IFCONFIG).arg(_ifNames[_currentInterface])) == false) {
        return;
    }

    QString output;
    RUN_PROCESS(QString("%1 %2 scan").arg(IWLIST).arg(_ifNames[interfaceIndex]), output);
    const QStringList& list = output.split("\n");

    WiFiAccessPoint* ap = 0;
    for (const QString& item : list) {
        int index = item.indexOf(addressPattern);
        if (index == 20) {
            ap = new WiFiAccessPoint;
            ap->address = item.mid(index + addressPattern.length());
            ap->ssid = "";
            ap->quality = 0;
            ap->channel = 0;
            ap->frequency = 0;
            ap->encryption = "";
            _apList.append(ap);
        }
        if (ap != 0) {
            if ((index = item.indexOf(channelPattern)) != -1 && index == 20) {
                ap->channel = item.mid(index + channelPattern.length()).toInt();
            }
            else if ((index = item.indexOf(frequencyPattern)) != -1 && index == 20) {
                QString str = item.mid(index + frequencyPattern.length());
                index = str.indexOf(" ");
                if (index != -1) {
                    ap->frequency = str.left(index).toFloat();
                }
            }
            else if ((index = item.indexOf(qualityPattern)) != -1 && index == 20) {
                QString str = item.mid(index + qualityPattern.length());
                index = str.indexOf(" ");
                if (index != -1) {
                    str = str.left(index);
                    QStringList strList = str.split("/");
                    if (strList.count() == 2) {
                        ap->quality = static_cast<double>((strList[0].toInt()) / static_cast<double>(strList[1].toInt()));
                    }
                }
            }
            else if ((index = item.indexOf(encryptionKeyPattern)) != -1 && index == 20) {
                ap->encryption = item.mid(index + encryptionKeyPattern.length());
            }
            else if ((index = item.indexOf(essidPattern)) != -1 && index == 20) {
                ap->ssid = item.mid(index + essidPattern.length());
            }
        }
    }

#ifdef DEBUG
    qDebug() << "ssid"
             << "quality"
             << "encryption"
             << "address"
             << "channel"
             << "frequency";
    foreach (WiFiAccessPoint* ap, _apList) {
        qDebug() << ap->ssid << ap->quality << ap->encryption << ap->address << ap->channel << ap->frequency << "MHz";
    }
#endif
}

bool WifiManager::connectToNetwork(int apIndex)
{
    if (correctInterfaceIndex(_currentInterface) == -1) {
        return false;
    }
    if (apIndex < 0 || apIndex >= _apList.count()) {
        return false;
    }

    if (RUN_PROCESS(QString("%1 %2 essid %3").arg(IWCONFIG).arg(_ifNames[_currentInterface]).arg(_apList[apIndex]->ssid)) == false) {
        return false;
    }

    if (RUN_PROCESS(QString("%1 %2").arg(DHCPC).arg(_ifNames[_currentInterface])) == false) {
        return false;
    }

    _isConnect = true;
    return false;
}

bool WifiManager::disconnectFromToNetwork()
{
    if (_currentInterface >= 0) {
        Q_ASSERT(_currentInterface < _ifNames.count());
        if (RUN_PROCESS(QString("%1 %2 essid \"\"").arg(IWCONFIG).arg(_ifNames[_currentInterface])) == false) {
            return false;
        }
        if (RUN_PROCESS(QString("%1 %2 0.0.0.0").arg(IFCONFIG).arg(_ifNames[_currentInterface])) == false) {
            return false;
        }

        //    if (RUN_PROCESS(QString("%1 %2 down").arg(IFCONFIG).arg(_ifNames[_currentInterfaceIndex])) == false) {
        //        return false;
        //    }
        _isConnect = false;
    }
    return true;
}

int WifiManager::apCount()
{
    return _apList.count();
}

QString WifiManager::address(int index)
{
    Q_ASSERT(index >= 0 && index < _apList.count());

    return _apList[index]->address;
}

QString WifiManager::ssid(int index)
{
    Q_ASSERT(index >= 0 && index < _apList.count());

    return _apList[index]->ssid;
}

qreal WifiManager::quality(int index)
{
    Q_ASSERT(index >= 0 && index < _apList.count());

    return _apList[index]->quality;
}

int WifiManager::channel(int index)
{
    Q_ASSERT(index >= 0 && index < _apList.count());

    return _apList[index]->channel;
}

qreal WifiManager::frequency(int index)
{
    Q_ASSERT(index >= 0 && index < _apList.count());

    return _apList[index]->frequency;
}

QString WifiManager::encryption(int index)
{
    Q_ASSERT(index >= 0 && index < _apList.count());

    return _apList[index]->encryption;
}

qreal WifiManager::getQuality()
{
    if (_isConnect) {
        QString qualityPattern("Quality=");
        QString output;
        RUN_PROCESS(QString("%1 %2").arg(IWCONFIG).arg(_ifNames[_currentInterface]), output);
        const QStringList& strList = output.split("\n");
        const QStringList& strListFiltered = strList.filter(qualityPattern);
        if (strListFiltered.isEmpty() == false) {
            const QString& qualityStr = strListFiltered.first();
            int index1 = qualityStr.indexOf(qualityPattern) + qualityPattern.length();
            int index2 = qualityStr.indexOf(" ", index1);
            const QStringList& strList2 = qualityStr.mid(index1, (index2 - index1)).split("/");
            qreal quality = (static_cast<qreal>(strList2[0].toInt()) / strList2[1].toInt()) * 100;
            return quality;
        }
    }
    return -1;
}

int WifiManager::correctInterfaceIndex(int& interfaceIndex)
{
    if (interfaceIndex == -1) {
        if (_currentInterface == -1) {
            scanInterfaces();
            if (_ifNames.count() > 0) {
                _currentInterface = 0;
            }
        }
        interfaceIndex = _currentInterface;
    }

    if (interfaceIndex == -1) {
        qWarning() << "Incorrect interface index";
    }

    return interfaceIndex;
}

void WifiManager::apClear()
{
    for (WiFiAccessPoint* item : _apList) {
        if (item != 0) {
            delete item;
        }
    }
    _apList.clear();
}
