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

#ifndef EXPORT_H
#define EXPORT_H

#include "farray.h"
#include "proto.h"
#include "cluster.h"

/* I/O functions */
void export_cluster(cluster_t *c, farray_t *fa, char *file);
void export_dist(double *d, farray_t *f, char *file);
void export_proto(farray_t *p, farray_t *fa, assign_t *a, char *file);
void export_class(farray_t *p, farray_t *fa, assign_t *a, char *file);



#endif                          /* EXPORT_H */
