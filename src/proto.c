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
 * The module contains functions for extracting prototypical feature
 * vectors 
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

/* Global variables */
static long counter = 0;

/**
 * Creates an empty structure of prototypes
 * @param a Array of feature vectors
 * @return Prototypes
 */
static proto_t *proto_create(farray_t *fa)
{
    assert(fa);

    /* Allocate prototype structure */
    proto_t *p = malloc(sizeof(proto_t));    
    if (!p) {
        error("Could not allocate prototype structure");
        return NULL;
    }

    /* Allocate structure fields */
    p->protos = farray_create(fa->src);
    p->assign = calloc(fa->len, sizeof(unsigned int));
    p->alen = fa->len;
    
    if (!p->protos || !p->assign) {
        error("Could not allocate prototype structure");
        proto_destroy(p);
        return NULL;
    }
        
    return p;
}


/**
 * Extracts a set of prototypes using the prototype algorithm.
 * @param fa Array of feature vectors
 * @param n Maximum number of prototypes
 * @param m Minimum distance
 * @param z Number of repeats
 * @return Prototypes
 */
static proto_t *proto_run(farray_t *fa, long n, double m, double z) 
{
    assert(fa);
    int i, j, k;
    double dm, *di;

    /* Allocate prototype structure and distance arrays */
    proto_t *pr = proto_create(fa);
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

    /* Loop over feature vectors */
    for (i = 0; i < n; i++) {
        if (i == 0) {
            /* Select random prototype */
            j = rand() % fa->len;
        } else {
            /* Determine largest distance */
            for(k = 0, j = 0, dm = 0; k < fa->len; k++)
                if (dm < di[k]) {
                    dm = di[k];
                    j = k;
                }
        }
        
        /* Check for minimum distance between prototypes */
        if (di[j] < m)
            break;

        /* Add prototype */
        fvec_t *pv = fvec_clone(fa->x[j]);
        farray_add(pr->protos, pv, farray_get_label(fa, j));

        /* Update distances and assignments */
        #pragma omp parallel for shared(fa, pv)
        for (k = 0; k < fa->len; k++) {
            double d = fvec_dist(pv, fa->x[k]);
            if (d < di[k]) {
                pr->assign[k] = i & PA_ASSIGN_MASK;            
                di[k] = d;
            }
            
            /* Mark protoype */
            if (k == j) {
                pr->assign[k] = i | PA_PROTO_MASK;
                di[k] = 0;
            }
        } 
        
        #pragma omp critical (counter)
        {
            counter++;
            if (verbose) 
                prog_bar(0, n * z, counter);
        }
    }
    
    /* Compute average distance */
    for (k = 0, pr->avg_dist = 0; k < fa->len; k++)
        pr->avg_dist += di[k] / fa->len;
        
    /* Free memory */
    free(di);
        
    return pr;
} 


/**
 * Extracts a set of prototypes using the prototype algorithm.
 * Prototype algorithm is run multiple times and the best set
 * of prototypes is returned.
 * @param fa Array of feature vectors
 * @return Prototypes
 */
proto_t *proto_extract(farray_t *fa) 
{
    assert(fa);
    long i, repeats, maxnum;
    double maxdist;
    proto_t **p, *pr;

    /* Get configuration */    
    config_lookup_float(&cfg, "prototypes.max_dist", (double *) &maxdist);
    config_lookup_int(&cfg, "prototypes.repeats", (long *) &repeats);
    config_lookup_int(&cfg, "prototypes.max_num", (long *) &maxnum);

    /* Allocate multiple prototype structures */
    p = malloc(repeats * sizeof(proto_t *));
    if (verbose > 0) 
        printf("Extracting prototypes with maximum distance %4.2f "
               "and %ld repeats.\n", maxdist, repeats);
    
    counter = 0;
    
    #pragma omp parallel for shared(p)
    for (i = 0; i < repeats; i++)
        p[i] = proto_run(fa, maxnum, maxdist, repeats);

    if (verbose) 
       prog_bar(0, 1, 1);

    /* Determine best prototypes */
    for (i = 1; i < repeats; i++) {
        if (p[0]->protos->len > p[i]->protos->len) {
            proto_destroy(p[0]);
            p[0] = p[i];
        } else {
            proto_destroy(p[i]);
        }
    }
    
    pr = p[0];
    free(p);
    
    if (verbose > 0)
        printf("  Done. %ld prototypes with average distance %4.2f "
               "extracted.\n", pr->protos->len, pr->avg_dist);
    
    return pr;
}

/**
 * Destroys a structure containing prototypes and frees its memory. 
 * @param p Prototypes
 */
void proto_destroy(proto_t *p)
{
    if (!p)
        return;
    if (p->protos)
        farray_destroy(p->protos);        
    if (p->assign)
        free(p->assign);
    free(p);
}

/**
 * Prints a structure containing prototypes
 * @param p Prototypes
 */
void proto_print(proto_t *p)
{
    assert(p);

    double mem = (p->protos->mem + sizeof(p->protos) + 
                  p->alen * sizeof(unsigned int)) / 1e6;

    printf("prototypes\n  len: %lu, assign: %lu, avg_dist: %f, mem: %.2fMb \n", 
           p->protos->len, p->alen, p->avg_dist, mem);
    
    farray_print(p->protos);
} 

/**
 * Saves a structure of prototype to a file stram
 * @param p Prototypes
 * @param z Stream pointer
 */
void proto_save(proto_t *p, gzFile *z)
{
    assert(p && z);
    int i;

    gzprintf(z, "prototypes: alen=%lu, avg_dist=%g\n", p->alen, p->avg_dist);            
    for (i = 0; i < p->alen; i++)
        gzprintf(z, "  %u\n", p->assign[i]);
        
    farray_save(p->protos, z);
}

/**
 * Saves a structure of prototypes to a file
 * @param p Prototypes
 * @param f File name
 */
void proto_save_file(proto_t *p, char *f) 
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
    proto_save(p, z);
    gzclose(z);      
}

/**
 * Loads a structure of prototypes from a file
 * @param f File name
 * @return Prototypes
 */
proto_t *proto_load_file(char *f) 
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
    proto_t *pr = proto_load(z);
    gzclose(z);      
    
    return pr;
}

/**
 * Loads a structure of prototype from a file stram
 * @param z Stream pointer
 * @return Prototypes
 */
proto_t *proto_load(gzFile *z)
{
    char buf[512];
    unsigned long alen;
    double f;
    int i, r;
    
    /* Allocate prototype structure */
    proto_t *p = calloc(1, sizeof(proto_t));    
    if (!p) {
        error("Could not allocate prototype structure");
        return NULL;
    }

    gzgets(z, buf, 512);
    r = sscanf(buf, "prototypes: alen=%lu, avg_dist=%lg\n", &alen, &f); 
    if (r != 2)  {
        error("Could not parse prototypes");
        proto_destroy(p);
        return NULL;
    }

    p->assign = calloc(alen, sizeof(unsigned int));
    p->alen = alen;    
    p->avg_dist = f;
    if (!p->assign) {
        error("Could not allocate prototype assignments");
        proto_destroy(p);
        return NULL;
    }
    
    for (i = 0; i < p->alen; i++) {
        gzgets(z, buf, 512);
        r = sscanf(buf, "  %u\n", p->assign + i); 
        if (r != 1)  {
            error("Could not parse prototype assignments");
            proto_destroy(p);
            return NULL;
        }    
    }
    
    p->protos = farray_load(z);
    return p;
}

/** @} */
