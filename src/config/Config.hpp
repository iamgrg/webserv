#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../../include/webserv.hpp"
#include "../location/Location.hpp"

class Config {
private:
  // Attributs privés
  std::string _host;
  std::vector<int> _ports;
  std::vector<std::string> _names;
  std::map<int, std::string> _errorPages;
  int _maxBodySize;
  std::string _pathCGI;
  std::string _rootPath;
  std::vector<Location *> _locations;

  // Méthodes privées
  void _parseConfig(std::stringstream &serverBlockStream);
  void _parseLocationBlock(std::stringstream &locationBlockStream);
  void _createErrorPages();

public:
  // Constructeur & Destructeur
  Config(std::stringstream &serverBlockStream);
  Config();
  ~Config();

  // Getters
  std::string const &getHost() const;
  std::vector<Location *> const &getLocations() const;
  std::string const &getRootPath() const;
  std::vector<int> const &getPorts() const;
  std::vector<std::string> getPortsStr() const;
  std::vector<std::string> const &getNames() const;
  std::map<int, std::string> const &getErrorPages() const;
  int getMaxBodySize() const;
  std::string getPathCGI() const;

  // Opérateur d'amitié
  friend std::ostream &operator<<(std::ostream &os, const Config &config);
};

#endif // CONFIG_HPP
