#include "datatransfer_can.h"
//#include "debug.h"
#define OK 0
#define ERR (-(__LINE__))

int cDataTransferCanRaw::addConnection(const cISocket::cConnectionParams* const connection_params)
{
    if (connection_params == NULL) {
        return ERR;
    }

    const int connection_id = _connections_pull.size();

    _connections_pull.resize(connection_id + 1);

    cSocketCanRaw* p_cansocket = new (std::nothrow) cSocketCanRaw;
    if (p_cansocket == NULL) {
        _connections_pull[connection_id]._socket_host = 0;
        _connections_pull[connection_id]._socket_server = 0;
        _connections_pull[connection_id]._parameters = 0;
        return ERR;
    }

    _connections_pull[connection_id]._socket_host = p_cansocket;
    _connections_pull[connection_id]._socket_server = p_cansocket;

    cCanConnectionParams* p_param = new (std::nothrow) cSocketCanRaw::cSocketCanRawParams;

    if (!p_param) {
        _connections_pull[connection_id]._parameters = 0;
        return ERR;
    }

    p_param->_can_interface_num = static_cast<const cCanConnectionParams* const>(connection_params)->_can_interface_num;

    _connections_pull[connection_id]._parameters = p_param;

    return connection_id;
}

int cDataTransferCanRaw::openConnection(const int connection_id)
{
    if (isConnectionOpen(connection_id)) {
        return -1;
    }

    int connect_res = _connections_pull[connection_id]._socket_host->connect(_connections_pull[connection_id]._parameters);

    return connect_res;
}

bool cDataTransferCanRaw::write(const int connection_id, const unsigned char* const data_ptr, const unsigned int data_count)
{
    return cIDataTransfer::write(connection_id, data_ptr, data_count);
}

int cDataTransferCanRaw::read(const int connection_id, unsigned char* data_ptr, const unsigned int data_count)
{
    return cIDataTransfer::read(connection_id, data_ptr, data_count);
}
