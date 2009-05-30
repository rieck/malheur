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
 * @defgroup fvec Feature vector
 * Generic implementation of a feature vector. A feature vector contains 
 * a sparse representation of non-zero dimensions in the feature space. 
 * This allows for operating with vectors of high and even infinite 
 * dimensionality, as long as the association between dimensions and 
 * non-zero values is sparse. The dimensions are indexed by 64bit hash 
 * values (MD5) and sorted to allow for efficiently processing and 
 * comparing vectors.  
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */

#include "config.h"
#include "common.h"
#include "fvec.h"
#include "ftable.h"
#include "fmath.h"
#include "util.h"
#include "md5.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/* Local functions */
static void extract_wgrams(fvec_t *, char *x, int l, int n);
static void extract_ngrams(fvec_t *, char *x, int l, int n);
static int compare_feat(const void *, const void *);

/* Delimiter functions and table */
static void decode_delim(const char *s);
static char delim[256] = { DELIM_NOT_INIT };


/** 
 * Condense a feature vector by counting duplicate features.
 * @param fv Feature vector
 */
static void fvec_condense(fvec_t * fv)
{
    feat_t *p_dim = fv->dim;
    float n = 0, *p_val = fv->val;
    unsigned int i;

    /* Loop over features */
    for (i = 0; i < fv->len; i++) {
        /* Skip zero values */
        if (fabs(fv->val[i]) < 1e-12)
            continue;

        /* Check for duplicate dims */
        if (i < fv->len - 1 && fv->dim[i] == fv->dim[i + 1]) {
            n += fv->val[i];
        } else {
            *(p_dim++) = fv->dim[i];
            *(p_val++) = fv->val[i] + n;
            n = 0;
        }
    }

    /* Update length and memory */
    fv->len = p_dim - fv->dim;
    fv->mem += fv->len * (sizeof(feat_t) + sizeof(float));
 
    /* Reallocate memory */
    fvec_shrink(fv);
}

/**
 * Allocate and extract a feature vector from a sequence.
 * There is a global table of delimiter symbols which is only 
 * initialized once the first sequence is processed. 
 * See fvec_reset_delim();
 * @param x Sequence of bytes 
 * @param l Length of sequence
 * @param s Source of features, e.g. file name
 * @return feature vector
 */
fvec_t *fvec_extract(char *x, int l, char *s)
{
    fvec_t *fv;
    int nlen;
    const char *dlm_str, *em_str;
    assert(x && l >= 0);

    /* Allocate feature vector */
    fv = calloc(1, sizeof(fvec_t));
    if (!fv) {
        error("Could not extract feature vector");
        return NULL;
    }

    /* Initialize feature vector */
    fv->len = 0;
    fv->total = 0;
    fv->dim = (feat_t *) malloc(l * sizeof(feat_t));
    fv->val = (float *) malloc(l * sizeof(float));
    fv->mem = sizeof(fvec_t);
    
    /* Set source */
    if (s) {
        fv->src = strdup(s);
        fv->mem += strlen(s);
    }

    /* Check for empty sequence */
    if (l == 0)
        return fv;

    if (!fv->dim || !fv->val) {
        error("Could not allocate feature vector");
        fvec_destroy(fv);
        return NULL;
    }

    /* Get n-gram length */
    config_lookup_int(&cfg, "features.ngram_length", (long *) &nlen);

    /* Construct delimiter lookup table */
    config_lookup_string(&cfg, "features.ngram_delim", &dlm_str);    

    /* N-grams of bytes */
    if (!dlm_str || strlen(dlm_str) == 0) {
        /* Feature extraction */    
        extract_ngrams(fv, x, l, nlen);    
    } else { 
        if (delim[0] == DELIM_NOT_INIT) {
            memset(delim, 0, 256);
            decode_delim(dlm_str);
        }

        /* Feature extraction */
        extract_wgrams(fv, x, l, nlen);
    }
    
    /* Sort extracted features */
    qsort(fv->dim, fv->len, sizeof(feat_t), compare_feat);

    /* Condense duplicate features */
    fv->total = fv->len;
    fvec_condense(fv);

    /* Compute embedding */
    config_lookup_string(&cfg, "features.normalization", &em_str);
    if (!strcasecmp(em_str, "bin"))
        fvec_normalize(fv, NORM_BIN);
    else if (!strcasecmp(em_str, "l2"))
        fvec_normalize(fv, NORM_L2);
    else if (!strcasecmp(em_str, "l1"))
        fvec_normalize(fv, NORM_L1);

    return fv;
}

/**
 * Shrinks the memory of a feature vector. The function reallocates
 * the memory of features and its values, such that the required space
 * is reduced to a minimum.
 */
void fvec_shrink(fvec_t *fv) 
{
    feat_t *p_dim;
    float *p_val;

    /*
     * Explicit reallocation. Don't use realloc(). On some platforms 
     * realloc() will not shrink memory blocks or copy to smaller sizes.
     * Consequently, realloc() here results in a huge memory leak. 
     */
    p_dim = malloc(fv->len * sizeof(feat_t));
    p_val = malloc(fv->len * sizeof(float));        
    if (!p_dim || !p_val) {
        error("Could not re-allocate feature vector");
        free(p_dim);
        free(p_val);
        return;
    }

    /* Copy to new feature vector */
    memcpy(p_dim, fv->dim, fv->len * sizeof(feat_t));
    memcpy(p_val, fv->val, fv->len * sizeof(float));

    /* Free old */
    free(fv->dim); 
    fv->dim = p_dim; 
    free(fv->val); 
    fv->val = p_val;
}

/**
 * Destroys a feature vector 
 * @param fv Feature vector
 */
void fvec_destroy(fvec_t * fv)
{
    if (fv->dim)
        free(fv->dim);
    if (fv->val)
        free(fv->val);
    if (fv->src)
        free(fv->src);
    free(fv);
}

/**
 * Clones a feature vector
 * @param o Feature vector
 * @return Cloned feature vector
 */
fvec_t *fvec_clone(fvec_t * o)
{
    assert(o);
    fvec_t *fv;
    unsigned int i;

    /* Allocate feature vector */
    fv = calloc(1, sizeof(fvec_t));
    if (!fv) {
        error("Could not clone feature vector");
        return NULL;
    }

    /* Clone structure */
    fv->len = o->len;
    fv->total = o->total;
    fv->mem = o->mem;

    if (o->src)
        fv->src = strdup(o->src);

    /* Check for empty sequence */
    if (o->len == 0)
        return fv;

    fv->dim = (feat_t *) malloc(o->len * sizeof(feat_t));
    fv->val = (float *) malloc(o->len * sizeof(float));
    if (!fv->dim || !fv->val) {
        error("Could not allocate feature vector");
        fvec_destroy(fv);
        return NULL;
    }

    for (i = 0; i < o->len; i++) {
        fv->dim[i] = o->dim[i];
        fv->val[i] = o->val[i];
    }

    return fv;
}

/**
 * Print the content of a feature vector
 * @param fv feature vector
 */
void fvec_print(fvec_t * fv)
{
    assert(fv);
    int i, j;

    printf("feature vector\n  len: %lu, total: %lu, mem: %.2fkb\n", 
           fv->len, fv->total, fv->mem / 1e3);
        
    if (fv->src)
        printf("  src: '%s'\n", fv->src);
           
    if (verbose < 3)
        return;
        
    for (i = 0; i < fv->len; i++) {
        printf("    0x%.16llx: %6.4f", (long long unsigned int) fv->dim[i], 
               fv->val[i]);

        /* Lookup feature */
        fentry_t *fe = ftable_get(fv->dim[i]);
        if (!fe) {
            printf("\n");
            continue;
        }
    
        /* Print feature string */
        printf(" [");
        for (j = 0; j < fe->len; j++) {
            if (isprint(fe->data[j]) || fe->data[j] == '%')
                printf("%c", fe->data[j]);
            else   
                printf("%%%.2x", fe->data[j]);
        }        
        printf("]\n");
    }
}

/**
 * Extract n-grams from a sequence using the provided configuration.
 * The features (n-grams) are represented by 64 bit hash values.
 * Optionally, the features are stored in a global table. To improve 
 * concurrent processing, the features are first collected in a cache
 * and later flushed into the global feature table.
 * @param fv Feature vector
 * @param x Byte sequence 
 * @param l Length of sequence
 * @param nlen N-gram length
 */
static void extract_wgrams(fvec_t *fv, char *x, int l, int nlen)
{
    unsigned int i, j = l, k = 0, s = 0, n = 0, d;
    unsigned char buf[MD5_DIGEST_LENGTH];    
    char *t = malloc(l + 1);
    fentry_t *cache = NULL;
     
    assert(fv && x);

    if (ftable_enabled())
        cache = malloc(l * sizeof(fentry_t));
    
    /* Find first delimiter symbol */
    for (d = 0; !delim[(unsigned char) d] && d < 256; d++);

    /* Compress sequence (remove redundant delimiters) */
    for (i = 0, j = 0; i < l; i++) {
        if (delim[(unsigned char) x[i]]) {
            if (j == 0 || delim[(unsigned char) t[j - 1]])
                continue;
            t[j++] = d;
        } else {
            t[j++] = x[i];
        }
    }

    /* Add trailing delimiter */
    if (t[j - 1] != d)
        t[j++] = d;
        
    /* Extract n-grams */
    for (k = i = 0; i < j; i++) {
        /* Count delimiters */
        if (t[i] == d || d == 256) {
            n++;
            /* Remember next starting point */
            if (n == 1)
                s = i;
        }

        /* Store n-gram */
        if (n == nlen && i - k > 0) {
            MD5((unsigned char *) (t + k), i - k, buf);
            memcpy(fv->dim + fv->len, buf, sizeof(feat_t));

            /* Cache feature and key */
            if (ftable_enabled()) {
                cache[fv->len].len = i - k;
                cache[fv->len].key = fv->dim[fv->len];
                cache[fv->len].data = malloc(i - k);
                if (cache[fv->len].data) 
                    memcpy(cache[fv->len].data, (t + k), i - k);
                else    
                    error("Could not allocate feature data");
            }
            
            fv->val[fv->len] = 1;
            k = s + 1, i = s, n = 0;
            fv->len++;
        }
    }
    free(t);
    
    if (!ftable_enabled())
        return;
    
    /* Flush cache and add features to hash */
    #pragma omp critical 
    {
        for (i = 0; i < fv->len; i++) {
            ftable_put(cache[i].key, cache[i].data, cache[i].len);
            free(cache[i].data);
        }    
    }
    free(cache);
}

/**
 * Extract n-grams from a sequence using the provided configuration.
 * The features (n-grams) are represented by 64 bit hash values. 
 * Optionally, the features are stored in a global table. To improve 
 * concurrent processing, the features are first collected in a cache
 * and later flushed into the global feature table.
 * @param fv Feature vector
 * @param x Byte sequence 
 * @param l Length of sequence
 * @param nlen N-gram length
 */
static void extract_ngrams(fvec_t *fv, char *x, int l, int nlen) 
{
    unsigned int i = 0;
    unsigned char buf[MD5_DIGEST_LENGTH];    
    char *t = x;
    fentry_t *cache = NULL;

    assert(fv && x && nlen > 0);
    
    if (ftable_enabled())
        cache = malloc(l * sizeof(fentry_t));

    for (i = 1; t < x + l; i++) {
        /* Check for sequence end */
        if (t + nlen > x + l)
            break;

        MD5((unsigned char *) t, nlen, buf);
        memcpy(fv->dim + fv->len, buf, sizeof(feat_t));

        /* Cache feature and key */
        if (ftable_enabled()) {
            cache[fv->len].len = nlen;
            cache[fv->len].key = fv->dim[fv->len];
            cache[fv->len].data = malloc(nlen);
            if (cache[fv->len].data) 
                memcpy(cache[fv->len].data, t, nlen);
            else
                error("Could not allocate feature data");
        }
        
        fv->val[fv->len] = 1;
        t++;
        fv->len++;
    }

    if (!ftable_enabled())
        return;
    
    /* Flush cache and add features to hash */
    #pragma omp critical 
    {
        for (i = 0; i < fv->len; i++) {
            ftable_put(cache[i].key, cache[i].data, cache[i].len);
            free(cache[i].data);
        }    
    }
    free(cache);
}      

/**
 * Compares two features values (hashs)
 * @param x feature X
 * @param y feature Y
 * @return result as a signed integer
 */
static int compare_feat(const void *x, const void *y)
{
    if (*((feat_t *) x) > *((feat_t *) y))
        return +1;
    if (*((feat_t *) x) < *((feat_t *) y))
        return -1;
    return 0;
}

/**
 * Decodes a string containing delimiters to a global array
 * @param s String containing delimiters
 */
static void decode_delim(const char *s)
{
    char buf[5] = "0x00";
    unsigned int i, j;

    for (i = 0; i < strlen(s); i++) {
        if (s[i] != '%') {
            delim[(unsigned int) s[i]] = 1;
            continue;
        }

        /* Skip truncated sequence */
        if (strlen(s) - i < 2)
            break;

        buf[2] = s[++i];
        buf[3] = s[++i];
        sscanf(buf, "%x", (unsigned int *) &j);
        delim[j] = 1;
    }
}

/**
 * Saves a feature vector to a file stream
 * @param f Feature vector
 * @param z Stream pointer
 */
void fvec_save(fvec_t *f, gzFile *z)
{
    assert(f && z);
    int i;

    gzprintf(z, "feature vector: len=%lu, total=%lu, mem=%lu, src=%s\n", 
             f->len, f->total, f->mem, f->src);
    for (i = 0; i < f->len; i++)
        gzprintf(z, "  %.16llx:%.16g\n", (unsigned long long) f->dim[i], 
                                         (float) f->val[i]);
}


/**
 * Loads a feature vector form a file stream
 * @param z Stream point
 * @return Feature vector
 */
fvec_t *fvec_load(gzFile *z)
{
    assert(z);
    fvec_t *f;
    char buf[512], str[512];
    int i, r;

    /* Allocate feature vector (zero'd) */
    f = calloc(1, sizeof(fvec_t));
    if (!f) {
        error("Could not load feature vector");
        return NULL;
    }

    gzgets(z, buf, 512);
    r = sscanf(buf, "feature vector: len=%lu, total=%lu, mem=%lu, src=%s\n", 
              (unsigned long *) &f->len, (unsigned long *) &f->total,
              (unsigned long *) &f->mem, str);
    if (r != 4)  {
        error("Could not parse feature vector");
        fvec_destroy(f);
        return NULL;
    }

    /* Set source */
    if (!strcmp(str, "(null)")) 
        f->src = NULL;
    else
        f->src = strdup(str);
    
    /* Empty feature vector */
    if (f->len == 0) 
        return f;

    /* Allocate arrays */
    f->dim = (feat_t *) malloc(f->len * sizeof(feat_t));
    f->val = (float *) malloc(f->len * sizeof(float));
    if (!f->dim || !f->val) {
        error("Could not allocate feature vector contents");
        fvec_destroy(f);
        return NULL;
    }

    /* Load features */
    for (i = 0; i < f->len; i++) {
        gzgets(z, buf, 512);
        r = sscanf(buf, "  %llx:%g\n", (unsigned long long *) &f->dim[i], 
                                       (float *) &f->val[i]);
        if (r != 2) {
            error("Could not parse feature vector contents");
            fvec_destroy(f);
            return NULL;
        }
    }      
     
    return f;
}

/**
 * Resets delimiters table. There is a global table of delimiter 
 * symbols which is only initialized once the first sequence is 
 * processed. This functions is used to trigger a re-initialization.
 */
void fvec_reset_delim()
{
    delim[0] = DELIM_NOT_INIT;
}

/** @} */ 
 