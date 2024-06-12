# Automated File Organizer

This is a simple C program to automatically organize files in a specified directory by file type. The program categorizes files into directories based on their extensions and moves them accordingly.

## Usage

1. Compile the program using GCC:
    ```sh
    gcc -o file_manager automated_file_organizer.c
    ```
2. Run the executable:
    ```sh
    file_manager.exe
    ```
3. Enter the directory path to organize when prompted.

## Features

- Categorizes files into predefined categories.
- Creates directories for each category if not already present.
- Recursively organizes files in nested directories.
- Deletes empty directories after organizing files.

## License

This project is licensed under a custom License.
