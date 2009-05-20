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
 * Loads a textual file into a string. The string is allocated 
 * and need to be free'd later by the caller.
 * @param name file name
 * @return string 
 */
char *fio_load_file(char *name) 
{
    assert(name);
    long len, size = 0;
    char *str = NULL;
    struct stat st;    

    /* Open file */
    FILE *fptr = fopen(name, "r");
    if (!fptr) {
        error("Could not open file '%s'", name);
        return NULL;
    }

    /* Allocate memory */
    fstat(fileno(fptr), &st);
    size = st.st_size;
    str = malloc(sizeof(char) * (size + 1));
    if (!str) {
        error("Could not allocate memory");
        return NULL;
    }

    /* Read data */
    len = fread(str, sizeof(char), size, fptr);
    
    if (len != size) 
        warning("Could not read all data from file '%s'", name);
    
    str[len] = '\0';
    return str;
}
