#include "Config.hpp"

Config::Config(std::stringstream &serverBlockStream) {
  _parseConfig(serverBlockStream);
  _createErrorPages();
  _names.push_back("127.0.0.1");
}
Config::~Config() {}
Config::Config() { }
std::string const &Config::getHost() const { return this->_host; }
std::vector<Location *> const &Config::getLocations() const {
  return this->_locations;
}
std::string const &Config::getRootPath() const { return this->_rootPath; }
std::vector<int> const &Config::getPorts() const { return this->_ports; }
std::vector<std::string> Config::getPortsStr() const {
  std::vector<std::string> portsStr;
    for (std::vector<int>::const_iterator it = this->_ports.begin(); it != this->_ports.end(); ++it) {
        std::ostringstream ss;
        ss << *it;
        portsStr.push_back(ss.str());
    }
    return portsStr;
}
std::vector<std::string> const &Config::getNames() const {
  return this->_names;
}
std::map<int, std::string> const &Config::getErrorPages() const {
  return this->_errorPages;
}
int Config::getMaxBodySize() const { return this->_maxBodySize; }
std::string Config::getPathCGI() const { return this->_pathCGI; }

void Config::_parseConfig(std::stringstream &serverBlockStream) {
  std::string line;
  int lineNumber = 0;
  bool insideServerBlock = false;
  bool insideLocationBlock = false;
  std::stringstream locationBlockStream;

  while (std::getline(serverBlockStream, line)) {
    lineNumber++;

    // ================= check line ================= //
    // Supprimer les espaces en début de ligne
    size_t firstNonSpace = line.find_first_not_of(" \t");
    if (firstNonSpace != std::string::npos)
      line = line.substr(firstNonSpace);
	bool allSpaces = true;
	for (size_t i = 0; i < line.length(); ++i) {
		if (!isspace(line[i])) {
			allSpaces = false;
			break;
		}
	}
	if (line.empty() || allSpaces)
      continue;
    // Supprimer les espaces en fin de ligne
    size_t lastNonSpace = line.find_last_not_of(" \t;");
    line = line.substr(0, lastNonSpace + 2);
    // Vérifier si la ligne se termine par un point-virgule ou un crochet
    if (!line.empty() &&
        (line[line.size() - 1] != ';' && line[line.size() - 1] != '{' && line[line.size() - 1] != '}'))
	{
		std::ostringstream ss;
		ss << lineNumber;
		std::string lineNumberStr = ss.str();
      	throw std::runtime_error("Erreur de syntaxe à la ligne " +
                lineNumberStr + ": fin de ligne attendue avec ';'");
	}

    // ============================================== //

    // Détecter le début et la fin du bloc server
    if (line == "server {") {
      insideServerBlock = true;
      continue;
    } else if (line == "}" && insideServerBlock && !insideLocationBlock) {
      insideServerBlock = false;
      continue;
    }
    // Traiter les lignes à l'intérieur du bloc server
    if (insideServerBlock) {
      if (line[lastNonSpace] == '{') { // Début d'un bloc location
        insideLocationBlock = true;
        locationBlockStream.str(
            std::string()); // Réinitialiser pour un nouveau bloc location
        locationBlockStream.clear();
        locationBlockStream << line << "\n";
        continue;
      } else if (line == "}" && insideLocationBlock) {
        insideLocationBlock = false;
        // Traitement du bloc location complet
        _parseLocationBlock(locationBlockStream);
        continue;
      }
      if (insideLocationBlock) {
        locationBlockStream
            << line << "\n"; // Ajouter la ligne au flux du bloc location actuel
      } else {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        if (key == "listen") {
          std::string portstr;
          while (iss >> portstr) {
            if (portstr.find(";") != std::string::npos)
              portstr = portstr.substr(0, portstr.size() - 1);
            int port;
			      std::istringstream(portstr) >> port;
            if(std::find(_ports.begin(), _ports.end(), port) == _ports.end())
              _ports.push_back(port);
            else {
			  std::stringstream message;
			  message << "Erreur de syntaxe à la ligne " << lineNumber << ": port déjà utilisé";
              throw std::runtime_error(message.str());
			}
          }
        } else if (key == "server_name") {
          std::string serverName;
          while (iss >> serverName) {
            if (serverName.find(";") != std::string::npos)
              serverName = serverName.substr(0, serverName.size() - 1);
            _names.push_back(serverName);
          }
        } else if (key == "error_page") {
          int errorCode;
          std::string errorPage;
          iss >> errorCode >> errorPage;
          errorPage = errorPage.substr(0, errorPage.size() - 1);
          _errorPages[errorCode] = errorPage;
        } else if (key == "client_max_body_size") {
          std::string sizeStr;
          iss >> sizeStr;
		  std::istringstream iss(sizeStr);
		  iss >> _maxBodySize;
        } else if (key == "root") {
          std::string rootPath;
          iss >> rootPath;
          rootPath = rootPath.substr(0, rootPath.size() - 1);
          _rootPath = rootPath;
          } else if (key == "host") {
          std::string host;
          iss >> host;
          host = host.substr(0, host.size() - 1);
          _host = host;
          } else if (key == "cgi") {
          std::vector<std::string> methods;
          std::vector<std::string> filesPath;
          methods.push_back("GET");
          methods.push_back("POST");
          std::string scriptPath;
          std::string ext;
          iss >> ext >> scriptPath;
          std::string path = "/cgi" + ext;
          _pathCGI = scriptPath;
          filesPath.push_back(scriptPath);
          Location *location =
              new Location(path, methods, filesPath, false, ""); 
          _locations.push_back(location);
          } else {
          std::ostringstream ss;
          ss << lineNumber;
          std::string lineNumberStr = ss.str();
          throw std::runtime_error("Erreur de syntaxe à la ligne " + lineNumberStr + ": directive inconnue '" + key + "'");
		    }
      }
    }
  }
}

void Config::_parseLocationBlock(std::stringstream &locationBlockStream) {
  std::string line;
  std::string path;
  std::vector<std::string> methods;
  std::string rootPath;
  std::vector<std::string> indexFiles;
  bool autoindex = false;
  std::string redirectPath;
  int redirectStatus = -1;

  while (std::getline(locationBlockStream, line)) {
    std::istringstream iss(line);
    std::string key;
    iss >> key;
    if (key == "location") {
      iss >> path;
    } else if (key == "root") {
      iss >> rootPath;
      rootPath = rootPath.substr(0, rootPath.size() - 1);
    } else if (key == "index") {
      std::string indexFile;
      iss >> indexFile;
      indexFile = indexFile.substr(0, indexFile.size() - 1);
      indexFiles.push_back(indexFile);
    } else if (key == "allow_methods") {
      std::string method;
      while (iss >> method) {
        if (method.find(";") != std::string::npos)
          method = method.substr(0, method.size() - 1);
        methods.push_back(method);
      }
    } else if (key == "autoindex") {
      std::string flag;
      iss >> flag;
      flag = flag.substr(0, flag.size() - 1);
      autoindex = (flag == "on");
    } else if (key == "redirect") {
      iss >> redirectStatus >> redirectPath;
      redirectPath = redirectPath.substr(0, redirectPath.size() - 1);
    } else
      throw std::runtime_error("Unknown directive '" + key +
                               "' in location block");
  }
  // Création d'un objet Location avec les paramètres récupérés
  Location *location =
      new Location(path, methods, indexFiles, autoindex, redirectPath);
  _locations.push_back(location);
}

void touch(int code, std::map<int, std::string> &_errorPages) {
  if (_errorPages.find(code) == _errorPages.end()) {
	std::ostringstream ss;
	ss << code;
	std::string codeStr = ss.str();
    _errorPages[code] = "/error/" + codeStr + ".html";
  }
}

void Config::_createErrorPages(void) {
  touch(301, _errorPages);
  touch(400, _errorPages);
  touch(403, _errorPages);
  touch(404, _errorPages);
  touch(405, _errorPages);
  touch(413, _errorPages);
  touch(500, _errorPages);
}

std::ostream &operator<<(std::ostream &os, const Config &config) {
  os << GREEN << "//**************//" << RESET << std::endl;
  os << YELLOW << "Host: " << RESET << config.getHost() << std::endl;
  os << BLUE << "Ports: " << RESET;
  for (std::vector<int>::const_iterator it = config.getPorts().begin();
       it != config.getPorts().end(); ++it) {
    os << *it << " ";
  }
  os << std::endl;
  os << GREEN << "Server Names: " << RESET;
  for (std::vector<std::string>::const_iterator it = config.getNames().begin();
       it != config.getNames().end(); ++it) {
    os << *it << " ";
  }
  os << std::endl;
  os << RED << "Error Pages: " << RESET;
  for (std::map<int, std::string>::const_iterator it =
           config.getErrorPages().begin();
       it != config.getErrorPages().end(); ++it) {
    os << it->first << " -> " << it->second << "; ";
  }
  os << std::endl;
  os << MAGENTA << "Max Body Size: " << RESET << config.getMaxBodySize()
     << std::endl;
  os << CYAN << "CGI Path: " << RESET << config.getPathCGI() << std::endl;
  os << BLUE << "Root Path: " << RESET << config.getRootPath() << std::endl;
  os << GREENLIGHT << "Locations: " << RESET << std::endl;
  for (std::vector<Location *>::const_iterator it =
           config.getLocations().begin();
       it != config.getLocations().end(); ++it) {
    os << **it << std::endl;
  }
  os << GREEN << "//**************//" << RESET << std::endl;
  return os;
}
