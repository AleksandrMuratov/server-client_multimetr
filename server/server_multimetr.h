#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string_view>
#include <functional>


namespace m_server{

enum class ErrorServerMultimetr{
    NOT_ERROR,
    SOCKET,
    BIND,
    ACCEPT
};

std::ostream& operator<<(std::ostream& os, const ErrorServerMultimetr& err);

class ServerMultimetr{
public:
    ServerMultimetr(const char* path, size_t size_buf = 1024);
    void RunServer(std::function<std::string(std::string_view)> func);

    static std::string GetAnswear(std::string_view request, std::function<std::string(std::string_view)> func);

    void PrintErr(std::ostream& out = std::clog) const;

private:
    const char* path_to_socket_;
    ErrorServerMultimetr err_;
    size_t size_buf_;
    int server_sockfd;
    sockaddr_un server_address;
    int server_len;
};

}//namespace m_server


