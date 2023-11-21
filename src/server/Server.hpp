/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gansard <gansard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:32:01 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/20 17:33:31 by gansard          ###   ########.fr       */
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
  Server(Config *config);
  ~Server();
  static void stop(int signal) {
	(void) signal;
  	if (instance) {
  		instance->realStop();
  	}
  }
  void start();
  void realStop();
  static Server* instance;

private:
  Config _config;
  Config *_ptrConfig;
  Routes _routes;
  std::vector<int> _listen_fds;
  bool _isRunning;
  fd_set _readfds;
  std::vector<int> _clients;
  std::set<int> _clientsToClose;

  void _sendResponse(Response *response, int const &client);
  void _initialize();
  void _processClientRequest(int client_fd);
  void _acceptNewConnection(int listen_fd);
  void _handleRequests();
  void _prepareSocketSet(int &max_fd);
};

#endif // SERVER_HPP
