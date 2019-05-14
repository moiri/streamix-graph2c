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

/**
 * @brief checks whether a net is a certain type of net
 *
 * @param g     pointer to the dependency graph
 * @param vid   id of a vertex of the dependency graph
 * @param type  type string to check against
 * @return      1 if the type matches, 0 otherwise
 */
int smxgen_box_is_type( igraph_t* g, int vid, const char* type );
void smxgen_net_structs( igraph_t*, int );
void smxgen_box_structs_ports( igraph_t*, int, int, int );
void smxgen_box_fct_defs( igraph_t*, int );
void smxgen_box_fct_prots( igraph_t*, int );
void smxgen_boxes_c( igraph_t* );
void smxgen_boxes_h( igraph_t* );
const char* smxgen_get_port_name( igraph_t*, int, int );
void smxgen_main( const char*, igraph_t* );
void smxgen_network_create( igraph_t*, int, int*, int* );
void smxgen_network_create_timer( igraph_t* g, int ident, int eid, int edge_cnt,
        int ch_len, struct timespec* tt, int* tt_vcnt, int* tt_ecnt );
void smxgen_network_destroy( igraph_t*, int, int, int);
void smxgen_network_run( igraph_t*, int, int );
void smxgen_network_wait_end( igraph_t*, int, int );
int smxgen_timer_is_duplicate( struct timespec tt_elem, struct timespec* tt,
        int len );

#endif /* SMXGEN_H */
