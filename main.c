#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <shellapi.h>

BOOL IsUserAnAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    
    // Allocate and initialize a SID for the administrators group.
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

int main() {
    if (!IsUserAnAdmin()) {
        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = "runas";
        sei.lpFile = "local-ttp.exe";
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;

        if (!ShellExecuteEx(&sei)) {
            printf("This program needs admin privleges to work.\n");
            return 1;
        }

        return 0; // Exits unpriveleged program.
    }

    char data[128] = "\n\n# Timmartillpermis\r\n10.80.72.87 timmartillpermis.se";

    FILE *hosts;
    hosts = fopen("C:\\Windows\\System32\\drivers\\etc\\hosts", "a");

    if (hosts == NULL) {
        printf("Couldn't open hosts file.\n");
        return 1;
    }

    fputs(data, hosts);
    fclose(hosts);
    printf("Hosts file successfully modified!\nYou may now visit http://timmartillpermis.se.\n");
    
    return 0;   
}