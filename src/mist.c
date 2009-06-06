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

/**
 * @defgroup mist Functions for MIST reports
 * The module contains functions for processing of so-called MIST 
 * reports as developed at University of Mannheim.
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */
#include "config.h"
#include "common.h"
#include "mist.h"
#include "util.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Preprocesses a MIST report
 * @param report Report as string
 * @return preprocessed report
 */
char *mist_preproc(char *report)
{
    long level, rlen, tlen;

    /* Get MIST configuration */
    config_lookup_int(&cfg, "input.mist_level", (long *) &level);  
    config_lookup_int(&cfg, "input.mist_report_len", (long *) &rlen);
    config_lookup_int(&cfg, "input.mist_thread_len", (long *) &tlen);

    report = mist_trunc_report(report, rlen);
    report = mist_trunc_thread(report, tlen);
    report = mist_trunc_level(report, level);

    return report;
}


char *mist_trunc_report(char *report, int len)
{
    long j, i, k = 0;
    
    if (len <= 0)
        return report;

    for (i = j = 0; i < strlen(report); i++) {
        /* Check for new line */
        if (report[i] != MIST_NEWLINE)
            continue;
        /* Count instructions */
        if (report[j] == MIST_INSTRUCT)
            k++;
        /* Stop after len instructions */
        if (k >= len) 
            break;
            
        j = i + 1;    
    }
    
    /* Terminate string */
    report[i] = '\0';

    return report;
}

char *mist_trunc_thread(char *report, int len)
{
    long j, i, k = 0, l = 0, n = strlen(report);
    
    if (len <= 0)
        return report;

    for (i = j = l = 0; i < n; i++, j++) {
        /* Check for new line */
        if (report[i] == MIST_NEWLINE) {
            /* Count instructions */
            if (report[l] == MIST_INSTRUCT)
                k++;
            if (report[l] == MIST_COMMENT)
                k = 0;
                
            if (k >= len) {
                /* Skip over remaining instructions */
                while (i < n && report[++i] != MIST_COMMENT);
                if (i == n)
                    break;
                i--;
                k = 0;
            }
                
            l = i + 1;
        }   
        
         report[j] = report[i]; 
    }
    
    /* Terminate string */
    report[j] = '\0';

    return report;
}


/**
 * Truncates a report to a given MIST level and removes comments. 
 * The truncated report is likely smaller than the original report. 
 * Hence, the caller may issue a realloc() to free memory. 
 * @param report Report as string
 * @param level MIST level 
 * @return truncated report
 */
char *mist_trunc_level(char *report, int level)
{
    int l, i, j, len = strlen(report);
    
    /* Skip if level 0 */
    if (level <= 0)
        return report;
    
    /* Loop over file */
    for (i = j = l = 0; i < len; i++, j++) {
        /* Determine current level */
        if (report[i] == MIST_LEVEL) {
            l = (l + 1) % level;
            if (l == 0) {
                /* Skip over remaining levels */
                while (i < len && report[++i] != MIST_NEWLINE);
                if (i == len)
                    break;
            }    
        }

        /* Skip comments */
        if (report[i] == MIST_COMMENT) {
            while (i < len && report[++i] != MIST_NEWLINE);
            if (i == len)
                break;
        }    

        /* Check for new line */
        if (report[i] == MIST_NEWLINE) 
            l = 0;

        /* Copy contents */
        report[j] = report[i];
    }


    /* Terminate string */
    report[j] = '\0';
    return report;
}

/** @} */
