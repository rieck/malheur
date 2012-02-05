/*
 * MALHEUR - Automatic Analysis of Malware Behavior
 * Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)
 * University of Goettingen, Berlin Institute of Technology 
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 * --
 */

#ifndef TESTS_H
#define TESTS_H

#include "config.h"
#include "common.h"
#include "util.h"

/* With of text line */
#define LINE_WIDTH          60

/* Macros for faking a configuration */
#define config_set_string(c,x,s) \
      config_setting_set_string(config_lookup(c,x),s)
#define config_set_int(c,x,s) \
      config_setting_set_int(config_lookup(c,x),s)
#define config_set_float(c,x,s) \
      config_setting_set_float(config_lookup(c,x),s)

/* Functions */
void test_printf(char *fmt, ...);
void test_return(int, int);
void test_error(char *fmt, ...);

#endif
