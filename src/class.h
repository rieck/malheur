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

#ifndef CLASSIFY_H
#define CLASSIFY_H

#include "farray.h"
#include "proto.h"

/* Functions */
void class_apply(assign_t *, farray_t *);
farray_t *class_rejected(assign_t *, farray_t *f);

#endif                          /* CLASSIFY_H */
