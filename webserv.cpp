#include <iostream>
#include <cerrno>
#include <cstring>

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
    return 0;
}
