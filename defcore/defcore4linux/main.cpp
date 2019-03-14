#include "ChannelsTable.h"
#include "DeviceConfig_Avk31.h"
#include "platforms.h"
#include "Definitions.h"
#include "DeviceCalibration.h"
#include "CriticalSection_Lin.h"
#include "ThreadClassList_Lin.h"
#include "EventManager_Lin.h"
#include "Device.h"

#include <unistd.h>
#include <iostream>
#include <string.h>
using namespace std;

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
//    return a.exec();
    (void)argc;
    (void)argv;

    cChannelsTable     * table = new cChannelsTable;
    cDeviceConfig      * config = new cDeviceConfig_Avk31(table, 32, 16);
    cCriticalSection   * criticalSection = new cCriticalSection_Lin();
    cDeviceCalibration * calibration = new cDeviceCalibration("calibration.dat", table, criticalSection);
    //config->setProtocolForUmuNum();

    if (calibration->Count() == 0) {
        calibration->CreateNew();
        calibration->SetCurrent(0);
    }
    else {
        calibration->SetCurrent(0);
    }

    cThreadClassList   * threadList = new cThreadClassList_Lin;
    cEventManager      * deviceEventManager = new cEventManager_Lin();
    cEventManager      * umuEventManager = new cEventManager_Lin();

    //cDataTransferLan      * dataTransfer = new cDataTransferLan();
    /*TDataChannelIdList   idList;
    idList[0] = 0;
    idList[1] = 1;
    idList[2] = 2;//*/

    cDevice            * device = new cDevice(threadList,
       // dataTransfer,
        //idList,
        config,
        calibration,
        table,
        deviceEventManager,
        umuEventManager,
        criticalSection);

    device->SetMode(dmSearch); // Выбор режима
    device->SetChannel(0x01);  // Выбор канала
    device->SetSide(dsLeft);  // Выбор стороны
    device->Update(true);      // Установка выбранных значений

    const unsigned int PORT_out = 43000;
    const unsigned int PORT_in = 43001;

    cDataTransferLan::cLanConnectionParams connection_params;
    strcpy(connection_params._remote_ip, "192.168.100.100");
    connection_params._port_in = PORT_in;
    connection_params._port_out = PORT_out;
    connection_params._port_in_tcp = false;
    connection_params._port_out_tcp = true;

    config->setProtocolForUmuNum(std::make_pair(eProtLAN, &connection_params));

    /*int DataChannel = -1;
    while (DataChannel == -1) {
        printf("Connecting ...\n");
        cDataTransferLan::cLanConnectionParams params;

        strcpy(params._remote_ip, "192.168.100.100");
        connection_params._port_in = PORT_in;
        connection_params._port_out = PORT_out;
        connection_params._port_in_tcp = false;
        connection_params._port_out_tcp = true;

        DataChannel = dataTransfer->addConnection(&params);
    }//*/

    device->StartWork();

    while (true) {
        if (deviceEventManager->WaitForEvent()) // Ожидание данных от Device
        {
            while (deviceEventManager->EventDataSize() > 0) {
                unsigned long dataID = 0;
                deviceEventManager->ReadEventData(&dataID, SIZE_OF_TYPE(dataID), NULL); // Чтения заголовока новых данных
                switch(dataID)
                {
                case edAScanMeas: // А-развертка, точное значение амплитуды и задержки
                    {
                        PtDEV_AScanMeasure Ptr = NULL;
                        deviceEventManager->ReadEventData(&Ptr, SIZE_OF_TYPE(&Ptr), NULL); // Чтение указателя на данные
                        cout << Ptr->Channel;
                        // ... Работа с данными
                    }
                    break;
                case edAScanData: // Данные кадра AScan
                    {
                        PtDEV_AScanHead Ptr1 = NULL; // Информация: Сторона, Канал
                        PAScanData      Ptr2 = NULL; // Данные кадра А-развертки
                        deviceEventManager->ReadEventData(&Ptr1, SIZE_OF_TYPE(&Ptr1), NULL); // Чтение указателя на данные
                        deviceEventManager->ReadEventData(&Ptr2, SIZE_OF_TYPE(&Ptr2), NULL); // Чтение указателя на данные
                        cout << Ptr2[1];
                        // ... Работа с данными
                    }
                    break;
                case edTVGData: // TVG
                    {
                        PtDEV_AScanHead Ptr1 = NULL; // Информация: Сторона, Канал
                        PAScanData      Ptr2 = NULL; // Данные кадра ВРЧ
                        deviceEventManager->ReadEventData(&Ptr1, SIZE_OF_TYPE(&Ptr1), NULL); // Чтение указателя на данные
                        deviceEventManager->ReadEventData(&Ptr2, SIZE_OF_TYPE(&Ptr2), NULL); // Чтение указателя на данные
                        // ... Работа с данными
                    }
                    break;
                }
            }
        }
    }

    return 0;
}
