#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	int client_sockfd;
	struct sockaddr_in client_address;
	struct sockaddr *client_addr;
	socklen_t client_addrlen;

	int server_sockfd;
	struct sockaddr_in server_address;
	struct sockaddr *server_addr;
	socklen_t server_addrlen;

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_address.sin_port = 9734;

	server_addr = (struct sockaddr *)&server_address;
	server_addrlen = sizeof(server_address);

	bind(server_sockfd, server_addr, server_addrlen);

	listen(server_sockfd, 5);
	while(1) {
		char ch;

		printf("Server waiting..\n");

		client_addr = (struct sockaddr *)&client_address;
		client_addrlen = sizeof(client_address);
		client_sockfd = accept(server_sockfd, client_addr, &client_addrlen);

		recv(client_sockfd, &ch, 1, 0);
		printf("Servier: The received ch is %c\n", ch);

		ch++;
		send(client_sockfd, &ch, 1, 0);
		printf("Servier: send ch is %c\n", ch);

		close(client_sockfd);
	}

	return 0;
}
