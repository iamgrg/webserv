
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gansard <gansard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 16:32:27 by gregoire          #+#    #+#             */
/*   Updated: 2023/10/24 14:28:29 by gansard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP
#include <iostream>
#include <map>
#include <string>
#include <cctype>  // pour isdigit
#include <sstream> // pour std::stringstream
#include <stdexcept>
#include <fstream>


class BitcoinExchange
{
public:
    BitcoinExchange();
    ~BitcoinExchange();
	double convertToCurrency(double btcAmount, const std::string& date);
	void printFormatedData(const std::pair<std::string, double>& data);
	static bool isValidDate(const std::string& date);
	static std::pair<std::string, double> getClosestPreviousDate(const std::string& date, const std::map<std::string, double>& data);
	std::map<std::string, double> readCSV(const std::string& filepath);
    std::pair<std::string, double> readInput(const std::string &line);

private:
	BitcoinExchange(const BitcoinExchange& src);
	BitcoinExchange& operator=(const BitcoinExchange& rhs);
	double _getExchangeRate(const std::string& date);
	std::map<std::string, double> _btcData;
	static bool _isLeapYear(int year);
};

#endif // BITCOINEXCHANGE_HPP


