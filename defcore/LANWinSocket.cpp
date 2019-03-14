//---------------------------------------------------------------------------

#pragma hdrstop

//#define Dbg

#include "LANWinSocket.h"

const SOCKET SOCKT::InvalidSocket = SOCKET(~0);
//---------------------------------------------------------------------------

//#pragma package(smart_init)


//---------------------------------------------------------------------------
SOCKT::SOCKT()
{
 State = false;
}

//---------------------------------------------------------------------------
SOCKT::~SOCKT()
{
//
}
//---------------------------------------------------------------------------
UDP_SOCKET::UDP_SOCKET()
{

}

//---------------------------------------------------------------------------
UDP_SOCKET::~UDP_SOCKET()
{

}
//---------------------------------------------------------------------------
bool UDP_SOCKET::Connect(char* localIPAddress,int PortNo, char* remouteIPAddress)
{
	WSADATA wsadata;

	State = false;

	int error = WSAStartup(0x0202, &wsadata);

    if (error)
        return false;

	if (wsadata.wVersion != 0x0202)
    {
		WSACleanup();
        return false;
    }

	SOCKADDR_IN target;

	target.sin_family = AF_INET;
	target.sin_port = htons(PortNo);

    if (localIPAddress) target.sin_addr.s_addr = inet_addr(localIPAddress);
 	    else target.sin_addr.s_addr = htonl (INADDR_ANY);


	sock =	socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		return false;
	}
//
	int i=1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i));

    int bufsize = 262144;  //”величиваем размер буффера под вход€щие сообщени€
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, sizeof(bufsize));


// делаем сокет неблокирующим
	u_long p = 1;
    if (ioctlsocket(sock,FIONBIO,&p) == SOCKET_ERROR)
	{
		#ifdef Dbg
		std::cout << "ioctlsocket FIONBIO Error! - " << WSAGetLastError() << endl;
		#endif
        Disconnect();
		return false;
	}

//
	if (bind(sock, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
	{
		#ifdef Dbg
		std::cout << "bind Error! - " << WSAGetLastError() << endl;
		#endif
        Disconnect();
		return false;
	}


	target.sin_port = htons(PortNo);
    target.sin_addr.s_addr = inet_addr(remouteIPAddress);
	if (connect(sock, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
    {
		#ifdef Dbg
		std::cout << "connect Error! - " << WSAGetLastError() << endl;
		#endif
                                         Disconnect();
		return false;
	}

	State = true;
	return true;
}
//---------------------------------------------------------------------------
// используем неблокирующий сокет
unsigned int UDP_SOCKET::ReciveData(unsigned char *msg, int length)
{
         int res = recv(sock, reinterpret_cast<char *>(msg), length, 0);
         if (res == SOCKET_ERROR)  return 0;
         return res;
}
//---------------------------------------------------------------------------
bool SOCKT::getState()
{
	return State;
}

//---------------------------------------------------------------------------
void SOCKT::setState(bool val)
{
    State = val;
}

//---------------------------------------------------------------------------
SOCKET SOCKT::getSocket()
{
	return sock;
}
//---------------------------------------------------------------------------
void SOCKT::Disconnect()
{
	if (sock)
	closesocket(sock);
	WSACleanup();
}


//---------------------------------------------------------------------------
TCP_SOCKET::TCP_SOCKET()
{
}
//---------------------------------------------------------------------------

TCP_SOCKET::~TCP_SOCKET()
{

}

//---------------------------------------------------------------------------
bool TCP_SOCKET::Connect(char* localIPAddress,int remoutePortNo, char* remouteIPAddress)
{   int i;
	WSADATA wsadata;
	int errorCode;

	State = false;

	int error = WSAStartup(0x0202, &wsadata);

	if (error)
		return false;

	if (wsadata.wVersion != 0x0202)
	{
		WSACleanup();
		return false;
	}

	SOCKADDR_IN target;

	target.sin_family = AF_INET;
	target.sin_port = 0;   // собственный порт выдел€ем динамически, начина€ с 49152     // htons(PortNo);
//	target.sin_addr.s_addr = htonl (INADDR_ANY);

					if (localIPAddress)
						target.sin_addr.s_addr = inet_addr(localIPAddress);
					   else
			               target.sin_addr.s_addr = htonl (INADDR_ANY);


	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		return false;
	}

	i=1;
//	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) true, sizeof(bool));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i));

/*
// делаем сокет неблокирующим
	  u_long p = 1;
					   if (ioctlsocket(sock,FIONBIO,&p) == SOCKET_ERROR)
	{
		#ifdef Dbg
		std::cout << "ioctlsocket FIONBIO Error! - " << WSAGetLastError() << endl;
		#endif
										 CloseConnections();
		return false;
	}
*/

	if (bind(sock, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
	{
		#ifdef Dbg
		std::cout << "bind Error! - " << WSAGetLastError() << endl;
		#endif
		return false;
	}

					target.sin_addr.s_addr = inet_addr(remouteIPAddress);
					target.sin_port =  htons(remoutePortNo);


	if (connect(sock, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
	{
		#ifdef Dbg
		std::cout << "connect Error! - " << WSAGetLastError() << endl;
		#endif
		return false;
	}

	State = true;
	return true;
}
//-------------------------------------------------------------------------------------------------------------------------------------
// используетс€ блокирующий сокет
unsigned int TCP_SOCKET::ReciveData(unsigned char *msg, int length)
{
u_long count;

		  ioctlsocket(sock, FIONREAD, &count) ;
		  if (!count) return 0;
//


         int res = recv(sock, reinterpret_cast<char *>(msg), length, 0);
		 if (res == SOCKET_ERROR)   return 0;
		 return res;
}

//---------------------------------------------------------------------------
int TCP_SOCKET::getDataCountInTCPBuffer()
{
	u_long count;
	ioctlsocket( sock, FIONREAD, &count) ;
	return count;
}

//---------------------------------------------------------------------------
bool TCP_SOCKET::sendData(char * msg, int length)
{
	SOCKADDR_IN target;

	target.sin_family = AF_INET;
	if(0)
	{
	target.sin_port = htons(1200);
	target.sin_addr.s_addr = inet_addr("192.168.1.1");
	}
	  else
	  {
	target.sin_port = htons(0);
	target.sin_addr.s_addr = htonl (INADDR_ANY);
	   }
	int i = sendto(sock,msg,length,0,(SOCKADDR *)&target, sizeof(target));
	#ifdef Dbg
	std::cout << "sended - " << msg << endl;
	#endif
	if (i != -1)
	{
		return true;
	}
	else
	{

		#ifdef Dbg
		std::cout << "connect Error! - " << WSAGetLastError() << endl;
		#endif
		return false;
	}
}
