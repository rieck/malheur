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
#include "fhash.h"
#include "util.h"

/* Hash table */
static fentry_t *lookup_table      = NULL;
static int lookup_enabled          = FALSE;
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
void fhash_put(feat_t key, char *x, int l)
{
    assert(x && l > 0);
    fentry_t *f;
    if (!lookup_enabled)
        return;

    HASH_FIND(hh, lookup_table, &key, sizeof(feat_t), f);
    /* Check if bucket is used */
    if (f) {
        /* Already present */
        if (l == f->len && !memcmp(x, f->data, l)) 
            return;

        /* Collision! Remove old */            
        fhash_remove(key);            
        collisions++;    
    }

    /* Allocate new entry */
    f = malloc(sizeof(fentry_t));
    if (!f) {
        error("Could not allocate entry in lookup table.");
        return;
    }
    
    f->key = key; 
    f->data = malloc(l);
    f->len = l;
    if (!f->data) {
        error("Could not allocate entry in lookup table.");
        return;
    }
    
    memcpy(f->data, x, l);
    HASH_ADD(hh, lookup_table, key, sizeof(feat_t), f);
    
    /* Count insertion */
    insertions++;
}      


/**
 * Gets a entry from the lookup table. The returned memory must not 
 * be free'd.
 * @param f Feature key
 * @return feature table entry
 */
fentry_t *fhash_get(feat_t key)
{
    fentry_t *f;
    
    if (!lookup_enabled)
        return NULL;
    
    HASH_FIND(hh, lookup_table, &key, sizeof(feat_t), f);
    return f;
}      

/**
 * Initialize the feature lookup table
 */
void fhash_init()
{
    if (lookup_enabled)
        fhash_destroy();

    lookup_enabled = TRUE;
    collisions = 0;
    insertions = 0;
} 

/**
 * Destroy the feature lookup table
 */
void fhash_destroy()
{
    fentry_t *f;
    
    if (!lookup_enabled)
        return;
    
    while(lookup_table) {
        f = lookup_table;
        HASH_DEL(lookup_table, f);
        free(f->data);
        free(f);
    }
    
    lookup_enabled = FALSE;
    collisions = 0;
    insertions = 0;
}


/**
 * Removes an element from the lookup hash
 * @param f Feature to remove
 */
void fhash_remove(feat_t key)
{
    fentry_t *f;

    /* Find element */
    HASH_FIND(hh, lookup_table, &key, sizeof(feat_t), f);
    if (!f)
        return;
    
    /* Remove */
    HASH_DEL(lookup_table, f);
}

/**
 * Print the feature lookup table
 */
void fhash_print()
{
    fentry_t *f;
    int i;
    
    if (!lookup_enabled)
        return;

    printf("feature table [size: %ld, puts: %ld, colls: %ld (%5.2f%%), %p]\n", 
            fhash_size(), insertions, collisions, 
            (collisions * 100.0) / insertions, (void *) lookup_table);
    
    if (verbose < 3)
        return;
        
    for (f = lookup_table; f != NULL; f = f->hh.next) {
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
long fhash_size()
{
    if (!lookup_enabled)
        return 0;
        
    return HASH_COUNT(lookup_table);
}
