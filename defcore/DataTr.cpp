#include <stdio.h>
#include <string.h>

#include "DataTr.h"
#include "TickCount.h"

cDataTransfer::cDataTransfer()
{
    ActiveUMU = - 1;
}

cDataTransfer::~cDataTransfer()
{
    Finish();
}

//
// возвращает неотрицательное число - индекс  созданного соединения в vConnections[]
// отрицательное число - код ошибки
// - 1 - сокеты не были созданы, либо соединение не установлено
//  -2 - приемный буфер не был выделен
// localIP[] может быть пустой строкой
int cDataTransfer::AddConnection(char localIP[16], char IP[16], int PortIn, int PortOut, bool PortInTCP, bool PortOutTCP)
{
    SOCKT* Server = NULL;
    SOCKT* Host = NULL;
    bool res = false;
    int result;
    char* pLocalIP;
    if (localIP[0] != 0) {
        pLocalIP = localIP;
    }
    else {
        pLocalIP = NULL;
    }
    if (PortOutTCP) {
        Host = dynamic_cast<TCP_SOCKET*> (new TCP_SOCKET);
    }
    else {
        Host = dynamic_cast<UDP_SOCKET*> (new UDP_SOCKET);
    }
    if (PortOut == PortIn) {
        if (PortOutTCP) {
            Server = dynamic_cast<TCP_SOCKET*> (Host);
        }
        else {
            Server = dynamic_cast<UDP_SOCKET*> (Host);
        }
    }
    else {
        if (PortInTCP) {
            Server = dynamic_cast<TCP_SOCKET*> (new TCP_SOCKET);
        }
        else {
            Server = dynamic_cast<UDP_SOCKET*> (new UDP_SOCKET);
        }
    }
    if ((Host != NULL) && (Server != NULL)) {
        if (PortOut != PortIn) {
            res = Host->Connect(pLocalIP, PortOut, IP);
            if (res) {
                res &= Server->Connect(pLocalIP, PortIn, IP);
            }
        }
        else {
            res = Host->Connect(pLocalIP, PortOut, IP);
        }
    }
    if ((Host != NULL) && (Server != NULL) && res) {
        BufferData* ptr = new BufferData;
        if (ptr) {
            int idx = vConnections.size();
            vConnections.resize(idx + 1);
            (vConnections[idx]).Host = Host;
            (vConnections[idx]).Server = Server;
            (vConnections[idx]).parameters.PortIn = PortIn;
            (vConnections[idx]).parameters.PortInTCP = PortInTCP;
            (vConnections[idx]).parameters.PortOut = PortOut;
            (vConnections[idx]).parameters.PortOutTCP = PortOutTCP;
            strcpy((vConnections[idx]).parameters.IP, IP);
            strcpy((vConnections[idx]).parameters.localIP, localIP);
            (vConnections[idx]).RecvBuffer = ptr;
            memset(ptr, 0 , sizeof(BufferData));
            result = idx;
        }
        else {
            res = false;
            result = -2;
            Host->Disconnect();
            if (Host != Server) {
                Server->Disconnect();
            }
        }
    }
    else {
        result = -1;
    }
    if (res == false) {
        if (Host != NULL) {
            delete Host;
        }
        if (Server != NULL) {
            delete Server;
        }
    }
    return result;
}

void cDataTransfer::Finish(void)
{
    int a = vConnections.size();
    for (int i = 0; i < a; i++) {
        if (vConnections.at(i).Host) {
            vConnections.at(i).Host->Disconnect();
            delete vConnections.at(i).Host;
            vConnections.at(i).Host = NULL;
        }
        if (vConnections.at(i).Host !=  vConnections.at(i).Server) {
            if (vConnections.at(i).Server) {
                vConnections.at(i).Server->Disconnect();
                delete vConnections.at(i).Server;
                vConnections.at(i).Server = NULL;
            }
        }
    }
    if (a) {
        vConnections.clear();
    }
}

void cDataTransfer::updateStatistics(unsigned char id)
{
    cLanDevice* pDevice = &vConnections.at(id);
    unsigned long currTime = GetTickCount_();
    const unsigned long updateInterval = 400;
    if((currTime - pDevice->prevUpdateTime) > updateInterval)
    {
        double TimeLeftSec=double(currTime - pDevice->prevUpdateTime)/1000.0;

        pDevice->downloadSpeed = pDevice->bytesDownloaded ? (double(pDevice->bytesDownloaded)/TimeLeftSec) : 0;
        pDevice->uploadSpeed = pDevice->bytesUploaded ? (double(pDevice->bytesUploaded)/TimeLeftSec) : 0;

        pDevice->bytesDownloaded = pDevice->bytesUploaded = 0;

        pDevice->prevUpdateTime = currTime;
    }
}

unsigned int cDataTransfer::getUploadSpeed(unsigned char DataChannel)
{
    return vConnections.at(DataChannel).uploadSpeed;
}

unsigned int cDataTransfer::getDownloadSpeed(unsigned char DataChannel)
{
    return vConnections.at(DataChannel).downloadSpeed;
}

bool cDataTransfer::WriteBuffer(unsigned char id, unsigned char* ptr, unsigned int count)
{
    bool retVal = false;
    for (int tryNumber = 0; tryNumber < 3; ++tryNumber) {
        retVal = vConnections.at(id).Host->sendData((char *)ptr, count);
        if (retVal == true)  {
            break;
        }
        else if (vConnections.at(id).Host->getSocket() == SOCKT::InvalidSocket) {
            restoreConnection(id);
        }
    }

    if(retVal) //Update transfer statistics
    {
        vConnections.at(id).bytesUploaded += count;
        updateStatistics(id);
    }

    return retVal;
}

unsigned int cDataTransfer::ReadBuffer(unsigned char id, unsigned char* ptr, unsigned int count)
{
    BufferData* pdata = vConnections.at(id).RecvBuffer;
    if (count > (pdata->MaxIdx - pdata->CurrentPosition)) {
        register int cnt = vConnections.at(id).Server->ReciveData(pdata->RBuffer, 2048);
        if (cnt != 0) {
            pdata->MaxIdx = cnt;
            pdata->CurrentPosition = 0;
            count = std::min((unsigned int)cnt, count);
        }
        else {
            SLEEP(1);
            return cnt;
        }
    }

    memcpy(ptr, &(pdata->RBuffer[pdata->CurrentPosition]), count);

    //Update transfer statistics
    vConnections.at(id).bytesDownloaded += count;
    updateStatistics(id);

	pdata->CurrentPosition += count;
	return count;
}

//  возвращает 0  при нормальном завершении
// -1 - неправильно задан параметр
int cDataTransfer::closeConnections(unsigned char idx)
{
    unsigned int a =  vConnections.size();
    SOCKT* Server = NULL;
    SOCKT* Host = NULL;
    if ((a) && (idx < a)) {
        Host = (vConnections[idx]).Host;
        Server = (vConnections[idx]).Server;
        if (Host != Server) {
            Host->Disconnect();
            Server->Disconnect();
        }
        else {
            Host->Disconnect();
        }
        SLEEP(100) ;
        return 0;
    }
    return -1;
}

//  0 - соединение установлено
// -1 - неправильно задан параметр
// -2 - соединение не установлено
int cDataTransfer::restoreConnection(unsigned char idx)
{
    unsigned int a =  vConnections.size();
    SOCKT* Server = NULL;
    SOCKT* Host = NULL;
    bool res;
    char* pLocalIP;
    //
    if ((vConnections[idx]).parameters.localIP[0] != 0) {
        pLocalIP = (char*) & (vConnections[idx]).parameters.localIP;
    }
    else {
        pLocalIP = NULL;
    }
    if ((a) && (idx < a)) {
        Host = (vConnections[idx]).Host;
        Server = (vConnections[idx]).Server;
        if (Host != Server) {
            res = Host->Connect(pLocalIP, (vConnections[idx]).parameters.PortOut, (vConnections[idx]).parameters.IP);
            if (res) {
                res &= Server->Connect(pLocalIP, (vConnections[idx]).parameters.PortIn, (vConnections[idx]).parameters.IP);
            }
        }
        else {
            res = Host->Connect(pLocalIP, (vConnections[idx]).parameters.PortOut, (vConnections[idx]).parameters.IP);
        }
        if (res) {
            return 0;
        }
        else {
            return -2;
        }
    }
    return -1;
}
