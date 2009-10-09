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
 * Exports a distance matrix to a text file
 * @param d Pointer to matrix
 * @param fa Feature vector array
 * @param file File name 
 */
void export_distance_html(double *d, farray_t *fa, char *file)
{
    assert(d && fa && file);
    int i,j;
    char bg[8];
    double min = DBL_MAX, max = DBL_MIN, avg = 0;
    
    if (verbose > 0)
        printf("Exporting distance matrix to '%s'.\n", file);
    
    FILE *f = fopen(file, "w");
    if (!f) {
        error("Could not create file '%s'.", file);
        return;
    }
    
    /* Determine minimum, average and maximum values */
    for (i = 0; i < fa->len; i++)
        for (j = i; j < fa->len; j++) {
            if (d[i * fa->len + j] < min)
                min = d[i * fa->len + j];
            if (d[i * fa->len + j] > max)
                max = d[i * fa->len + j];
            avg += d[i * fa->len + j];
        }
    avg /= (fa->len * fa->len) / 2 + fa->len;
    
    
    fprintf(f, "<html><body>%s<h1>Distance Matrix</h1>", BODY);
    
    fprintf(f, "<table cellpadding='0' cellspacing='0' style='font-size: 11pt;'>");
    fprintf(f, TS "Number of total reports:" TM "%lu" TE, fa->len);
    fprintf(f, TS "Report source:" TM "%s", fa->src);    
    fprintf(f, TS "Minimum distance:" TM "%7.5f" TE, min);
    fprintf(f, TS "Average distance:" TM "%7.5f" TE, avg);
    fprintf(f, TS "Maximum distance:" TM "%7.5f" TE, max);
    fprintf(f, "</table>\n");
    
    /* Write configuration */
    fprintf(f, "<h2>Configuration</h2><pre>");
    config_fprint(f, &cfg);
    fprintf(f, "</pre>");   
    
    
    fprintf(f, "<table cellpadding='0' cellspacing='1' style='font-size: 6pt;'>");
    for (i = 0; i < fa->len; i++) {
        fprintf(f, "<tr><td>%s:&nbsp;&nbsp;", fa->x[i]->src);
        for (j = 0; j < fa->len; j++) {
            int x = (int) round(255 * (d[i * fa->len + j] - min) / (max - min));
            snprintf(bg, 8, "#%.2xff%.2x", x, x);
            fprintf(f, "</td><td bgcolor='%s'>%4.2f", bg, d[i * fa->len + j]);
        }
        fprintf(f, "</td></tr>\n");
    }
    fprintf(f, "</table></body></html>\n");
    
    fclose(f);
}


/**
 * Extracts a CWSandbox URL from a filename. The function uses a static 
 * buffer and thus is not thread-safe.
 * @param f File name
 * @return URL to CWSandbox report
 */
static char *cwsandbox_url(char *f)
{
    static char buf[1024];
    char *ptr = f + strlen(f) - 1;
    int sid, aid;
    
    /* Determine basename */
    while(ptr != f && *(ptr - 1) != '/')
        ptr--;
    
    /* Get sample and analysis id */
    sscanf(ptr, "%d_%d.%*s\n", &sid, &aid);
    
    /* Construct URL */
    snprintf(buf, 1024, "%s&analysisid=%d", CWS_URL, aid);
    return buf;
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

/**
 * Exports a structure of prototypes to a HTML file. This function is 
 * ugly and should be replaced by some template engine.
 * @param p Prototype structure
 * @param fa Feature vector array
 * @param file File name
 */
void export_proto_html(farray_t *p, farray_t *fa, char *file)
{
    assert(p && fa && file);
    int i, j, x = 0, *lidx, *pidx, *cnt;
    long cws_urls;
    FILE *f;

    /* Assign data to prototypes */
    assign_t *c = proto_assign(fa, p);
    
    if (verbose > 0)
        printf("Exporting prototypes to '%s'.\n", file);
    
    if (!(f = fopen(file, "w"))) {
        error("Could not open '%s' for writing", file);
        return;
    }
    
    /* Get configuration */
    config_lookup_int(&cfg, "output.cws_urls", &cws_urls);
    
    /* Sort labels and compute quality */
    lidx = qsort_idx(fa->y, fa->len, sizeof(unsigned int), cmp_uint);
    double *e = quality(fa->y, c->proto, fa->len);
    
    /* Sort prototypes by members */
    cnt = calloc(p->len, sizeof(unsigned int));
    for (i = 0; i < p->len; i++) 
        for (j = 0; j < fa->len; j++) 
            if (c->proto[j] == i) 
                cnt[i]--;
    pidx = qsort_idx(cnt, p->len, sizeof(int), cmp_int);        
    
    /* Write generic */
    fprintf(f, "<html><body>%s<h1>Prototypes</h1>", BODY);
    fprintf(f, "<table cellpadding='0' cellspacing='0' style='font-size: 11pt;'>");
    fprintf(f, TS "Number of prototypes:" TM "%lu (%3.1f%%)" TE, p->len, 
            p->len / (double) fa->len * 100);    
    fprintf(f, TS "Number of total reports:" TM "%lu" TE, fa->len);
    fprintf(f, TS "Report source:" TM "%s", p->src);    
    fprintf(f, TS "Precision of labels:" TM "%7.5f" TE, e[Q_PRECISION]);
    fprintf(f, "</table>\n");
    
    /* Write configuration */
    fprintf(f, "<h2>Configuration</h2><pre>");
    config_fprint(f, &cfg);
    fprintf(f, "</pre>");    
    
    /* Write prototypes and assignments */
    fprintf(f, "<h2>Assignments</h2><ol>\n");
    for (i = 0; i < p->len; i++) {
        fprintf(f, "<li><a href='%s'><b>Prototype</b></a> (members: %d)<br>\n", 
                cws_urls ? cwsandbox_url(p->x[pidx[i]]->src) : 
                p->x[pidx[i]]->src, -cnt[pidx[i]]);
        
        /* Write list of assignments */
        for (j = 0, x = 0; j < fa->len; j++) {
            if (c->proto[lidx[j]] != pidx[i])
                continue;
            
            fprintf(f, "<a href='%s' style='text-decoration: none;'>", 
                    cws_urls ? cwsandbox_url(fa->x[lidx[j]]->src) : 
                    fa->x[lidx[j]]->src);
            
            /* Print dots if label is repreated */
            if (x && x == fa->y[lidx[j]]) 
                fprintf(f, "&middot;");
            else
                fprintf(f, "%s", farray_get_label(fa, lidx[j]));
            
            x = fa->y[lidx[j]];
            fprintf(f, "</a> ");
        }
        fprintf(f, "<br>\n");
    }
    fprintf(f,"</ol></body></html>\n");
    fclose(f);

    assign_destroy(c);
    free(pidx);
    free(lidx);
}

/** @} */
