#include "server_multimetr.h"

namespace m_server{

int readall(int s, char *buf, int len, int flags = 0){
    int total = 0;
    int n;
    while(total < len)
    {
        n = recv(s, buf+total, len-total, flags);
        if(n == -1) {
            break;
        }
        total += n;
        if(buf[total - 1] == '\n'){
            break;
        }
    }
    return (n==-1 ? -1 : total);
}

int writeall(int s, char* buf, int len, int flags = 0){
    int total = 0;
    int n;
    while(total < len){
        n = send(s, buf+total, len-total, flags);
        if(n == -1){
            break;
        }
        total += n;
    }
    return (n==-1 ? -1 : total);
}

std::string_view ToStringView(const ErrorServerMultimetr& err){
    using namespace std::literals;
    switch (err) {
    case ErrorServerMultimetr::SOCKET:
        return "socket"sv;
    case ErrorServerMultimetr::BIND:
        return "bind"sv;
    case ErrorServerMultimetr::ACCEPT:
        return "accept"sv;
    case ErrorServerMultimetr::NOT_ERROR:
        return "not error"sv;
    }
}

std::ostream& operator<<(std::ostream& os, const ErrorServerMultimetr& err){
    os << ToStringView(err);
    return os;
}

ServerMultimetr::ServerMultimetr(const char* path, size_t size_buf):
    path_to_socket_(path)
    , err_(ErrorServerMultimetr::NOT_ERROR)
    , size_buf_(size_buf)
{
    unlink(path_to_socket_);
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_sockfd < 0){
        err_ = ErrorServerMultimetr::SOCKET;
        PrintErr();
    }
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, path_to_socket_);
    server_len = sizeof(server_address);
    if(bind(server_sockfd, (sockaddr*)&server_address, server_len) < 0){
        err_ = ErrorServerMultimetr::BIND;
        PrintErr();
        close(server_sockfd);
    }
}

void ServerMultimetr::RunServer(std::function<std::string(std::string_view)> func){
    if(err_ != ErrorServerMultimetr::NOT_ERROR){
        return;
    }
    int client_sockfd;
    socklen_t client_len;
    sockaddr_un client_address;
    listen(server_sockfd, 5);
    while(true){
        char buf[size_buf_];
        std::cout << "server waiting" << std::endl;
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (sockaddr *)&client_address, &client_len);
        if(client_sockfd < 0){
            err_ = ErrorServerMultimetr::ACCEPT;
            PrintErr();
            break;
        }
        int len_request = readall(client_sockfd, buf, size_buf_);
        if(len_request > 0){
            std::string request(buf, buf + len_request);
            std::string answear = GetAnswear(request, func);
            writeall(client_sockfd, answear.data(), answear.size());
        }
        else{
            std::string answear = "false\n";
            writeall(client_sockfd, answear.data(), answear.size());
        }
        close(client_sockfd);
    }
    unlink(path_to_socket_);
}

std::string ServerMultimetr::GetAnswear(std::string_view request, std::function<std::string(std::string_view)> func){
    return func(request);
}

void ServerMultimetr::PrintErr(std::ostream& out) const{
    out << "Error " << err_ << '\n';
}

}//namespace m_server


