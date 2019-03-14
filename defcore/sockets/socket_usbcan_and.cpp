#include <QDebug>
#include "Export.h"
#include "socket_usbcan_and.h"

cSocketUSBCanAnd::cSocketUSBCanAnd() :cISocket() {
    bum::init();
}

bool cSocketUSBCanAnd::connect(const cConnectionParams *) {
    bool res = bum::startAsync();
    if (res) _socket = 0;
    return res;
}

void cSocketUSBCanAnd::disconnect() {
    if (_socket < 0) {
        return;
    }
    _socket = -1;
    bum::finishAsync();
}

int cSocketUSBCanAnd::reciveData(unsigned char * msg, const int length) {
    return bum::Read(reinterpret_cast<char *>(msg), length, 0);
}

bool cSocketUSBCanAnd::sendData(const unsigned char * msg, const int length) {
    const int n = bum::Write(reinterpret_cast<const char *>(msg), length);
    return n;
}
