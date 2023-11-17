/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gregoire <gregoire@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:32:05 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/17 18:26:13 by gregoire         ###   ########.fr       */
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
    std::vector<int>::const_iterator it;
    for(it =  _config.getPorts().begin(); it != _config.getPorts().end(); ++it){
        int port = *it;
        int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd == -1) {
            std::cerr << "Error: Could not create socket." << std::endl;
            exit(EXIT_FAILURE);
        }

        int opt = 1;
        if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            std::cerr << "Error: setsockopt failed." << std::endl;
            close(listen_fd);
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(_config.getHost().c_str());
        server_addr.sin_port = htons(port);

        if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            std::cerr << "Error: bind failed." << std::endl;
            close(listen_fd);
            exit(EXIT_FAILURE);
        }

        if (listen(listen_fd, 10) == -1) {
            std::cerr << "Error: listen failed." << std::endl;
            close(listen_fd);
            exit(EXIT_FAILURE);
        }
        _listen_fds.push_back(listen_fd);
    }
}


void Server::start() {
    _isRunning = true;
    std::cout << "Server started and listening on port " << this->_config.getPorts().at(0) << "..." << std::endl;
    while (_isRunning) {
        _handleRequests();
    }
}

void Server::stop() {
    _isRunning = false;
    for (std::vector<int>::iterator it = _listen_fds.begin(); it != _listen_fds.end(); ++it) {
        close(*it);
    }
    for (std::vector<int>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        close(*it);
    }
    std::cout << "Server stopped" << std::endl;
}


void Server::_handleRequests() {
    int max_fd = 0;
    std::set<int>::iterator fd;
    _prepareSocketSet(max_fd);

    fd_set tmpfds = _readfds;
    if (select(max_fd + 1, &tmpfds, NULL, NULL, NULL) < 0) {
        perror("select");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i <= max_fd; ++i) {
        if (FD_ISSET(i, &tmpfds)) {
            if (std::find(_listen_fds.begin(), _listen_fds.end(), i) != _listen_fds.end()) {
                _acceptNewConnection(i);
            } else {
                _processClientRequest(i);
            }
        }
    }
    for (fd = _clientsToClose.begin(); fd != _clientsToClose.end(); ++fd) {
        close(*fd);
        FD_CLR(*fd, &_readfds);
        _clients.erase(std::remove(_clients.begin(), _clients.end(), *fd), _clients.end());
    }
    _clientsToClose.clear();
}

void Server::_sendResponse(Response const &response, int const &client) {
    std::string responseString = response.buildResponse();
    if (send(client, responseString.c_str(), responseString.length(), 0) == -1) {
        std::cerr << "Error: send failed." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::_prepareSocketSet(int &max_fd) {
    FD_ZERO(&_readfds);
    
    // Ajouter les sockets d'écoute à _readfds
    for (std::vector<int>::iterator it = _listen_fds.begin(); it != _listen_fds.end(); ++it) {
        FD_SET(*it, &_readfds);
        max_fd = std::max(max_fd, *it);
    }

    // Ajouter les sockets clients valides à _readfds
    std::vector<int>::iterator it = _clients.begin();
    while (it != _clients.end()) {
        if (*it >= 0) {  // Vérifiez que le descripteur est valide
            FD_SET(*it, &_readfds);
            max_fd = std::max(max_fd, *it);
            ++it;
        } else {
            it = _clients.erase(it);  // Supprimez les descripteurs invalides
        }
    }
}


void Server::_acceptNewConnection(int listen_fd) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_fd < 0) {
        perror("accept");
        return;
    }
    FD_SET(client_fd, &_readfds);
    _clients.push_back(client_fd);
    std::cout << "Nouvelle connexion acceptée : " << client_fd << std::endl;
}

void Server::_processClientRequest(int client_fd) {
    char buffer[10000];
    int bytesRead = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        if (bytesRead < 0) {
            std::cerr << "Recv failed" << std::endl;
        }
        close(client_fd);
        FD_CLR(client_fd, &_readfds);
        _clients.erase(std::remove(_clients.begin(), _clients.end(), client_fd), _clients.end());
        _clientsToClose.insert(client_fd);
        return;
    }
    std::string message(buffer, bytesRead);
    // A FAIRE !! : check validité du format du message => pas de requetes HTTPS ou autres formats
    Request request(message, this->_config.getMaxBodySize());
    bool validHost = false;
    for (std::vector<std::string>::const_iterator it = this->_config.getNames().begin(); it != this->_config.getNames().end(); ++it)
    {
        if(request.getHeader("Host") == *it + ":" + std::to_string(this->_config.getPorts().at(0))
            || request.getHeader("Host") == *it + ":" + std::to_string(this->_config.getPorts().at(1)))
            validHost = true;
    }
    std::cout << "PORT[1] : " << this->_config.getPorts().at(1) << std::endl;
    if(!validHost)
    {
        std::string error = "HTTP/1.1 400 Bad Request\r\n\r\n";
        if (send(client_fd, error.c_str(), error.length(), 0) == -1) {
            std::cerr << "Error: send failed." << std::endl;
            exit(EXIT_FAILURE);
        }
        close(client_fd);
        FD_CLR(client_fd, &_readfds);
        return;
    }
    std::cout << request << std::endl;
    Response response = _routes.handle(request);
    this->_sendResponse(response, client_fd);
    _clientsToClose.insert(client_fd);
}