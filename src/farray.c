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
 * @defgroup farray Array of feature vectors
 * Generic array of feature vectors. This module contains functions for
 * maintenance of feature vectors in an array along with a set of 
 * textual labels, such as AV labels. Moreover, functionality is provided
 * for extraction of feature vectors from directories and compressed
 * archives of malware reports.
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */

#include <archive.h>
#include <archive_entry.h>

#include "config.h"
#include "common.h"
#include "farray.h"
#include "fvec.h"
#include "md5.h"
#include "util.h"

/* External variables */
extern int verbose;

/**
 * Adds a label to the table of labels.
 * @param a Feature array
 * @param name label name
 * @return index of label 
 */ 
static unsigned int label_add(farray_t *fa, char *name)
{
    label_t *entry;
    assert(fa && name);
    unsigned char buf[MD5_DIGEST_LENGTH];

    /* Check if label is known */
    HASH_FIND(hn, fa->label_name, name, strlen(name), entry);    
    if (entry) 
        return entry->index;

    /* Create new label */
    entry = malloc(sizeof(label_t));
    strncpy(entry->name, name, sizeof(entry->name)); 
    entry->name[sizeof(entry->name) - 1] = 0;
    
    MD5((unsigned char *) entry->name, strlen(entry->name), buf);
    memcpy(&entry->index, buf, sizeof(unsigned int));    
             
    /* Add label to both tables */
    HASH_ADD(hi, fa->label_index, index, sizeof(unsigned int), entry);
    HASH_ADD(hn, fa->label_name, name, strlen(entry->name), entry);     
                    
    /* Update memory */
    fa->mem += sizeof(label_t) + sizeof(name);

    /* Return new index */
    return entry->index;
}

/**
 * Creates and allocates an empty array of feature vectors
 * @param s Source of array, e.g. directory
 * @return empty array
 */
farray_t *farray_create(char *s)
{
    farray_t *fa = calloc(1, sizeof(farray_t));
    if (!fa) {
        error("Could not allocate array of feature vectors");
        return NULL;
    }

    /* Init elements of array */
    fa->len = 0;
    fa->mem = sizeof(farray_t);
    
    /* Set source */
    if (s) {
        fa->src = strdup(s);
        fa->mem += strlen(s);
    }    
    
    return fa;
} 


/**
 * Destroys an array of feature vectors
 * @param fa array of feature vectors
 */
void farray_destroy(farray_t *fa)
{
    if (!fa)
        return;
    
    /* Free feature vectors */
    if (fa->x) {
        for (int i = 0; i < fa->len; i++)
            fvec_destroy(fa->x[i]);
        free(fa->x);
    }

    if (fa->y)
        free(fa->y);
    if (fa->src)
        free(fa->src);
        
    /* Free lable table */
    while(fa->label_name) {
        label_t *current = fa->label_name;        
        HASH_DELETE(hn, fa->label_name, current);
        HASH_DELETE(hi, fa->label_index, current);
        free(current);           
    }    
       
    free(fa);
}

/**
 * Adds a feature vector to the array
 * @param fa Feature array
 * @param fv Feature vector 
 * @param label Label of feature vector 
 */
void farray_add(farray_t *fa, fvec_t *fv, char *label)
{
    assert(fa && fv && label);

    /* Expand size of array */
    if (fa->len % BLOCK_SIZE == 0) {
        int l = fa->len + BLOCK_SIZE;
        fa->x = realloc(fa->x, l * sizeof(fvec_t *));
        fa->y = realloc(fa->y, l * sizeof(int));
        fa->mem += BLOCK_SIZE * (sizeof(fvec_t *) + sizeof(int));
        if (!fa->x || !fa->y) {
            error("Could not re-size feature array");
            farray_destroy(fa);
            return;
        }
    }
    
    /* Update table */
    fa->x[fa->len] = fv;
    fa->y[fa->len] = label_add(fa, label);
    fa->len++;    
    fa->mem += fv->mem;
}

/**
 * Extracts an array of feature vectors from an archive or directory.
 * @param arc archive containing files.
 * @return array of feature vectors
 */
farray_t *farray_extract(char *path)
{
    struct stat st; 
    assert(path);
    farray_t *fa = NULL;
        
    if (stat(path, &st)) {
        error("Could not access file '%s'", path);
        return NULL;
    }    

    if (verbose > 0)
        printf("Extracting features from '%s'.\n", path);
    
    if (S_ISREG(st.st_mode)) 
        fa = farray_extract_archive(path);
    else if (S_ISDIR(st.st_mode))
        fa = farray_extract_dir(path);
    else
        error("Unsupported file type of input '%s'", path);
    
    if (verbose > 0)
        printf("  Done. %ld feature vectors using %.2fMb extracted.\n", 
               fa->len, fa->mem / 1e6);
    
    return fa;
}

/**
 * Extracts an array of feature vectors from an archive. The function 
 * loads and converts files from the given archive. It does not process
 * subdirectories recursively.
 * @param arc archive containing files.
 * @return array of feature vectors
 */
farray_t *farray_extract_archive(char *arc)
{
    struct archive *a;
    struct archive_entry *entry;
    int i, fnum, total;
    char *x, *l;
    assert(arc);
    
    /* Allocate empty array */
    farray_t *fa = farray_create(arc);
    if (!fa) 
        return NULL;
        
    list_arc_entries(arc, &fnum, &total);

    /* Open archive */
    a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    archive_read_open_filename(a, arc, 4096);

    /* Read contents */
    #pragma omp parallel for shared(a) private(x,l)
    for (i = 0; i < total; i++) {
    
        #pragma omp critical (farray)
        {
            /* Perform reading of archive in critical region */
            archive_read_next_header(a, &entry);
            const struct stat *s = archive_entry_stat(entry);
            if (!S_ISREG(s->st_mode)) {
                x = NULL;
                archive_read_data_skip(a);
                l = NULL;
            } else {
                x = malloc((s->st_size + 1) * sizeof(char));            
                archive_read_data(a, x, s->st_size);
                l = strdup((char *) archive_entry_pathname(entry));
                x[s->st_size] = 0;
            }
        }    
        
        /* Skip non-regular files */
        if (!x && !l)
            continue;

        /* Preprocess and extract feature vector*/
        x = fvec_preproc(x);
        fvec_t *fv = fvec_extract(x, strlen(x), l);
        if (fv->len == 0) {
            warning("Discarding empty feature vector for '%s'", l);
            fvec_destroy(fv);
            fnum--;
            continue;
        }
        
        #pragma omp critical (farray)
        {
            /* Add feature vector to array */
            farray_add(fa, fv, file_suffix(l));            
            if (verbose > 0)
                prog_bar(0, fnum, fa->len);        
        }    
            
        free(x);
        free(l);            
    }

    /* Close archive */
    archive_read_finish(a);
    return fa;
}

/**
 * Extracts an array of feature vectors from a directory. The function 
 * loads and converts files from the given directory. It does not process
 * subdirectories recursively.
 * @param dir directory containing file.
 * @return array of feature vectors
 */
farray_t *farray_extract_dir(char *dir)
{
    int i, fnum, total, maxlen;
    assert(dir);

    /* Allocate empty array */
    farray_t *fa = farray_create(dir);
    if (!fa) 
        return NULL;
    
    /* Open directory */    
    DIR *d = opendir(dir);
    if (!d) {
        farray_destroy(fa);
        error("Could not open directory '%s'", dir);
        return NULL;
    }
    
    
    /*
     * Prepare concurrent readdir_r(). There is a race condition in the 
     * following code. The maximum  length 'maxlen' could have changed 
     * between the previous call to opendir() and the following call to
     * pathconf(). I'll take care of this at a later time.
     */
    list_dir_entries(dir, &fnum, &total);
    maxlen = pathconf(dir, _PC_NAME_MAX);

    /* Loop over directory entries */
    #pragma omp parallel for shared(d,fa)
    for (i = 0; i < total; i++) {        
        
        /* Read directory entry to local buffer */
        struct dirent *buf, *dp;
        buf = malloc(offsetof(struct dirent, d_name) + maxlen + 1);                
        readdir_r(d, buf, &dp);

        /* Skip non-regular entries */
        if (dp->d_type != DT_REG) {
            free(buf);
            continue;
        }    
    
        /* Extract feature vector from file */
        char *raw = load_file(dir, dp->d_name);
        raw = fvec_preproc(raw);
        fvec_t *fv = fvec_extract(raw, strlen(raw), dp->d_name);
        if (fv->len == 0) {
            warning("Discarding empty feature vector for '%s'", dp->d_name);
            fvec_destroy(fv);
            fnum--;
            continue;
        }

        #pragma omp critical (farray)
        {        
            /* Add feature vector to array */        
            farray_add(fa, fv, file_suffix(dp->d_name));
            if (verbose > 0)
                prog_bar(0, fnum, fa->len);
        }
        
        /* Clean string and  directory buffer */
        free(raw);
        free(buf);
    }   

    closedir(d);
    return fa;
}

/**
 * Prints a feature array
 * @param fa feature array
 */
void farray_print(farray_t *fa)
{
    assert(fa);
    int i;
    label_t *entry;

    printf("feature array\n  len: %lu, labels: %d, mem: %.2fMb\n", 
           fa->len, HASH_CNT(hn, fa->label_name), fa->mem / 1e6);
           
    if (fa->src)
        printf("  src: '%s'\n", fa->src);
           
    if (verbose < 2)
        return;
    
    for (i = 0; i < fa->len; i++) {
        HASH_FIND(hi, fa->label_index, &fa->y[i], sizeof(int), entry);        
        fvec_print(fa->x[i]);
        printf("  label: %s, index: %u\n", entry->name, fa->y[i]);                
    }   
}

/**
 * Merges two arrays into one. The second array is destroy and all 
 * its memory is free'd.
 * @param x First array of feature vectors
 * @param y Second array of feature vectors
 * @return array of feature vectors
 */
farray_t *farray_merge(farray_t *x, farray_t *y) 
{
    int i;
    
    /* Check for arguments */
    if (!x && y)
        return y;
    if (!y && x)
        return x;

    /* Add to old array */
    for (i = 0; i < y->len; i++) { 
        farray_add(x, y->x[i], farray_get_label(y, i));
        y->x[i] = NULL;
    }

    /* Clean up */
    farray_destroy(y); 
    return x;
}

/**
 * Saves an array of feature vectors to a file 
 * @param fa Array of feature vectors
 * @param z Stream pointer
 */
void farray_save(farray_t *fa, gzFile *z)
{
    assert(fa && z);
    int i;

    gzprintf(z, "feature array: len=%lu, labels=%d, mem=%lu, src=%s\n", 
            fa->len, HASH_CNT(hn, fa->label_name), fa->mem, fa->src);
            
    for (i = 0; i < fa->len; i++) {
        fvec_save(fa->x[i], z);
        gzprintf(z, "  label=%s\n", farray_get_label(fa, i));
    }    
}

/**
 * Returns the textual label for the i-th feature vector
 * @param fa Array of feature vectors
 * @param i index
 * @return Textual label
 */
char *farray_get_label(farray_t *fa, int i)
{
    assert(fa);
    label_t *entry;

    HASH_FIND(hi, fa->label_index, &fa->y[i], sizeof(int), entry);   
    return entry->name;
} 

/**
 * Loads an array of feature vector form a file stream
 * @param z Stream point
 * @return  Array of feature vectors
*/
farray_t *farray_load(gzFile *z)
{
    assert(z);
    char buf[512], str[512];
    long len, mem;
    int lab, r, i;

    /* Allocate feature array */
    farray_t *f = farray_create(NULL);
    if (!f) 
        return NULL;

    gzgets(z, buf, 512);
    r = sscanf(buf, "feature array: len=%lu, labels=%d, mem=%lu, src=%s\n", 
              (unsigned long *) &len, (int *) &lab, 
              (unsigned long *) &mem, str);              
    if (r != 4)  {
        error("Could not parse feature array");
        farray_destroy(f);
        return NULL;
    }
       
    /* Set source */
    if (strcmp(str, "(null)"))  {
        f->src = strdup(str);
        f->mem += strlen(str);
    }    
    
    /* Load contents */
    for (i = 0; i < len; i++) {
        /* Load feature vector */
        fvec_t *fv = fvec_load(z);
        
        /* Load labels */
        gzgets(z, buf, 512);
        r = sscanf(buf, "  label=%s\n", str);
        if (r != 1) {
            error("Could not parse feature vector contents");
            farray_destroy(f);
            return NULL;
        }
        
        /* Add to array */
        farray_add(f, fv, str); 
    }           
    return f;
}

/** }@ */

 
