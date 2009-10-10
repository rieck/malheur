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
 * @defgroup proto Extraction of prototypes
 * The module contains functions for extracting prototypical 
 * feature vectors 
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */

#include "config.h"
#include "common.h"
#include "fmath.h"
#include "util.h"
#include "proto.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Extracts prototypes using an extended version of Gonzalez' algorithm.
 * @param fa Array of feature vectors
 * @param n Maximum number of prototypes
 * @param m Minimum distance between prototypes
 * @return Prototypes
 */
static farray_t *proto_gonzalez(farray_t *fa, long n, double m)  
{
    assert(fa);
    int i, j, k;
    double *di;

    /* Allocate prototype structure and distance arrays */
    farray_t *pr = farray_create(fa->src);
    di = malloc(fa->len * sizeof(double));
    if (!pr || !di) {
        error("Could not allocate memory for prototype extraction");
        return NULL;
    }

    /* Init distances to maximum value */
    for (i = 0; i < fa->len; i++)
        di[i] = DBL_MAX;    
        
    /* Check for maximum number of protos */
    if (n == 0)
        n = fa->len;

    /* Loop over feature vectors. First prototype: j = 0.*/
    for (i = 0; i < n; i++) {
        /* Determine largest distance */
        if (i > 0)
            j = array_max(di, fa->len);
        else
            j = 0;

        /* Check for minimum distance between prototypes */
        if (di[j] < m)
            break;

        /* Add prototype */
        fvec_t *pv = fvec_clone(fa->x[j]);
        farray_add(pr, pv, farray_get_label(fa, j));

        /* Update distances and assignments */
        #pragma omp parallel for shared(fa, pv)
        for (k = 0; k < fa->len; k++) {
            double d = fvec_dist(pv, fa->x[k]);
            if (d < di[k]) 
                di[k] = d;
            
            if (j == k)
                di[k] = 0;
        } 
        
        if (verbose) 
            prog_bar(0, n, i);
    }

    /* Update progress bar */
    if (verbose)
        prog_bar(0, n, n);
        
    /* Free memory */
    free(di);
    return pr;
} 


/**
 * Extracts a set of prototypes using the prototype algorithm.
 * Prototype algorithm is run multiple times and the smallest set
 * of prototypes is returned.
 * @param fa Array of feature vectors
 * @return Prototypes
 */
farray_t *proto_extract(farray_t *fa) 
{
    assert(fa);
    farray_t *p;
    long maxnum;
    double maxdist;

    /* Get configuration */    
    config_lookup_float(&cfg, "prototypes.max_dist", (double *) &maxdist);
    config_lookup_int(&cfg, "prototypes.max_num", (long *) &maxnum);

    if (verbose > 0) 
        printf("Extracting prototypes with maximum distance %4.2f.\n", 
               maxdist);
    
    /* Extract prototypes */
    p = proto_gonzalez(fa, maxnum, maxdist);
    
    if (verbose > 0)
        printf("  Done. %ld prototypes using %.2fMb extracted.\n", 
               p->len, p->mem / 1e6);
    
    return p;
}

/**
 * Saves a structure of prototypes to a file
 * @param p Prototypes
 * @param f File name
 */
void proto_save_file(farray_t *p, char *f) 
{ 
    assert(p && f);

    if (verbose)
        printf("Saving prototypes to '%s'.\n", f);
    
    /* Open file */
    gzFile *z = gzopen(f, "w9");
    if (!z) {
        error("Could not open '%s' for writing", f);
        return;
    }
        
    /* Save data */
    farray_save(p, z);
    gzclose(z);      
}

/**
 * Loads a structure of prototypes from a file
 * @param f File name
 * @return Prototypes
 */
farray_t *proto_load_file(char *f) 
{ 
    assert(f);
    if (verbose)
        printf("Loading prototypes from '%s'.\n", f);
    
    /* Open file */
    gzFile *z = gzopen(f, "r");
    if (!z) {
        error("Could not open '%s' for reading", f);
        return NULL;
    }
        
    /* Save data */
    farray_t *pr = farray_load(z);
    gzclose(z);      
    
    return pr;
}

/**
 * Creates an empty structure of assignments
 * @param a Array of feature vectors
 * @return assignment structure
 */
static assign_t *assign_create(farray_t *fa)
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
 * Assign a set of vector to prototypes
 * @param fa Feature vectors
 * @param p Prototype vectors
 */
assign_t *proto_assign(farray_t *fa, farray_t *p)
{
    assert(fa && p);
    int i, k, j, cnt = 0;
    double d = 0;
    double maxdist;

    assign_t *c = assign_create(fa);
    config_lookup_float(&cfg, "classify.max_dist", (double *) &maxdist);
    
    if (verbose > 0) 
        printf("Assigning feature vectors to %lu prototypes of %d classes.\n", 
               p->len, HASH_CNT(hn, fa->label_name));

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
        c->dist[i] = d;
        if (d < maxdist)
            c->label[i] = p->y[j];
        else
            c->label[i] = 0;

        #pragma omp critical (cnt)
        if (verbose)
            prog_bar(0, fa->len, ++cnt);
    }
    
    if (verbose > 0)
        printf("  Done. %ld feature vectors assigned to %lu prototypes.\n", 
               fa->len, p->len);
     
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


/** @} */
