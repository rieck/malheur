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
#include "farray.h"
#include "fvec.h"
#include "fio.h"
#include "util.h"

/* External variables */
extern int verbose;

/* Global variables */
static label_t *label_hash = NULL;
static int label_index = 0;

/**
 * Extracts the label from a file name and add its to hash table
 * of labels. The function also assigns a unique index to each label
 * that later acts as an index for it. If the file name does not have
 * a suffix, the label name is "unknown".
 * @param file File name to extract label from
 * @return Associated index of label
 */ 
static int label_hash_add(char *file)
{
    label_t *entry;
    char *name = file + strlen(file) - 1;

    /* Determine dot in file name */
    while (name != file && *name != '.') 
        name--;

    /* Check for files with no suffix */
    if (name == file)
        name = "unknown";
    else
        name++;

    /* Check if label is known */
    HASH_FIND(hh, label_hash, name, strlen(name), entry);    
    if (!entry) {
        /* Insert new label */
        entry = malloc(sizeof(label_t));
        entry->name = strdup(name); 
        entry->index = label_index++;
        HASH_ADD_KEYPTR(hh, label_hash, entry->name, 
                        strlen(entry->name), entry); 
    }
    
    return entry->index;
}

/**
 * Condenses the label hash table into an array of label names, where 
 * each label is located at its associated index. The array is terminated
 * using a NULL pointer.
 * @return array of label names
 */
static char **label_hash_condense()
{
    int n = HASH_COUNT(label_hash);
    assert(n == label_index);

    /* Allocate list of label names */
    char **labels = malloc((n + 1) * sizeof(char *));
    label_t *current_label;
    
    /* Loop over hash table */
    while(label_hash) {
        current_label = label_hash;        
        HASH_DEL(label_hash, current_label);
        labels[current_label->index] = current_label->name;
        free(current_label);           
    }
    
    /* Null terminated */
    labels[n] = NULL;
    return labels;
}

/**
 * Update the content of the array with a new file
 * @param fa Array of feature vectors
 * @param i Free index in array
 * @param dir Directory to read from
 * @param file File to read in 
 */
static void farray_update_file(farray_t *fa, int i, char *dir, char *file)
{
    /* Load file contents */
    char *x = fio_load_file(dir, file);
    
    /* Preprocess and create feature vector*/
    x = fio_preproc(x);
    fa->x[i] = fvec_create(x, strlen(x));
    free(x);

    /* Extract label from name */
    fa->y[i] = label_hash_add(file);
}

/**
 * Extracts an array of feature vectors from a directory. The function 
 * loads and converts files in the given directory. It does not process
 * subdirectories recursively.
 * @param dir directory containing file.
 */
farray_t *farray_create_dir(char *dir)
{
    DIR *d;
    long i = 0;
    struct dirent *dp;

    assert(dir);

    farray_t *fa = calloc(1, sizeof(farray_t));
    if (!fa) {
        error("Could not allocate array of feature vectors");
        return NULL;
    }

    /* Allocate elements of array */
    fa->len = fio_count_files(dir);    
    fa->x = malloc(fa->len * sizeof(fvec_t *));
    fa->y = malloc(fa->len * sizeof(int));
    fa->mem = sizeof(farray_t) + fa->len * (sizeof(fvec_t) + sizeof(int));    
    if (!fa->x || !fa->y) {
        farray_destroy(fa);
        error("Could not allocate elements of array");
        return NULL;
    }
    
    /* Open directory */    
    d = opendir(dir);
    if (!d) {
        farray_destroy(fa);
        error("Could not open directory '%s'", dir);
        return NULL;
    }
    
    /* Loop over directory entries */
    while((dp = readdir(d)) != NULL) {
        if (dp->d_type != DT_REG) 
            continue;
    
        /* Update array */
        farray_update_file(fa, i, dir, dp->d_name);
     
        /* Update memory and index */
        fa->mem += fa->x[i]->mem;
        i++;

        if (verbose > 0)
            prog_bar(0, fa->len, i);
    }   
    
    if (verbose > 0)
        printf("\n");
    closedir(d);

    /* Retrieve label array */
    fa->labels = label_hash_condense();

    /* Update memory usage */
    for (i = 0; fa->labels[i]; i++)
        fa->mem += sizeof(char *) + strlen(fa->labels[i]);

    return fa;
}

/**
 * Destroys an array of feature vectors
 * @param a array of feature vectors
 */
void farray_destroy(farray_t *fa)
{
    int i;    
    if (!fa)
        return;
    
    /* Free vectors */
    if (fa->x) {
        for (i = 0; i < fa->len; i++)
            fvec_destroy(fa->x[i]);
        free(fa->x);
    }

    /* Free classes */
    if (fa->labels) {
        for (i = 0; fa->labels[i]; i++)
            free(fa->labels[i]);
        free(fa->labels);
    }    

    /* Free labels */
    if (fa->y)
        free(fa->y);
       
    free(fa);
}

/**
 * Prints a feature array
 * @param feature array
 */
void farray_print(farray_t *fa)
{
    assert(fa);
    int i;
    
    /* Count labels */
    for (i = 0; fa->labels[i]; i++);

    printf("feature array [len: %u, labels: %d, ", fa->len, i);
    printf("%.2fMb, %p/%p/%p]\n", fa->mem / 1e6,
           (void *) fa, (void *) fa->x, (void *) fa->y);
           
    if (verbose < 2)
        return;
    
    for (i = 0; i < fa->len; i++) {
        fvec_print(fa->x[i]);
        printf("  label: %s, index %d\n", fa->labels[fa->y[i]], fa->y[i]);        
    }   
}
 
