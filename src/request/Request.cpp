#include "Request.hpp"
#include <sstream>
#include <iostream>

Request::Request(const std::string& rawRequest) : _validRequest(false) {
    parse(rawRequest);
}
Request::~Request(){ }

void Request::parse(const std::string& rawRequest) {
    std::istringstream stream(rawRequest);
    std::string line;
    std::vector<std::string> headerSection;
    std::string bodySection;
    // Obtenir la première ligne (Request Line)
    if (std::getline(stream, line) && !line.empty()) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        _parseRequestLine(line);
    }
    // Obtenir les headers
    while (std::getline(stream, line) && line != "\r") {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        headerSection.push_back(line + "\n");
    }
    _parseHeaders(headerSection);
    // Obtenir le body
    while (std::getline(stream, line)) {
        bodySection += line + "\n";
    }
    _body = bodySection.substr(0, bodySection.size() - 1);
    // Déterminer le type de contenu
	_validRequest = true;
}

void Request::_parseRequestLine(const std::string& requestLine) {
    std::istringstream iss(requestLine);
    iss >> _method >> _url >> _httpVersion;
    // Extraire la query string de l'URL
    std::size_t found = _url.find('?');
    if (found != std::string::npos) {
        _query = _url.substr(found + 1);
        _url = _url.substr(0, found);
    }
    else
        _query = "";
}

void Request::_parseHeaders(const std::vector<std::string>& headerLines) {
    for (std::vector<std::string>::const_iterator it = headerLines.begin(); it != headerLines.end(); ++it) {
        std::istringstream iss(*it);
        std::string key;
        std::string value;
        if (std::getline(iss, key, ':')) {
            std::getline(iss, value);
            // Supprimer les espaces blancs en début de la valeur
            value.erase(0, value.find_first_not_of(" \t"));
            _headers[key] = value;
        }
    }
    if(std::string::npos != _headers["content-type"].find("multipart/form-data"))
    {
        std::string boundary = _headers["content-type"].substr(_headers["content-type"].find("boundary=") + 9);
        _headers["boundary"] = boundary;
    }
}

std::string Request::getMethod() const {return _method;}
std::string Request::getUrl() const {return _url;}
std::string Request::getQuery() const {return _query;}
std::string Request::getHttpVersion() const {return _httpVersion;}
std::string Request::getBody() const {return _body;}
bool Request::isValid() const {return _validRequest;}
std::string Request::getContentType() const {
    std::map<std::string, std::string>::const_iterator it = _headers.find("Content-Type");
    if (it != _headers.end()) {
        return it->second;
    }
    return "";
}

std::ostream& operator<<(std::ostream& os, const Request& req) {
	const std::string border = "------------------------------------------------\n";
    os << border <<MAGENTA << "Request Line:\n" << RESET
	   << "  " << YELLOW << "Method: " << RESET << req.getMethod() << "\n"
       << "  " << YELLOW << "URL: " << RESET << req.getUrl() << "\n"
       << "  " << YELLOW << "Query: " << RESET << req.getQuery() << "\n"
       << "  " << YELLOW << "HTTP Version: " << RESET << req.getHttpVersion() << "\n"
       << MAGENTA << "Headers:\n" << RESET;
	   
    for (std::map<std::string, std::string>::const_iterator it = req._headers.begin(); it != req._headers.end(); ++it) {
        os << "  " << GREEN << it->first << RESET << ": " << it->second << "\n";
    }
    os << MAGENTA << "Body:\n" << RESET << BLUE << req.getBody() << RESET << border;
    return os;
}


