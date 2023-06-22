#include <string>
#include <fstream>
#include <stdexcept>

static void checkFileExtension(const std::string& file) {
    if (file.rfind(".conf") == std::string::npos) {
        errno = 134;
        throw std::invalid_argument("Webserv: Invalid configuration file provided: " + file);
    }
}

static void checkOpenFile(const std::string& file) {
    std::ifstream fileStream(file);

    if (!fileStream.is_open()) {
        errno = 134;
        throw std::runtime_error("Webserv: Could not open file: " + file);
    }
}

void checkFile(const std::string& file) {
    //TODO check more file errors:
    //1. Check if file is empty
    checkFileExtension(file);
    checkOpenFile(file);
}
