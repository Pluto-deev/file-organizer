#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define NUM_CATEGORIES 10
#define PATH_BUFFER_SIZE 512

const char *categories[NUM_CATEGORIES] = {
    "Batch_Files", "Python_Files", "C_CPP_Files", "JSON_Files",
    "Apps", "Videos", "Books", "Music", "Downloads", "Other_Files"
};

const char *extensions[NUM_CATEGORIES][10] = {
    {".bat", NULL}, // Batch files
    {".py", NULL},  // Python files
    {".c", ".cpp", ".h", NULL},  // C/C++ files
    {".json", NULL}, // JSON files
    {".exe", ".apk", ".app", NULL}, // Apps
    {".mp4", ".avi", ".mkv", ".mov", NULL}, // Videos
    {".pdf", ".epub", ".mobi", ".txt", NULL}, // Books
    {".mp3", ".wav", ".flac", ".aac", NULL}, // Music
    {".zip", ".rar", ".tar", ".gz", NULL}, // Downloads
    {NULL} // Other files
};

// Function to get the file extension from a filename
const char *getFileExtension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot;
}

// Function to create a directory if it does not exist
void createDirectoryIfNeeded(const char *path, const char *category) {
    char fullPath[PATH_BUFFER_SIZE];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path, category);
    struct stat st = {0};
    if (stat(fullPath, &st) == -1) {
        if (mkdir(fullPath) != 0) {
            printf("Error creating directory %s: %s\n", fullPath, strerror(errno));
        }
    }
}

// Function to check if a directory is empty
int isDirectoryEmpty(const char *dirname) {
    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(dirname);
    if (dir == NULL) return 1;
    while ((d = readdir(dir)) != NULL) {
        if (++n > 2) break;
    }
    closedir(dir);
    return n <= 2;
}

// Recursive function to delete empty directories
void deleteEmptyDirectoriesRecursively(const char *path) {
    struct dirent *de;
    DIR *dr = opendir(path);
    
    if (dr == NULL) return;
    
    while ((de = readdir(dr)) != NULL) {
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
            char subpath[PATH_BUFFER_SIZE];
            snprintf(subpath, sizeof(subpath), "%s/%s", path, de->d_name);
            
            struct stat st;
            if (stat(subpath, &st) == -1) {
                printf("Error stating %s: %s\n", subpath, strerror(errno));
                continue;
            }
            
            if (S_ISDIR(st.st_mode)) {
                deleteEmptyDirectoriesRecursively(subpath);
            }
        }
    }
    closedir(dr);
    
    if (isDirectoryEmpty(path)) {
        if (rmdir(path) != 0) {
            printf("Error removing directory %s: %s\n", path, strerror(errno));
        }
    }
}

// Recursive function to categorize and move files
void categorizeAndMoveFilesRecursively(const char *path) {
    struct dirent *de;
    DIR *dr = opendir(path);
    
    if (dr == NULL) {
        printf("Could not open directory: %s\n", path);
        return;
    }
    
    int filesMoved[NUM_CATEGORIES] = {0};
    
    while ((de = readdir(dr)) != NULL) {
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
            char oldPath[PATH_BUFFER_SIZE];
            snprintf(oldPath, sizeof(oldPath), "%s/%s", path, de->d_name);
            
            struct stat st;
            if (stat(oldPath, &st) == -1) {
                printf("Error stating %s: %s\n", oldPath, strerror(errno));
                continue;
            }
            
            if (S_ISREG(st.st_mode)) {
                const char *ext = getFileExtension(de->d_name);
                int categoryIndex = NUM_CATEGORIES - 1; // Default to "Other_Files"
                
                for (int i = 0; i < NUM_CATEGORIES - 1; i++) {
                    for (int j = 0; extensions[i][j] != NULL; j++) {
                        if (strcmp(ext, extensions[i][j]) == 0) {
                            categoryIndex = i;
                            break;
                        }
                    }
                    if (categoryIndex != NUM_CATEGORIES - 1) break;
                }
                
                if (!filesMoved[categoryIndex]) {
                    createDirectoryIfNeeded(path, categories[categoryIndex]);
                    filesMoved[categoryIndex] = 1;
                }
                
                char newPath[PATH_BUFFER_SIZE];
                snprintf(newPath, sizeof(newPath), "%s/%s/%s", path, categories[categoryIndex], de->d_name);
                
                if (rename(oldPath, newPath) != 0) {
                    printf("Error moving file: %s -> %s\n", oldPath, newPath);
                    printf("Error: %s\n", strerror(errno));
                } else {
                    printf("Moved: %s -> %s/%s\n", de->d_name, categories[categoryIndex], de->d_name);
                }
            } else if (S_ISDIR(st.st_mode)) {
                categorizeAndMoveFilesRecursively(oldPath);
            }
        }
    }
    
    closedir(dr);
}

int main() {
    char path[PATH_BUFFER_SIZE];

    printf("Enter the directory path to organize: ");
    scanf("%s", path);
    
    // Remove any surrounding quotes from the input path
    size_t len = strlen(path);
    if (path[0] == '\"' && path[len - 1] == '\"') {
        memmove(path, path + 1, len - 2);
        path[len - 2] = '\0';
    }

    deleteEmptyDirectoriesRecursively(path);
    categorizeAndMoveFilesRecursively(path);
    deleteEmptyDirectoriesRecursively(path);
    
    printf("Files have been organized.\n");
    return 0;
}
