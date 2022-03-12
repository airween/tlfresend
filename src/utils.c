/*
 * Tlf - contest logging program for amateur radio operators
 * Copyright (C) 2021           Thomas Beierlein <tb@forth-ev.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "getpx.h"

/* \brief find named file in actual directory or in share
 *
 * \returns filename of actual available file or NULL if not found
 *	    returned pointer has to be freed
 */
char *find_available(char *filename) {
    char *path;

    if (g_access(filename, R_OK) == 0) {
	path = g_strdup(filename);
    } else {
	path = g_strconcat(".", G_DIR_SEPARATOR_S,
			   filename, NULL);
	if (g_access(path, R_OK) != 0) {
	    g_free(path);
	    path = g_strdup("");
	}
    }
    return path;
}

typedef struct {
    char *alt_prefix;
    char *prefix;
    char *area;
    char *suffix;
    char *alt_area;
} call_parts_t;

static void free_call_parts(call_parts_t *cp) {
    if (cp == NULL) {
        return;
    }
    g_free(cp->alt_prefix);
    g_free(cp->prefix);
    g_free(cp->area);
    g_free(cp->suffix);
    g_free(cp->alt_area);
    g_free(cp);
}

// split a call into alt_prefix-prefix-area-suffix-alt_area parts
// returns non-NULL on success with all pointers being also non-NULL
static call_parts_t *split_call(char *call) {

    static const char *PATTERN = "^"
        "([A-Z0-9]+/)?"     // alt_prefix (optional)
        "([A-Z0-9]*?[A-Z])" // prefix
        "(\\d+)"            // area
        "([A-Z]+)"          // suffix
        "(/[0-9A-Z])?"      // alt_area (optional)
        "$"
    ;

    static GRegex *regex = NULL;
    if (regex == NULL) {
        regex = g_regex_new(PATTERN, 0, 0, NULL);
    }

    call_parts_t *result = NULL;
    GMatchInfo *match_info;
    g_regex_match(regex, call, 0, &match_info);

    if (g_match_info_matches(match_info)) {
        result = g_new(call_parts_t, 1);
        result->alt_prefix = g_match_info_fetch(match_info, 1);
        if (result->alt_prefix == NULL) {
            result->alt_prefix = g_strdup("");
        }
        result->prefix = g_match_info_fetch(match_info, 2);
        result->area = g_match_info_fetch(match_info, 3);
        result->suffix = g_match_info_fetch(match_info, 4);
        result->alt_area = g_match_info_fetch(match_info, 5);
        if (result->alt_area == NULL) {
            result->alt_area = g_strdup("");
        }

        // for an invalid single letter prefix
        // shift the first digit of area to it
        // example: S 52 -> S5 2
        // valid single letter prefixes: B F G I K M N R W
        if (strlen(result->prefix) == 1
                && strchr("BFGIKMNRW", result->prefix[0]) == NULL
                && strlen(result->area) >= 2) {
            char *p = g_strdup_printf("%s%c", result->prefix, result->area[0]);
            char *q = g_strdup(result->area + 1);
            g_free(result->prefix); result->prefix = p;
            g_free(result->area); result->area = q;
        }

    }

    g_match_info_free(match_info);

    return result;
}

void get_partial_callsign_alt(char *call1, char *call2, char *partial) {
    if (strcmp(call1, call2) == 0) {
        strcpy(partial, "");            // calls are equal, nothing to send
        return;
    }
#if 0
    if (strlen(call2) <= 3) {           // call2 too short?
        strcpy(partial, call2);         // send it as-is
        return;
    }
#endif
    strcpy(partial, call2);     // default: send as-is

    call_parts_t *cp1 = split_call(call1);
    if (cp1 == NULL) {
        return;                 // can't split call1
    }
    call_parts_t *cp2 = split_call(call2);
    if (cp2 == NULL) {
        free_call_parts(cp1);
        return;                 // can't split call2
    }

#define ALT_PREFIX  1
#define PREFIX      2
#define AREA        4
#define SUFFIX      8
#define ALT_AREA    16

    int change = (strcmp(cp1->alt_prefix, cp2->alt_prefix) ? ALT_PREFIX : 0);
    change += (strcmp(cp1->prefix, cp2->prefix) ? PREFIX : 0);
    change += (strcmp(cp1->area, cp2->area) ? AREA : 0);
    change += (strcmp(cp1->suffix, cp2->suffix) ? SUFFIX : 0);
    change += (strcmp(cp1->alt_area, cp2->alt_area) ? ALT_AREA : 0);

#if 0
printf("1:|%s|%s|%s|%s|%s|\n", alt_prefix1, prefix1, area1, suffix1, alt_area1);
printf("2:|%s|%s|%s|%s|%s|\n", alt_prefix2, prefix2, area2, cp2->suffix, alt_area2);
printf("c=%d\n", change);
#endif

    // handle removal of optional parts
    if (change == ALT_AREA && strlen(cp2->alt_area) == 0) {
        change = SUFFIX;    // treat as suffix change
    }
    if (change == ALT_PREFIX && strlen(cp2->alt_prefix) == 0) {
        change = PREFIX;    // treat as prefix change
    }

    switch (change) {
        case ALT_PREFIX:
            strcpy(partial, cp2->alt_prefix);
#if 0
            if (strlen(partial) >= 3) {
                partial[strlen(partial) - 1] = 0;   // suppress '/'
            }
#endif
            break;

        case PREFIX:
            strcpy(partial, cp2->prefix);
            if (strlen(partial) == 1) { // if too short,
                partial[1] = cp2->area[0];  // add first digit of area
                partial[2] = 0;
            }
            break;

        case AREA:
            // prepend prefix if area is too short
            strcpy(partial, (strlen(cp2->area) == 1 ? cp2->prefix : ""));
            strcat(partial, cp2->area);
            break;

        case SUFFIX:
            // prepend last digit of area if suffix is too short
            if (strlen(cp2->suffix) == 1) {
                partial[0] = cp2->area[strlen(cp2->area) - 1];
                partial[1] = 0;
            } else {
                partial[0] = 0;
            }
            strcat(partial, cp2->suffix);
            break;

        case ALT_AREA:
            strcpy(partial, cp2->alt_area);
            break;

        case ALT_PREFIX + PREFIX:
            sprintf(partial, "%s%s", cp2->alt_prefix, cp2->prefix);
            break;

        case PREFIX + AREA:
            sprintf(partial, "%s%s", cp2->prefix, cp2->area);
            break;

        case AREA + SUFFIX:
            sprintf(partial, "%s%s", cp2->area, cp2->suffix);
            break;

        case SUFFIX + ALT_AREA:
            sprintf(partial, "%s%s", cp2->suffix, cp2->alt_area);
            break;

        default:        // for any other combination send as-is
            ;
    }

    free_call_parts(cp1);
    free_call_parts(cp2);
}

/* \brief get a substring from corrected call to repeat it
 *
 * \returns a substring based on the sent and corrected callsign
 */
void get_partial_callsign(char *call1, char *call2, char *partial) {

    size_t len1 = strlen(call1), len2 = strlen(call2);
    unsigned int len = (len1 < len2) ? len1 : len2;
    unsigned int i;
    unsigned int plen, plen1, plen2;
    int min = -1, max = -1;
    char tpartial[20];


    tpartial[0] = '\0';

    char *pfx1 = get_wpx_pfx(call1);
    char *pfx2 = get_wpx_pfx(call2);

    plen1 = strlen(pfx1);
    plen2 = strlen(pfx2);

    plen = (plen1 > plen2) ? plen1 : plen2;

    for (i = 0; i < len; i++) {
	if (call1[i] != call2[i]) {
	    if (min < 0) {
		min = i;
		max = i;
	    }
	    if (max < i) {
		max = i;
	    }
	}
    }

    // if all existing chars are the same
    // AB1CD / AB1CDE -> CDE
    // AB1CDE / AB1CD -> 1CD
    if (min == -1 && max == -1) {
	if (len2 < len1) {  // if the new call is shorter
	    plen--;         // include the last char of suffix
	}
	strncpy(tpartial, call2 + plen, len2 - plen + 1); // the full suffix
	tpartial[len2 - plen + 1] = '\0';
    } else {
	// if there is only 1 diff, and it's at the end
	// AB1CD / AB1CE -> CE
	if (min == max && max == len2 - 1) {
	    min--; // add the previous char too
	}
	if (len1 == len2) {
	    // if the mismatch is in the prefix
	    // AB1CD / AB2CD -> AB2
	    if (max <= plen - 1) {
		strncpy(tpartial, call2, plen);
		tpartial[plen] = '\0';
	    } else {
		strncpy(tpartial, call2 + min, len2 - min + 1);
		tpartial[len2 - min + 1] = '\0';
	    }
	} else {
	    strncpy(tpartial, call2 + min, len2 - min + 1);
	    tpartial[len2 - min + 1] = '\0';
	}
    }

    strcpy(partial, tpartial);
    free(pfx1);
    free(pfx2);

    return;
}
