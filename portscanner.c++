#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>

bool is_port_open(const std::string& ip, int port)
{
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    fcntl(sock, F_SETFL, O_NONBLOCK);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if (select(sock + 1, NULL, &fds, NULL, &tv) > 0)
    {
        int error;
        socklen_t len = sizeof(error);

        getsockopt(
            sock,
            SOL_SOCKET,
            SO_ERROR,
            &error,
            &len
        );

        close(sock);

        return error == 0;
        
    }
    close(sock);
    return false;
}

int main()
{
    std::string target;
    std::cout <<"Enter your target IP: ";
    std::cin >> target;

    int open_count = 0;
    int closed_count = 0;

    for (int port = 1; port <= 65535; port++)
    {
        if (is_port_open(target, port))
        {
            std::cout << "Port " << port << " is OPEN\n";
            open_count++;
        }
        else
        {
            closed_count++;
        }
    }

    std::cout << "\nScan complete.\n";
    std::cout << "Open ports: " << open_count << '\n';
    std::cout << "Closed/unresponsive ports: " << closed_count << '\n';

    return 0;
}
