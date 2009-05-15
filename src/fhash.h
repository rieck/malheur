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

#ifndef FHASH_H
#define FHASH_H

#include "fvec.h"
#include "uthash.h"

/** 
 * Entry of feature table
 */
typedef struct {
    feat_t key;            /**< Feature key */
    char *data;            /**< Feature data */
    int len;               /**< Length of data */
    UT_hash_handle hh;     /**< Uthash handle */
} fentry_t;

void fhash_put(feat_t, char *, int);
fentry_t *fhash_get(feat_t);
void fhash_init();
void fhash_destroy();
void fhash_remove(feat_t);
long fhash_size();
void fhash_print();

#endif                          /* FHASH_H */
