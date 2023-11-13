#include "Location.hpp"

Location::	Location(std::string _path, 
	std::vector<std::string> _methods,
	std::vector<std::string> _filesPath,
	bool _autoindex)
	: _path(_path), _methods(_methods), _filesPath(_filesPath), _autoindex(_autoindex){ }

Location::~Location() { }

std::string const &Location::getPath() const {
	return this->_path;
}

std::vector<std::string> const &Location::getMethods() const {
	return this->_methods;
}

std::vector<std::string> const &Location::getFilesPath() const {
	return this->_filesPath;
}

bool const &Location::getAutoindex() const {
	return this->_autoindex;
}

Location const &Location::operator=(Location const &rhs) {
	if (this != &rhs)
	{
		this->_path = rhs._path;
		this->_methods = rhs._methods;
		this->_filesPath = rhs._filesPath;
		this->_autoindex = rhs._autoindex;
	}
	return *this;
}