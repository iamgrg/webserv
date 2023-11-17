#ifndef LOCATION_HPP
#define LOCATION_HPP
#include "../../include/webserv.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"

class Location {
private:
  std::string _path;
  std::vector<std::string> _methods;
  std::vector<std::string> _filesPath;
  bool _autoindex;
  std::string _redirectPath;

public:
  Location(std::string _path, std::vector<std::string> _methods,
           std::vector<std::string> _filesPath, bool _autoindex,
           std::string _redirectPath);
  ~Location();
  Location const &operator=(Location const &rhs);
  std::string const &getPath() const;
  std::vector<std::string> const &getMethods() const;
  std::vector<std::string> const &getFilesPath() const;
  bool const &getAutoindex() const;
  std::string const &getRedirectPath() const;
  friend std::ostream &operator<<(std::ostream &os, const Location &location);
};

#endif // LOCATION_HPP