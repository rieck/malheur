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
 * @defgroup fmath Math for feature vectors
 * This module contains standard mathematical functions defined over
 * sparse feature vectors. In favor of a generic interface, only assorted
 * functions are implemented that provide linear-time processing.
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */

#include "config.h"
#include "common.h"
#include "fvect.h"
#include "fmath.h"
#include "util.h"

/* External variables */
extern int verbose;

/**
 * Normalize a feature vector to a norm.
 * @param f Feature vector 
 * @param n Normalization
 */
void fvect_normalize(fvect_t *f, norm_t n)
{
    int i = 0;
    double s = 0;
    assert(f);

    switch (n) {
    case NORM_BIN:
        for (i = 0; i < f->len; i++)
            f->val[i] = 1;
        break;
    case NORM_L1:
        s = fvect_norm1(f);
        for (i = 0; i < f->len; i++)
            f->val[i] /= s;
        break;
    case NORM_L2:
        s = fvect_norm2(f);
        for (i = 0; i < f->len; i++)
            f->val[i] /= s;
        break;
    }
}

/**
 * Multiplies vector with a scalar (f = s * f)
 * @param f Feature vector 
 * @param s Scalar value
 */
void fvect_mul(fvect_t *f, double s)
{
    int i = 0;
    assert(f);

    for (i = 0; i < f->len; i++)
        f->val[i] *= s;
}

/**
 * Divides vector by a scalar (f = 1/s * f)
 * @param f Feature vector 
 * @param s Scalar value
 */
void fvect_div(fvect_t *f, double s)
{
    fvect_mul(f, 1/s);
}


/** 
 * Dot product between two feature vectors (s = <a,b>). The function 
 * uses a loop to sum over all dimensions.
 * @param fa Feature vector (a)
 * @param fb Feature vector (b)
 * @return s Inner product
 */
static double fvect_dot_loop(fvect_t *fa, fvect_t *fb) 
{
    unsigned long i = 0, j = 0;
    double s = 0;

    /* Loop over features in a and b */
    while (i < fa->len && j < fb->len) {
        if (fa->dim[i] > fb->dim[j]) {
            j++;
        } else if (fa->dim[i] < fb->dim[j]) {
            i++;
        } else {
            s += fa->val[i++] * fb->val[j++];
        }
    }
    
    return s;
}

/** 
 * Dot product between two feature vectors (s = <a,b>). The function 
 * uses a binary search to sum over all dimensions.
 * @param fa Feature vector (a)
 * @param fb Feature vector (b)
 * @return s Inner product
 */
static double fvect_dot_bsearch(fvect_t *fa, fvect_t *fb) 
{
    unsigned long i = 0, j = 0, p, q, k;
    double s = 0;

    /* Check if fa is larger than fb */
    if (fa->len < fb->len) {
        fvect_t *tmp = fa;
        fa = fb, fb = tmp;
    }

    /* Loop over dimensions fb */
    for (i = 0, j = 0; j < fb->len; j++) {
        /* Binary search */
        p = i, q = fa->len;
        do {
            k = i, i = ((q - p) >> 1) + p;
            if (fa->dim[i] > fb->dim[j]) {
                q = i;
            } else if (fa->dim[i] < fb->dim[j]) {
                p = i;
            } else {
                s += fa->val[i] * fb->val[j];
                break;
            }
        } while (i != k);
    }

    return s;
}

/** 
 * Dot product between arrays of feature vectors (s = <a,b>). 
 * @param fa Array of feature vectors (a)
 * @param fb Array of feature vectors (b)
 * @return matrix of dot products 
 */
double *farray_dot(farray_t *fa, farray_t *fb)
{
    assert(fa && fb);
    long i, r = 0;
    
    double *d = malloc(fa->len * fb->len * sizeof(double));
    if (!d) {
        error("Could not allocate double matrix\n");
        return NULL;
    }
    
    if (verbose > 0)
        prog_bar(0, 1, 0);
    
    if (fa == fb) {
        #pragma omp parallel for shared(d)
        for (i = 0; i < fa->len; i++) {
            for (int j = i; j < fb->len; j++) {
                d[i * fb->len + j] = fvect_dot(fa->x[i], fb->x[j]);
                d[j * fb->len + i] = d[i * fb->len + j];                
            }
            
            if (verbose > 0) {
                #pragma omp critical
                {
                    r += fb->len - i;
                    prog_bar(0, (fa->len * fa->len + fa->len) / 2.0, r);
                }
            }    
        }    
    } else {
        #pragma omp parallel for shared(d)
        for (i = 0; i < fa->len; i++) {
            for (int j = 0; j < fb->len; j++) {
                d[i * fb->len + j] = fvect_dot(fa->x[i], fb->x[j]);
            }
            
            if (verbose > 0) {
                #pragma omp critical
                {
                    r += fb->len;
                    prog_bar(0, fa->len * fb->len, r);
                }
            }
        }    
    }
    if (verbose > 0)
        printf("\n");
    
    return d;
}

/** 
 * Dot product between two feature vectors (s = <a,b>). The function 
 * uses a loop or a binary search to sum over all dimensions depending
 * on the size of the considered vectors.
 * @param fa Feature vector (a)
 * @param fb Feature vector (b)
 * @return s Inner product
 */
double fvect_dot(fvect_t *fa, fvect_t *fb) 
{
    assert(fa && fb);
    double a, b;
 
    /* Sort vectors according to size */
    if (fa->len > fb->len) {
        a = fa->len, b = fb->len;
    } else {
        b = fa->len, a = fb->len;
    }
    
    /* Choose dot functions */
    if (a + b > ceil(b * log2(a)))
        return fvect_dot_bsearch(fa, fb);
    else
        return fvect_dot_loop(fa, fb);
}

/** 
 * Adds two feature vectors and create a new one (c = a + b * s)
 * @param fa Feature vector (a)
 * @param fb Feature vector (b)
 * @param s Scalar value
 * @return new feature vector
 */
fvect_t *fvect_adds(fvect_t *fa, fvect_t *fb, double s) 
{
    unsigned long i = 0, j = 0, len = 0;
    assert(fa && fb);
    fvect_t *f;
    
    /* Allocate feature vector (zero'd) */
    f = calloc(1, sizeof(fvect_t));
    if (!f) {
        error("Could not create feature vector");
        return NULL;
    }

    f->mem = sizeof(fvect_t);
    f->total = fa->total + fb->total;
    f->src = NULL;

    /* Allocate arrays */
    f->dim = (feat_t *) malloc((fa->len + fb->len) * sizeof(feat_t));
    f->val = (float *) malloc((fa->len + fb->len) * sizeof(float));
    if (!f->dim || !f->val) {
        error("Could not allocate feature vector contents");
        fvect_destroy(f);
        return NULL;
    }
    
    /* Loop over features in a and b */
    while (i < fa->len && j < fb->len) {
        if (fa->dim[i] > fb->dim[j]) {
            f->dim[len] = fb->dim[j];
            f->val[len++] = fb->val[j++] * s;
        } else if (fa->dim[i] < fb->dim[j]) {
            f->dim[len] = fa->dim[i];
            f->val[len++] = fa->val[i++];
        } else {
            f->dim[len] = fa->dim[i];
            f->val[len++] = fa->val[i++] + fb->val[j++] * s;
        }
    }
    
    /* Loop over remaining features  */
    while (j < fb->len) {
        f->dim[len] = fb->dim[j];
        f->val[len++] = fb->val[j++] * s;
    } 
    while (i < fa->len) {
        f->dim[len] = fa->dim[i];
        f->val[len++] = fa->val[i++];
    }

    /* Set new length and reallocate */
    f->len = len;
    f->mem += f->len * (sizeof(feat_t) + sizeof(float));

    /* Reallocate memory */
    fvect_realloc(f);
    
    return f;
}

/** 
 * Adds two feature vectors and create a new one (c = a + b)
 * @param fa Feature vector (a)
 * @param fb Feature vector (b)
 * @return new feature vector
 */
fvect_t *fvect_add(fvect_t *fa, fvect_t *fb)
{   
    return fvect_adds(fa, fb, 1.0);
} 


/** 
 * Substractes two feature vectors and create a new one (c = a - b)
 * @param fa Feature vector (a)
 * @param fb Feature vector (b)
 * @return new feature vector
 */
fvect_t *fvect_sub(fvect_t *fa, fvect_t *fb) 
{
    return fvect_adds(fa, fb, -1.0);
}

/**
 * Adds all feature vectors to a linear combination
 * @param fa Array of feature vector
 * @param s Array of scalar values
 * @return Linear combination
 */
fvect_t *farray_sums(farray_t *fa, double *s)
{
    fvect_t *g, *f = fvect_zero();
    int i;
            
    for (i = 0; i < fa->len; i++) {
        /* Skip zero elements */
        if (fabs(s[i]) < 1e-8)
            continue;
            
        /* Add elements */
        g = fvect_adds(f, fa->x[i], s[i]);
        fvect_destroy(f);
        f = g;
    }
    
    return f;
} 

/**
 * Adds all feature vectors to a linear combination
 * @param fa Array of feature vector
 * @return Linear combination
 */
fvect_t *farray_sum(farray_t *fa)
{
    int i;

    double *s = malloc(fa->len * sizeof(double));
    for (i = 0; i < fa->len; i++)
        s[i] = 1.0;
    
    fvect_t *f = farray_sums(fa, s);
    
    free(s);
    return f;
} 

/**
 * Computers the mean vector of the given array of feature vector
 * @param fa Array of feature vector
 * @return Mean vector
 */
fvect_t *farray_mean(farray_t *fa)
{
    int i;

    double *s = malloc(fa->len * sizeof(double));
    for (i = 0; i < fa->len; i++)
        s[i] = 1.0 / fa->len;
    
    fvect_t *f = farray_sums(fa, s);
    
    free(s);
    return f;
} 


/**
 * Computes the l1-norm of the feature vector (n = ||f||_1)
 * @param f Feature vector 
 * @return sum of values 
 */
double fvect_norm1(fvect_t *f)
{
    int i = 0;
    double s = 0;    
    assert(f);

    for (i = 0; i < f->len; i++)
        s += f->val[i];
    
    return s;
}

/**
 * Computes the l2-norm of the feature vector (n = ||f||_2)
 * @param f Feature vector 
 * @return sum of values 
 */
double fvect_norm2(fvect_t *f)
{
    int i = 0;
    double s = 0;
    assert(f);

    for (i = 0; i < f->len; i++)
        s += pow(f->val[i], 2);
    
    return sqrt(s);
}

/**
 * Sparsifies a feature vector by removing zero dimensions 
 * @parma f Feature vectore
 */
void fvect_sparsify(fvect_t *f)
{
    int i, j = 0;
    
    for (i = 0, j = 0; i < f->len; i++) {
        /* Skip over values close to zero */
        if (fabs(f->val[i]) < 1e-9)
            continue;
        
        /* Copy contents */
        if (i != j) {
            f->val[j] = f->val[i];
            f->dim[j] = f->dim[i];
        }    
        j++;
    }    
    
    /* No change in size? */
    if (f->len == j)
        return;
    
    /* Update length and memory */
    f->mem -= (f->len - j) * (sizeof(feat_t) + sizeof(float)); 
    f->len = j;   
    
    /* Reallocate memory */
    fvect_realloc(f);
}

/** }@ */
