#define SERVER_H
#ifdef  SERVER_H

#include <iostream>
#include <winsock2.h>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <QString>

#include "cqueue.h"
#include "message.h"

extern ConcurrentQueue<Message> msgQueue;

namespace net {

    class Server {
    private:
        SOCKET lstSocket{};
        std::string _addr;
        int _port;
    public:
        Server(const std::string& _addr, int _port)
            : _addr(_addr),
              _port(_port)
            {}
        ~Server();
        void Start();
        void Stop();
        void startRecieving();
        void sendToAddress(const std::string&, int, const Message&);
        static std::string getLocalIP();
        std::string ip() const;
        int port() const;
    };

    std::string Server::ip() const {
        return _addr;
    }

    int Server::port() const {
        return _port;
    }
    Server::~Server()
    {
        closesocket(lstSocket);
        WSACleanup();
    }

    void Server::Stop() {
        closesocket(lstSocket);
        WSACleanup();
    }

    void Server::Start() {
        WSADATA wsaData;
        assert(WSAStartup(MAKEWORD(2, 2), &wsaData) == 0 && "Failed to initialize winsock");

        assert((lstSocket = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET && "Failed to create socket");

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(_port);
        serverAddr.sin_addr.s_addr = inet_addr(_addr.c_str());

        if (bind(lstSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Failed to bind socket" << std::endl;
            closesocket(lstSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        if (listen(lstSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Failed to listen on socket" << std::endl;
            closesocket(lstSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        std::cout << "Server started on " << _addr << std::endl;
        std::cout << "Listening on port " << _port << "..." << std::endl;
    }

    void Server::startRecieving() {
        while (true) {
            sockaddr_in clientAddr{};
//            clientAddr.sin_addr.S_un.S_addr = inet_addr(recipientIpAdress.toStdString().c_str());
//            clientAddr.sin_port = htons(recPort);
//            clientAddr.sin_family = AF_INET;

            int clientAddressSize = sizeof(clientAddr);

            SOCKET clientSocket = accept(lstSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddressSize);
            if (clientSocket == static_cast<SOCKET>(SOCKET_ERROR)) {
                std::cerr << "Failed to accept client connection" << std::endl;
                closesocket(lstSocket);
                WSACleanup();
                exit(EXIT_FAILURE);
            }

            char requestBuffer[sizeof(Message)];
            memset(requestBuffer, 0, sizeof(Message));

            if (recv(clientSocket, requestBuffer, sizeof(Message), 0) == SOCKET_ERROR) {
                std::cerr << "Failed to receive request" << std::endl;
                closesocket(clientSocket);
                WSACleanup();
                exit(EXIT_FAILURE);
            }

            msgQueue.push(*reinterpret_cast<Message*>(requestBuffer));

            closesocket(clientSocket);

            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void Server::sendToAddress(const std::string& ipAddress, int port, const Message& msg) {
        /*WSADATA wsaData;
        assert(WSAStartup(MAKEWORD(2, 2), &wsaData) == 0 && "Failed to initialize winsock");*/

        //assert((lstSocket = socket(AF_INET, SOCK_STREAM, NULL)) != INVALID_SOCKET && "Failed to create socket");

        sockaddr_in recAddr{};
        recAddr.sin_family = AF_INET;
        recAddr.sin_port = htons(port);
        recAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());

        SOCKET connection = socket(AF_INET, SOCK_STREAM, 0);
        if (connection == INVALID_SOCKET) {
            std::cerr << "Failed to create sender connection socket" << std::endl;
            closesocket(lstSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        assert(connect(connection, reinterpret_cast<sockaddr*>(&recAddr), sizeof(recAddr)) == 0 && "Failed to connect to reciever");

        //std::cout << "Sender connected" << std::endl;

        auto buf = reinterpret_cast<const char*>(&msg);
        if (send(connection, buf,  sizeof(Message), 0) == 0) {
            std::cerr << "Failed to send" << std::endl;
        }

        closesocket(connection);
        //WSACleanup();
    }

    std::string Server::getLocalIP()
    {
        std::string out = "WinSock ERR";
        WSADATA wsaData;
        if (!WSAStartup(WINSOCK_VERSION, &wsaData)) {
            char chInfo[64];
            if (!gethostname(chInfo, sizeof(chInfo))) {
                hostent *sh;
                sh=gethostbyname((char*)&chInfo);
                if (sh!=NULL) {
                    int nAdapter = 0;
                    while (sh->h_addr_list[nAdapter]) {
                        struct sockaddr_in adr;
                        memcpy(&adr.sin_addr, sh->h_addr_list[nAdapter], sh->h_length);
                        out = inet_ntoa(adr.sin_addr);
                        nAdapter++;
                    }
                }
            }
        }
        WSACleanup();
        return out;
    }
}

#endif //SERVER_H
