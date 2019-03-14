#include <stdio.h>
#include <stdlib.h>

#include "DataTr.h"
#include "TickCount.h"

#define PORT_out 43000
#define PORT_in  43001

const unsigned char UMU = 0x02;
const unsigned char uEnable = 0x43;             // БУИ - Управление работой БУМ
const unsigned char wmDeviceID = 0xDE;          // ОБЩ - Запросить информацию о версии и самоидентификации устройства
const unsigned char cFWver = 0xDF;              // ОБЩ - Версия и дата прошивки блока
const unsigned char cSerialNumber = 0xDB;       // ОБЩ - Серийный номер устройства
const int LanDataMaxSize = 1019;

typedef struct {
    unsigned char Id;
    unsigned char Source;
    unsigned short Size;
    unsigned char MessageCount;
    unsigned char NotUse;
    unsigned char Data[LanDataMaxSize];
} tLAN_Message;

tLAN_Message Block;                             // Буфер загрузки / выгрузки данных в LAN

enum ReadState {                                // Состояние процесса чтения LAN сообщения
    rsOff = 0,                              // Выключенно
    rsHead,                                 // Ожидаем заголовок LAN сообщения
    rsBody                                  // Ожидаем тело LAN сообщения
};

typedef signed long int DWORD;

int main(int argc, char* argv[])
{
    cDataTransfer* DT = new cDataTransfer();
    int DataChannel = - 1;
    printf("Connecting ...\n");
    DataChannel = DT->AddConnection("", "192.168.100.100", PORT_in, PORT_out, false, true);
    if (DataChannel != - 1) {
        printf("Connection OK\n");
        printf("DataChannel %d\n\n", DataChannel);
        printf("Request device identity\n\n");
        Block.Id      =  uEnable;
        Block.Source  =  UMU;
        Block.Size    =  0x02;
        Block.Data[0] =  wmDeviceID;
        DT->WriteBuffer(DataChannel, (unsigned char*)&Block, Block.Size + 6);
        printf("waiting for response\n\n");
        ReadState RState;                       // Состояние чтения данных из LAN
        RState = rsHead;
        int res;
        DWORD DelayTime = GetTickCount_();
        bool isExit = false;
        while (isExit == false) {
            switch (RState) {
            case rsHead:
                res = DT->ReadBuffer(DataChannel, (unsigned char*)&Block, 6);
                if (res == 6) {
                    RState = rsBody;
                }
                break;
            case rsBody:
                res = DT->ReadBuffer(DataChannel, (unsigned char*)&Block.Data, Block.Size);
                if (res == Block.Size) {
                    RState = rsHead;
                    switch (Block.Id) {
                    case cFWver: // ОБЩ - Версия и дата прошивки блока
                        printf("Firmware Info: %d\n", Block.Data[0]);
                        printf("Day: %d\n",           Block.Data[5]);
                        printf("Month: %d\n",         Block.Data[4]);
                        printf("Year: %d\n",          2000 + Block.Data[3]);
                        printf("Hour: %d\n",          Block.Data[6]);
                        printf("Minute: %d\n",        Block.Data[7]);
                        printf("Second: %d\n",        Block.Data[8]);
                        printf("VersionHi: %d\n",     Block.Data[2]);
                        printf("VersionLo: %d\n",     Block.Data[1] & 0x0F);
                        break;
                    case cSerialNumber:  // ОБЩ - Серийный номер устройства
                        printf("SerialNumber %d\n", Block.Data[0] + (Block.Data[1] << 8));
                        break;
                    }
                    isExit = true;
                }
                break;
            }
        }
    }
    else {
        printf("Connection Error\n");
    }
    printf("\n");
    system ("pause");
    return 0;
}
