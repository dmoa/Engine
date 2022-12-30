#pragma once

#include <string>

char* LoadFile(char *path);
char* LoadFile(std::string path);

#ifdef ENGINE_IMPLEMENTATION

char* LoadFile(char *path) {
    FILE *file = fopen(path, "r");

    if (! file) {
        print("Couldn't load %s !", path);
        return NULL;
    }

    // 0L because fseek takes a long integer, but we still only want zero
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char *buffer = (char *)calloc(file_size, sizeof(char));
    fread(buffer, sizeof(char), file_size, file);

    fclose(file);

    return buffer;
}

char* LoadFile(std::string path) {
    FILE *file = fopen(path.c_str(), "r");

    if (! file) {
        print("Couldn't load %s !", path.c_str());
        return NULL;
    }

    // 0L because fseek takes a long integer, but we still only want zero
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char *buffer = (char *)calloc(file_size, sizeof(char));
    fread(buffer, sizeof(char), file_size, file);

    fclose(file);

    return buffer;
}

#endif