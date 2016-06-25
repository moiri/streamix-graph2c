/* 
 * Generates the Streamix C code
 *
 * @file    smxgen.h
 * @author  Simon Maurer
 *
 * */

#ifndef SMXGEN_H
#define SMXGEN_H

#include <igraph.h>

void smxgen_box_structs( igraph_t*, int );
void smxgen_box_fct_defs( igraph_t*, int );
void smxgen_box_fct_prots( igraph_t*, int );
void smxgen_boxes_c( igraph_t* );
void smxgen_boxes_h( igraph_t* );
void smxgen_main( igraph_t* );
void smxgen_network_create( igraph_t*, int );
void smxgen_network_destroy( igraph_t*, int );
void smxgen_network_run( igraph_t*, int );
void smxgen_network_wait_end( igraph_t*, int );

#endif /* ifndef SMXGEN_H */
