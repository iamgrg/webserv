#include "Request.hpp"
#include <iostream>
#include <sstream>

Request::Request(const std::string &rawRequest, int maxBodySize)
    : _validRequest(false) {
  parse(rawRequest, maxBodySize);
  if (this->_httpVersion != "HTTP/1.1")
    this->_validRequest = false;
}
Request::~Request() {}

void Request::parse(const std::string &rawRequest, int maxBodySize) {
  std::istringstream stream(rawRequest);
  std::string line;
  std::vector<std::string> headerSection;
  std::string bodySection;
  // Obtenir la première ligne (Request Line)
  if (std::getline(stream, line) && !line.empty()) {
	std::string cleanedLine;
	for (size_t i = 0; i < line.length(); ++i) {
		if (line[i] != '\r') {
			cleanedLine += line[i];
		}
	}
	line = cleanedLine;
    _parseRequestLine(line);
  }
  // Obtenir les headers
  while (std::getline(stream, line) && line != "\r") {
	std::string cleanedLine;
	for (size_t i = 0; i < line.length(); ++i) {
		if (line[i] != '\r') {
			cleanedLine += line[i];
		}
	}
	line = cleanedLine;
    //line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    headerSection.push_back(line + "\n");
  }
  _parseHeaders(headerSection);
  // Obtenir le body
  while (std::getline(stream, line)) {
    bodySection += line + "\n";
  }
  _body = bodySection.substr(0, bodySection.size() - 1);
  if (_body.size() < (size_t)maxBodySize)
    _validRequest = true;
}

void Request::_parseRequestLine(const std::string &requestLine) {
  std::istringstream iss(requestLine);
  iss >> _method >> _url >> _httpVersion;
  // Extraire la query string de l'URL
  // vérifié que le requete n'est pas crypté
  // if (_url.find("%") != std::string::npos)
  //     _validRequest = false;
  std::size_t found = _url.find('?');
  if (found != std::string::npos) {
    _query = _url.substr(found + 1);
    _url = _url.substr(0, found);
  } else
    _query = "";}

void Request::_parseHeaders(const std::vector<std::string> &headerLines) {
  for (std::vector<std::string>::const_iterator it = headerLines.begin();
       it != headerLines.end(); ++it) {
    std::istringstream iss(*it);
    std::string key;
    std::string value;
    if (std::getline(iss, key, ':')) {
      std::getline(iss, value);
      value.erase(0, value.find_first_not_of(" \t"));
      _headers[key] = value;
    }
  }
  if (std::string::npos !=
      _headers["content-type"].find("multipart/form-data")) {
    std::string boundary = _headers["content-type"].substr(
        _headers["content-type"].find("boundary=") + 9);
    _headers["boundary"] = boundary;
  }
}

std::string Request::getMethod() const { return _method; }
std::string Request::getUrl() const { return _url; }
std::string Request::getQuery() const {
  std::string cleanQuery(_query);
  std::string::size_type pos = 0;
  while ((pos = cleanQuery.find("%20", pos)) != std::string::npos) {
    cleanQuery.replace(pos, 3, " ");
    pos += 1;
  }
  pos = 0;
  while ((pos = cleanQuery.find("%27", pos)) != std::string::npos) {
    cleanQuery.replace(pos, 3, "'");
    pos +=
        1; // Déplacer après le nouveau caractère pour éviter une boucle infinie
  }
  return cleanQuery;
}

std::string Request::getHttpVersion() const { return _httpVersion; }
std::string Request::getBody() const { return _body; }
bool Request::isValid() const { return _validRequest; }
std::string Request::getContentType() const {
  std::map<std::string, std::string>::const_iterator it =
      _headers.find("Content-Type");
  if (it != _headers.end()) {
    return it->second;
  }
  return "";
}
std::string Request::getHeader(const std::string &headerName) const {
  std::map<std::string, std::string>::const_iterator it =
      _headers.find(headerName);
  if (it != _headers.end()) {
    return it->second;
  }
  return "";
}

std::ostream &operator<<(std::ostream &os, const Request &req) {
  const std::string border =
      "------------------------------------------------\n";
  os << border << MAGENTA << "Request Line:\n"
     << RESET << "  " << YELLOW << "Method: " << RESET << req.getMethod()
     << "\n"
     << "  " << YELLOW << "URL: " << RESET << req.getUrl() << "\n"
     << "  " << YELLOW << "Query: " << RESET << req.getQuery() << "\n"
     << "  " << YELLOW << "HTTP Version: " << RESET << req.getHttpVersion()
     << "\n"
     << MAGENTA << "Headers:\n"
     << RESET;

  for (std::map<std::string, std::string>::const_iterator it =
           req._headers.begin();
       it != req._headers.end(); ++it) {
    os << "  " << GREEN << it->first << RESET << ": " << it->second << "\n";
  }
  os << MAGENTA << "Body:\n"
     << RESET << BLUE << req.getBody() << RESET << border;
  return os;
}