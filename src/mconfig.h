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

#ifndef MCONFIG_H
#define MCONFIG_H

/* Default configuration struct */
typedef struct {
    char *group;
    char *name;
    long inum;
    double fnum;
    char *str;
} config_default_t;

/* Functions */
void config_print(config_t *cfg);
void config_check(config_t *cfg);

#endif                          /* CFG_H */
