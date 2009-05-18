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
#include "ftable.h"
#include "util.h"

/* Hash table */
static fentry_t *feature_table     = NULL;
static int table_enabled          = FALSE;
static unsigned long collisions    = 0;
static unsigned long insertions    = 0;

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Add a feature and its key to the lookup table. The byte sequence
 * is copied into separate memory which needs to be free'd individually.
 * @param f Feature key
 * @param x Byte sequence 
 * @param l Length of sequence
 */
void ftable_put(feat_t key, char *x, int l)
{
    assert(x && l > 0);

    fentry_t *f;
    if (!table_enabled)
        return;

    HASH_FIND(hh, feature_table, &key, sizeof(feat_t), f);
    /* Check if bucket is used */
    if (f) {
        /* Already present */
        if (l == f->len && !memcmp(x, f->data, l)) 
            return;

        /* Collision! Remove old */            
        #pragma omp critical 
        {
            ftable_remove(key);            
            collisions++;    
        }
    }

    /* Allocate new entry */
    f = malloc(sizeof(fentry_t));
    if (!f) {
        error("Could not allocate entry in feature table.");
        return;
    }
    
    f->key = key; 
    f->data = malloc(l);
    f->len = l;
    if (!f->data) {
        error("Could not allocate entry in feature table.");
        return;
    }
    
    memcpy(f->data, x, l);

    #pragma omp critical 
    {
        /* Add to hash and count insertion */    
        HASH_ADD(hh, feature_table, key, sizeof(feat_t), f);
        insertions++;
    }
}      

/**
 * Gets a entry from the lookup table. The returned memory must not 
 * be free'd.
 * @param f Feature key
 * @return feature table entry
 */
fentry_t *ftable_get(feat_t key)
{
    fentry_t *f;
    
    if (!table_enabled)
        return NULL;
    
    HASH_FIND(hh, feature_table, &key, sizeof(feat_t), f);
    return f;
}      

/**
 * Initialize the feature lookup table
 */
void ftable_init()
{
    if (table_enabled)
        ftable_destroy();

    table_enabled = TRUE;
    collisions = 0;
    insertions = 0;
} 

/**
 * Destroy the feature lookup table
 */
void ftable_destroy()
{
    fentry_t *f;
    
    if (!table_enabled)
        return;
    
    while(feature_table) {
        f = feature_table;
        HASH_DEL(feature_table, f);
        free(f->data);
        free(f);
    }
    
    table_enabled = FALSE;
    collisions = 0;
    insertions = 0;
}


/**
 * Removes an element from the lookup hash
 * @param f Feature to remove
 */
void ftable_remove(feat_t key)
{
    fentry_t *f;


    /* Find element */
    HASH_FIND(hh, feature_table, &key, sizeof(feat_t), f);
    if (!f)
        return;
    
    /* Remove */
    #pragma omp critical 
    {
        HASH_DEL(feature_table, f);
    }
}

/**
 * Print the feature lookup table
 */
void ftable_print()
{
    fentry_t *f;
    int i;
    
    if (!table_enabled)
        return;

    printf("feature table [size: %ld, puts: %ld, colls: %ld (%g%%), %p]\n", 
            ftable_size(), insertions, collisions, 
            (collisions * 100.0) / insertions, (void *) feature_table);
    
    if (verbose < 3)
        return;
        
    for (f = feature_table; f != NULL; f = f->hh.next) {
        printf("  0x%.16llx: ", f->key);

        for (i = 0; i < f->len; i++) {
            if (isprint(f->data[i]) || f->data[i] == '%')
                printf("%c", f->data[i]);
            else   
                printf("%%%.2x", f->data[i]);                
        }        
        printf("\n");
    }
}

/**
 * Returns the size of the feature lookup table
 * @return size of table
 */
long ftable_size()
{
    if (!table_enabled)
        return 0;
        
    return HASH_COUNT(feature_table);
}

