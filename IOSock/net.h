#pragma once

#include <sys/types.h>
#include <stdint.h>


typedef uint32_t ipv4;


ssize_t io_read(int sock, uint8_t *buf, int size);
ssize_t io_write(int sock, uint8_t *buf, int size);
int io_socket();
int io_bind(int sock, uint16_t port);
int io_listen(int sock, int queue);
int io_sock_connect(int sock, ipv4 ip, uint16_t port);
int io_get_ipv4(const char *ip_in, ipv4 *ip_out);
int io_ipv4_to_string(ipv4 *ip_in, const char *ip_out);
