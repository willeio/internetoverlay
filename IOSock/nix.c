#include "net.h"

#include <IOLib/io.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


int io_socket()
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (set_nonblock(sock) != 0)
    return -1;

  return sock;
}


int io_bind(int sock, uint16_t port)
{
  struct sockaddr_in serv_addr;
  
  memset(&serv_addr, 0, sizeof(serv_addr));
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  
  if (bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
    return -1;
  
  return 0;
}


int io_listen(int sock, int queue)
{
  if (listen(sock, queue) != 0)
    return -1;
  
  return 0;
}


int io_sock_connect(int sock, ipv4 ip, uint16_t port)
{
  struct sockaddr_in serv_addr;
  
  memset(&serv_addr, 0, sizeof(serv_addr));
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = ip;
  
  /*if (*/connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); // IN PROGRESS !< 0)
    //return -1;

  if (wait_socket_read_ready(sock) != 0)
    return -2;
  
  return 0;
}


int io_get_ipv4(const char *ip_in, ipv4 *ip_out)
{
  if (inet_pton(AF_INET, ip_in, ip_out) <= 0)
    return -1;
  
  return 0;
}


int io_ipv4_to_string(ipv4 *ip_in, const char *ip_out)
{
  //ip_out = inet_ntoa(serv_addr.sin_addr); // TODO: do not use inet_ntoa ...
  //return 0;
}
