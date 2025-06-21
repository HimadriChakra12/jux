#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

// Function to get file extension (in lowercase)
const char* get_file_extension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

// Function to get filename from URL (everything after last '/')
void get_filename_from_url(const char* url, char* filename_out, size_t size) {
    const char* slash = strrchr(url, '/');
    if (slash && *(slash + 1) != '\0') {
        strncpy(filename_out, slash + 1, size);
        filename_out[size - 1] = '\0'; // Safety null-terminate
    } else {
        // Fallback filename if URL ends with slash or no filename
        snprintf(filename_out, size, "downloaded.file");
    }
}

// Function to determine folder type based on extension
const char* get_folder_type(const char* ext) {
    if (strcmp(ext, "exe") == 0) return "exe";
    if (strcmp(ext, "zip") == 0 || strcmp(ext, "rar") == 0 || strcmp(ext, "7z") == 0 || strcmp(ext, "tar") == 0 || strcmp(ext, "gz") == 0) return "files";
    if (strcmp(ext, "mp4") == 0 || strcmp(ext, "mkv") == 0 || strcmp(ext, "avi") == 0 || strcmp(ext, "mov") == 0 || strcmp(ext, "wmv") == 0) return "video";
    return "others";
}

void download_file(const char* url) {
    char filename[256];
    get_filename_from_url(url, filename, sizeof(filename));

    // Convert filename extension to lowercase for comparison
    char ext[16];
    const char* file_ext = get_file_extension(filename);
    size_t i;
    for (i = 0; i < sizeof(ext) - 1 && file_ext[i]; i++) {
        ext[i] = (char)tolower(file_ext[i]);
    }
    ext[i] = '\0';

    const char* folder = get_folder_type(ext);

    // Build destination path:
    // Example: C:\Users\<User>\Downloads\jux\exe\file.exe
    char dest_path[512];
#ifdef _WIN32
    char* userprofile = getenv("USERPROFILE");
    if (!userprofile) userprofile = "C:\\Users\\Default";
    snprintf(dest_path, sizeof(dest_path), "%s\\Downloads\\jux\\%s\\%s", userprofile, folder, filename);
#else
    char* home = getenv("HOME");
    if (!home) home = "/tmp";
    snprintf(dest_path, sizeof(dest_path), "%s/Downloads/jux/%s/%s", home, folder, filename);
#endif

    // Make sure the destination directory exists (simple approach)
#ifdef _WIN32
    char mkdir_cmd[512];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir \"%s\\Downloads\\jux\\%s\"", getenv("USERPROFILE"), folder);
    system(mkdir_cmd);
#else
    char mkdir_cmd[512];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p \"%s/Downloads/jux/%s\"", getenv("HOME"), folder);
    system(mkdir_cmd);
#endif

    // Compose PowerShell BITSTransfer command for Windows
#ifdef _WIN32
    char command[1024];
    snprintf(command, sizeof(command),
        "powershell -Command \"Start-BitsTransfer -Source '%s' -Destination '%s'\"",
        url, dest_path);
    printf("Downloading:\nURL: %s\nDestination: %s\n", url, dest_path);
    system(command);
#else
    // For Linux/macOS you can replace this with curl or wget (optional)
    printf("This download manager is currently Windows only.\n");
#endif
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: jux.exe <download_url>\n");
        return 1;
    }

    download_file(argv[1]);

    return 0;
}
