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

static int mist_read_line(char **ptr, char *buffer)
{
    /* Read line */
    int r = sscanf(*ptr, "%1023[^\n]", buffer); 
                
    /* Empty line */
    if (r < 1) 
        buffer[0] = 0;

    /* Move line */
    *ptr += strlen(buffer) + 1;

    return (r != -1);
}

static char *mist_copy_instr(char *ptr, char *line, int level)
{
    int i, l = 0;
    for (i = 0; i < strlen(line); i++) {
        if (line[i] == MIST_LEVEL)
            l++;
        if (l >= level)
            break;
    
        ptr[i] = line[i];
    }

    /* Add carriage return */    
    ptr[i] = '\n';
        
    /* Update pointer */
    return ptr + i + 1;
}

/**
 * Preprocesses a MIST report
 * @param report Report as string
 * @return preprocessed report
 */
char *mist_preproc(char *report)
{
    long level, rlen, tlen, ti = 0, ri = 0;
    char *read_ptr = report, *write_ptr = report, line[1024];
    
    /* Get MIST configuration */
    config_lookup_int(&cfg, "input.mist_level", (long *) &level);  
    config_lookup_int(&cfg, "input.mist_report_len", (long *) &rlen);
    config_lookup_int(&cfg, "input.mist_thread_len", (long *) &tlen);

    /* Process MIST file */
    while(mist_read_line(&read_ptr, line)) {   
        switch(line[0]) {
        
        /* Instruction in MIST format */
        case MIST_INSTRUCT:
            if (ti < tlen) {
                write_ptr = mist_copy_instr(write_ptr, line, level);
                ri++; ti++;
            }
            break;
            
        /* Comment in MIST format */
        case MIST_COMMENT:
            /* Reset threat counter on new thread */
            if (strstr(line, MIST_THREAD)) 
                ti = 0;
            break;
        }
        
        if (ri >= rlen)
            break;
    } 
    
    /* Terminate string */
    *write_ptr = 0;                
    return report;
}


/** @} */
