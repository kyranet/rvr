#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>

#define MAX_RESPONSE_LEN 15

/*
  argv[0] ---> nombre del programa
  argv[1] ---> primer argumento (char *)
  ./time_client 127.0.0.1 3000 t
    argv[0] = "./time_client"
    argv[1] = "127.0.0.1"
    argv[2] = "3000"
    argv[3] = "t"
      |
      |
      V
    res->ai_addr ---> (socket + bind)
*/
int main(int, char **argv) {
  struct addrinfo hints;
  struct addrinfo *res;

  // ---------------------------------------------------------------------- //
  // INICIALIZACIÓN SOCKET//
  // ---------------------------------------------------------------------- //

  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

  if (rc != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
    return -1;
  }

  int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  freeaddrinfo(res);
  // ---------------------------------------------------------------------- //
  // ENVIO MENSAJE DE CLIENTE //
  // ---------------------------------------------------------------------- //
  char buffer[MAX_RESPONSE_LEN];

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[2]));
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);

  sendto(sd, argv[3], 2, 0, (struct sockaddr *)&server_addr,
         sizeof(server_addr));

  socklen_t server_addr_len;
  ssize_t bytes = recvfrom(sd, buffer, (MAX_RESPONSE_LEN - 1) * sizeof(char), 0,
                           (struct sockaddr *)&server_addr, &server_addr_len);

  if (bytes == -1) {
    std::cerr << "recvfrom: " << std::endl;
    return -1;
  }

  std::cout << buffer << std::endl;

  return 0;
}
