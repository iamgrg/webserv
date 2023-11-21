/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gansard <gansard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:18:27 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/21 11:56:30 by gansard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server/Server.hpp"
#include "location/Location.hpp"
#include <csignal>
#include <iostream>

int main() {
  try {
    Config *config = new Config("config/default.conf");
    Server *server = new Server(config);
	signal(SIGINT, Server::stop);
    server->start();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  return (0);
}