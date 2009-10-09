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
void export_distance_text(double *d, farray_t *fa, char *file)
{
    assert(d && fa && file);
    int i,j;
    
    if (verbose > 0)
        printf("Exporting distance matrix to '%s'.\n", file);
    
    FILE *f = fopen(file, "w");
    if (!f) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    for (i = 0; i < fa->len; i++) {
        fprintf(f, "%s:", fa->x[i]->src);
        for (j = 0; j < fa->len; j++)
            fprintf(f, " %g", d[i * fa->len + j]);
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
void export_proto_text(farray_t *p, farray_t *fa, char *file)
{
    assert(p && fa && file);
    int i, j;

    /* Assign data to prototypes */
    assign_t *c = proto_assign(fa, p);

    if (verbose > 0)
        printf("Exporting prototypes to '%s'.\n", file);
    
    FILE *f = fopen(file, "w");
    if (!f) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    for (i = 0; i < fa->len; i++) {
        j = c->proto[i];
        fprintf(f, "%s: %s (%s)\n", fa->x[i]->src, p->x[j]->src, 
                farray_get_label(p, j));
    }
    
    fclose(f);
    assign_destroy(c);
}

/** @} */
