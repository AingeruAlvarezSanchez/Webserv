#include <string>
#include <fstream>
#include <stdexcept>

static void checkFileExtension(const std::string& file) {
    if (file.rfind(".conf") == std::string::npos) {
        throw std::invalid_argument("Webserv: " + file + ": ");
    }
}

static void checkOpenFile(const std::string& file) {
    std::ifstream fileStream(file);

    if (!fileStream.is_open()) {
        throw std::runtime_error("Webserv: " + file + ": ");
    }
}

void checkFile(const std::string& file) {
    //TODO check more file errors:
    //1. Check if file is empty
    checkFileExtension(file);
    checkOpenFile(file);
}
