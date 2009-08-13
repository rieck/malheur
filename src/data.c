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
 * @defgroup data Data preprocessing and I/O functions
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
#include "farray.h"
#include "fmath.h"
#include "data.h"
#include "util.h"
#include "mist.h"
#include "mconfig.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Loads a textual file into a string. The string is allocated 
 * and need to be free'd later by the caller.
 * @param path Path to file
 * @param name file name or NULL
 * @return string 
 */
char *data_load_file(char *path, char *name) 
{
    assert(path);
    long len, size = 0;
    char *str = NULL, file[1024];
    struct stat st;    
     
    #pragma omp critical (snprintf) 
    {
        if (name)
            snprintf(file, 1024, "%s/%s", path, name);
        else
            snprintf(file, 1024, "%s", path);
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
    
    str[size] = '\0';
    return str;
}

/**
 * Returns the number of entries in a directory. 
 * @param dir Directory to analyse
 * @param fnum Return pointer for number of regular files
 * @param total Return pointer for number of total files
 */
void data_dir_entries(char *dir, int *fnum, int *total)
{
    struct dirent *dp;
    DIR *d;

    *fnum = 0;
    *total = 0;

    d = opendir(dir);
    while (d && (dp = readdir(d)) != NULL) {
        if (dp->d_type == DT_REG)
            ++*fnum;
        ++*total;           
    }            
    closedir(d);
}

/**
 * Returns the number of file entries in an archive.
 * @param arc archive containing files
 * @param fnum Return pointer for number of regular files
 * @param total Return pointer for number of total files
 */
void data_archive_entries(char *arc, int *fnum, int *total) 
{
    struct archive *a;
    struct archive_entry *entry;
    assert(arc);
    
    *fnum = 0;
    *total = 0;
    
    /* Open archive */
    a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    archive_read_open_filename(a, arc, 4096);
    
    /* Jump through archive */
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const struct stat *s = archive_entry_stat(entry);
        if (S_ISREG(s->st_mode))      
            ++*fnum;
        ++*total;    
        archive_read_data_skip(a);
    }
    archive_read_finish(a);
}  

/**
 * Preprocess input format according to configuration. The function takes 
 * a raw string and formats it according to the given configuration. 
 * @param x Raw string
 * @return Preprocessed output.
 */
char *data_preproc(char *x) 
{
    const char *fm_str;

    config_lookup_string(&cfg, "input.format", &fm_str);
    
    /* MIST transformation */
    if (!strcasecmp(fm_str, "mist")) {
        x = mist_preproc(x);
    }    

    return x;
}

/**
 * Exports a kernel matrix to a compressed file
 * @param d Pointer to matrix
 * @param f Feature vector array
 * @param n File name 
 */
void data_export_kernel(double *d, farray_t *f, char *file)
{
    assert(d && f && file);
    int i,j;

    if (verbose > 0)
        printf("Exporting kernel matrix to '%s'.\n", file);

    gzFile *z = gzopen(file, "w");
    if (!z) {
        error("Could not create file '%s'.", file);
        return;
    }    
    
    for (i = 0; i < f->len; i++) {
        gzprintf(z, "%s:", f->x[i]->src);
        for (j = 0; j < f->len; j++)
            gzprintf(z, " %g", d[i * f->len + j]);
        gzprintf(z, "\n");
    }
    
    gzclose(z);
}

static char *data_cwsandbox_url(char *f)
{
    static char buf[1024];
    char *ptr = f + strlen(f) - 1;
    int rid, aid;
    
    /* Determine basename */
    while(ptr != f && *(ptr - 1) != '/')
        ptr--;
        
    sscanf(ptr, "%d_%d.%*s\n", &rid, &aid);
    snprintf(buf, 1024, 
             "https://pcert.cwsandbox.org/?page=analysis&format=xml&analysisid=%d", 
             aid);
             
    return buf;
}

/**
 * Exports prototypes to a HTML file (Urgently needs support for templates)
 * @param p Prototype structure
 * @param f Feature vector array 
 * @param n File name
 */
void data_export_proto(proto_t *p, farray_t *fa, char *file)
{
    assert(p && fa && file);
    int i, j, n = 0, x = 0;
    FILE *f = fopen(file, "w");
    if (!f) {
        error("Could not open '%s' for writing", file);
        return;
    }
    
    /* Write generic */
    fprintf(f, "<html><body %s><h1>Prototypes</h1><table %s>\n", 
            CSS_FONT, CSS_FONT);
    fprintf(f, "<tr><td>Number of prototypes: </td><td>%lu</td></tr>\n", 
            p->protos->len);
    fprintf(f, "<tr><td>Number of total reports: </td><td>%lu</td></tr>\n", 
            p->alen);
    fprintf(f, "<tr><td>Compression ratio: </td><td>%4.1f%%</td></tr>\n", 
            (1.0 - p->protos->len / (double) fa->len) * 100);
    fprintf(f, "<tr><td>Source: </td><td>%s</td></tr>\n",
            p->protos->src);
    fprintf(f, "</table>\n");
    
    /* Write configuration */
    fprintf(f, "<h2>Configuration</h2><pre>");
    config_fprint(f, &cfg);
    fprintf(f, "</pre>");    
    
    /* Write prototypes and assignments */
    fprintf(f, "<h2>Assignments</h2><ul>");
    for (i = 0; i < p->protos->len; i++) {
        for (j = 0, n = 0; j < p->alen; j++) 
            n += ((p->assign[j] & PA_ASSIGN_MASK) == i);

        fprintf(f, "<li><a href='%s'><b>Prototype %d</b></a><br>\n", 
                data_cwsandbox_url(p->protos->x[i]->src), i);
               
        fprintf(f, "%d members: ", n);
        for (j = 0, x = 0; j < p->alen; j++) {
            if ((p->assign[j] & PA_ASSIGN_MASK) != i)
                continue;
                
            fprintf(f, "<a href='%s'>", data_cwsandbox_url(fa->x[j]->src));
            if (x && x == fa->y[j]) 
                fprintf(f, "&middot;");
            else
                fprintf(f, "%s", farray_get_label(fa, j));
            x = fa->y[j];
            fprintf(f, "</a> ");
        }
        fprintf(f, "<br><br>\n");
    }
    fprintf(f,"</ul>"); 
    fprintf(f, "</body></html>\n");
    fclose(f);
}

/** @} */
 
