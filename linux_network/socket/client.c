#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	int socket_fd;
	struct sockaddr_in address;
	int ret;
	char ch = 'A';

	struct sockaddr *addr;
	socklen_t addrlen;

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == socket_fd) {
		perror("Error:socket()");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = 9734;

	addr = (struct sockaddr *)&address;
	addrlen = sizeof(address);

	ret = connect(socket_fd, addr, addrlen);
	if (-1 == ret) {
		perror("Error:connect()");
		return -1;
	}

	send(socket_fd, &ch, 1, 0);
	recv(socket_fd, &ch, 1, 0);

	printf("Client: The received ch is %c\n", ch);

	close(socket_fd);

	return 0;
}
