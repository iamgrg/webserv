#include "Response.hpp"
#include <fstream>
#include <sstream>

Response::Response() {
  // Définir une réponse par défaut
  setStatus(200, "OK");
  addHeader("Content-Type", "text/html");
  setBody("<html><body><h1>Hello, World!</h1></body></html>");
}
Response::~Response() {}

// Définir le statut de la réponse
void Response::setStatus(int code, const std::string &message) {
  std::ostringstream oss;
  oss << "HTTP/1.1 " << code << " " << message;
  _statusLine = oss.str();
}

// Ajouter un en-tête
void Response::addHeader(const std::string &key, const std::string &value) {
  _headers[key] = value;
}

// Définir le corps de la réponse
void Response::setBody(const std::string &body) { _body = body; }
std::string const &Response::getBody() const { return _body; }
// Construire la réponse HTTP complète
std::string Response::buildResponse() const {
  std::ostringstream response;
  response << _statusLine << "\r\n";
  for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
       it != _headers.end(); ++it) {
    response << it->first << ": " << it->second << "\r\n";
  }
  // Ajouter une double ligne vide pour séparer les en-têtes du corps
  response << "\r\n\r\n";
  response << _body;
  return response.str();
}

// Fonction helper pour lire le contenu d'un fichier
std::string const Response::fileToString(const std::string &filePath) {
  std::ifstream fileStream(filePath.c_str());
  std::stringstream buffer;
  buffer << fileStream.rdbuf();
  return buffer.str();
}

  std::vector<std::string> Response::getHeaders() const{
    std::vector<std::string> headers;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
      headers.push_back(it->first + ": " + it->second);
    }
    return headers;
  }

std::ostream &operator<<(std::ostream &os, const Response &response) {
    os << GREEN << "Status Line: " << RESET << response._statusLine << "\n" 
       << YELLOW << "Headers:" << RESET << "\n";
    std::vector<std::string> headers = response.getHeaders();
    for (std::vector<std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        os << CYAN << *it << "  "  << "\n";
    }
    os << BLUE << "Body:\n" << RESET << response._body;
    return os;
}
