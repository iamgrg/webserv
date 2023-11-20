#include "Cgi.hpp"
#include <cstdlib>
#include <ctime>

Cgi::Cgi() {
  std::vector<std::string> largest_cities;
  largest_cities.push_back("Tokyo");
  largest_cities.push_back("Paris");
  largest_cities.push_back("Seoul");
  largest_cities.push_back("Sao Paulo");
  largest_cities.push_back("Mumbai");
  largest_cities.push_back("Rome");
  largest_cities.push_back("Beijing");
  largest_cities.push_back("Osaka");
  largest_cities.push_back("Cairo");
  largest_cities.push_back("New York");
  // Initialisation du générateur de nombres aléatoires
  std::srand(std::time(NULL));

  // Sélection aléatoire d'une ville
  int random_index = std::rand() % largest_cities.size();
  this->_city = largest_cities[random_index];
}

Cgi::Cgi(Request const &request) { this->_extractData(request); }
Cgi::~Cgi() {}
void Cgi::setCity(std::string city) { this->_city = city; }

std::string Cgi::getHTMLstr() const { return _HTMLstr; };

void Cgi::_extractData(Request const &request) {
  std::string city = request.getBody().substr(request.getBody().find("=") + 1);
  city = Utils::toLower(city);
  city[0] = toupper(city[0]);
  this->_city = city;
}

int Cgi::exec() {
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    std::cerr << "Échec de création du pipe" << std::endl;
    return 1;
  }

  pid_t pid = fork();
  if (pid < 0) {
    std::cerr << "Échec du fork" << std::endl;
    return 1;
  }
  if (pid == 0) {
    // Processus enfant
    close(pipefd[0]);               // Ferme le côté lecture du pipe
    dup2(pipefd[1], STDOUT_FILENO); // Redirige STDOUT vers le pipe
    close(pipefd[1]);

    char *argv[] = {(char *)"python", (char *)"./src/cgi/script.py",
                    (char *)_city.c_str(), NULL};
    execve("/usr/bin/python", argv, NULL);
    std::cerr << "Échec de l'execution du script" << std::endl;
    exit(1);
  } else {
    // Processus parent
    close(pipefd[1]); // Ferme le côté écriture du pipe
    char buffer[4096];
    ssize_t nbytes;
    while ((nbytes = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
      _HTMLstr.append(buffer, nbytes);
    }
    close(pipefd[0]);
    waitpid(pid, NULL, 0);
  }
  if(_HTMLstr.empty())
    return 1;
  return 0;
}
