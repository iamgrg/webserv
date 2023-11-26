#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../../include/webserv.hpp"

class Response {
private:
  std::string _statusLine;
  std::map<std::string, std::string> _headers;
  std::string _body;

public:
  Response();
  ~Response();

  void setStatus(int code, const std::string &message);
  void addHeader(const std::string &key, const std::string &value);
  std::vector<std::string> getHeaders() const;
  void setBody(const std::string &body);
  std::string const &getBody() const;
  std::string const fileToString(const std::string &filePath);
  std::string buildResponse() const;
  friend std::ostream &operator<<(std::ostream &os, const Response &response);
};

#endif // RESPONSE_HPP
