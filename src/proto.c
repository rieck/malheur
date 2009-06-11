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
#include "util.h"
#include "proto.h"

/* External variables */
extern int verbose;


/**
 * Extracts a set of prototypes
 * @param a Array of feature vectors
 * @param p Prototypes
 */
proto_t *proto_extract(farray_t *fa) 
{
    assert(fa);

    proto_t *p = malloc(sizeof(proto_t));
    if (!p) {
        error("Could not allocate prototype structure");
        return NULL;
    }
    
    p->assign = calloc(1, fa->len * sizeof(int));
    p->len = fa->len;
    
    return p;
} 

/** @} */
