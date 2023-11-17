#include "Location.hpp"

Location::Location(std::string _path, std::vector<std::string> _methods,
                   std::vector<std::string> _filesPath, bool _autoindex,
                   std::string _redirectPath)
    : _path(_path), _methods(_methods), _filesPath(_filesPath),
      _autoindex(_autoindex), _redirectPath(_redirectPath) {}

Location::~Location() {}

std::string const &Location::getPath() const { return this->_path; }

std::vector<std::string> const &Location::getMethods() const {
  return this->_methods;
}

std::vector<std::string> const &Location::getFilesPath() const {
  return this->_filesPath;
}

bool const &Location::getAutoindex() const { return this->_autoindex; }

std::string const &Location::getRedirectPath() const {
  return this->_redirectPath;
}

Location const &Location::operator=(Location const &rhs) {
  if (this != &rhs) {
    this->_path = rhs._path;
    this->_methods = rhs._methods;
    this->_filesPath = rhs._filesPath;
    this->_autoindex = rhs._autoindex;
    this->_redirectPath = rhs._redirectPath;
  }
  return *this;
}

// Surcharge de l'opérateur <<
std::ostream &operator<<(std::ostream &os, const Location &location) {
  os << GREENLIGHT << "Location Path: " << RESET << location._path << std::endl;
  os << "   " << GREENLIGHT << " - "
     << "Methods: " << RESET;
  for (std::vector<std::string>::const_iterator it = location._methods.begin();
       it != location._methods.end(); ++it) {
    os << *it << " ";
  }
  os << std::endl;
  os << "   " << GREENLIGHT << " - "
     << "Files Path: " << RESET;
  for (std::vector<std::string>::const_iterator it =
           location._filesPath.begin();
       it != location._filesPath.end(); ++it) {
    os << *it << " ";
  }
  os << std::endl;
  os << "   " << GREENLIGHT << " - "
     << "Autoindex: " << RESET << (location._autoindex ? "On" : "Off")
     << std::endl;
  os << "   " << GREENLIGHT << " - "
     << "Redirect Path: " << RESET << location._redirectPath << std::endl;
  return os;
}
