#include "datatransfer_lan.h"

#define OK 0
#define ERR (-(__LINE__))

int cDataTransferLan::addConnection(const cISocket::cConnectionParams* const connection_params)
{
    if (connection_params == NULL) {
        return ERR;
    }

    cISocket* server = NULL;
    cISocket* host = NULL;

    const cLanConnectionParams* lan_connection_params = static_cast<const cLanConnectionParams* const>(connection_params);

    if (lan_connection_params->_port_out_tcp) {
        host = new (std::nothrow) cSocketLanTcp;
    }
    else {
        host = new (std::nothrow) cSocketLanUdp;
    }
    if (host == NULL) {
        return ERR;
    }

    if (lan_connection_params->_port_out == lan_connection_params->_port_in) {
        server = host;
    }
    else {
        if (lan_connection_params->_port_in_tcp) {
            server = new (std::nothrow) cSocketLanTcp;
        }
        else {
            server = new (std::nothrow) cSocketLanUdp;
        }
    }
    if (server == NULL) {
        delete host;
        return ERR;
    }

    const int connection_id = _connections_pull.size();

    _connections_pull.resize(connection_id + 1);


    _connections_pull[connection_id]._socket_host = host;
    _connections_pull[connection_id]._socket_server = server;

    cLanConnectionParams* param = new (std::nothrow) cLanConnectionParams;

    if (param == NULL) {
        _connections_pull[connection_id]._parameters = 0;
        return ERR;
    }

    cBufferData* p_buff = new (std::nothrow) cBufferData;

    if (p_buff == NULL) {
        delete param;
        return ERR;
    }

    memset(p_buff->_buffer, 0, sizeof(unsigned char) * cBufferData::_buffer_size);
    p_buff->_current_position = 0;
    p_buff->_max_id = 0;
    _connections_pull[connection_id]._recv_buffer = p_buff;  //*/

    _connections_pull[connection_id]._parameters = param;

    memcpy(param->_local_ip, lan_connection_params->_local_ip, sizeof(char) * cLanConnectionParams::_ip_size);

    memcpy(param->_remote_ip, lan_connection_params->_remote_ip, sizeof(char) * cLanConnectionParams::_ip_size);

    param->_port_in = lan_connection_params->_port_in;
    param->_port_out = lan_connection_params->_port_out;
    param->_port_in_tcp = lan_connection_params->_port_in_tcp;
    param->_port_out_tcp = lan_connection_params->_port_out_tcp;

    return connection_id;
}

// +
int cDataTransferLan::openConnection(const int connection_id)
{
    if (!isConnectionExist(connection_id)) {
        return ERR;
    }

    cISocket* server = _connections_pull[connection_id]._socket_server;
    cISocket* host = _connections_pull[connection_id]._socket_host;

    cLanConnectionParams* p_param = 0;
    p_param = static_cast<cLanConnectionParams*>(_connections_pull[connection_id]._parameters);
    if (p_param == NULL) {
        return ERR;
    }
    if ((host == NULL) || (server == NULL)) {
        return ERR;
    }

    bool bool_result = false;
    p_param->_local_ip_address = p_param->_local_ip;
    p_param->_remoute_ip_address = p_param->_remote_ip;
    p_param->_remoute_port_num = p_param->_port_out;
    if (p_param->_port_out != p_param->_port_in) {
        bool_result = host->connect(p_param);
        if (bool_result) {
            p_param->_remoute_port_num = p_param->_port_in;
            bool_result = server->connect(p_param);
        }
    }
    else {
        bool_result = host->connect(p_param);
    }

    if (!bool_result) {
        if (host != NULL) {
            host->disconnect();
        }
        if (host != server) {
            server->disconnect();
        }

        return ERR;
    }
    return OK;
}

bool cDataTransferLan::write(const int connection_id, const unsigned char* const data_ptr, const unsigned int data_count)
{
    return cIDataTransfer::write(connection_id, data_ptr, data_count);
}

int cDataTransferLan::read(const int connection_id, unsigned char* data_ptr, const unsigned int data_count)
{
    if (isConnectionExist(connection_id)) {
        if (!(_connections_pull[connection_id].checkConnection())) {
            return ERR;
        }
    }
    else {
        return ERR;
    }

    int ret_val = -1;
    int count = data_count;

    cBufferData* pdata = _connections_pull[connection_id]._recv_buffer;

    if (count > (pdata->_max_id - pdata->_current_position)) {
        ret_val = _connections_pull[connection_id]._socket_server->reciveData(pdata->_buffer, pdata->_buffer_size);

        if (ret_val >= 0) {
            pdata->_max_id = ret_val;
            pdata->_current_position = 0;
            count = std::min(ret_val, count);
        }
        else {
            SLEEP(1);
            return ret_val;
        }
    }

    memcpy(data_ptr, &(pdata->_buffer[pdata->_current_position]), count);

    // Update transfer statistics
    if (ret_val > 0) {
        _connections_pull[connection_id]._bytes_downloaded += count;
        updateStatistics(connection_id);
    }

    pdata->_current_position += count;
    return count;
}
