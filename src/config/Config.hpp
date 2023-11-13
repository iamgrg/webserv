# ifndef CONFIG_HPP
# define CONFIG_HPP
# include "../../include/webserv.hpp"
# include "../location/Location.hpp"

class Config
{
private:
	std::string _host;
	std; _port;
	std::string _rootPath;
	std::vector<Location *> _locations;

public:
	Config(std::string const & configPath);
	~Config();
	std::string const & getHost() const;
	int const & getPort() const;
	std::vector<Location *> const & getLocations() const;
	std::string const & getRootPath() const;
};


# endif // CONFIG_HPP