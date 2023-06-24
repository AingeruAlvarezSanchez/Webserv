#include <fstream>
#include <stdio.h>

static long fileSize(FILE *fileContent) {
    long size = 0;
    if (fileContent != NULL) {
        fseek(fileContent, 0, SEEK_END);
        size = ftell(fileContent);
    }
    return size;
}

static bool isCorrectFileExtension(const std::string& file) {
    if (file.rfind(".conf") == std::string::npos) {
        return false;
    }
    return true;
}

static bool isAccessibleFile(const std::string& file) {
    std::ifstream fileStream(file);

    if (!fileStream.is_open()) {
        return false;
    }
    return true;
}

bool isValidFile(const std::string& file) {
    FILE *fileContent = fopen(file.c_str(), "rb");
    if (fileSize(fileContent) != 0
        && isCorrectFileExtension(file)
        && isAccessibleFile(file)) {
        return true;
    }
    return false;
}
