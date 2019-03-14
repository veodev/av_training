#ifndef UTILS_CAN
#define UTILS_CAN

/*
 * Данный файл объявлет формат can сообщения и инкапсулирует в себя
 * низкоуровневую работу с ним
*/

// Далее в большом объеме кода будет идти взаимодействовие с cMessageCan,
// для того чтобы инкапсулировать работу с форматом CAN и скрыть повторяющуюся
// битовую арифметику мы определяем сответствующие функции в пространстве
// имен can_utils и делаем их inline, убирая за счет этого оверхед на вызов
namespace can_utils
{

// Все именованные константы CAN протокола доступны только через этот неймспейс
#include "prot_umu/prot_umu_can_constants.h"

//------------------------------ СAN сообщение ---------------------------------
// Для того чтобы cMessageCan оставался максимально легким и при этом POD-типом
// все вспомогательные функции для работы с ним определяются отдельно
struct cMessageCan
{
    enum eCONSTANTS { CAN_HEAD_SIZE = 5,
                      CAN_MAX_DATA_SIZE = 8,
                      CAN_MAX_FRAME_SIZE = 16 };

	unsigned char _msg[CAN_MAX_FRAME_SIZE];
	bool _delayedUnload;
};
//------------------------------------------------------------------------------

    inline void SetCommandId(cMessageCan &message, const unsigned char command_id)
    {
        unsigned char buf1 = (command_id << 3);
        unsigned char buf2 = ((command_id & 0xe0) >> 5);
        message._msg[0] |= buf1;
        message._msg[1] |= buf2;
    }
    inline unsigned char GetCommandId(const cMessageCan &message)
    {
        unsigned char buf1 = message._msg[0] >> 3;
        unsigned char buf2 = message._msg[1] << 5;
        return (buf1 | buf2);
    }

    inline void SetDeviceId(cMessageCan &message, const unsigned char device_id)
    {
        unsigned char buf = (device_id & 0x7);
        message._msg[0] |= buf;
    }
    inline unsigned char GetDeviceId(const cMessageCan &message)
    {
        return message._msg[0] & 0x7;
    }

    inline void SetSizeOfData(cMessageCan &message, const unsigned char data_size)
    {
        message._msg[4] = data_size;
    }
    inline unsigned char GetDataSize(const cMessageCan &message)
    {
        return message._msg[4];
    }

    inline void SetDataAndSize(cMessageCan &message,
                        const unsigned char *bytes,
                        const unsigned char size = 1)
    {
        for (size_t i = 0; i < cMessageCan::CAN_MAX_DATA_SIZE; ++i)
        {
            if (i < size)
            {
                message._msg[8 + i] = bytes[i];
            }
            else
            {
                message._msg[8 + i] = 0;
            }
        }

        SetSizeOfData(message, size);
    }

    inline const unsigned char *GetData(const cMessageCan &message)
    {
        return &(message._msg[8]);
    }
}

#endif // UTILS_CAN

