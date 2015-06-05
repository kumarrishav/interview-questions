/* Write a simple webserver that is capable of processing only one request.
 * The server can only serve one client at any point in time, and the only command
 * implemented by the server is HTTP GET for html files.
 *
 * If a requested html file does not exist or the path is otherwise invalid, the server
 * should reply with the 404 Not Found error message.
 *
 * Source: Adapted from Kurose & Ross
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define HTTP_PORT 80
#define PENDING_BACKLOG 16
#define HTTP_VERSION "HTTP/1.1"

void handle_client(int client_sock) {

	static char buff[1024];
	static char path_buff[1024];
	static char reply_400[] = HTTP_VERSION " 400 Bad Request\r\nConnection: close\r\n\r\n";
	static char reply_404[] = HTTP_VERSION " 404 Not Found\r\nConnection: close\r\n\r\nThe requested document was not found on this server.\r\n";

	if (recv(client_sock, buff, sizeof(buff)-1, 0) < 0) {
		perror("recv() failed");
		close(client_sock);
		return;
	}

	if (sscanf(buff, " GET %1024s %*s", path_buff) != 1) {
		send(client_sock, reply_400, sizeof(reply_400)-1, 0);
		close(client_sock);
		return;
	}

	/* Ignore possible leading slash in the file path */
	const char *path = path_buff;
	if (*path == '/') {
		path++;
	}

	struct stat statbuf;
	if (stat(path, &statbuf) < 0) {
		// We map any error looking up the file to the infamous 404 error
		send(client_sock, reply_404, sizeof(reply_404)-1, 0);
		close(client_sock);
		return;
	}

	FILE *to_send = fopen(path, "r");
	if (to_send == NULL) {
		send(client_sock, reply_404, sizeof(reply_404)-1, 0);
		close(client_sock);
		return;
	}

	sprintf(buff, HTTP_VERSION " 400 OK\r\nConnection: close\r\nContent-Length: %ld\r\nContent-Type: text/html\r\n\r\n", (long) statbuf.st_size);
	send(client_sock, buff, strlen(buff), 0);

	size_t n;
	while ((n = fread(buff, 1, sizeof(buff), to_send)) > 0) {
		send(client_sock, buff, n, 0);
	}

	fclose(to_send);
	close(client_sock);
}

int main(void) {

	/* Step 1 - Create the socket descriptor */
	int main_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (main_sock < 0) {
		perror("Couldn't create socket");
		exit(EXIT_FAILURE);
	}

	/* Now, we need to bind an address to the socket.
	 * Because this is an AF_INET socket, we will bind a sockaddr_in
	 */
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(HTTP_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(main_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Couldn't bind address to socket");
		exit(EXIT_FAILURE);
	}

	/* Inform that we are willing to accept new connection requests starting now */
	if (listen(main_sock, PENDING_BACKLOG) < 0) {
		perror("Couldn't listen on socket");
		exit(EXIT_FAILURE);
	}

	/* Accept a (possibly pending) connection request and process it */
	int client;
	while (1) {
		client = accept(main_sock, NULL, NULL);
		if (client < 0) {
			perror("Error accepting new client");
			continue;
		}

		handle_client(client);
	}

	return 0;
}
