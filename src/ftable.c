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
static int table_enabled           = FALSE;
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
 */
void ftable_put(feat_t key, char *x, int l)
{
    assert(x && l > 0);
    int collision = FALSE;
    fentry_t *f;
    
    if (!table_enabled)
        return;
    
    HASH_FIND(hh, feature_table, &key, sizeof(feat_t), f);

    /* Check for collission */
    if (f && (l != f->len || memcmp(x, f->data, l))) {
        /* Collision! Remove old */            
        HASH_DEL(feature_table, f);            
        collisions++;    
        collision = TRUE;
    }

    /* Already there */
    if (f && !collision)
        return;

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

    /* Add to hash and count insertion */    
    HASH_ADD(hh, feature_table, key, sizeof(feat_t), f);
    insertions++;
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
    HASH_FIND(hh, feature_table, &key, sizeof(feat_t), f);
    return f;
}      

/**
 * Initialize the feature lookup table.
 */
void ftable_init()
{
    if (feature_table)
        ftable_destroy();

    table_enabled = TRUE;
    collisions = 0;
    insertions = 0;
} 

/**
 * Destroy the feature lookup table. 
 */
void ftable_destroy()
{
    fentry_t *f;
        
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
 * Removes an element from the lookup hash. 
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
    HASH_DEL(feature_table, f);
}


/**
 * Print the feature lookup table. 
 */
void ftable_print()
{
    fentry_t *f;
    int i;
    
    printf("feature table [size: %ld, puts: %ld, colls: %ld (%g%%), %p]\n", 
            ftable_size(), insertions, collisions, 
            (collisions * 100.0) / insertions, (void *) feature_table);
    
    if (verbose < 3)
        return;

    for (f = feature_table; f != NULL; f = f->hh.next) {
        printf("  0x%.16llx: ", (long long unsigned int) f->key);

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
 * Returns the size of the feature lookup table.
 * @return size of table
 */
unsigned long ftable_size()
{ 
    return HASH_COUNT(feature_table);
}

/**
 * Saves a feature table to a file stream.
 * @param z Stream pointer
 */
void ftable_save(gzFile *z)
{
    fentry_t *f;
    int i;
        
    gzprintf(z, "feature table: len=%lu\n", HASH_COUNT(feature_table));
    for (f = feature_table; f != NULL; f = f->hh.next) {
        gzprintf(z, "  %.16llx: ", (long long unsigned int) f->key);
        for (i = 0; i < f->len; i++) {
            if (isprint(f->data[i]) || f->data[i] == '%')
                gzprintf(z, "%c", f->data[i]);
            else   
                gzprintf(z, "%%%.2x", f->data[i]);                
        }        
        gzprintf(z, "\n");
    }
}

/**
 * Loads a feature table from a file stream (Not synchronized)
 * @param z Stream pointer
 */
void ftable_load(gzFile *z)
{
    int i, r;
    unsigned long len;
    char buf[512], str[512];
    feat_t key;
        
    gzgets(z, buf, 512);
    r = sscanf(buf, "feature table: len=%lu\n", (unsigned long *) &len);
    if (r != 1)  {
        error("Could not parse feature table");
        return;
    }
    
    for (i = 0; i < len; i++) {
        gzgets(z, buf, 512);
        r = sscanf(buf, "  %llx:%511s\n", (unsigned long long *) &key, (char *) str);
        if (r != 2) {
            error("Could not parse feature table contents");
            return;
        }
        
        /* Decode string */
        r = decode_string(str);
        
        /* Put string to table */                        
        ftable_put(key, str, r);
    }
}

/**
 * Returns true if the feature table is enabled
 * @return true if enabled false otherwise
 */
int ftable_enabled()
{
    return table_enabled;
} 
