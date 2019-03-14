#include "datatransfer_usbcan_win.h"

#define OK 0
#define ERR -(__LINE__)

int cDataTransferUsbCanWin::addConnection(const cISocket::cConnectionParams *const connection_params = 0)
{
const int connection_id = 0;

	if (_connections_pull.size() != 0)
	{
		return -1;
	}

	try
	{
		_connections_pull.resize(_connections_pull.size() + 1);
	}
	catch (std::bad_alloc &)
	{
		return ERR;
	}

	_connections_pull[connection_id]._parameters = 0;

	cSocketUSBCanWin *p_cansocket = new (std::nothrow) cSocketUSBCanWin;
	if (p_cansocket == 0)
	{
		_connections_pull[connection_id]._socket_host   = 0;
		_connections_pull[connection_id]._socket_server = 0;
        return ERR;
    }

    _connections_pull[connection_id]._socket_host = p_cansocket;
    _connections_pull[connection_id]._socket_server = p_cansocket;

    return 0;
}

int cDataTransferUsbCanWin::openConnection(const int connection_id)
{
    if (isConnectionOpen(connection_id))
    {
        return -1;
    }

    int connect_res =
            _connections_pull[connection_id].
            _socket_host->connect(_connections_pull[connection_id]._parameters);

    return connect_res;
}

bool cDataTransferUsbCanWin::write(const int connection_id,
                                const unsigned char *const data_ptr,
                                const unsigned int data_count)
{
    return cIDataTransfer::write(connection_id, data_ptr, data_count);
}

int cDataTransferUsbCanWin::read(const int connection_id,
                              unsigned char *data_ptr,
                              const unsigned int data_count)
{
    return cIDataTransfer::read(connection_id, data_ptr, data_count);
}
