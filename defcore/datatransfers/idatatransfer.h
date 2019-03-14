#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include "platforms.h"

#include "sockets/socket_lan.h"
#include "sockets/socket_can.h"

#if (defined(DEFCORE_OS_WIN)) && (!defined(DEFCORE_CC_GNU))
#include "windows.h"
#else
#include <sys/time.h>
#include <cstddef>
#endif

#include <vector>

class cIDataTransfer
{
protected:
    struct cBufferData
    {
        enum
        {
            _buffer_size = 2048
        };
        int _current_position;
        int _max_id;
        unsigned char _buffer[_buffer_size];
    };  //*/

    struct cConnection
    {
        cConnection()
            : _socket_host(0)
            , _socket_server(0)
            , _parameters(0)
            , _recv_buffer(0)
            , _bytes_downloaded(0)
            , _bytes_uploaded(0)
            , _download_speed(0)
            , _upload_speed(0)
            , _prev_update_time(0)
        {
        }
        ~cConnection() {}
        inline bool checkConnection() const
        {
            if (_socket_host == 0) return false;
            if (_socket_server == 0) return false;
            if (_socket_host->getSocket() < 0) return false;
            if (_socket_server->getSocket() < 0) return false;
            return true;
        }

        cISocket* _socket_host;
        cISocket* _socket_server;
        cISocket::cConnectionParams* _parameters;
        cBufferData* _recv_buffer;

        // Statistics
        int _bytes_downloaded;
        int _bytes_uploaded;
        int _download_speed;
        int _upload_speed;
        long _prev_update_time;
    };

    std::vector<cConnection> _connections_pull;

    bool updateStatistics(const int connection_id);

private:
    // Запрещаем копирование
    explicit cIDataTransfer(const cIDataTransfer&);
    void operator=(const cIDataTransfer&);
//For Debug
    void print_hex_memory(const void *mem, int size);

public:
    cIDataTransfer() {}
    virtual ~cIDataTransfer();

    bool isConnectionExist(const int connection_id) const;
    bool isConnectionOpen(const int connection_id) const;

    bool closeConnection(const int connection_id);
    void closeAllConnections();

    int getUploadSpeed(const int connection_id) const;
    int getDownloadSpeed(const int connection_id) const;

    // Методы, которые необходимо определить/дополнить для конкретной реализации
    virtual int addConnection(const cISocket::cConnectionParams* const connection_params) = 0;
    virtual int openConnection(const int connection_id) = 0;
    virtual bool write(const int connection_id, const unsigned char* const data_ptr, const unsigned int data_count);
    virtual int read(const int connection_id, unsigned char* data_ptr, const unsigned int data_count);
};

#endif  // DATA_TRANSFER_H
