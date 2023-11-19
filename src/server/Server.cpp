/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gansard <gansard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:32:05 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/19 18:57:54 by gansard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(Config const &config) : _config(config), _routes(config) {
  _initialize();
}
Server::~Server() {}

void Server::_initialize() {
  std::vector<int>::const_iterator it;
  for (it = _config.getPorts().begin(); it != _config.getPorts().end(); ++it) {
    int port = *it;
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
      std::cerr << "Error: Could not create socket." << std::endl;
      exit(EXIT_FAILURE);
    }
	int flags = fcntl(listen_fd, F_GETFL, 0);
	if (flags == -1) {
      exit(EXIT_FAILURE);
	}
	flags = (flags | O_NONBLOCK);
	if (fcntl(listen_fd, F_SETFL, flags) == -1) {
      exit(EXIT_FAILURE);
	}
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
        -1) {
      std::cerr << "Error: setsockopt failed." << std::endl;
      close(listen_fd);
      exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(_config.getHost().c_str());
    server_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
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
  std::cout << "Server started and listening on port "
            << this->_config.getPorts().at(0) << "..." << std::endl;
  while (_isRunning) {
    _handleRequests();
  }
}

void Server::stop() {
  _isRunning = false;
  for (std::vector<int>::iterator it = _listen_fds.begin();
       it != _listen_fds.end(); ++it) {
    close(*it);
  }
  for (std::vector<int>::iterator it = _clients.begin(); it != _clients.end();
       ++it) {
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
      if (std::find(_listen_fds.begin(), _listen_fds.end(), i) !=
          _listen_fds.end()) {
        _acceptNewConnection(i);
      } else {
        _processClientRequest(i);
      }
    }
  }
  for (fd = _clientsToClose.begin(); fd != _clientsToClose.end(); ++fd) {
    close(*fd);
    FD_CLR(*fd, &_readfds);
    _clients.erase(std::remove(_clients.begin(), _clients.end(), *fd),
                   _clients.end());
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
  for (std::vector<int>::iterator it = _listen_fds.begin();
       it != _listen_fds.end(); ++it) {
    FD_SET(*it, &_readfds);
    max_fd = std::max(max_fd, *it);
  }

  // Ajouter les sockets clients valides à _readfds
  std::vector<int>::iterator it = _clients.begin();
  while (it != _clients.end()) {
    if (*it >= 0) { // Vérifiez que le descripteur est valide
      FD_SET(*it, &_readfds);
      max_fd = std::max(max_fd, *it);
      ++it;
    } else {
      it = _clients.erase(it); // Supprimez les descripteurs invalides
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
  int client_flags = fcntl(client_fd, F_GETFL, 0);
  client_flags |= O_NONBLOCK;
  if (fcntl(client_fd, F_SETFL, client_flags) == -1) {
  	perror("no block flag");
  }
  FD_SET(client_fd, &_readfds);
  _clients.push_back(client_fd);
  std::cout << "Nouvelle connexion acceptée : " << client_fd << std::endl;
}

void Server::_processClientRequest(int client_fd) {
	std::ostringstream ss;
	std::ostringstream ss2;
	ss << this->_config.getPorts().at(0);
	ss2 << this->_config.getPorts().at(1);
	std::string portStr = ss.str();
	std::string port2Str = ss2.str();
	std::string message;
	int bytesRead;
	int totalBytesRead = 0;
	int contentLength = -1; // Valeur initiale indiquant que Content-Length n'est pas encore connu
	bool validMethod = true;
	std::string left;
	// Lire l'en-tête
	do {
		char buffer[1024];
		bytesRead = recv(client_fd, buffer, sizeof(buffer), 0);
		if (bytesRead <= 0) {
			if (bytesRead < 0 && (errno != EWOULDBLOCK && errno != EAGAIN)) {
				std::cerr << "Erreur lors de la réception des données" << std::endl;
				// Gérer l'erreur
			}
			break; // Pause ou fin de la connexion
		}
		message.append(buffer, bytesRead);
		std::string tmp = message.substr(0, 10);
		std::cout << "CATAL TMP : " << tmp << std::endl;
		if(tmp.find("POST") == std::string::npos && tmp.find("GET") == std::string::npos && tmp.find("DELETE") == std::string::npos)
		{
			std::cout << "CATAL TPMP HANOUNAZDAS : " << tmp << std::endl;
			contentLength = 0;
			validMethod = false;
			break ;
		}
		std::cout << "YAAAAAAAAAAAAAAAAAAAAASSSSSSSS" << std::endl;
		std::cout << message << std::endl;
		std::cout << "YAAAAAAAAAAAAAAAAAAAAASSSSSSSS" << std::endl;
		// Vérifier si l'en-tête est complet
		if (message.find("\r\n\r\n") != std::string::npos) {
			size_t contentLengthPos = message.find("Content-Length:");
			if (contentLengthPos != std::string::npos) {
				size_t start = message.find(":", contentLengthPos) + 1; // Début de la valeur numérique
				size_t end = message.find("\r\n", start); // Fin de la ligne
				std::string contentLengthStr = message.substr(start, end - start);
				std::istringstream iss(contentLengthStr);
				if (!(iss >> contentLength)) {
					std::cerr << "Invalid Content-Length: " << contentLengthStr << std::endl;
				}
				// logic 
				left = message.substr(message.find("\r\n\r\n") + 4, std::string::npos);
				totalBytesRead += left.length();
			} else {
				contentLength = 0; // Si Content-Length n'est pas trouvé, supposez qu'il n'y a pas de corps
			}
			break ;
		}
	} while (true);
	while (totalBytesRead < contentLength) {
		char buffer[1024];
		bytesRead = recv(client_fd, buffer, sizeof(buffer), 0);
		std::cout << "buffer : " << buffer << std::endl;
		std::cout << "bytesRead : " << bytesRead << std::endl;
		std::cout << "contentLength : " << contentLength << std::endl;
		if (bytesRead <= 0) {
			if (bytesRead < 0 && (errno != EWOULDBLOCK && errno != EAGAIN)) {
				std::cerr << "Erreur lors de la réception des données" << std::endl;
				break ;
			}
			continue; // Pause ou fin de la connexion
		}
		message.append(buffer, bytesRead);
		totalBytesRead += bytesRead;
		if(totalBytesRead > _config.getMaxBodySize())
		{
			std::cerr << "body shaming" << std::endl;
			break ;
		}
	}
	// Traiter le message complet ici
	// ----------------------------------------
  // A FAIRE !! : check validité du format du message => pas de requetes HTTPS
  // ou autres formats
	if (!validMethod) {
    std::string error = "HTTP/1.1 400 Bad Request\r\n\r\n";
    if (send(client_fd, error.c_str(), error.length(), 0) == -1) {
      std::cerr << "Error: send failed." << std::endl;
      exit(EXIT_FAILURE);
    }
  	_clientsToClose.insert(client_fd);
    return;
  }
  Request request(message, this->_config.getMaxBodySize());
  bool validHost = false;
  for (std::vector<std::string>::const_iterator it =
           this->_config.getNames().begin();
       it != this->_config.getNames().end(); ++it) {
    if (request.getHeader("Host") == *it + ":" + portStr ||
        request.getHeader("Host") == *it + ":" + port2Str)
      validHost = true;
  }
  if (!validHost) {
    std::string error = "HTTP/1.1 400 Bad Request\r\n\r\n";
    if (send(client_fd, error.c_str(), error.length(), 0) == -1) {
      std::cerr << "Error: send failed." << std::endl;
      exit(EXIT_FAILURE);
    }
  	_clientsToClose.insert(client_fd);
    return;
  }
  std::cout << request << std::endl;
  Response response = _routes.handle(request);
  this->_sendResponse(response, client_fd);
  _clientsToClose.insert(client_fd);
}