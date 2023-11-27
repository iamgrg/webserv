/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gansard <gansard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:32:05 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/27 14:39:15 by gansard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server* Server::instance = NULL;

Server::Server(std::string const &configPath) {
  _splitServerBlocks(configPath);
  for (std::vector<std::stringstream*>::iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it) {
    try {
      bool used = false;
      Config *config = new Config(**it);
      for(std::vector<Config *>::const_iterator it = _configs.begin(); it != _configs.end(); ++it){
      std::vector<int> tmp = (*it)->getPorts();
      for(std::vector<int>::const_iterator it2 = tmp.begin(); it2 != tmp.end(); ++it2){
        if ((std::find(config->getPorts().begin(), config->getPorts().end(), *it2) != config->getPorts().end()) 
        && (*it)->getHost() == config->getHost()) {used = true;}
        }
      }
      if(!used)
      {
        _configs.push_back(config);
        Routes *route = new Routes(*config);
        _routesS.push_back(route);
      }
      else
        delete config;
    }
    catch(const std::exception &e){
      std::cerr << e.what() << std::endl;
    }
  }
  for(std::vector<Routes *>::iterator it = _routesS.begin(); it != _routesS.end(); ++it)
  {
    std::cout << "Host : " << (*it)->getHost() << std::endl;
    for (std::vector<int>::const_iterator it2 = (*it)->getPorts().begin(); it2 != (*it)->getPorts().end(); ++it2)
      std::cout << "Port : " << *it2 << std::endl;
  }
  instance = this;
  _initialize();
}
//================================================================================================// 
Server::~Server() {}
//================================================================================================// 
std::vector<Config *> const & Server::getConfigs() const {
  return this->_configs;
}
//================================================================================================// 
void Server::_initialize() {
  // Parcourir toutes les routes
  for (std::vector<Routes*>::const_iterator routeIt = _routesS.begin(); routeIt != _routesS.end(); ++routeIt) {
    Routes* route = *routeIt;
    const std::vector<int>& ports = route->getPorts();
    std::string host = route->getHost();
    // Créer un socket d'écoute pour chaque port de la route
    for (std::vector<int>::const_iterator portIt = ports.begin(); portIt != ports.end(); ++portIt) {
      int port = *portIt;
      // pourt chaque port de chaque routes, créer un socket d'écoute
      int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
      if (listen_fd == -1) {
        std::cerr << "Error: Could not create socket." << std::endl;
        exit(EXIT_FAILURE);
      }
      int flags = fcntl(listen_fd, F_GETFL, 0);
      if (flags == -1) {
          exit(EXIT_FAILURE);
      }
      // rendre le socket non bloquant
      flags = (flags | O_NONBLOCK);
      if (fcntl(listen_fd, F_SETFL, flags) == -1) {
          exit(EXIT_FAILURE);
      }
      int opt = 1;
      // définir les options du socket pour permettre la réutilisation de l'adresse et du port
      if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Error: setsockopt failed." << std::endl;
        close(listen_fd);
        exit(EXIT_FAILURE);
      }
     // Créer une structure d'adresse pour le socket d'écoute
      struct sockaddr_in server_addr;
      memset(&server_addr, 0, sizeof(server_addr));
      // remplir la structure d'adresse
      server_addr.sin_family = AF_INET;
      server_addr.sin_addr.s_addr = inet_addr(host.c_str()); // Utilise l'hôte de la route
      server_addr.sin_port = htons(port);

      // lier le socket d'écoute à l'adresse et au port
      if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error: bind failed." << std::endl;
        close(listen_fd);
        exit(EXIT_FAILURE);
      }
      // mettre le socket d'écoute en mode écoute
      if (listen(listen_fd, 10000) == -1) {
        std::cerr << "Error: listen failed." << std::endl;
        close(listen_fd);
        exit(EXIT_FAILURE);
      }

      _listen_fds.push_back(listen_fd);
    }
  }
}
//================================================================================================// 
void Server::_splitServerBlocks(const std::string &configPath) {
    this->_serverBlocks.clear();
    std::ifstream configFile(configPath.c_str());
    std::string line;
    bool insideServerBlock = false;
    std::stringstream *currentBlock = NULL;
    if (!configFile.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier de configuration : " + configPath);
    }
    while (std::getline(configFile, line)) {
        if (line == "server {") {
            insideServerBlock = true;
            currentBlock = new std::stringstream();
        }
        if (insideServerBlock) {
            *currentBlock << line << "\n";
        }
        if (line == "}" && insideServerBlock) {
            insideServerBlock = false;
            this->_serverBlocks.push_back(currentBlock);
            currentBlock = NULL; // Important pour éviter de réutiliser accidentellement le même pointeur
        }
    }
    if (currentBlock) {
        delete currentBlock; // Nettoyage dans le cas où la dernière accolade fermante manque
    }
}
//================================================================================================// 
void Server::start() {
  _isRunning = true;
  std::cout << "Server started..." << std::endl;
  int i = 0;
  while (_isRunning) {
    std::cout << i++ << std::endl;
    _handleRequests();
  }
}
//================================================================================================// 
void Server::_sendResponseIfReady(int client_fd) {
    // Vérifiez si une réponse est prête pour ce client et l'envoyez
    std::map<int, Response*>::iterator response_it = _responses.find(client_fd);
    if (response_it != _responses.end()) {
        Response *response = response_it->second;
        _sendResponse(response, client_fd);
        _responses.erase(response_it);
        _clientsToClose.insert(client_fd);
    }
}
//================================================================================================// 
void Server::_handleRequests() {
  int max_fd = 0;
  std::set<int>::iterator fd;
  _prepareSocketSet(max_fd);

  fd_set tmp_readfds = _readfds;
  fd_set tmp_writefds = _writefds;
  if (select(max_fd + 1, &tmp_readfds, &tmp_writefds, NULL, NULL) < 0) 
    return ;

  for (int i = 0; i <= max_fd; ++i) {
    if (FD_ISSET(i, &tmp_readfds)) {
      if (std::find(_listen_fds.begin(), _listen_fds.end(), i) != _listen_fds.end()) {
        _acceptNewConnection(i);
      } else {
        _processClientRequest(i);
      }
    }
    if (FD_ISSET(i, &tmp_writefds)) {
      _sendResponseIfReady(i);
    }
  }
  for (fd = _clientsToClose.begin(); fd != _clientsToClose.end(); ++fd) {
    close(*fd);
    FD_CLR(*fd, &_readfds);
    _clients.erase(std::remove(_clients.begin(), _clients.end(), *fd), _clients.end());
  }
  _clientsToClose.clear();
}
//================================================================================================//
void Server::_prepareSocketSet(int &max_fd) {
  FD_ZERO(&_readfds);
  FD_ZERO(&_writefds);
  // Ajouter les sockets d'écoute à _readfds
  for (std::vector<int>::iterator it = _listen_fds.begin(); it != _listen_fds.end(); ++it) {
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
  // Ajouter les sockets clients valides à _writefds
    std::map<int, Response*>::iterator response_it = _responses.begin();
    for(; response_it != _responses.end(); ++response_it) {
        int client_fd = response_it->first;
        FD_SET(client_fd, &_writefds);
        max_fd = std::max(max_fd, client_fd);
    }
}
//================================================================================================// 
void Server::_sendResponse(Response *response, int const &client) {
  std::string responseString = response->buildResponse();
  std::cout << "Sending response to client " << client << " : " << std::endl;
  std::cout << *response << std::endl; 
  delete response;
  ssize_t bytesSent = send(client, responseString.c_str(), responseString.length(), 0);
  if (bytesSent == -1) {
      std::cerr << "Error: send failed with errno " << errno << std::endl;
      exit(EXIT_FAILURE);
  } else if (bytesSent == 0) {
      std::cerr << "Connection closed by peer." << std::endl;
      exit(EXIT_FAILURE);
  }
}
//================================================================================================// 
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
//================================================================================================// 
Routes* Server::findMatchingRoute(std::string const &requestHost, int requestPort) {
  for (std::vector<Routes*>::const_iterator it = _routesS.begin(); it != _routesS.end(); ++it) {
    Routes* route = *it;
    if (route->getHost() == requestHost) {
      const std::vector<int>& ports = route->getPorts();
      for (std::vector<int>::const_iterator portIt = ports.begin(); portIt != ports.end(); ++portIt) {
        if (*portIt == requestPort) {
          return route;
        }
      }
    }
  }
  return NULL; // Aucune route correspondante trouvée
}
//================================================================================================//
void Server::_extractHost(std::string const &message, int &requestPort, std::string &requestHost) {
    // Trouver le début du header "Host"
    std::size_t hostStart = message.find("Host:");
    if (hostStart == std::string::npos) {
        std::cerr << "Host header not found" << std::endl;
        return;
    }
    // Trouver la fin de la ligne du header "Host"
    std::size_t hostEnd = message.find("\r\n", hostStart);
    std::string hostHeader = message.substr(hostStart, hostEnd - hostStart);
    // Extraire le contenu du header "Host" (sans "Host: ")
    hostHeader = hostHeader.substr(6); // 6 est la longueur de "Host: "
    std::size_t colonPos = hostHeader.find(":");
    requestPort = 80; // Port par défaut HTTP si aucun port n'est spécifié
    if (colonPos != std::string::npos) {
        requestHost = hostHeader.substr(0, colonPos);
        std::istringstream(hostHeader.substr(colonPos + 1)) >> requestPort;
    } else {
        requestHost = hostHeader; // Si aucun port n'est spécifié, utilise le host tel quel
    }
}
//================================================================================================//
void Server::_processClientRequest(int client_fd) {
	std::string message;
	int bytesRead;
	int totalBytesRead = 0;
	int contentLength = -1; // Valeur initiale indiquant que Content-Length n'est pas encore connu
  std::string host;
  int requestPort;
	bool validMethod;
  message = _readHttpRequestHeader(client_fd, validMethod, contentLength, totalBytesRead);
  _extractHost(message, requestPort, host);
  Routes *matchingRoute = findMatchingRoute(host, requestPort);
  if(matchingRoute == NULL) {_sendBadRequest(client_fd); return;}
  if(host == "") {std::cerr << "no host" << std::endl; return;}
	while (totalBytesRead < contentLength) {
		char buffer[1024];
		bytesRead = recv(client_fd, buffer, sizeof(buffer), 0);
		if (bytesRead == 0 || bytesRead == -1)
			continue;
		message.append(buffer, bytesRead);
		totalBytesRead += bytesRead;
		if(totalBytesRead > matchingRoute->getMaxBodySize()) {std::cerr << "body shaming" << std::endl; break ;}
	}
	if (!validMethod) {_sendBadRequest(client_fd); return;}
  Request request(message, matchingRoute->getMaxBodySize());
  Response *response = matchingRoute->handle(request);
  _responses[client_fd] = response; // Stockez la réponse pour l'envoyer plus tard
  FD_SET(client_fd, &_writefds); // Marquez ce socket pour la surveillance de l'écriture
}
//================================================================================================//
std::string Server::_readHttpRequestHeader(int client_fd, bool &validMethod, int &contentLength, int &totalBytesRead) {
    std::string message;
    int bytesRead;
    std::string left;
    validMethod = true;
    while (true) {
        char buffer[1024];
        bytesRead = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytesRead == 0 || bytesRead == -1) {
            break;
        }
        message.append(buffer, bytesRead);
        if (totalBytesRead == 0) {
            std::string tmp = message.substr(0, bytesRead > 10 ? 10 : bytesRead);
            if (tmp.find("POST") == std::string::npos && tmp.find("GET") == std::string::npos && tmp.find("DELETE") == std::string::npos) {
                validMethod = false;
                break;
            }
        }
        size_t endOfHeader = message.find("\r\n\r\n");
        if (endOfHeader != std::string::npos) {
            size_t contentLengthPos = message.find("Content-Length:");
            if (contentLengthPos != std::string::npos) {
                size_t start = message.find(':', contentLengthPos) + 1;
                size_t end = message.find("\r\n", start);
                std::istringstream iss(message.substr(start, end - start));
                iss >> contentLength;
                left = message.substr(endOfHeader + 4);
                totalBytesRead += left.length();
            } else {
                contentLength = 0;
            }
            break;
        }
    }
    return message;
}
//================================================================================================//
void Server::_sendBadRequest(int client_fd) {
    std::string error = "HTTP/1.1 400 Bad Request\r\n\r\n";
    ssize_t bytesSent = send(client_fd, error.c_str(), error.length(), 0);
    if (bytesSent == -1) {
      std::cerr << "Error: send failed." << std::endl;
      exit(EXIT_FAILURE);
    } else if (bytesSent == 0) {
      std::cerr << "Connection closed by peer." << std::endl;
      exit(EXIT_FAILURE);
    }
    _clientsToClose.insert(client_fd);
}
//================================================================================================// 
void Server::realStop() {
    _isRunning = false;
    for (std::vector<int>::iterator it = _listen_fds.begin(); it != _listen_fds.end(); ++it) {
        close(*it);
    }
    for (std::vector<int>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        close(*it);
    }
    for (std::vector<Routes *>::iterator it = _routesS.begin(); it != _routesS.end(); ++it) {
        delete (*it);
    }
    _routesS.clear(); 
    for (std::vector<Config *>::iterator it = _configs.begin(); it != _configs.end(); ++it) {
        for (std::vector<Location *>::const_iterator it2 = (*it)->getLocations().begin(); it2 != (*it)->getLocations().end(); ++it2) {
            delete (*it2);
        }
        delete (*it);
    }
    _configs.clear();
    for (std::vector<std::stringstream *>::iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it) {
        delete (*it);
    }
    _serverBlocks.clear();
    //delete (this);
    std::cout << std::endl << "Server clean stopped with success !" << std::endl;
}
