#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <shellapi.h>
#include <string.h>

BOOL IsUserAnAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    
    // Allocate and initialize a SID for the administrators group.
    // Windows API Stuff.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&NtAuthority, 2,
                                 SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS,
                                 0, 0, 0, 0, 0, 0,
                                 &adminGroup)) {
        // Check if the user is part of the administrators group
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);  // Free the SID when done
    }
    
    return isAdmin;
}

int ttp_already_exists() {
    FILE *hosts = fopen("C:\\Windows\\System32\\drivers\\etc\\hosts", "r");
    FILE *temp  = fopen("C:\\Windows\\System32\\drivers\\etc\\hosts_temp", "w");
    char buffer[256];
    int found = 0;
    int success = 1;

    // Reads through the hosts file line by line
    while (fgets(buffer, sizeof(buffer), hosts)) {
        
        // If current line is the line after timmartillpermis
        if (found) {
            found = 0;
            continue;
        }

        // If timmartillpermis entry exists in the hosts file
        if (strstr(buffer, "# Timmartillpermis")) {
            found = 1;
            success = 0;
            continue;
        }
        
        // Writes every line of hosts into a temp file
        fputs(buffer, temp);
    }
    fclose(hosts);
    fclose(temp);

    remove("C:\\Windows\\System32\\drivers\\etc\\hosts");
    rename("C:\\Windows\\System32\\drivers\\etc\\hosts_temp", "C:\\Windows\\System32\\drivers\\etc\\hosts");

    if (success == 0) {
        return 0;
    } else {
        return 1;
    }
}

char *get_ip() {
    FILE *arp_process;
    char data[256];
    char result[256] ="";
    char *ip = malloc(32);

    if (ip == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    // Opens a powershell process as a "file", reading the output of it.
    arp_process = popen("powershell -Command \"arp -a | Select-String -Pattern '00-11-11-03-83-fa'\"", "r");
    if (arp_process == NULL) {
        printf("Couldn't run command.\n");
        return NULL;
    }
    
    // Filters the output until the line with the IP is read. Appends it to result.
    for (int i = 0; i < 3; i++) {
        char *res = fgets(data, sizeof(data), arp_process); 
        if (i == 1 && res != NULL) {
            strcat(result, data);
        }
    }
    pclose(arp_process);

    // Searches the result line for the start of the IP address.
    // Creates a pointer to it.
    char *ip_start = strstr(result, "10.");
    if (ip_start != NULL) {
        sscanf(ip_start, "%15[0-9.]", ip);  // Reads the 15 first numbers and dots from the start of the IP and writes them to the ip variable.
    } else {
        printf("Couldn't find IP in arp cache.\n");
        return NULL;
    }
    
    return ip;
}

int main() {

    // Runs the program with elevated priveleges to be able to access the hosts file.
    if (!IsUserAnAdmin()) {
        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = "runas";
        sei.lpFile = "local-ttp-v1.1.exe";
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;

        if (!ShellExecuteEx(&sei)) {
            printf("This program needs admin privleges to work.\n");
            return 1;
        }

        return 0; // Exits unpriveleged program.
    }

    // Checks if entry already exists in the hosts file.
    int ttp_existed = ttp_already_exists();
    char data[128];
    FILE *hosts = fopen("C:\\Windows\\System32\\drivers\\etc\\hosts", "a");
    
    // Fixes formatting for the hosts file
    char *hosts_str;
    if (ttp_existed == 1) {
        hosts_str = "\n\n# Timmartillpermis\r\n%s timmartillpermis.se";
    } else {
        hosts_str = "# Timmartillpermis\r\n%s timmartillpermis.se";
    }
    // Gets the correct IP, formats it into the string and writes it to the data variable.
    char *ip = get_ip();
    sprintf(data, hosts_str, ip);

    if (hosts == NULL) {
        printf("Couldn't open hosts file.\n");
        return 1;
    }

    fputs(data, hosts);
    fclose(hosts);
    printf("Hosts file successfully modified!\nYou may now visit http://timmartillpermis.se.\n");
    Sleep(3000);
    
    free(ip);
    return 0;   
}
