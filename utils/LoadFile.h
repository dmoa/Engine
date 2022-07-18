#pragma once

char *LoadFile(char *path);

#ifdef ENGINE_IMPLEMENTATION

char *LoadFile(char *path) {
    print("%s", path);
    FILE *file = fopen(path, "r");
    if (!file) {
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

#endif
