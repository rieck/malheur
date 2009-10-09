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
 * @defgroup data Export functions
 * The module contains functions for exporting data computed by 
 * Malheur to external format such as plain text and HTML documents.
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */

#include "config.h"
#include "common.h"
#include "farray.h"
#include "util.h"
#include "export.h"
#include "mconfig.h"
#include "quality.h"
#include "class.h"

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
    fprintf(f, "# Matrix size: %ld x %ld\n# ---\n", fa->len, fa->len);
    fprintf(f, "# <report> <dist1> <dist2> ... <distn>\n");
    
    /* Print matrix */
    for (i = 0; i < fa->len; i++) {
        fprintf(f, "%s ", fa->x[i]->src);
        for (j = 0; j < fa->len; j++)
            fprintf(f, "%g ", d[i * fa->len + j]);
        fprintf(f, "\n");
    }
    
    fclose(f);
}

/**
 * Exports a structure of prototypes to a text file
 * @param p Prototype structure
 * @param fa Feature vector array
 * @param file File name
 */
void export_proto(farray_t *p, farray_t *fa, char *file)
{
    assert(p && fa && file);
    int i, j;
    FILE *f;

    /* Assign data to prototypes */
    assign_t *c = proto_assign(fa, p);

    if (verbose > 0)
        printf("Exporting prototypes to '%s'.\n", file);
    
    if (!(f = fopen(file, "w"))) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    /* Print version header */
    malheur_version(f);
    
    /* Evaluate some quality functions */
    double *e = quality(fa->y, c->proto, c->len);

    /* Print prototype header */
    fprintf(f, "# ---\n# Prototypes for %s\n", fa->src);
    fprintf(f, "# Number of prototypes: %ld (%3.1f%%)\n", p->len, 
            p->len * 100.0 / (double) fa->len);
    fprintf(f, "# Precision of prototypes: %4.1f%%\n", 
            e[Q_PRECISION] * 100.0);
    fprintf(f, "# ---\n# <report> <prototype> <label> <distance>\n");
    
    for (i = 0; i < fa->len; i++) {
        j = c->proto[i];
        fprintf(f, "%s %s %s %g\n", fa->x[i]->src, p->x[j]->src, 
                farray_get_label(p, j), c->dist[i]);
    }
    
    fclose(f);
    assign_destroy(c);
}

/**
 * Exports a clustering structure to a text file
 * @param c Clustering structure
 * @param fa Feature vector array
 * @param file File name
 */
void export_cluster(cluster_t *c, farray_t *fa, char *file)
{
    assert(c && fa && file);
    FILE *f;

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
    fprintf(f, "# Number of cluster: %ld\n", c->num);
    fprintf(f, "# Precision of clusters: %4.1f%%\n", e[Q_PRECISION] * 100.0);
    fprintf(f, "# Recall of clusters: %4.1f%%\n", e[Q_RECALL] * 100.0);
    fprintf(f, "# F-measure of clusters: %4.1f%%\n", e[Q_FMEASURE] * 100.0);
    fprintf(f, "# ---\n# <report> <cluster> <prototype> <label> <distance>\n");
    
    fclose(f);
}


/** @} */
