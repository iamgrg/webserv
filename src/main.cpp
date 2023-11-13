/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gregoire <gregoire@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 12:18:27 by gregoire          #+#    #+#             */
/*   Updated: 2023/11/11 12:25:49 by gregoire         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server/Server.hpp"

int main()
{
	Config *config = new Config("config/webserv.conf");
	Server *server = new Server(*config);
	server->start();
	delete server;
	return (0);
}