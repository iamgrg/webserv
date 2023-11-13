#include "Routes.hpp"
#include <sys/stat.h>

Routes::Routes(Config const & config) : _locations(config.getLocations()), _rootPath(config.getRootPath()){
	_macrosType["FORM"] = "application/x-www-form-urlencoded";
	_macrosType["UPLOAD"] = "multipart/form-data";
}
Routes::~Routes(){ }

Response const &Routes::handle(Request const &request)
{
	Response *res;
	std::vector<Location *>::iterator it = this->_locations.begin();
	while(it != this->_locations.end())
	{
		if (request.getUrl() == (*it)->getPath())
		{
			if (std::find((*it)->getMethods().begin(), (*it)->getMethods().end(), request.getMethod()) != (*it)->getMethods().end())
			{
				if (request.getMethod() == "GET")
                {
					if(request.getUrl() == "/cgi.py")
						res = this->_handleCgi(request, "GET");
                    else if(request.getQuery() != "")
                        res = Utils::downloadFile(request.getQuery(), _rootPath);
                    else
                        res = this->_handleGet((*it)->getFilesPath());
                }
				else if (request.getMethod() == "POST")
				{
					if(request.getUrl() == "/cgi.py")
						res = this->_handleCgi(request, "POST");
					else
						res = this->_handlePost(request.getBody(), request.getContentType());
				}
				else if (request.getMethod() == "DELETE")
					res = this->_handleDelete((*it)->getFilesPath());
				else
					res = this->_handleForbidden();
				return *res;
			}
		}
		it++;
	}
	res = this->_handleForbidden2();
	return *res;
}


Response *Routes::_handleGet(std::vector<std::string> const &filesPath) {
    Response *res = new Response();
    for (std::vector<std::string>::const_iterator it = filesPath.begin(); it != filesPath.end(); ++it) {
		std::string path = _rootPath + "/" + (*it);
		std::cout << "PATH : " << path << std::endl;
        if (Utils::fileExists(path)) {
            res->setStatus(200, "OK");
            res->addHeader("Content-Type", "text/html");
            res->setBody(res->fileToString(path));
            return res;
        }
    }
    res->setStatus(404, "Not Found");
    res->addHeader("Content-Type", "text/html");
    res->setBody(res->fileToString("www/404.html"));
    return res;
}

Response *Routes::_handlePost(std::string const &body, std::string const &type) {
    Response *res = new Response();
    if (std::string::npos != type.find(_macrosType["FORM"])) {
        std::string name = body.substr(body.find("=") + 1);
        std::string responseBody = "Merci d'avoir rempli le formulaire " + name + " !";
        res->setStatus(200, "OK");
        res->addHeader("Content-Type", "text/html");
        res->setBody(responseBody);
    }
    else if (std::string::npos != type.find(_macrosType["UPLOAD"]))
        Utils::uploadFile(body, type, res, _rootPath);
    return res;
}

Response *Routes::_handleCgi(Request const &request, std::string const &method){
	Response *res = new Response();
	int execResult;
	if (method == "GET")
	{
		Cgi cgi;
		execResult = cgi.exec();
		std::cout << execResult << std::endl;
		std::cout << cgi.getHTMLstr() << std::endl;
		res->setBody(cgi.getHTMLstr());
	}
	else
	{
		Cgi cgi(request);
		execResult = cgi.exec();
		res->setBody(cgi.getHTMLstr());
	}

	if (execResult == 0) { // Si exec() réussit
        res->setStatus(200, "OK");
    } else { // Si exec() échoue
        res->setBody("Erreur interne du serveur"); // Remplacer par une page d'erreur complète si disponible
        res->setStatus(500, "Internal Server Error");
		res->setBody(res->fileToString("www/500.html"));
    }
    res->addHeader("Content-Type", "text/html");
    return res;
}


Response *Routes::_handleDelete(std::vector<std::string> const &filesPath)
{
	(void)filesPath;
	Response *res = new Response();
	// std::vector<std::string>::iterator it;
	// while(it != filesPath.end())
	// {
	// 	if (request.getUrl() == (*it))
	// 	{
	// 		if (remove((*it).c_str()) != 0)
	// 		{
	// 			res->setStatus(404);
	// 			res->setBody("404 Not Found");
	// 		}
	// 		else
	// 		{
	// 			res->setStatus(200);
	// 			res->setBody("200 OK");
	// 		}
	// 		return res;
	// 	}
	// 	it++;
	// }
	res->setStatus(404, "Not Found");
	res->addHeader("Content-Type", "text/html");
    res->setBody(res->fileToString("www/404.html"));
	return res;
}

Response *Routes::_handleForbidden()
{
	Response *res = new Response();
	res->setStatus(300, "Forbidden je crois");
	res->addHeader("Content-Type", "text/html");
	res->setBody("300 on verra jsp encore");
	return res;
}

Response *Routes::_handleForbidden2()
{
	Response *res = new Response();
	res->setStatus(300, "cette vie la ");
	res->addHeader("Content-Type", "text/html");
	res->setBody("300 cette vie là");
	return res;
}