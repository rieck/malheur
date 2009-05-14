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
 * @param pivot Pivot section with level + 1
 * @return report as string
 */
char *mist_load_report(char *name, int level, int pivot)
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
    report = mist_trunc_level(report, level, pivot);
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
 * @param picot Pivot section with level + 1
 * @return truncated report
 */
char *mist_trunc_level(char *report, int level, int pivot)
{
    int l, i, j, k, len = strlen(report);
    for (i = j = l = 0, k = -1; i < len; i++, j++) {
    
        if (report[i] == MIST_LEVEL) {
            if (k == pivot)
               l = (l + 1) % (level + 1);            
            else 
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
        
        if (report[i] == MIST_DELIM) {
            l = 0;
            k = -1;
        }    

        /* 
         * Determine pivot section (level == 0) and no section has 
         * been detected so far (k == -1)
         */
        if (l == 0 && k == -1) {
            for(k = i; k + 1 < len && !isdigit(report[k]); k++);
            
            /* Dirty hack to efficient parse hexadecimal number */
            if (k + 1 < len) {
                if (isdigit(report[k + 1]))
                    k = (report[k] - '0') * 16 + report[k + 1] - '0';
                else    
                    k = (report[k] - '0') * 16 + report[k + 1] - 'a' + 10;           
            }        
        }

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
