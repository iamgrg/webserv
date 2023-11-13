#include "Utils.hpp"

Utils::Utils() { }
Utils::~Utils() { }

bool Utils::fileExists(const std::string &path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string const Utils::generateFileLinks(const std::string& directory) {
    DIR *dir;
    struct dirent *ent;
    std::stringstream html;

    html << "<!DOCTYPE html>\n"
         << "<html lang='fr'>\n"
         << "<head>\n"
         << "    <meta charset='UTF-8'>\n"
         << "    <title>Liste des fichiers</title>\n"
         << "    <style>\n"
         << "        body { font-family: Arial, sans-serif; margin: 40px; }\n"
         << "        a { color: #0275d8; text-decoration: none; }\n"
         << "        a:hover { text-decoration: underline; }\n"
         << "        .file-list { margin-top: 20px; }\n"
         << "    </style>\n"
         << "</head>\n"
         << "<body>\n"
         << "<h1>Fichiers disponibles au téléchargement</h1>\n"
         << "<div class='file-list'>\n";

    if ((dir = opendir(directory.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string file_name = ent->d_name;
            // Ignorer les dossiers "." et ".."
            if (file_name != "." && file_name != "..") {
                html << "<a href='/download?" << file_name << "'>" << file_name << "</a><br>\n";
            }
        }
        closedir(dir);
    } else {
        html << "<p>Impossible d'ouvrir le répertoire.</p>\n";
    }
    html << "</div>\n</body>\n</html>";
    return html.str();
}

void Utils::stringToHTML(const std::string& content, const std::string& path, const std::string& name) {
    std::string fullPath = path + "/" + name;

    std::ofstream file(fullPath.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Erreur : Impossible d'ouvrir le fichier pour l'écriture.");
    }

    file << content;
    if (!file.good()) {
        throw std::runtime_error("Erreur : Échec de l'écriture dans le fichier.");
    }

    file.close();
}

Response *Utils::downloadFile(std::string const &query, std::string const rootPath) {
    Response *res = new Response();
    std::string path = rootPath + "/uploads/" + query;
    if (fileExists(path)) {
        res->setStatus(200, "OK");
        res->addHeader("Content-Type", "text/html");
        res->addHeader("Content-Disposition", "attachment; filename=" + query);
        res->setBody(res->fileToString(path));
        return res;
    }
    res->setStatus(404, "Not Found");
    res->addHeader("Content-Type", "text/html");
    res->setBody(res->fileToString("www/404.html"));
    return res;
}

void Utils::uploadFile(std::string const &body, std::string const &type, Response *res, std::string const &_rootPath) {
	std::string boundary = type.substr(type.find("boundary=") + 9);
	std::string tmp = body.substr(body.find("name=") + 6, body.find("\r\n\r\n") - body.find("name=") - 8);
	std::string contentFile = body.substr(body.find("\r\n\r\n") + 4, body.find("--" + boundary) - body.find("\r\n\r\n") - 6);
    std::size_t start = tmp.find("filename=\"") + 10; // "filename=\"" a 10 caractères
    std::string filename = tmp.substr(start, tmp.find("\"", start) - start);
    std::size_t endPos = contentFile.find(boundary);
    contentFile = contentFile.substr(0, endPos - 2);
    if(contentFile == "" || filename == "")
    {
        res->setStatus(400, "Bad Request");
        res->addHeader("Content-Type", "text/html");
        res->setBody("<html><body>Requête invalide.</body></html>");
        return;
    }
    std::ofstream tmpFile(_rootPath + "/uploads/" + filename, std::ofstream::binary);
    if (tmpFile.is_open()) {
        tmpFile.write(contentFile.c_str(), contentFile.size());
        tmpFile.close();
        Utils::stringToHTML(Utils::generateFileLinks(_rootPath + "/uploads"), _rootPath, "download.html");
        res->setStatus(200, "OK");
        res->addHeader("Content-Type", "text/html");
        res->setBody("<html><body>Fichier uploadé avec succès.</body></html>");
    } else {
        res->setStatus(500, "Internal Server Error");
        res->addHeader("Content-Type", "text/html");
        res->setBody("<html><body>Erreur lors de l'upload du fichier.</body></html>");
    }
}

std::string Utils::toLower(std::string str)
{
    for (size_t i = 0; i < str.length(); i++)
        str[i] = std::tolower(str[i]);
    return str;
}