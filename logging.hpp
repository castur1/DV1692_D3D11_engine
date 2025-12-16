#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <stdio.h>
#include <string.h>

#include <Windows.h>

static inline const char *LogHelper(const char *path) {
    const char *slash = strrchr(path, '/');
    const char *backslash = strrchr(path, '\\');

    const char *seperator = slash > backslash ? slash : backslash;

    return seperator ? seperator + 1 : path;
}

#ifdef _DEBUG
#define LogInfo(format, ...) \
    printf("[INFO] " format, __VA_ARGS__)

#define LogError(format, ...) \
    do { \
        printf("[ERROR] %s:%d in %s(): " format, LogHelper(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
        getchar(); \
    } while (0)

#else
#define LogInfo(format, ...) (void)0

#define LogError(format, ...) \
    do { \
        wchar_t buffer[512]; \
        swprintf_s(buffer, L"[ERROR] %hs:%d in %hs(): " L##format, LogHelper(__FILE__), __LINE__, __func__, ##__VA_ARGS__); \
        MessageBox(NULL, buffer, L"Fatal error", MB_OK | MB_ICONERROR); \
    } while (0)

#endif

#endif