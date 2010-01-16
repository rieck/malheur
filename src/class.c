/*
 * MALHEUR - Automatic Analysis of Malware Behavior
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
 * @defgroup class Classification using prototypes
 * The module contains functions for assigning and classifying feature 
 * vectors to known clusters. Clusters and classes are both represented
 * by appropriate prototype vectors.
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

/**
 * Creates an empty structure of assignments. The assignments can be 
 * either computed for matching prototypes against feature vectors or 
 * for classification of feature vectors.
 * @param fa Array of feature vectors
 * @return assignment structure
 */
assign_t *assign_create(farray_t *fa)
{
    assert(fa);

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

    return c;
}


/**
 * Destroys an assignment structure.
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
 * Classify feature vectors using labeled prototypes. The function assigns
 * the given feature vectors to the given prototypes and predicts labels. 
 * Feature vectors with a too large distance are rejected from the 
 * classification by setting their label to 0.
 * @param fa Array of feature vectors
 * @param p Array of prototypes
 * @return Assignment structure
 */
assign_t *class_assign(farray_t *fa, farray_t *p)
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
            d = fvec_dist(fa->x[i], p->x[k]);
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
        printf("  Done. Classified %lu feature vectors to %lu prototypes.\n",
               fa->len, p->len);

    return c;
}

/**
 * Return an array of rejected feature vectors.
 * @param as Assignment structure
 * @param fa Array of feature vectors
 * @return Rejected feature vectors
 */
farray_t *class_get_rejected(assign_t *as, farray_t *fa)
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
