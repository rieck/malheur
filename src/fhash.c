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
static fentry_t *lookup_table   = NULL;
static int lookup_enabled       = TRUE;

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
void fhash_add(feat_t key, char *x, int l)
{
    assert(x && l > 0);
    if (!lookup_enabled)
        return;

    fentry_t *f = malloc(sizeof(fentry_t));
    if (!f) {
        error("Could not allocate entry in lookup table.");
        return;
    }
    
    f->key = key; 
    f->data = malloc(l);
    if (!f->data) {
        error("Could not allocate entry in lookup table.");
        return;
    }
    
    memcpy(f->data, x, l);
    HASH_ADD(hh, lookup_table, key, sizeof(feat_t), f);
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

