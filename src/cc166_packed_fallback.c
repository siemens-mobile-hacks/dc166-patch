#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    SOURCE_PACKED = 1,
    SOURCE_BITFIELD = 2
};

static int scan_source(const char *path, unsigned depth)
{
    FILE *file;
    char *text;
    char *cursor;
    char directory[MAX_PATH];
    long size;
    int result = 0;

    if (depth > 8 || path[0] == '-' || path[0] == '/' || path[0] == '\0')
        return 0;
    file = fopen(path, "rb");
    if (!file)
        return 0;
    if (fseek(file, 0, SEEK_END) != 0 || (size = ftell(file)) < 0 ||
        fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return 0;
    }
    text = (char *)malloc((size_t)size + 1);
    if (!text) {
        fclose(file);
        return 0;
    }
    size = (long)fread(text, 1, (size_t)size, file);
    fclose(file);
    text[size] = '\0';

    if (strstr(text, "_packed") != NULL)
        result |= SOURCE_PACKED;
    for (cursor = text; *cursor; ++cursor) {
        char *next;
        if (*cursor != ':')
            continue;
        next = cursor + 1;
        while (*next == ' ' || *next == '\t')
            ++next;
        if (*next >= '0' && *next <= '9')
            result |= SOURCE_BITFIELD;
    }

    strncpy(directory, path, sizeof(directory) - 1);
    directory[sizeof(directory) - 1] = '\0';
    cursor = strrchr(directory, '\\');
    if (!cursor)
        cursor = strrchr(directory, '/');
    if (cursor)
        cursor[1] = '\0';
    else
        directory[0] = '\0';

    cursor = text;
    while ((cursor = strstr(cursor, "#include")) != NULL) {
        char *quote = strchr(cursor + 8, '"');
        char *end;
        char include_path[MAX_PATH];
        size_t name_len;
        if (!quote || (end = strchr(quote + 1, '"')) == NULL) {
            cursor += 8;
            continue;
        }
        name_len = (size_t)(end - quote - 1);
        if (strlen(directory) + name_len < sizeof(include_path)) {
            strcpy(include_path, directory);
            strncat(include_path, quote + 1, name_len);
            result |= scan_source(include_path, depth + 1);
        }
        cursor = end + 1;
    }
    free(text);
    return result;
}

int main(int argc, char **argv)
{
    char executable[MAX_PATH];
    char launch_name[MAX_PATH];
    char **child_argv;
    char *slash;
    intptr_t result;
    int source_features = 0;
    int fallback_optimization = 0;
    int i;
    int out;

    if (!GetModuleFileNameA(NULL, executable, sizeof(executable)))
        return 255;
    slash = strrchr(executable, '\\');
    if (!slash)
        slash = strrchr(executable, '/');
    if (!slash)
        return 255;
    strcpy(slash + 1, "cc166-original.exe");
    if (!GetShortPathNameA(executable, launch_name, sizeof(launch_name)))
        strcpy(launch_name, executable);

    for (i = 1; i < argc; ++i) {
        source_features |= scan_source(argv[i], 0);
        if (strcmp(argv[i], "-O0") == 0 || strcmp(argv[i], "-O3") == 0)
            fallback_optimization = 2;
    }
    if ((source_features & (SOURCE_PACKED | SOURCE_BITFIELD)) ==
        (SOURCE_PACKED | SOURCE_BITFIELD))
        fallback_optimization = 1;
    else if (!(source_features & SOURCE_PACKED))
        fallback_optimization = 0;

    child_argv = (char **)calloc((size_t)argc + 2, sizeof(*child_argv));
    if (!child_argv)
        return 255;
    out = 0;
    /* MSVCRT's spawn command-line builder does not reliably quote argv[0]. */
    child_argv[out++] = launch_name;
    if (fallback_optimization)
        child_argv[out++] = fallback_optimization == 1 ? "-O0" : "-O2";
    for (i = 1; i < argc; ++i) {
        if (fallback_optimization && argv[i][0] == '-' && argv[i][1] == 'O')
            continue;
        child_argv[out++] = argv[i];
    }
    child_argv[out] = NULL;

    result = _spawnv(_P_WAIT, launch_name, (const char *const *)child_argv);
    free(child_argv);
    if (result == -1) {
        fprintf(stderr, "cc166: cannot start %s\n", executable);
        return 255;
    }
    return (int)result;
}
