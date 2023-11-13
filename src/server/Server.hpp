/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gregoire <gregoire@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:32:01 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/11 16:47:29 by gregoire         ###   ########.fr       */
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
        int _listen_fd;
        bool _isRunning;
        struct sockaddr_in _server_addr;
        fd_set _readfds;
        std::vector<int> _clients;
        
        void _sendResponse(Response const & response, int const &client);
        void _initialize();
        void _handleRequests();
};

#endif // SERVER_HPP
