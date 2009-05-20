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

fvec_t *mist_extract(...) 
{
}

/**
 * Truncates a report to a given MIST level and remove comments. 
 * The truncated report is likely smaller than the original report. 
 * Hence, the caller may issue a realloc() to free memory. 
 * @param report Report as string
 * @param level MIST level 
 * @return truncated report
 */
char *mist_trunc_level(char *report, int level)
{
    int l, i, j, len = strlen(report);
    for (i = j = l = 0; i < len; i++, j++) {
    
        /* Determine current level */
        if (report[i] == MIST_LEVEL) {
            l = (l + 1) % level;
            if (l == 0) {
                /* Skip over remaining levels */
                while (i < len && report[++i] != MIST_DELIM);
                if (i == len)
                    break;
            }    
        }

        /* Skip comments */
        if (report[i] == MIST_COMMENT) {
            while (i < len && report[++i] != MIST_DELIM);
            if (i == len)
                break;
        }    

        /* Check for new line */
        if (report[i] == MIST_DELIM) 
            l = 0;

        /* Copy contents */
        report[j] = report[i];
    }


    /* Terminate string */
    report[j] = '\0';
    return report;
}


/**
 * Truncates a report to a given MIST level using an enhanced section. 
 * This function extracts a MIST level but extends a specified MIST
 * section to (level + 1).
 * @param report Report as string
 * @param level MIST level 
 * @param sect Enhanced section with level + 1
 * @return truncated report
 */
char *mist_trunc_level2(char *report, int level, int sect)
{
    int l, i, j, k, len = strlen(report);
    for (i = j = l = 0, k = -1; i < len; i++, j++) {
    
        if (report[i] == MIST_LEVEL) {
            if (k == sect)
               l = (l + 1) % (level + 1);            
            else 
               l = (l + 1) % level;
      
            if (l == 0) {
                /* Skip over remaining levels */            
                while (i < len && report[++i] != MIST_DELIM);
                if (i == len)
                    break;
            }    
        }
            
        /* Skip comments */            
        if (report[i] == MIST_COMMENT) {
            while (i < len && report[++i] != MIST_DELIM);
            if (i == len)
                break;
        }    
        
        /* Check for new line */        
        if (report[i] == MIST_DELIM) {
            l = 0;
            k = -1;
        }    

        /* 
         * Determine enhanced section (level == 0) and no section has 
         * been detected so far (k == -1)
         */
        if (l == 0 && k == -1) {
            for(k = i; k + 1 < len && !isdigit(report[k]); k++);
            
            /* Dirty hack to efficiently parse hexadecimal number */
            if (k + 1 < len) {
                if (isdigit(report[k + 1]))
                    k = (report[k] - '0') * 16 + report[k + 1] - '0';
                else    
                    k = (report[k] - '0') * 16 + report[k + 1] - 'a' + 10;           
            }        
        }

        /* Copy contents */
        report[j] = report[i];
    }


    /* Terminate string */
    report[j] = '\0';
    return report;
}
