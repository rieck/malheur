/*
 * MALHEUR - Automatic Analysis of Malware Behavior
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
void export_proto(farray_t *, farray_t *, assign_t *, char *);
void export_cluster(cluster_t *, farray_t *, farray_t *, assign_t *, char *);
void export_shared_ngrams(cluster_t *, farray_t *, char *);
void export_dist(double *, farray_t *, char *);
void export_class(farray_t *, farray_t *, assign_t *, char *);
void export_increment1(farray_t *, farray_t *, assign_t *, char *);
void export_increment2(cluster_t *, farray_t *, farray_t *, assign_t *, char *);



#endif                          /* EXPORT_H */
