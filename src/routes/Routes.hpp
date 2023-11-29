#ifndef ROUTES_HPP
#define ROUTES_HPP
#include "../../include/webserv.hpp"
#include "../cgi/Cgi.hpp"
#include "../config/Config.hpp"
#include "../location/Location.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"
#include "../utils/Utils.hpp"

class Routes {
private:
  std::vector<Location *> _locations;
  std::string _rootPath;
  std::map<std::string, std::string> _macrosType;
  std::map<int, std::string> _errorPages;
  std::vector<int> _ports;
  std::string _host;
  int _maxBodySize;
  Response *_handleGet(std::vector<std::string> const &filesPath, std::string const &redirectPath);
  Response *_handlePost(std::string const &body, std::string const &type, std::string const &location);
  Response *_handleDelete(std::string const &query);
  Response *_handleCgi(Request const &request, std::string const &method);
  Response *_handleError(int code);
  Response *_handleAutoindex(std::string const &directoryPath);

public:
  Routes(Config const &config);
  Routes();
  ~Routes();
  Response *handle(Request const &request);
  bool isHTTPMethod(std::string const &httpRequest);
  std::vector<int> const &getPorts() const;
  std::string const &getHost() const;
  int getMaxBodySize() const;
};

#endif // ROUTES_HPP