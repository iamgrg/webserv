#ifndef UTILS_HPP
#define UTILS_HPP

#include "../../include/webserv.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"

class Utils {
private:
  Utils();
  ~Utils();

public:
  static std::string const generateFileLinks(const std::string &directory);
  static void stringToHTML(const std::string &content, const std::string &path,
                           const std::string &name);
  static Response *downloadFile(std::string const &query,
                                std::string const rootPath);
  static bool fileExists(const std::string &path);
  static void uploadFile(std::string const &body, std::string const &type,
                         Response *res, std::string const &_rootPath);
  static std::string toLower(std::string str);
  static bool isValidUtf8(const std::string& str);
};

#endif // UTILS_HPP