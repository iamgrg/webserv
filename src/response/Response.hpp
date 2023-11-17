#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../../include/webserv.hpp"
#include <string>
#include <map>

class Response {
private:
    std::string _statusLine;
    std::map<std::string, std::string> _headers;
    std::string _body;

public:
    Response();
    ~Response();

    void setStatus(int code, const std::string& message);
    void addHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);
    std::string const & getBody() const;
	std::string const fileToString(const std::string& filePath);
    std::string buildResponse() const;
};

#endif // RESPONSE_HPP
