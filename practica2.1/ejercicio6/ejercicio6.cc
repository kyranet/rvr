#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <iostream>

#define MAX_RESPONSE_LEN 15

/*
  argv[0] ---> nombre del programa
  argv[1] ---> primer argumento (char *)
  ./time_server_multi 0.0.0.0 2222
    argv[0] = "./time_server_multi"
    argv[1] = "0.0.0.0"
    argv[2] = "2222"
      |
      |
      V
    res->ai_addr ---> (socket + bind)
*/

class Connection
{
	int fd_;

public:
	Connection(int fd) : fd_(fd) {}

	void run() const noexcept
	{
		// ---------------------------------------------------------------------- //
		// RECEPCIÓN MENSAJE DE CLIENTE //
		// ---------------------------------------------------------------------- //
		time_t time_;
		struct tm *tm_;

		char buffer[80];
		char host[NI_MAXHOST];
		char service[NI_MAXSERV];

		struct sockaddr client_addr;
		socklen_t client_len = sizeof(struct sockaddr);

		// Escribe algo de información sobre este thread:
		std::cout << "Thread [" << std::this_thread::get_id() << "]: Activo." << std::endl;

		bool exit = false;
		char response[MAX_RESPONSE_LEN];
		while (!exit)
		{
			// Recive un mensaje de un cliente:
			ssize_t bytes = recvfrom(fd_, buffer, 79 * sizeof(char), 0, &client_addr,
									 &client_len);

			// Si se ha producido un error, por ejemplo cuando el servidor se cierra,
			// bytes será asignado como -1, en ese caso cerraremos el loop ya que el
			// socket es potencialmente inválido:
			if (bytes == -1)
			{
				std::cerr << "recvfrom: " << std::endl;
				break;
			}

			// Añadimos una pequeña espera:
			std::this_thread::sleep_for(std::chrono::seconds(1));

			// Escribe algo de información del cliente:
			getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
						NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

			std::cout << bytes << " bytes de " << host << ":" << service << std::endl;

			// Si no se mandaron suficientes bytes, continua:
			if (bytes == 0)
			{
				continue;
			}

			switch (buffer[0])
			{
			case 't':
				time(&time_);
				tm_ = localtime(&time_);
				bytes = strftime(response, MAX_RESPONSE_LEN, "%H:%M:%S %p", tm_);
				sendto(fd_, response, bytes, 0, &client_addr, client_len);
				break;
			case 'd':
				time(&time_);
				tm_ = localtime(&time_);
				bytes = strftime(response, MAX_RESPONSE_LEN, "%Y-%m-%d", tm_);
				sendto(fd_, response, bytes, 0, &client_addr, client_len);
				break;
			default:
				std::cout << "Comando no soportado " << buffer[0] << std::endl;
			}
		}

		std::cout << "Thread [" << std::this_thread::get_id() << "]: Inactivo." << std::endl;
	}
};

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Debes proveer la dirección. Por ejemplo: 0.0.0.0"
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
	// INICIALIZACIÓN SOCKET & BIND //
	// ---------------------------------------------------------------------- //

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	// res contiene la representación como sockaddr de dirección + puerto
	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rc != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
		return EXIT_FAILURE;
	}

	int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (bind(sd, res->ai_addr, res->ai_addrlen) != 0)
	{
		std::cerr << "bind: " << std::endl;
		return EXIT_FAILURE;
	}

	freeaddrinfo(res);

	// Crea tantos threads como se hayan especificado, o 3 threads en caso de
	// no haberse escrito el argumento:
	int n_threads = argc >= 4 ? atoi(argv[3]) : 3;
	for (int i = 0; i < n_threads; ++i)
	{
		// Crea una conexión, el thread se encarga de liberar la memoria tan
		// pronto como haya terminado de trabajar:
		const auto *conn = new Connection(sd);
		std::thread([conn]() {
			conn->run();
			delete conn;
		}).detach();

		// Espera un segundo antes de crear el siguiente thread para evitar
		// posibles errores al llamar recvfrom de manera simultánea:
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	// Lee el input de la consola, si el usuario escribe 'q' en la consola, el
	// servidor debe cerrar:
	std::string v;
	do {
		std::cin >> v;
	} while (v != "q");

	close(sd);

	return EXIT_SUCCESS;
}
