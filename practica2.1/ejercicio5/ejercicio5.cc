#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>

#define MAX_RESPONSE_LEN 80

/*
  argv[0] ---> nombre del programa
  argv[1] ---> primer argumento (char *)
  ./echo_client 127.0.0.1 2222
    argv[0] = "./echo_client"
    argv[1] = "127.0.0.1"
    argv[2] = "2222"
      |
      |
      V
    res->ai_addr ---> (socket + bind)
*/
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Debes proveer la dirección. Por ejemplo: 127.0.0.1"
				  << std::endl;
		return EXIT_FAILURE;
	}

	if (argc < 3)
	{
		std::cerr << "Debes proveer el puerto. Por ejemplo: 2222"
				  << std::endl;
		return EXIT_FAILURE;
	}

	struct addrinfo hints;
	struct addrinfo *res;

	// ---------------------------------------------------------------------- //
	// INICIALIZACIÓN SOCKET//
	// ---------------------------------------------------------------------- //

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rc != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
		return EXIT_FAILURE;
	}

	int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (connect(sd, res->ai_addr, res->ai_addrlen) == -1)
	{
		std::cerr << "No se pudo establecer una conexión al servidor." << std::endl;
		shutdown(sd, SHUT_RDWR);
		return EXIT_FAILURE;
	}

	freeaddrinfo(res);

	while (true)
	{
		// ---------------------------------------------------------------------- //
		// ENVIO MENSAJE DE CLIENTE //
		// ---------------------------------------------------------------------- //
		char message[MAX_RESPONSE_LEN];
		fgets(message, MAX_RESPONSE_LEN, stdin);

		size_t length = strlen(message);
		if (length == 2 && message[0] == 'Q')
			break;

		// ---------------------------------------------------------------------- //
		// ENVÍO DEL MENSAJE DEL CLIENTE AL SERVIDOR //
		// ---------------------------------------------------------------------- //
		ssize_t sentBytes = send(sd, message, length, 0);
		if (sentBytes <= 0)
			break;

		// ---------------------------------------------------------------------- //
		// RECEPCIÓN DEL MENSAJE DEL SERVIDOR //
		// ---------------------------------------------------------------------- //
		char response[MAX_RESPONSE_LEN];
		ssize_t receivedBytes = recv(sd, response, sizeof(char) * (MAX_RESPONSE_LEN - 1), 0);

		if (receivedBytes <= 0)
			break;

		// ---------------------------------------------------------------------- //
		// ESCRITURA DEL MENSAJE DEL SERVIDOR //
		// ---------------------------------------------------------------------- //
		printf("%.*s", static_cast<int>(receivedBytes), response);
	}

	shutdown(sd, SHUT_RDWR);

	return EXIT_SUCCESS;
}
