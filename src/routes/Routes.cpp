#include "Routes.hpp"
#include <sys/stat.h>

Routes::Routes(Config const &config)
    : _locations(config.getLocations()), _rootPath(config.getRootPath()),
      _errorPages(config.getErrorPages()) {
  _macrosType["FORM"] = "application/x-www-form-urlencoded";
  _macrosType["UPLOAD"] = "multipart/form-data";
}
Routes::~Routes() {}

bool Routes::isHTTPMethod(std::string const &httpRequest) {
  std::istringstream requestStream(httpRequest);
  std::string startLine;
  std::getline(requestStream, startLine);

  // Vérifier si la requête semble être cryptée (par exemple, contient des
  // caractères non imprimables)
  for (std::string::const_iterator it = startLine.begin();
       it != startLine.end(); ++it) {
    if (!isprint(*it)) {
      return false;
    }
  }
  std::istringstream startLineStream(startLine);
  std::vector<std::string> tokens;
  std::string token;
  while (startLineStream >> token) {
    tokens.push_back(token);
  }
  if (tokens.size() != 3) {
    return false;
  }
  const std::string &httpVersion = tokens[2];
  return (httpVersion == "HTTP/1.1");
}

Response const &Routes::handle(Request const &request) {
  Response *res;
  if (!request.isValid()) {
    res = _handleError(413);
    return *res;
  }
  std::vector<Location *>::iterator it = this->_locations.begin();
  while (it != this->_locations.end()) {
    if (request.getUrl() == (*it)->getPath()) {
      if (std::find((*it)->getMethods().begin(), (*it)->getMethods().end(),
                    request.getMethod()) != (*it)->getMethods().end()) {
        if ((*it)->getAutoindex() &&
            !(Utils::fileExists(_rootPath + (*it)->getPath() + "index.html"))) {
          if (request.getMethod() != "GET")
            return *_handleError(405);
          res = this->_handleAutoindex(_rootPath + (*it)->getPath());
          return *res;
        }
        if (request.getMethod() == "GET") {
          if (request.getUrl() == "/cgi.py")
            res = this->_handleCgi(request, "GET");
          else if (request.getQuery() != "")
            res = Utils::downloadFile(request.getQuery(), _rootPath);
          else
            res = this->_handleGet((*it)->getFilesPath(), (*it)->getRedirectPath());
          return *res;
        } else if (request.getMethod() == "POST") {
          if (request.getUrl() == "/cgi.py")
            res = this->_handleCgi(request, "POST");
          else
            res = this->_handlePost(request.getBody(), request.getContentType());
          return *res;
        } else if (request.getMethod() == "DELETE") {
          if (request.getQuery() != "")
            res = this->_handleDelete(request.getQuery());
          else
            res = _handleError(400);
        } else
          res = _handleError(405);
        return *res;
      } else {
        res = _handleError(405);
        return *res;
      }
    }
    it++;
  }
  res = _handleError(404);
  return *res;
}

Response *Routes::_handleGet(std::vector<std::string> const &filesPath,
                             std::string const &redirectPath) {
  Response *res = new Response();
  if (redirectPath != "") {
    Response *res = _handleError(301);
    res->addHeader("Location", redirectPath);
    return res;
  }
  for (std::vector<std::string>::const_iterator it = filesPath.begin();
       it != filesPath.end(); ++it) {
    std::string path = _rootPath + "/" + (*it);
    if (Utils::fileExists(path)) {
      res->setStatus(200, "OK");
      res->addHeader("Content-Type", "text/html");
      res->setBody(res->fileToString(path));
	std::ostringstream ss;
	ss << res->getBody().size();
	res->addHeader("Content-Length", ss.str());
      return res;
    }
  }
  res = _handleError(404);
  return res;
}

Response *Routes::_handlePost(std::string const &body,
                              std::string const &type) {
  Response *res = new Response();
  if (std::string::npos != type.find(_macrosType["FORM"])) {
    std::string name = body.substr(body.find("=") + 1);
    std::string responseBody =
        "Merci d'avoir rempli le formulaire " + name + " !";
    res->setStatus(200, "OK");
    res->addHeader("Content-Type", "text/html");
    res->setBody(responseBody);
	std::ostringstream ss;
	ss << res->getBody().size();
	res->addHeader("Content-Length", ss.str());
    return res;
  } else if (std::string::npos != type.find(_macrosType["UPLOAD"]))
    	Utils::uploadFile(body, type, res, _rootPath);
  else
    res = _handleError(404);
  return res;
}

Response *Routes::_handleCgi(Request const &request, std::string const &method) {
  Response *res = new Response();
  int execResult;
  if (method == "GET") {
    Cgi cgi;
    execResult = cgi.exec();
    res->setBody(cgi.getHTMLstr());
  } else {
    Cgi cgi(request);
    execResult = cgi.exec();
    res->setBody(cgi.getHTMLstr());
  }
  if (execResult != 0) {
    delete res;
    res = _handleError(500);
    return res;
  }
  res->setStatus(200, "OK");
  res->addHeader("Content-Type", "text/html");
	std::ostringstream ss;
	ss << res->getBody().size();
	res->addHeader("Content-Length", ss.str());
  return res;
}

Response *Routes::_handleDelete(std::string const &query) {
  std::cout << query << std::endl;
  Response *res = new Response();
  if (Utils::fileExists("www/uploads/" + query)) {
    std::cout << "FILE EXISTS" << std::endl;
    if (remove(("www/uploads/" + query).c_str()) != 0)
      res = _handleError(404);
    else {
      Utils::stringToHTML(Utils::generateFileLinks("www/uploads/"), _rootPath,
                          "download.html");
      res->setStatus(200, "OK");
      res->addHeader("Content-Type", "text/html");
      res->setBody(res->fileToString("www/dowload.html"));
	std::ostringstream ss;
	ss << res->getBody().size();
	res->addHeader("Content-Length", ss.str());
    }
    return res;
  }
  res = _handleError(403);
  return res;
}

Response *Routes::_handleAutoindex(std::string const &directoryPath) {
  Response *res = new Response();
  DIR *dir;
  struct dirent *entry;
  std::stringstream html;
  html << "<html><head><title>Index of " << directoryPath
       << "</title></head><body>";
  html << "<h1>Index of " << directoryPath << "</h1><ul>";
  dir = opendir(directoryPath.c_str());
  if (dir == NULL) {
    res = _handleError(500);
    return res;
  }
  while ((entry = readdir(dir)) != NULL) {
    std::string fileName = entry->d_name;
    if (fileName == "." || fileName == "..") {
      continue;
    }
    html << "<li><a href=\"" << fileName << "\">" << fileName << "</a></li>";
  }
  closedir(dir);
  html << "</ul></body></html>";
  res->setStatus(200, "OK");
  res->addHeader("Content-Type", "text/html");
  res->setBody(html.str());
	std::ostringstream ss;
	ss << res->getBody().size();
	res->addHeader("Content-Length", ss.str());
  return res;
}

Response *Routes::_handleError(int code) {
  Response *res = new Response();
  std::map<int, std::string> msg;
  msg[301] = "Moved Permanently";
  msg[400] = "Bad Request";
  msg[403] = "Forbidden";
  msg[404] = "Not Found";
  msg[405] = "Method Not Allowed";
  msg[413] = "Payload Too Large";
  msg[500] = "Internal Server Error";
  res->addHeader("Content-Type", "text/html");
  res->setBody(res->fileToString(_rootPath + _errorPages[code]));
	std::ostringstream ss;
	ss << res->getBody().size();
	res->addHeader("Content-Length", ss.str());
  return res;
}
