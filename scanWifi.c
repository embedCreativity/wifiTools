#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char wlanUpCmd[] = "ifconfig wlan0 up";
char scanCmd[] = "iw dev wlan0 scan";

typedef enum state_t {
    BSS = 0,
    SSID,
    RSN,
    RSN_CIPHER,
    RSN_AUTH_SUITE,
    UNKNOWN
} state_t;

char *strBSS = "BSS";
char *strSSID = "SSID: ";
char *strRSN = "RSN";
char *strCipher = "Group cipher: ";
char *strAuthSuite = "Authentication suites: ";
char *searchTerm[UNKNOWN];

state_t state;

// function prototypes
state_t ClassifyLine ( char *line );

int main( int argc, char *argv[] )
{

    FILE *fp;
    char line[1024];
    char ssid[64];
    char cipher[64];
    char authSuite[64];

    state_t found;

    state = BSS; // init state variable
    searchTerm[0] = strBSS;
    searchTerm[1] = strSSID;
    searchTerm[2] = strRSN;
    searchTerm[3] = strCipher;
    searchTerm[4] = strAuthSuite;

    // blindly bring up wlan0 (don't check for error)
    system(wlanUpCmd);

    // run scan command and prep for reading the output
    fp = popen(scanCmd, "r");
    //fp = fopen("/home/user/scanout.txt", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    // read the output of the scan command - one line at a time
    while (fgets(line, sizeof(line)-1, fp) != NULL) {
        // strip newline off of line
        line[strcspn(line, "\n")] = 0;

        switch (state)
        {
            case BSS: // only look for BSS - ignore everything else
                found = ClassifyLine(line);
                if ( found == BSS ) {
                    state = SSID; // advance state
                }
                break;
            case SSID:
                found = ClassifyLine(line);
                if ( found == SSID) {
                    // grab SSID
                    strncpy(ssid, (char*)(strstr(line, ":") + 2), 64);
                    if ( 0 == strlen(ssid) ) {
                        state = BSS; // Do not show empty SSIDs
                    } else {
                        state = RSN; // advance state
                    }
                } else if ( found == BSS ) { // error, but don't lose next SSID
                    state = SSID;
                } else if ( found != UNKNOWN ) { // error - ditch and look for BSS
                    state = BSS;
                }
                break;
            case RSN:
                found = ClassifyLine(line);
                if ( found == RSN) {
                    state = RSN_CIPHER; // this SSID has crypto! Find details
                } else if ( found == BSS) { // no crypto for this SSID - print now
                    printf("[%s][OPEN][]\n", ssid);
                    state = SSID;
                } else if ( found != UNKNOWN ) { // error - ditch and look for BSS
                    state = BSS;
                }
                break;
            case RSN_CIPHER:
                found = ClassifyLine(line);
                if ( found == RSN_CIPHER) {
                    strncpy(cipher, (char*)(strstr(line, ":") + 2), 64);
                    //strncpy(cipher, (char*)(line + strlen(searchTerm[found])), 64);
                    state = RSN_AUTH_SUITE; // this SSID has crypto! Find details
                } else if ( found == BSS ) { // error, but don't lose next SSID
                    state = SSID;
                } else if ( found != UNKNOWN ) { // error - ditch and look for BSS
                    state = BSS;
                }
                break;
            case RSN_AUTH_SUITE:
                found = ClassifyLine(line);
                if ( found == RSN_AUTH_SUITE) {
                    strncpy(authSuite, (char*)(strstr(line, ":") + 2), 64);
                    //strncpy(authSuite, (char*)(line + strlen(searchTerm[found])), 64);
                    printf("[%s][%s][%s]\n", ssid, cipher, authSuite);
                    state = BSS; // all done! Do it again for next SSID!
                } else if ( found == BSS ) { // error, but don't lose next SSID
                    state = SSID;
                } else if ( found != UNKNOWN ) { // error - ditch and look for BSS
                    state = BSS;
                }
                break;
            default:
                return -1;
        }
    }

    if (state == RSN) { // SSID found, but no RSN
        printf("[%s] [OPEN][]\n", ssid);
    }

    /* close */
    pclose(fp);

    return 0;
}

state_t ClassifyLine ( char *line )
{
    int i;

    for ( i = 0; i < (int)UNKNOWN; i++ ) {
        if ( NULL != strstr(line, searchTerm[i]) ) {
            return (state_t)i;
        }
    }

    return UNKNOWN;
}
