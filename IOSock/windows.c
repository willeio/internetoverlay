#include "net.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>


int io_socket()
{

}


int io_bind(int sock, uint16_t port)
{
}


int io_listen(int sock, int queue)
{
}


int io_sock_connect(int sock, ipv4 ip, uint16_t port)
{
}


int io_get_ipv4(const char *ip_in, ipv4 *ip_out)
{
}


int io_ipv4_to_string(ipv4 *ip_in, const char *ip_out)
{
}
