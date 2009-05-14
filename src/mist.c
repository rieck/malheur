/*
 * MALHEUR - Automatic Malware Analysis on Steroids
 * Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
 * Berlin Institute of Technology (TU Berlin).
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 * --
 */

#include "config.h"
#include "common.h"
#include "mist.h"
#include "util.h"

/* External variables */
extern int verbose;

/**
 * Loads a report from a file and extracts the specified MIST level
 * @param name report file name 
 * @param level Extraction level
 * @return report as string
 */
char *mist_load_report(char *name, int level)
{
    assert(name);
    if (level < 1) 
        warning("Level too small. Increasing to 1.");

    /* Open file */
    FILE *fptr = fopen(name, "r");
    if (!fptr) {
        error("Could not open MIST report '%s'", name);
        return NULL;
    }

    /* Get length of report */
    fseek(fptr, 0, SEEK_END);
    long len = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    /* Allocate and load report data */
    char *report = malloc(sizeof(char) * (len + 1));
    if (!report) {
        error("Could not allocate %ld bytes for MIST report '%s'", len,
              name);
        return NULL;
    }
    if (fread(report, sizeof(char), len, fptr) != len) {
        error("Could not load MIST report '%s'", name);
        free(report);
        return NULL;
    }
    fclose(fptr);

    /* Truncate MIST level and remove comments */
    report = mist_trunc_level(report, level);
    report = realloc(report, strlen(report) + 1);
    if (!report) {
        error("Could not re-allocate MIST report");
        return NULL;
    }
    
    return report;
}

/**
 * Truncates a report to a given MIST level and remove comments
 * @param report Report as string
 * @param level MIST level 
 * @return truncated report
 */
char *mist_trunc_level(char *report, int level)
{
    int l, i, j, len = strlen(report);
    for (i = 0, j = 0, l = 0; i < len; i++, j++) {
        if (report[i] == MIST_LEVEL) {
            l = (l + 1) % level;
            if (l == 0) {
                while (i < len && report[++i] != MIST_DELIM);
                if (i == len)
                    break;
            }    
        }
            
        if (report[i] == MIST_COMMENT) {
            while (i < len && report[++i] != MIST_DELIM);
            if (i == len)
                break;
        }    

        if (report[i] == MIST_DELIM)
            l = 0;

        if (report[i] == '\0') {
            warning("Report contains \\0 char, replacing with space.");
            report[i] = ' ';
            break;
        }

        report[j] = report[i];
    }

    /* Terminate string */
    report[j] = '\0';
    return report;
}
