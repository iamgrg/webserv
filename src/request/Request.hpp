#ifndef REQUEST_HPP
#define REQUEST_HPP
#include "../include/webserv.hpp"

class Request {
friend std::ostream& operator<<(std::ostream& os, const Request& req);
private:
	Request(); // Constructeur privé pour empêcher l'instanciation de la classe sans paramètres.
    std::string _method;
    std::string _url;
    std::string _query;
    std::string _httpVersion;
    std::map<std::string, std::string> _headers;
    std::string _body;
    bool _validRequest;

public:
    Request(const std::string& rawRequest, int maxBodySize);
	~Request();
    void parse(const std::string& rawRequest, int maxBodySize);

    // Getters
    std::string getMethod() const;
    std::string getUrl() const;
    std::string getQuery() const;
    std::string getHttpVersion() const;
    std::string getHeader(const std::string& headerName) const;
    std::string getBody() const;
    std::string getContentType() const;
    bool isValid() const;

    // Helper functions
private:
    void _parseRequestLine(const std::string& requestLine);
    void _parseHeaders(const std::vector<std::string>& headerLines);
};


#endif // REQUEST_HPP
