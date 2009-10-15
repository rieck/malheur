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
#include "fmath.h"

/* External variables */
extern int verbose;
extern config_t cfg;
extern runtime_t runtime;

/**
 * Creates an empty structure of assignments
 * @param a Array of feature vectors
 * @return assignment structure
 */
assign_t *assign_create(farray_t *fa)
{
    assert(fa);
    int i;

    /* Allocate assignment structure */
    assign_t *c = malloc(sizeof(assign_t));
    if (!c) {
        error("Could not allocate assignment structure");
        return NULL;
    }

    /* Allocate structure fields */
    c->label = calloc(fa->len, sizeof(unsigned int));
    c->proto = calloc(fa->len, sizeof(unsigned int));
    c->dist = calloc(fa->len, sizeof(double));
    c->len = fa->len;

    if (!c->label || !c->proto || !c->dist) {
        error("Could not allocate assignment structure");
        assign_destroy(c);
        return NULL;
    }
    
    /* Dummy assignments */
    for (i = 0; i < fa->len; i++) {
        c->label[i] = fa->y[i];
        c->proto[i] = i;
        c->dist[i] = 0;
    }
    

    return c;
}


/**
 * Destroys an assignment
 * @param c Assignment structure
 */
void assign_destroy(assign_t *c)
{
    if (!c)
        return;
    if (c->label)
        free(c->label);
    if (c->proto)
        free(c->proto);
    if (c->dist)
        free(c->dist);
    free(c);
}


/**
 * Classify feature vectors using prototypes und update assignments. 
 * Feature vectors with a too large distance are rejected from the 
 * classification by setting their label to 0.
 * @param as Array of feature vectors
 * @param p Array of prototypes
 */
assign_t *classify_apply(farray_t *fa, farray_t *p)
{
    assert(fa && p);
    int i, k, j, cnt = 0;
    double d = 0;
    double maxdist;

    config_lookup_float(&cfg, "classify.max_dist", &maxdist);

    assign_t *c = assign_create(fa);

    if (verbose > 0)
        printf("Classifying feature vectors to %lu prototypes.\n", p->len);

#pragma omp parallel for shared(fa,c,p) private(k,j)
    for (i = 0; i < fa->len; i++) {
        double min = DBL_MAX;
        for (k = 0, j = 0; k < p->len; k++) {
            rt_start(distclass);
            d = fvec_dist(fa->x[i], p->x[k]);
            rt_stop(distclass);
            if (d < min) {
                min = d;
                j = k;
            }
        }

        /* Compute assignments */
        c->proto[i] = j;
        c->dist[i] = min;
        c->label[i] = p->y[j];

        if (c->dist[i] > maxdist)
            c->label[i] = 0;

#pragma omp critical (cnt)
        if (verbose)
            prog_bar(0, fa->len, ++cnt);
    }

    if (verbose > 0)
        printf("  Done. Classified %ld feature vectors to %ld prototypes.\n",
               fa->len, p->len);

    return c;
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
