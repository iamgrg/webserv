/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gregoire <gregoire@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:32:01 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/17 18:18:17 by gregoire         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <set>
# include "../config/Config.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"
# include "../routes/Routes.hpp"

// Inclure d'autres fichiers d'en-tête nécessaires ici.

class Server {
    public:
        Server(Config const & config);
        ~Server();
        void start();
        void stop();

    private:
        Config _config;
        Routes _routes;
        // int _listen_fd;
        std::vector<int> _listen_fds;
        bool _isRunning;
        // struct sockaddr_in _server_addr;
        fd_set _readfds;
        std::vector<int> _clients;
        std::set<int> _clientsToClose;
        
        void _sendResponse(Response const & response, int const &client);
        void _initialize();
        // void _manageConnections();
        void _processClientRequest(int client_fd);
        void _acceptNewConnection(int listen_fd);
        void _handleRequests();
        void _prepareSocketSet(int &max_fd);
};

#endif // SERVER_HPP
