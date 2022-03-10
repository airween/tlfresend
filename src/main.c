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
        {"HA2MH",   "S52OS",   "OS"},
        {"S2MH",    "S52OS",   ""},
        {"S2MH",    "S52MH",   ""},
        {"OK2FHI",  "OK1FHI",  "OK1"},
        {"OK1FH",   "OK1FHI",  "FHI"},
        {"OK2FH",   "OK1FHI",  "1FHI"},
        {"WB6AB",   "WB6A",    "6A"},
        {"HG5N",    "HA5N",    "HA"},
        {"HG5N",    "HG55N",   ""},
        {"HG5N",    "HG6N",    "HG6"},
        {"HG5N",    "HG5D",    "5D"},
        {"PA1234X", "PA1834X", ""},
        {"PA1234X", "PA1834K", ""},
        {"PA1234X", "PA1234K", "4K"},
        {"PA1234X", "RA1234X", ""},
        {"F1234X",  "R1234X",  ""},
        {"F1234X",  "R1234C",  "R1234C"},
        {"K1A",     "K1W",     "1W"},
        {"K1A",     "W1A",     "W1"},
        {"K1A",     "K2A",     "K2"},
        {"DL1ABC",  "DL1ADC",  ""},
        {"R1ABC/2", "R1ABC/3", "/3"},
        {"R1ABC/2", "R1ADC/2", ""},
        {"R1ABC/2", "R4ADC/2", ""},
        {"R1ABC/2", "R1ADC/3", ""},
        {"R1A/2",   "R1W/2", ""},
        {"EA8/HB9ABC", "EA8/HB9ANC", ""},
        {"EA8/HB9ABC", "EA6/HB9ABC", ""},
        {"EA8/HB9ABC", "EA6/HB9ANC", ""},
        {"EA8/DL1ABC", "EA8/DK1ABC", ""},
        {"EA8/DL1ABC", "EA6/DK1ABC", ""},
        {"G/DF1ABC",   "F/DF1ABC",   "F/"},
        {"LA/DF1ABC",  "PA/DF1ABC",  "PA/"},
        {"HA5ABC/P",   "HA5ABC",    ""},
        {"HA5ABC",     "HA5ABC/P",  ""},
        {"HB9/OK1ABC", "OK1ABC",    ""},
        {"OK1ABC",     "HB9/OK1ABC",  ""},
        {"", "", ""}
    };
    int i = 0;
    char partial[20];
    char partial_alt[20];

    char *result;
    if (tests[i][0][0] != '\0') {
        printf(" Status | CALL 1st            | CALL 2nd            | Expected            | Got                 \n");
        printf("--------+---------------------+---------------------+---------------------+---------------\n");
    }
    while(tests[i][0][0] != '\0') {
        get_partial_callsign(tests[i][0], tests[i][1], partial);
        if (strcmp(tests[i][2], partial) != 0) {
            result = "ERR";
        }
        else {
            result = "OK";
        }
        get_partial_callsign_alt(tests[i][0], tests[i][1], partial_alt);
        char *diff = (strcmp(partial, partial_alt) ? " *" : "");
        printf(" %-3s    | %-20s| %-20s| %-20s| %-20s| %s%s\n",
            result, tests[i][0], tests[i][1], tests[i][2],
            partial, partial_alt, diff);
        i++;
    }

    return 0;
}
