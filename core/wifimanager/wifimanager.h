#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <QObject>
#include <QStringList>

struct WiFiAccessPoint;
class WifiManager : public QObject
{
    Q_OBJECT

public:
    explicit WifiManager(QObject* parent = 0);

    void scanInterfaces();
    QStringList availableInterfaces() const;
    bool isInterfaceConnected(int interfaceIndex = -1);
    QString getInetAddrForInterface(int interfaceIndex = -1);
    QString getEssidForInterface(int interfaceIndex = -1);
    bool enableInterface(bool value = true, int interfaceIndex = -1);
    bool setInetAddress(const QString& addr, int interfaceIndex = -1);

    void scanAcessPoints(int interfaceIndex = -1);
    bool connectToNetwork(int apIndex = -1);
    bool disconnectFromToNetwork();
    int apCount();
    QString address(int index);
    QString ssid(int index);
    qreal quality(int index);
    int channel(int index);
    qreal frequency(int index);
    QString encryption(int index);
    qreal getQuality();

protected:
    int correctInterfaceIndex(int& interfaceIndex);
    void apClear();

private:
    QStringList _ifNames;
    int _currentInterface;
    QList<WiFiAccessPoint*> _apList;
    bool _isConnect;
};

#endif  // WIFIMANAGER_H
