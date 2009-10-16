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
 * @defgroup ftable Lookup table for features
 * Lookup table for extracted features. The extracted feature, such as
 * substrings and n-grams, a stored in sparse feature vectors with each
 * feature represented by hash value. This global hash table is used
 * to efficiently lookup the original feature for a given hash. The table
 * keeps also track of counting insertions and collisions, such that 
 * the quality of the hashing can be assessed. Maintaining a global hash 
 * table impacts performance when using OpenMP, thus it is adviced to 
 * disable the table if not required. 
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */

#include "config.h"
#include "common.h"
#include "fvec.h"
#include "ftable.h"
#include "util.h"

/* Hash table */
static fentry_t *feature_table = NULL;
static int table_enabled = FALSE;
static unsigned long collisions = 0;
static unsigned long insertions = 0;

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Add a feature and its key to the lookup table. The function clones 
 * all input arguments, that is new memory is allocated and the data 
 * is copied. This memory is free'd when destroy the feature table.
 * @param k Key for feature
 * @param x Data of feature
 * @param l Length of feature
 */
void ftable_put(feat_t k, char *x, int l)
{
    assert(x && l > 0);
    fentry_t *g, *h;

    if (!table_enabled)
        return;

    /* Check for duplicate */
    HASH_FIND(hh, feature_table, &k, sizeof(feat_t), g);

    /* Check for collision */
    if (g) {
        if (l != g->len || memcmp(x, g->data, l))
            collisions++;
        return;
    }

    /* Build new entry */
    h = malloc(sizeof(fentry_t));
    h->len = l;
    h->key = k;
    h->data = malloc(l);
    if (h->data)
        memcpy(h->data, x, l);
    else
        error("Could not allocate feature data");

    /* Add to hash and count insertion */
    HASH_ADD(hh, feature_table, key, sizeof(feat_t), h);
    insertions++;
}

/**
 * Gets an entry from the lookup table. The returned memory must not 
 * be free'd.
 * @param key Feature key
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

    while (feature_table) {
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
 * @param key Feature to remove
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
void ftable_save(gzFile * z)
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
void ftable_load(gzFile * z)
{
    int i, r;
    unsigned long len;
    char buf[512], str[512];
    feat_t key;

    gzgets(z, buf, 512);
    r = sscanf(buf, "feature table: len=%lu\n", (unsigned long *) &len);
    if (r != 1) {
        error("Could not parse feature table");
        return;
    }

    for (i = 0; i < len; i++) {
        gzgets(z, buf, 512);
        r = sscanf(buf, "  %llx:%511s\n", (unsigned long long *) &key,
                   (char *) str);
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

/** @} */
