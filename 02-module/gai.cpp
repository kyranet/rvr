#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

#include <iostream>

bool scan_socktype(char *address, char *port, struct addrinfo *hints, struct addrinfo *res, int ai_socktype) {
    hints->ai_socktype = ai_socktype;

    int rc = getaddrinfo(address, port, hints, &res);

    if (rc != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return false;
    }

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    getnameinfo(res->ai_addr, res->ai_addrlen, host, NI_MAXHOST, service,
                NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    std::cout << "IP: " << host << " " << res->ai_family << " " << res->ai_socktype << std::endl;
    return true;
}

bool scan_family(char *address, char *port, struct addrinfo *hints, struct addrinfo *res, int ai_family) {
    hints->ai_family = ai_family;

    return scan_socktype(address, port, hints, res, SOCK_STREAM)
           && scan_socktype(address, port, hints, res, SOCK_DGRAM)
           && scan_socktype(address, port, hints, res, SOCK_RAW);
}

/*
    argv[0] ---> nombre del programa
    argv[1] ---> primer argumento (char *)

    ./addrinfo www.ucm.es 80
        argv[0] = "./addrinfo"
        argv[1] = "www.ucm.es"
        argv[2] = "80"
            |
            |
            V
        res->ai_addr ---> (socket + bind)
            |
            |
            V
        host (numeric)

    ./addrinfo 127.0.0.1 80
    ./addrinfo www.ucm.es http
*/
int main(int argc, char **argv) {
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof(struct addrinfo));

    scan_family(argv[1], argv[2], &hints, res, AF_INET);
    scan_family(argv[1], argv[2], &hints, res, AF_INET6);

    freeaddrinfo(res);

    return 0;
}
