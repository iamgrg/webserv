# ifndef CGI_HPP
# define CGI_HPP
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <cstring>
# include "../request/Request.hpp"
# include "../utils/Utils.hpp"

# include <string>

class Cgi
{
	private:
		std::string _city;
		std::string _HTMLstr;
		void _extractData(Request const &request);
	public:
		Cgi();
		Cgi(Request const &request);
		~Cgi();
		void setCity(std::string city);
		int exec();
		std::string getHTMLstr() const;
};

# endif

