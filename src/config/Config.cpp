#include "Config.hpp"

Config::Config(std::string const & configPath) : _host("127.0.0.1"), _port(8080), _rootPath("/Users/gregoire/Documents/current/webserve/webserv/www") {
    (void) configPath;
    
    // Méthodes communes pour toutes les pages
    std::vector<std::string> commonMethods;
	std::vector<std::string> formMethods;
	std::vector<std::string> uploadMethods;
    commonMethods.push_back("GET");

    // Page d'accueil
    std::vector<std::string> indexFiles;
    indexFiles.push_back("index.html");
    Location *index = new Location("/", commonMethods, indexFiles, false);

    // Pages d'erreur
    std::vector<std::string> errorFiles;
    errorFiles.push_back("404.html");
    errorFiles.push_back("500.html");
    Location *error = new Location("/error", commonMethods, errorFiles, false);

    // Page de téléchargement
    std::vector<std::string> downloadFiles;
    downloadFiles.push_back("download.html");
    Location *download = new Location("/download", commonMethods, downloadFiles, false);

	// Page d'upload
	std::vector<std::string> uploadFiles;
	uploadMethods.push_back("GET");
	uploadMethods.push_back("POST");
	uploadFiles.push_back("upload.html");
	Location *upload = new Location("/upload", uploadMethods, uploadFiles, false);

    // Page de formulaire
    std::vector<std::string> formFiles;
	formMethods.push_back("GET");
	formMethods.push_back("POST");
    formFiles.push_back("form.html");
    Location *form = new Location("/form", formMethods, formFiles, false);

    // Page de test CGI
    std::vector<std::string> cgiTestFiles;
    cgiTestFiles.push_back("cgi-test.html");
    Location *cgiTest = new Location("/cgi-test", commonMethods, cgiTestFiles, false);
    std::vector<std::string> cgiScriptsMethods;
    std::vector<std::string> cgiScriptsFiles;
    cgiScriptsMethods.push_back("GET");
    cgiScriptsMethods.push_back("POST");
    Location *cgiScript = new Location("/cgi.py", cgiScriptsMethods, cgiScriptsFiles, false);

    // Ajouter les locations au vecteur de locations
    _locations.push_back(index);
    _locations.push_back(error);
    _locations.push_back(download);
    _locations.push_back(form);
	_locations.push_back(upload);
    _locations.push_back(cgiTest);
    _locations.push_back(cgiScript);
}

Config::~Config() {
	// Destructor implementation
}

std::string const & Config::getHost() const {
	return this->_host;
}

int const & Config::getPort() const {
	return this->_port;
}

std::vector<Location *> const & Config::getLocations() const {
	return this->_locations;
}

std::string const & Config::getRootPath() const {
	return this->_rootPath;
}