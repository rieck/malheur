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

/* Size of read buffer */
#define BUFFER_SIZE     1024

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Copy a line into the given buffer and advance pointer
 * @param ptr Pointer to data
 * @param buffer Buffer to write data to
 * @return true on success, false otherwise 
 */
static int mist_read_line(char **ptr, char *buffer)
{
    int i = 0;

    /* Copy line */
    for (i = 0; i < BUFFER_SIZE; i++) {
        if ((*ptr)[i] == 0)
            return FALSE;
        if ((*ptr)[i] == '\n')
            break;
        buffer[i] = (*ptr)[i];
    }
    buffer[i] = 0;

    /* Move line */
    *ptr += strlen(buffer) + 1;
    return TRUE;
}

/**
 * Copy a MIST instruction to the given pointer
 * @param ptr pointer to destrination
 * @param line pointer to source (line of instruction)
 * @param level MIST level to keep
 * @return destination for next instruction
 */
static char *mist_copy_instr(char *ptr, char *line, int level)
{
    int i, l = 0, m = strlen(line);
    for (i = 0; i < m; i++) {
        if (line[i] == MIST_LEVEL)
            l++;
        if (l >= level)
            break;

        ptr[i] = line[i];
    }

    /* Add a carriage return */
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
    assert(report);

    long level, rlen, tlen, ti = 0, ri = 0;
    char *read_ptr = report, *write_ptr = report;
    char line[BUFFER_SIZE];

    /* Get MIST configuration */
    config_lookup_int(&cfg, "input.mist_level", (long *) &level);
    config_lookup_int(&cfg, "input.mist_rlen", (long *) &rlen);
    config_lookup_int(&cfg, "input.mist_tlen", (long *) &tlen);

    /* Process MIST file */
    while (mist_read_line(&read_ptr, line)) {
        switch (line[0]) {

            /* Instruction in MIST format */
        case MIST_INSTRUCT:
            if (tlen == 0 || ti < tlen) {
                write_ptr = mist_copy_instr(write_ptr, line, level);
                ri++;
                ti++;
            }
            break;

            /* Comment in MIST format */
        case MIST_COMMENT:
            /* Reset threat counter on new thread */
            if (strstr(line, MIST_THREAD))
                ti = 0;
            break;
        }

        if (rlen > 0 && ri >= rlen)
            break;
    }

    /* Terminate string */
    *write_ptr = 0;
    return report;
}


/** @} */
