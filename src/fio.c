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
 * @defgroup fio Preprocessing and I/O functions
 * The module contains a set of generic functions for reading and 
 * writing to files, as well as preprocessing routines converting 
 * between input formats.
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */

#include <archive.h>
#include <archive_entry.h>

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
     
    #pragma omp critical (snprintf) 
    {
        snprintf(file, 1024, "%s/%s", path, name);
    }

    /* Open file */
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
        error("Could not allocate memory for file data");
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
 * Returns the number of file entries in a directory. Symlinks,  
 * device nodes, directories and fifos are discarded. The function 
 * ignores errors and returns 0 if a directory is not accessible.
 * @param dir directory containing files
 * @return number of entries with type "regular file"
 */
long fio_count_files(char *dir)
{
    long e = 0;
    struct dirent *dp;
    DIR *d;

    d = opendir(dir);
    while (d && (dp = readdir(d)) != NULL)
        if (dp->d_type == DT_REG)
            e++;            
    closedir(d);
    
    return e;          
}

/**
 * Returns the number of entries in a directory.  The function ignores 
 * errors and returns 0 if a directory is not accessible.
 * @param dir directory containing files
 * @return number of all entries 
 */
long fio_count_entries(char *dir)
{
    long e = 0;
    struct dirent *dp;
    DIR *d;

    d = opendir(dir);
    while((dp = readdir(d)) != NULL)
        e++;        
    closedir(d);
    
    return e;          
}

/**
 * Returns the number of entries in an archive. 
 * @param arc archive containing files
 * @return number of entries
 */ 
long fio_count_archive(char *arc) 
{
    struct archive *a;
    struct archive_entry *entry;
    unsigned long n = 0;
    assert(arc);
    
    /* Open archive */
    a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    archive_read_open_filename(a, arc, 4096);
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        archive_read_data_skip(a);
        n++;
    }
    archive_read_finish(a);
    
    return n;
}    

/**
 * Preprocess input format according to configuration. The function takes 
 * a raw string and formats it according to the given configuration. 
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

/** @} */
 
