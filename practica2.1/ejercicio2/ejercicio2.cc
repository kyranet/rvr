#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#define MAX_RESPONSE_LEN 15

/*
  argv[0] ---> nombre del programa
  argv[1] ---> primer argumento (char *)
  ./time_server 0.0.0.0 2222
    argv[0] = "./time_server"
    argv[1] = "0.0.0.0"
    argv[2] = "2222"
      |
      |
      V
    res->ai_addr ---> (socket + bind)
*/

int main(int, char **argv)
{
	time_t time_;
	struct tm *tm_;
	struct addrinfo hints;
	struct addrinfo *res;

	// ---------------------------------------------------------------------- //
	// INICIALIZACIÓN SOCKET & BIND //
	// ---------------------------------------------------------------------- //

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rc != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
		return -1;
	}

	// res contiene la representación como sockaddr de dirección + puerto

	int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (bind(sd, res->ai_addr, res->ai_addrlen) != 0)
	{
		std::cerr << "bind: " << std::endl;
		return -1;
	}

	// Libera la información de la dirección una vez ya hemos usado sus datos:
	freeaddrinfo(res);

	// ---------------------------------------------------------------------- //
	// RECEPCIÓN MENSAJE DE CLIENTE //
	// ---------------------------------------------------------------------- //
	char buffer[80];
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	struct sockaddr client_addr;
	socklen_t client_len = sizeof(struct sockaddr);

	bool exit = false;
	char response[MAX_RESPONSE_LEN];
	while (!exit)
	{
		ssize_t bytes = recvfrom(sd, buffer, 79 * sizeof(char), 0, &client_addr,
								 &client_len);

		if (bytes == -1)
		{
			std::cerr << "recvfrom: " << std::endl;
			return EXIT_FAILURE;
		}

		getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
					NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

		std::cout << bytes << " bytes de " << host << ":" << service << std::endl;

		if (bytes == 0)
		{
			continue;
		}

		switch (buffer[0])
		{
		case 'q':
			exit = true;
			std::cout << "Saliendo..." << std::endl;
			break;
		case 't':
			time(&time_);
			tm_ = localtime(&time_);
			bytes = strftime(response, MAX_RESPONSE_LEN, "%H:%M:%S %p", tm_);
			sendto(sd, response, bytes, 0, &client_addr, client_len);
			break;
		case 'd':
			time(&time_);
			tm_ = localtime(&time_);
			bytes = strftime(response, MAX_RESPONSE_LEN, "%Y-%m-%d", tm_);
			sendto(sd, response, bytes, 0, &client_addr, client_len);
			break;
		default:
			std::cout << "Comando no soportado " << buffer[0] << std::endl;
		}
	}

	// Cierra el socket:
	close(sd);

	return EXIT_SUCCESS;
}
