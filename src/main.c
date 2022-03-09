#include <stdio.h>
#include <string.h>

#include "utils.h"

char wpx_prefix[11];

int main(int argc, char ** argv) {
    char tests[][3][20] = {
        {"WB6A",    "W6BA",    "W6B"},
        {"HA2OH",   "HA2OS",   "OS"},
        {"HA2MS",   "HA2OS",   "OS"},
        {"HA2MH",   "HA2OS",   "OS"},
        {"OK2FHI",  "OK1FHI",  "OK1"},
        {"OK1FH",   "OK1FHI",  "FHI"},
        {"OK2FH",   "OK1FHI",  "1FHI"},
        {"WB6AB",   "WB6A",    "6A"},
        {"", "", ""}
    };
    int i = 0;
    char partial[20];

    char result[5] = " ";
    if (tests[i][0][0] != '\0') {
        printf(" Status | CALL 1st            | CALL 2nd            | Result              | Got                 \n");
        printf("--------+---------------------+---------------------+---------------------+---------------\n");
    }
    while(tests[i][0][0] != '\0') {
        get_partial_callsign(tests[i][0], tests[i][1], partial);
        if (strcmp(tests[i][2], partial) != 0) {
            strcpy(result, "!");
        }
        else {
            result[0] = ' ';
            result[1] = '\0';
        }
        printf("%sOK     | %-20s| %-20s| %-20s| %-20s\n", result, tests[i][0], tests[i][1], tests[i][2], partial);
        i++;
    }

    return 0;
}
