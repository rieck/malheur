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
#include "fvec.h"
#include "fio.h"
#include "util.h"

/* External variables */
extern int verbose;
extern config_t cfg;


/**
 * Loads a report from a file and extracts features
 * @param name report file name 
 * @param extract extraction function
 * @return feature vector
 */
fvec_t *fio_load_report(char *name, fvec_extract_t extract)
{
    assert(name && extract);

    /* Open file */
    FILE *fptr = fopen(name, "r");
    if (!fptr) {
        error("Could not open report '%s'", name);
        return NULL;
    }

    /* Get length of report */
    fseek(fptr, 0, SEEK_END);
    long len = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    /* Allocate and load report data */
    char *report = malloc(sizeof(char) * (len + 1));
    if (!report) {
        error("Could not allocate %ld bytes for report '%s'", len,
              name);
        return NULL;
    }
    if (fread(report, sizeof(char), len, fptr) != len) {
        error("Could not load report '%s'", name);
        free(report);
        return NULL;
    }
    fclose(fptr);

    fvec_t *f = extract(report, strlen(report));
    free(report);

    return f;
}
