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

#ifndef MCONFIG_H
#define MCONFIG_H

#ifdef HAVE_LIBCONFIG_H
#include <libconfig.h>
#endif

#ifdef HAVE_UTHASH_UTHASH_H
#include <uthash/uthash.h>
#else 
#ifdef HAVE_UTHASH_H
#include <uthash.h>
#else
#include "uthash.h"
#endif
#endif

/** 
 * Default configuration. This structure is used to define a default
 * configuration. The type can be determined by first testing for a 
 * string and then for a float argument.
 */
typedef struct {
    char *group;        /**< Configuration group */
    char *name;         /**< Configuration name */
    long inum;          /**< Integer value of configuration (or 0) */
    double fnum;        /**< Float value of configuraiton (or NAN) */
    char *str;          /**< String of configuration (or NULL) */
} config_default_t;

/* Empty value for floating value */
#define FLT_NONE	-0.1234567890123456789

/* Functions */
void config_print(config_t *cfg);
void config_check(config_t *cfg);
void config_fprint(FILE *, config_t *cfg);

#endif                          /* CFG_H */
