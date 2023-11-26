/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gregoire <gregoire@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:32:01 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/25 09:26:21 by gregoire         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

# include "../config/Config.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"
# include "../routes/Routes.hpp"
# include <csignal>



class Server {
public:
  // Server(Config *config);
  Server(std::string const &configPath, Config *config);
  ~Server();
  static void stop(int sgnl) {
	(void) sgnl;
  	if (instance)
  		instance->realStop();
  }
  void start();
  void realStop();
  static Server* instance;
  std::vector<Config *> const &getConfigs() const;

private:
  std::vector<std::stringstream *> _serverBlocks;
  std::vector<Config *> _configs;
  Config _config;
  std::vector<Routes *> _routesS;
  Routes _routes;
  Config *_ptrConfig;
  std::vector<int> _listen_fds;
  bool _isRunning;
  fd_set _readfds;
  std::vector<int> _clients;
  std::set<int> _clientsToClose;

  // Méthodes privées
  void  _splitServerBlocks(const std::string &configPath);
  void  _sendResponse(Response *response, int const &client);
  void  _initialize();
  void  _processClientRequest(int client_fd);
  void  _acceptNewConnection(int listen_fd);
  void  _handleRequests();
  void  _prepareSocketSet(int &max_fd);
  Routes* findMatchingRoute(const Request& request);
};

#endif // SERVER_HPP
