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
 * @defgroup proto Classification
 * The module contains functions for classification of feature
 * vectors using labeled prototypes
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */
 
#include "config.h"
#include "common.h"
#include "util.h"
#include "fmath.h"
#include "proto.h"
#include "class.h"
 
/**
 * Creates an empty structure of prototypes
 * @param a Array of feature vectors
 * @return Prototypes
 */
static class_t *class_create(farray_t *fa)
{
    assert(fa);

    /* Allocate classification structure */
    class_t *c = malloc(sizeof(class_t));    
    if (!c) {
        error("Could not allocate classification structure");
        return NULL;
    }

    /* Allocate structure fields */
    c->label = calloc(fa->len, sizeof(unsigned int));
    c->proto = calloc(fa->len, sizeof(unsigned int));
    c->dist = calloc(fa->len, sizeof(double));
    c->len = fa->len;
    
    if (!c->label || !c->proto || !c->dist) {
        error("Could not allocate classification structure");
        class_destroy(c);
        return NULL;
    }
        
    return c;
}

/**
 * Predict the classes of feature vectors using labeled prototypes.
 * @param fa Feature vectors
 * @param p Prototype vectors
 */
class_t *class_predict(farray_t *fa, farray_t *p)
{
    assert(fa && p);

    int i, k, j;
    double min, d;
    class_t *c = class_create(fa);

    #pragma omp parallel for shared(fa,p)
    for (i = 0; i < fa->len; i++) {
        min = DBL_MAX;
        for (k = 0, j = 0; k < p->len; k++) {
            d = fvec_dist(fa->x[i], p->x[k]);
            if (d < min) {
                min = d;
                j = k;
            }
        }
        
        /* Compute classification */
        c->proto[i] = j;
        c->dist[i] = d;
        c->label[i] = p->y[j];
    }
    
    return c;
}
  
 
/**
 * Destroys a classification structure
 * @param c Classification structure
 */
void class_destroy(class_t *c)
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

/** @} */
