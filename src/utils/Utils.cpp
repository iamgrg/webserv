#include "Utils.hpp"

Utils::Utils() { }
Utils::~Utils() { }

bool Utils::fileExists(const std::string &path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

#include <dirent.h>
#include <sstream>
#include <string>

std::string const Utils::generateFileLinks(const std::string& directory) {
    DIR *dir;
    struct dirent *ent;
    std::stringstream html;

    // Début de la structure HTML
    html << "<!DOCTYPE html>\n"
         << "<html lang='fr'>\n"
         << "<head>\n"
         << "    <meta charset='UTF-8'>\n"
         << "    <title>Liste des fichiers</title>\n"
         << "<style>\n"
         << "    body {\n"
         << "        font-family: Arial, sans-serif;\n"
         << "        background-color: #f4f4f4;\n"
         << "        color: #333;\n"
         << "        margin: 0;\n"
         << "        padding: 0;\n"
         << "        display: flex;\n"
         << "        justify-content: center;\n"
         << "        align-items: center;\n"
         << "        height: 100vh;\n"
         << "        width: 60%;       /* Largeur de la div à 60% de l'écran */\n"
         << "        margin: 0 auto;  \n"
         << "    }\n"
         << "    h1 {\n"
         << "        width: 60vh;\n"
         << "        color: #333;\n"
         << "        text-align: center;\n"
         << "}\n"
         << "    ul {\n"
         << "        list-style: none;\n"
         << "        padding: 0;\n"
         << "    }\n"
         << "    li {\n"
         << "        margin: 10px 0;\n"
         << "    }\n"
         << "    a {\n"
         << "        text-decoration: none;\n"
         << "        color: black;\n"
         << "        font-weight: bold;\n"
         << "    }\n"
         << "    a:hover {\n"
         << "        color: #0056b3;\n"
         << "    }\n"
         << "    .link-box {\n"
         << "            border: 1px solid #ddd;\n"
         << "            border-radius: 8px;\n"
         << "            padding: 20px;\n"
         << "            text-align: center;\n"
         << "            margin-bottom: 20px;\n"
         << "            transition: box-shadow 0.3s;\n"
         << "        }\n"
         << "        .link-box img {\n"
         << "            max-width: 100%;\n"
         << "            height: auto;\n"
         << "            border-radius: 4px;\n"
         << "        }\n"
         << "    .delete-btn {\n"
         << "        float: right;\n"
         << "        color: rgb(0, 0, 0);\n"
         << "        cursor: pointer;\n"
         << "        font-size: 20px;\n"
         << "        padding-right: 10px;\n"
         << "    }\n"
         << "    .delete-btn:hover {\n"
         << "        color: #ff0000;\n"
         << "    }\n"
         << "</style>\n"
         << "</head>\n"
         << "<body>\n"
         << "<div class='file-list'>\n"
         << "<h1>Fichiers disponibles au téléchargement</h1>\n";

    if ((dir = opendir(directory.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string file_name = ent->d_name;
            // Ignorer les dossiers "." et ".."
            if (file_name != "." && file_name != "..") {
                html << "<div class=\"link-box\">\n"
                     << "<span class=\"delete-btn\" onclick=\"deleteFile('" << file_name << "')\">&times;</span>\n"
                     << "<a href='/download?" << file_name << "'>" << file_name << "</a><br>\n"
                     << "</div>\n";
            }
        }
        closedir(dir);
    } else {
        html << "<p>Impossible d'ouvrir le répertoire.</p>\n";
    }
    html << "</div>\n"
         << "<script>\n"
         << "    function deleteFile(fileName) {\n"
         << "        if (confirm('Êtes-vous sûr de vouloir supprimer ce fichier ?')) {\n"
         << "            fetch('/download?' + fileName, { method: 'DELETE' })\n"
         << "            .then(response => {\n"
         << "                if (response.ok) {\n"
         << "                    alert('Fichier supprimé avec succès.');\n"
         << "                    window.location.reload(); // Recharger la page pour refléter le changement\n"
         << "                } else {\n"
         << "                    alert('Erreur lors de la suppression du fichier.');\n"
         << "                }\n"
         << "            })\n"
         << "            .catch(error => alert('Erreur : ' + error));\n"
         << "        }\n"
         << "    }\n"
         << "</script>\n"
         << "</script>\n"
         << "</body>\n</html>";
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

std::string fileToBinaryString(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    std::string contents;

    if (file) {
        contents.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    return contents;
}


Response *Utils::downloadFile(std::string const &query, std::string const rootPath) {
    Response *res = new Response();
    std::string path = rootPath + "/uploads/" + query;
    if (fileExists(path)) {
        res->setStatus(200, "OK");
        res->addHeader("Content-Type", "image/png");
        res->addHeader("Content-Disposition", "attachment; filename=" + query);
        res->setBody(res->fileToString(path));
        return res;
    }
    std::cout << "FILE NOT FOUND" << std::endl;
    res->setStatus(404, "Not Found");
    res->addHeader("Content-Type", "text/html");
    res->setBody(res->fileToString("www/404.html"));
    std::cout << "BODY : " << res->getBody() << std::endl;
    res->addHeader("Content-Length", std::to_string(res->getBody().size()));
    return res;
}

void    Utils::uploadFile(std::string const &body, std::string const &type, Response *res, std::string const &_rootPath) {
	std::cout << "UPLOAD FILE" << std::endl;
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
        res->addHeader("Content-Length", std::to_string(res->getBody().size()));
        return;
    }
    std::ofstream tmpFile(_rootPath + "/uploads/" + filename, std::ofstream::binary);
    if (tmpFile.is_open()) {
        std::cout << "Fichier ouvert" << std::endl;
        tmpFile.write(contentFile.c_str(), contentFile.size());
        tmpFile.close();
        Utils::stringToHTML(Utils::generateFileLinks(_rootPath + "/uploads"), _rootPath, "download.html");
        res->setStatus(200, "OK");
        res->addHeader("UTF-8", "text/html");
        std::string body = "<html><head>\n"
	                        "<meta charset=\"UTF-8\">\n"
	                        "<title>Succes !</title>\n"
                            "</head>\n"
                            "<body>\n"
                           "<h1>Fichier uploadé avec succès.</h1>\n"
                           "<a href='/download?" + filename + "'>Télécharger le fichier</a>\n"
                           "</body></html>";
        res->setBody(body);
    } else {
        res->setStatus(500, "Internal Server Error");
        res->addHeader("Content-Type", "text/html");
        res->setBody("<html><body>Erreur lors de l'upload du fichier.</body></html>");
        res->addHeader("Content-Length", std::to_string(res->getBody().size()));
    }
}

std::string Utils::toLower(std::string str)
{
    for (size_t i = 0; i < str.length(); i++)
        str[i] = std::tolower(str[i]);
    return str;
}