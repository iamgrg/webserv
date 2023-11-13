# ifndef LOCATION_HPP
# define LOCATION_HPP
# include "../../include/webserv.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"

class Location
{
private:
	std::string _path;
	std::vector<std::string> _methods;
	std::vector<std::string> _filesPath;
	bool _autoindex;

public:
	Location(std::string _path, std::vector<std::string> _methods, std::vector<std::string> _filesPath, bool _autoindex);
	~Location();
	Location const &operator=(Location const &rhs);
	std::string const &getPath() const;
	std::vector<std::string> const &getMethods() const;
	std::vector<std::string> const &getFilesPath() const;
	void addMethod(std::string const &method);
	bool const &getAutoindex() const;
};


# endif // LOCATION_HPP