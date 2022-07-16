#pragma once

// strmalloc without adding terminating character
inline char *strmalloc_wt(const char *src) {
    int len = strlen(src);
    char *s = bmalloc_arr(char, strlen(src) + 1);
    strcpy(s, src);

    return s;
}

inline char *strmalloc(const char *src) {
    int len = strlen(src);
    char *s = bmalloc_arr(char, strlen(src) + 1);
    strcpy(s, src);
    s[len] = '\0';

    return s;
}

inline bool strequal(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}
inline bool strequal(char *a, const char *b) {
    return strcmp(a, b) == 0;
}
inline bool strequal(char *a, char *b) {
    return strcmp(a, b) == 0;
}