/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gansard <gansard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:18:27 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/19 14:00:58 by gansard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server/Server.hpp"
#include "location/Location.hpp"

int main() {
  try {
    Config *config = new Config("config/default.conf");
    std::cout << *config << std::endl;
    Server *server = new Server(*config);
    server->start();
    delete server;
	std::vector<Location *> tmp = config->getLocations();
	std::vector<Location *>::iterator it;
	for(it = tmp.begin(); it != tmp.end(); it++)
		delete (*it);
    delete config;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  return (0);
}