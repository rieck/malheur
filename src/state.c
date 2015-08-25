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

/** 
 * @defgroup state State for incremental analysis
 * Functions for handling the internal state of Malheur including 
 * current prototypes as well as rejected feature vectors.
 * @author Konrad Rieck
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "state.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/* Global state of tool */
static state_t state = { NULL };

/** @} */
