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

#ifndef UTIL_H
#define UTIL_H

#include "config.h"

/* Progress bar stuff */
#define PROGBAR_LEN     48
#define PROGBAR_EMPTY   '.'
#define PROGBAR_FULL    '#'
#define PROGBAR_DONE    '='
#define PROGBAR_FRONT   '>'

/* Fatal message */
#ifndef fatal
#define fatal(...)     {err_msg("Error", __func__, __VA_ARGS__); exit(-1);} 
#endif
/* Error message */
#ifndef error
#define error(...)     {err_msg("Error", __func__, __VA_ARGS__);} 
#endif
/* Warning message */
#ifndef warning
#define warning(...)   {err_msg("Warning", __func__, __VA_ARGS__);} 
#endif

/* Error functions */
void err_msg(char *, const char *, char *, ...);
void prog_bar(double, double, double);
double time_stamp();
int decode_string(char *);
char *file_suffix(char *file);

#ifndef HAVE_FUNC_ROUND
double log2(double);
#endif
#ifndef HAVE_FUNC_LOG2
long round(double);
#endif

#endif                          /* UTIL_H */
