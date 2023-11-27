/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gansard <gansard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:18:27 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/27 14:40:58 by gansard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server/Server.hpp"
#include "location/Location.hpp"
#include <csignal>
#include <iostream>
#include <iostream>
#include <ctime>
#include <signal.h>

int main() {
  try {
    Server *server = new Server("config/default.conf");
    for(std::vector<Config *>::const_iterator it = server->getConfigs().begin(); it != server->getConfigs().end(); ++it) {
      std::cout << **it << std::endl;
    }
    signal(SIGINT, Server::stop);
    server->start();
	delete server;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  return 0;
}
