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
 * @defgroup classify Classification functionality
 * The module contains functions for classifying feature vectors 
 * to known clusters
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */
#include "config.h"
#include "common.h"
#include "util.h"
#include "class.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Classify feature vectors using prototypes und update assignments. 
 * Feature vectors with a too large distance are rejected from the 
 * classification by setting their label to 0.
 * @param as Classification assignments
 * @param fa Array of feature vectors 
 */
void classify_apply(assign_t *as, farray_t *fa)
{
    int i;
    double maxdist;

    config_lookup_float(&cfg, "classify.max_dist", &maxdist);

    /* Determine rejected reports */
    for (i = 0; i < as->len; i++) {
        if (as->dist[i] > maxdist)
            as->label[i] = 0;
    }
}

/**
 * Return an array of rejected feature vectors
 * @param as Classification assignments
 * @param fa Array of feature vectors
 * @return Rejected feature vectors
 */
farray_t *classify_get_rejected(assign_t *as, farray_t *fa)
{
    int i;
    farray_t *r = farray_create("rejected");

    for (i = 0; i < fa->len; i++) {
        if (as->label[i])
            continue;
        farray_add(r, fvec_clone(fa->x[i]), farray_get_label(fa, i));
    }

    return r;
}

/** @} */
