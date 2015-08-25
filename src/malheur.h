/*
 * MALHEUR - Automatic Analysis of Malware Behavior
 * Copyright (c) 2009-2015 Konrad Rieck (konrad@mlsec.org)
 * University of Goettingen, Berlin Institute of Technology
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 * --
 */

#ifndef MALHEUR_H
#define MALHEUR_H

/* Operation actions of Malheur  */
typedef enum {
    DISTANCE, PROTOTYPE, CLUSTER, CLASSIFY, INCREMENT, PROTODIST, INFO
} malheur_action_t;

/* Output file */
#define OUTPUT_FILE         "malheur.out"

/* Local malheur files */
#define REJECT_FILE         "rejected.zfa"
#define PROTO_FILE          "prototypes.zfa"
#define STATE_FILE          "malheur.state"

typedef struct {
    unsigned int run;                    /* Current run */
    unsigned int num_proto;              /* Number of prototype reports */
    unsigned int num_reject;             /* Number of rejected reports */
} malheur_state_t;

/* Libconfig macros */
#define config_set_string(c,x,s) \
      config_setting_set_string(config_lookup(c,x),s)
#define config_set_int(c,x,s) \
      config_setting_set_int(config_lookup(c,x),s)
#define config_set_float(c,x,s) \
      config_setting_set_float(config_lookup(c,x),s)

#endif                          /* MALHEUR_H */

