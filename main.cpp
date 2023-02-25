#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <unistd.h>
//
//#include <arpa/inet.h>
//#include <sys/socket.h>
//#include <sys/ioctl.h>
//#include <linux/if_packet.h>
//#include <net/if.h>
//#include <netinet/ether.h>
#include "RotelA14.h"


int main(void) {
	std::unique_ptr<rotel::RotelBase> rb = rotel::RotelBase::get("10.10.20.124");



//	int ret;
//	struct sockaddr_in sock_addr;
//	struct sockaddr_in sock_other;
//	int portnr = 9590;
//	int addr_size;
//	int sock;
//
//	sock = socket(AF_INET, SOCK_STREAM, 0);
//	if(-1 == sock) {
//		perror("socket");
//		return 1;
//	}
//
//	//ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
//	 //                &so_reuseaddr, sizeof(so_reuseaddr));
//	//if (-1 == ret) {
//	//	perror("setsockopt SO_REUSEADDR fail");
//	//	return 1;
//	//}
//
//	sock_addr.sin_family = AF_INET;
//	sock_addr.sin_port = htons(portnr);
//	//sock_addr.sin_addr.s_addr = htonl(data->ipv4_src_int);
//	sock_addr.sin_addr.s_addr = inet_addr("10.10.20.124");
//	addr_size = sizeof(sock_other);
//
//	ret = connect(sock, (struct sockaddr*)&sock_addr, addr_size);
//	if(ret) {
//		perror("connect");
//	}
//
//
//	char *hello = "power_on!";
//	char buffer[1024];
//	send(sock, hello, strlen(hello), 0);
//	printf("Hello message sent\n");
//	int valread = read(sock, buffer, 1024);
//	printf("%s\n", buffer);
//
//	close(sock);
}
