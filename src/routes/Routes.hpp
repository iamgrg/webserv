# ifndef ROUTES_HPP
# define ROUTES_HPP
# include "../../include/webserv.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"
# include "../location/Location.hpp"
# include "../config/Config.hpp"
# include <dirent.h>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <fstream>
# include "../utils/Utils.hpp"
# include "../cgi/Cgi.hpp"

class Routes
{
private:
	std::vector<Location *> _locations;
	std::string _rootPath;
	std::map<std::string, std::string> _macrosType;
	Response *_handleGet(std::vector<std::string> const &filesPath);
	Response *_handlePost(std::string const & body, std::string const &type);
	Response *_handleDelete(std::vector<std::string> const &filesPath);
	Response *_handleCgi(Request const &request, std::string const &method);
	Response *_handleForbidden();
	Response *_handleForbidden2();
public:
	Routes(Config const &config);
	~Routes();
	Response const &handle(Request const &request);
};


# endif // ROUTES_HPP