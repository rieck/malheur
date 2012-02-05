/*
 * MALHEUR - Automatic Analysis of Malware Behavior
 * Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)
 * University of Goettingen, Berlin Institute of Technology 
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 * --
 */

/**
 * @defgroup export Export functions
 * The module contains functions for exporting data computed by 
 * Malheur to external format such as plain text and HTML documents.
 * @author Konrad Rieck
 * @{
 */
#include "config.h"
#include "common.h"
#include "farray.h"
#include "util.h"
#include "export.h"
#include "mconfig.h"
#include "quality.h"
#include "fmath.h"
#include "ftable.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Exports a distance matrix to a text file
 * @param d Pointer to matrix
 * @param fa Feature vector array
 * @param file File name 
 */
void export_dist(double *d, farray_t *fa, char *file)
{
    assert(d && fa && file);
    int i, j;
    FILE *f;
    
    if (verbose > 0)
        printf("Exporting distance matrix to '%s'.\n", file);
    
    if (!(f = fopen(file, "w"))) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    /* Print version header */
    malheur_version(f);
    
    /* Print distance header */
    fprintf(f, "# ---\n# Distance matrix for %s\n", fa->src);
    fprintf(f, "# Matrix size: %lu x %lu\n# ---\n", fa->len, fa->len);
    fprintf(f, "# <report> <cluster> <dist1> <dist2> ... <distn>\n");
    
    /* Print matrix */
    for (i = 0; i < fa->len; i++) {
        fprintf(f, "%s %s ", fa->x[i]->src, farray_get_label(fa, i));
        for (j = 0; j < fa->len; j++)
            fprintf(f, "%g ", d[i * fa->len + j]);
        fprintf(f, "\n");
    }
    
    fclose(f);
}

/**
 * Exports a structure of prototypes to a text file
 * @param pr Prototype structure
 * @param fa Feature vector array
 * @param as Assignments to protoypes
 * @param file File name
 */
void export_proto(farray_t *pr, farray_t *fa, assign_t *as, char *file)
{
    assert(pr && fa && file);
    int i, j;
    FILE *f;
    
    if (verbose > 0)
        printf("Exporting prototypes to '%s'.\n", file);
    
    if (!(f = fopen(file, "w"))) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    /* Print version header */
    malheur_version(f);
    
    /* Evaluate some quality functions */
    double *e = quality(fa->y, as->proto, as->len);
    
    /* Print prototype header */
    fprintf(f, "# ---\n# Prototypes for %s\n", fa->src);
    fprintf(f, "# Number of prototypes: %lu\n", pr->len);
    fprintf(f, "# Compression of prototypes: %4.1f %%\n", 
            pr->len * 100.0 / (double) fa->len);
    fprintf(f, "# Precision of prototypes: %4.1f %%\n",
            e[Q_PRECISION] * 100.0);
    fprintf(f, "# ---\n# <report> <prototype> <distance>\n");
    
    for (i = 0; i < fa->len; i++) {
        j = as->proto[i];
        fprintf(f, "%s %s %g\n", fa->x[i]->src, pr->x[j]->src, as->dist[i]);
    }
    
    fclose(f);
}

/**
 * Exports a clustering structure to a text file
 * @param c Clustering structure
 * @param fa Feature vector array
 * @param p Prototype struture
 * @param a Assignments of prototypes
 * @param file File name
 */
void export_cluster(cluster_t *c, farray_t *p, farray_t *fa, assign_t *a,
                    char *file)
{
    assert(c && fa && file);
    FILE *f;
    int i, j;
    
    if (verbose > 0)
        printf("Exporting clusters to '%s'.\n", file);
    
    if (!(f = fopen(file, "w"))) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    /* Print version header */
    malheur_version(f);
    
    /* Evaluate some quality functions */
    double *e = quality(fa->y, c->cluster, c->len);
    
    /* Print prototype header */
    fprintf(f, "# ---\n# Clusters for %s\n", fa->src);
    fprintf(f, "# Number of cluster: %lu\n", c->num);
    fprintf(f, "# Precision of clusters: %4.1f %%\n", e[Q_PRECISION] * 100.0);
    fprintf(f, "# Recall of clusters: %4.1f %%\n", e[Q_RECALL] * 100.0);
    fprintf(f, "# F-measure of clusters: %4.1f %%\n", e[Q_FMEASURE] * 100.0);
    fprintf(f, "# ---\n# <report> <cluster> <prototype> <distance>\n");
    
    for (i = 0; i < fa->len; i++) {
        j = a->proto[i];
        fprintf(f, "%s %s %s %g\n", fa->x[i]->src, cluster_get_name(c, i),
                p->x[j]->src, a->dist[i]);
    }
    
    fclose(f);
}


/**
 * Exports classification results
 * @param p Prototype structure
 * @param fa Feature vector array
 * @param as Assignments to protoypes
 * @param file File name
 */
void export_class(farray_t *p, farray_t *fa, assign_t *as, char *file)
{
    assert(p && fa && file);
    int i, j;
    char *l;
    FILE *f;
    
    if (verbose > 0)
        printf("Exporting classification to '%s'.\n", file);
    
    if (!(f = fopen(file, "w"))) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    /* Print version header */
    malheur_version(f);
    
    /* Evaluate some quality functions */
    double *e = quality(fa->y, as->label, as->len);
    
    /* Print prototype header */
    fprintf(f, "# ---\n# Classification for %s\n", fa->src);
    fprintf(f, "# Precision of classification: %4.1f %%\n",
            e[Q_PRECISION] * 100.0);
    fprintf(f, "# Recall of classification: %4.1f %%\n", 
            e[Q_RECALL] * 100.0);
    fprintf(f, "# F-measure of classification: %4.1f %%\n", 
            e[Q_FMEASURE] * 100.0);
    fprintf(f, "# ---\n# <report> <label> <prototype> <distance>\n");
    
    for (i = 0; i < fa->len; i++) {
        j = as->proto[i];
        l = as->label[i] ? farray_get_label(p, j) : "rejected";
        fprintf(f, "%s %s %s %g\n", fa->x[i]->src, l, p->x[j]->src,
                as->dist[i]);
    }
    
    fclose(f);
}

/**
 * Exports results from the incremental analysis (phase 1). The results are
 * obtained by first classifying and then clustering reports. In the first
 * phase the classified reports are written to the output file.
 * @param p Prototype struture
 * @param fa Feature vector array
 * @param as Assignments to prototypes
 * @param file File name
 */
void export_increment1(farray_t *p, farray_t *fa, assign_t *as, char *file)
{
    int i, j;
    FILE *f;
    
    if (verbose > 0)
        printf("Exporting incremental analysis (1) to '%s'.\n", file);
    
    if (!(f = fopen(file, "w"))) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    /* Print version header */
    malheur_version(f);
    
    /* Print incremental header */
    fprintf(f, "# ---\n# Incremental analysis for %s\n", fa->src);
    fprintf(f, "# ---\n# <report> <cluster> <prototype> <distance>\n");
    
    if (!p || !as) {
        fclose(f);
        return;
    }
    
    for (i = 0; i < fa->len; i++) {
        if (!as->label[i])
            continue;
        j = as->proto[i];
        fprintf(f, "%s %s %s %g\n", fa->x[i]->src, farray_get_label(p, j), 
                p->x[j]->src, as->dist[i]);
    }
    
    fclose(f);
}

/**
 * Exports results from the incremental analysis (phase 2). The results are
 * obtained by first classifying and then clustering reports. The function
 * appends the output of clustering to an existing file from phase 1. 
 * @param c Clustering structure
 * @param p Prototype struture
 * @param fa Feature vector array
 * @param as Assignments to prototypes
 * @param file File name
 */
void export_increment2(cluster_t *c, farray_t *p, farray_t *fa, assign_t *as, 
                       char *file)
{
    assert(c && p && fa && as && file);
    int i, j;
    FILE *f;
    
    if (verbose > 0)
        printf("Exporting incremental analysis (2) to '%s'.\n", file);
    
    if (!(f = fopen(file, "a"))) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    for (i = 0; i < fa->len; i++) {
        j = as->proto[i];
        fprintf(f, "%s %s %s %g\n", fa->x[i]->src, cluster_get_name(c, i),
                p->x[j]->src, as->dist[i]);
    }
    
    fclose(f);
}

/**
 * Print shared n-grams for each cluster
 * @param c Clustering structure
 * @param fa Array of feature vectors
 * @param file Output file
 */
void export_shared_ngrams(cluster_t *c, farray_t *fa, char *file)
{
    assert(c && fa && file);
    int i, j, k;
    double shared;
    FILE *f;
    char *name = NULL;
    
    config_lookup_float(&cfg, "cluster.shared_ngrams", &shared);
    if (shared <= 0.0)
        return;
    
    if (verbose > 0)
        printf("Exporting shared n-grams with minimum ratio %4.2f.\n", shared);
    
    if (!(f = fopen(file, "a"))) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    /* Print incremental header */
    fprintf(f, "# ---\n# Shared n-grams for %s\n", fa->src);
    fprintf(f, "# Minimum ratio of shared n-grams: %4.2f (%2.0f%%)\n", shared, 
            shared * 100);
    fprintf(f, "# ---\n# <cluster> <ratio> <hash> <ngram>\n");
    
    /* Compute shared n-grams per cluster */
    for (i = 0; i < c->num; i++) {
        fvec_t *s = fvec_zero();
        
        for (j = 0, k = 0; j < c->len; j++) {
            if (c->cluster[j] != i)
                continue;
            
            /* Clone and binarize */
            fvec_t *x = fvec_clone(fa->x[j]);
            fvec_bin(x);
            
            if (k == 0) 
                name = cluster_get_name(c, j);
            
            /* Merge n-grams in cluster */
            fvec_t *y = fvec_add(s, x);
            fvec_destroy(s);            
            fvec_destroy(x);
            s = y;
            k++;
        }
        
        /* Check for empty cluster */
        if (k == 0)
            continue;
        
        fvec_div(s, k);
        
        /* Output shared n-grams */
        for (j = 0; j < s->len; j++) {
            if (s->val[j] < shared)
                continue;

            fprintf(f, "%s %6.4f %.16llx ", name, s->val[j],
                    (long long unsigned int) s->dim[j]);
            
            /* Lookup feature */
            fentry_t *fe = ftable_get(s->dim[j]);
            if (!fe) 
                error("Oops. Feature not in lookup table."); 
            
            /* Print feature */
            fprintf(f, "\"");
            for (k = 0; k < fe->len; k++) {
                if (isprint(fe->data[k]) || fe->data[k] == '%')
                    fprintf(f, "%c", fe->data[k]);
                else
                    fprintf(f, "%%%.2x", fe->data[k]);
            }
            fprintf(f, "\"\n");
        }        
        fvec_destroy(s);        
    }
    
    fclose(f);
}

/** @} */
