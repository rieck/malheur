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
#include "mist.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Loads a textual file into a string. The string is allocated 
 * and need to be free'd later by the caller.
 * @param path Path to file or empty
 * @param name file name
 * @return string 
 */
char *fio_load_file(char *path, char *name) 
{
    assert(name);
    long len, size = 0;
    char *str = NULL, file[1024];
    struct stat st;    
    

    /* Open file */
    snprintf(file, 1024, "%s/%s", path, name);
    FILE *fptr = fopen(file, "r");
    if (!fptr) {
        error("Could not open file '%s'", file);
        return NULL;
    }

    /* Allocate memory */
    stat(file, &st);
    size = st.st_size;
    str = malloc(sizeof(char) * (size + 1));
    if (!str) {
        error("Could not allocate memory");
        return NULL;
    }

    /* Read data */
    len = fread(str, sizeof(char), size, fptr);
    fclose(fptr);
    
    if (len != size) 
        warning("Could not read all data from file '%s'", file);
    
    str[len] = '\0';
    return str;
}

/**
 * Returns the number of file entries in a directory. Symlinks are 
 * not considered as regular files (otherwise dereferncing would be 
 * necessary).
 * @param dir directory containing files
 * @return entries with type "regular file"
 */
long fio_count_files(char *dir)
{
    long e = 0;
    struct dirent *dp;
    DIR *d;

    /* Open directory */
    d = opendir(dir);
    if (!d) {
        error("Could not open directory '%s'", dir);
        return 0;
    }

    while ((dp = readdir(d)) != NULL)
        if (dp->d_type == DT_REG)
            e++;
            
    closedir(d);
    return e;          
}

/**
 * Preprocess input data. The function takes a string as argument and 
 * preprocesses it according to the given configuration. 
 * @param x Raw string
 * @return Preprocessed output.
 */
char *fio_preproc(char *x) 
{
    const char *fm_str;
    int level;

    config_lookup_string(&cfg, "input.format", &fm_str);
    
    /* MIST transformation */
    if (!strcasecmp(fm_str, "mist")) {
        config_lookup_int(&cfg, "input.mist_level", (long *) &level);
        x = mist_trunc_level(x, level);
    }    

    return x;
}
 
