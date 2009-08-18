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
 * @defgroup data Generic evaluation functions
 * The module contains functions for evaluation the performance of 
 * classification and clustering methods.
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */

#include "config.h"
#include "common.h"
#include "eval.h"
#include "util.h"

#if 0

/* External variables */
extern int verbose;
extern config_t cfg;

static void hist_match(hist_t *h, int *y, int n)
{
    hist_t *hi;
    num_t *ii, *match;
    
    for (hi = h; hi != NULL; hi = hi->hh.next) {
        for (ii = hi->index; ii != NULL; ii = ii->hh.next) {
            HASH_FIND_INT(hi->match, &y[ii->num], match); 
            if (!match) {
                match = malloc(sizeof(num_t));
                if (!match) {
                    error("Could not match histogram");
                    return;
                }    
            
                /* Create new entry */
                match->num = y[ii->num];
                match->count = 0;
            
                /* Add entry */
                HASH_ADD_INT(hi->match, num, match);
            }
            
            match->count++;
        }
    }
}

hist_t *hist_create(int *y, int *c, int n)
{
    int i;
    hist_t *entry, *hist = NULL;
    num_t *index;
    
    for (i = 0; i < n; i++) {
        HASH_FIND_INT(hist, &y[i], entry); 
        if (!entry) {
            entry = malloc(sizeof(hist_t));
            if (!entry) {
                error("Could not allocate histogram");
                hist_destroy(hist);
                return NULL;
            }    
            
            /* Create new entry */
            entry->label = y[i];
            entry->total = 0;
            entry->index = NULL;
            entry->match = NULL;
            
            /* Add entry */
            HASH_ADD_INT(hist, label, entry);
        }
        
        entry->total++;
        index = malloc(sizeof(num_t));
        index->num = i;
        HASH_ADD_INT(entry->index, num, index);
    }
    
    /* Match histogram with clustering */
    hist_match(hist, c, n);
    
    return hist;
}

void hist_print(hist_t *h)
{
    hist_t *hi;
    num_t *ii;
    
    for(hi = h; hi != NULL; hi = hi->hh.next) {
        printf("Label: %d\n", hi->label);
        printf("Total: %f\n", hi->total);

        printf("Indices: ");
        for (ii = hi->index; ii != NULL; ii = ii->hh.next) {
            printf("%d ", ii->num);
        }
        printf("\n");
        
        printf("Matches: ");
        for (ii = hi->match; ii != NULL; ii = ii->hh.next) {
            printf("%d ", ii->num);
        }
        printf("\n");        
    }
}

double hist_agree(hist_t *h)
{
    double a;
    hist_t *hi;

    for(hi = h; hi != NULL; hi = hi->hh.next) {
        printf("Label: %d\n", hi->label);
        printf("Total: %f\n", hi->total);

        printf("Indices: ");
        for (ii = hi->index; ii != NULL; ii = ii->hh.next) {
            printf("%d ", ii->num);
        }
        printf("\n");
    }  

}

void hist_destroy(hist_t *h)
{
    hist_t *hi;
    num_t *ii;
    
    /* Iterate over classes */
    while(h) {
        hi = h;           
        
        /* Delete indices */
        while(hi->index) {
            ii = hi->index;            
            HASH_DEL(hi->index, ii);  
            free(ii);            
        }
        
        /* Delete matches */
        while(hi->match) {
            ii = hi->match;            
            HASH_DEL(hi->match, ii);  
            free(ii);            
        }
        
        HASH_DEL(h, hi);  
        free(hi);            
    }
}

#endif

/** @} */
 
