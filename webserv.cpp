#include <iostream>
#include <cerrno>
#include <cstring>
#include <fstream>

int conf_file_parsing(std::ifstream const &fileName) {
    std::cout << fileName << "\n";
    if (fileName.is_open()) {
        std::cout << "is open\n";
        return 1;
    } else {
        std::cout << "error de no abrir archivo: crear .h\n";
        return 1;
    }
    return 0;
}

int main(int argc, __attribute__((unused)) char **argv) {
    if (argc < 2) {
        errno = 22;
        std::cout << strerror(errno) << "\n";
        return 1;
    } else if (argc > 2) {
        errno = 7;
        std::cout << strerror(errno) << "\n";
        return 1;
    }
    try {
        /**
         * Config file health check, first, checks extension
         */
        conf_file_parsing(std::ifstream(argv[1], std::ifstream::in));
    }
    catch (std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
    return 0;
}
