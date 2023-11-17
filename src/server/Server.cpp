/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gregoire <gregoire@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:32:05 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/17 15:41:23 by gregoire         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

Server::Server(Config const & config) : _config(config), _routes(config){
    _initialize();
}
Server::~Server() { }

void Server::_initialize() {
    // Créer un socket
    if ((this->_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Error: Could not create socket." << std::endl;
        exit(EXIT_FAILURE);
    }
    // Configurer les options du socket
    int opt = 1;
    if (setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Error: setsockopt failed." << std::endl;
        exit(EXIT_FAILURE);
    }
    // Préparer la structure sockaddr_in
    memset(&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    std::string ipString = this->_config.getHost();
    _server_addr.sin_addr.s_addr = inet_addr(ipString.c_str());
    _server_addr.sin_port = htons(this->_config.getPorts().at(0));
    // _server_addr.sin_port = htons(this->_config.getPort());
    // Lier le socket à l'adresse et au port
    if (bind(_listen_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) {
        std::cerr << "Error: bind failed." << std::endl;
        exit(EXIT_FAILURE);
    }
    // std::cout << "Server initialized" << std::endl;
}


void Server::start() {
    _isRunning = true;
    // Écouter sur le socket
    if (listen(this->_listen_fd, 10) == -1) {
        std::cerr << "Error: listen failed." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Server started and listening on port " << this->_config.getPorts().at(0) << "..." << std::endl;
    while (_isRunning) {
        _handleRequests();
    }
}

void Server::stop() {
    _isRunning = false;
    // Fermez les sockets ou faites d'autres opérations de nettoyage ici.
    std::cout << "Server stopped" << std::endl;
}
void Server::_sendResponse(Response const &response, int const &client) {
    std::string responseString = response.buildResponse();
    if (send(client, responseString.c_str(), responseString.length(), 0) == -1) {
        std::cerr << "Error: send failed." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::_handleRequests() {
    FD_ZERO(&_readfds);
    FD_SET(this->_listen_fd, &_readfds);
    int max_fd = this->_listen_fd;

    while (_isRunning) {
        fd_set tmpfds = _readfds;
        if (select(max_fd + 1, &tmpfds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i <= max_fd; ++i) {
            if (FD_ISSET(i, &tmpfds)) {
                if (i == this->_listen_fd) {
                    // Accepter la nouvelle connexion
                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    int client_fd = accept(this->_listen_fd, (struct sockaddr *)&client_addr, &addrlen);
                    if (client_fd < 0) {
                        perror("accept");
                        continue;
                    }
                    FD_SET(client_fd, &_readfds);
                    max_fd = std::max(max_fd, client_fd);
                    std::cout << "Nouvelle connexion acceptée : " << client_fd << std::endl;
                } else {
                    char buffer[10000];
                    int bytesRead = recv(i, buffer, sizeof(buffer), 0);
                    if (bytesRead <= 0) {
                        if (bytesRead < 0)
                                throw std::runtime_error("Recv failed");
                        close(i);
                        FD_CLR(i, &_readfds);
                        _clients.erase(std::remove(_clients.begin(), _clients.end(), i), _clients.end());
                    } else {
                        std::string message(buffer, bytesRead);
                        // A FAIRE !! : check validité du format du message => pas de requetes HTTPS ou autres formats
                        Request request(message, this->_config.getMaxBodySize());
                        // && request.getHeader("Host") + ":" + std::to_string(_config.getPorts().at(1)) != this->_config.getHost())
                        bool validHost = false;
                        for (std::vector<std::string>::const_iterator it = this->_config.getNames().begin(); it != this->_config.getNames().end(); ++it)
                        {
                            if(request.getHeader("Host") == *it + ":" + std::to_string(this->_config.getPorts().at(0)))
                                validHost = true;
                        }
                        if(!validHost)
                        {
                            std::string error = "HTTP/1.1 400 Bad Request\r\n\r\n";
                            if (send(i, error.c_str(), error.length(), 0) == -1) {
                                std::cerr << "Error: send failed." << std::endl;
                                exit(EXIT_FAILURE);
                            }
                            close(i);
                            FD_CLR(i, &_readfds);
                            continue;
                        }
                        std::cout << request << std::endl;
                        Response response = _routes.handle(request);
                        this->_sendResponse(response, i);
                    }
                    close(i);
                    FD_CLR(i, &_readfds);
                }
            }
        }
    }
}

