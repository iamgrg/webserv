#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>

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
    Request(const std::string& rawRequest);
	~Request();
    void parse(const std::string& rawRequest);

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

const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string RESET = "\033[0m";



#endif // REQUEST_HPP
