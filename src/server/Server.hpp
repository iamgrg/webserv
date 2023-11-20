/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gregoire <gregoire@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:32:01 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/20 07:49:04 by gregoire         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "../config/Config.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"
#include "../routes/Routes.hpp"

class Server {
public:
  Server(Config const &config);
  ~Server();
  void start();
  void stop();

private:
  Config _config;
  Routes _routes;
  std::vector<int> _listen_fds;
  bool _isRunning;
  fd_set _readfds;
  std::vector<int> _clients;
  std::set<int> _clientsToClose;

  void _sendResponse(Response const &response, int const &client);
  void _initialize();
  void _processClientRequest(int client_fd);
  void _acceptNewConnection(int listen_fd);
  void _handleRequests();
  void _prepareSocketSet(int &max_fd);
};

#endif // SERVER_HPP
