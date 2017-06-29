/**
 * Generates the Streamix C code
 *
 * @file    smxgen.h
 * @author  Simon Maurer
 *
 */

#ifndef SMXGEN_H
#define SMXGEN_H

#include <igraph.h>

int smxgen_box_is_duplicate( const char*, const char**, int );
int smxgen_box_is_cp_sync( igraph_t*, int );
void smxgen_box_structs( igraph_t*, int );
void smxgen_box_structs_ports( igraph_t*, int, int, int );
void smxgen_box_fct_defs( igraph_t*, int );
void smxgen_box_fct_prots( igraph_t*, int );
void smxgen_boxes_c( igraph_t* );
void smxgen_boxes_h( igraph_t* );
void smxgen_main( const char*, igraph_t* );
void smxgen_network_create( igraph_t*, int );
void smxgen_network_destroy( igraph_t*, int );
void smxgen_network_run( igraph_t*, int );
void smxgen_network_wait_end( igraph_t*, int );

#endif /* SMXGEN_H */
