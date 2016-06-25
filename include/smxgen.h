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

void smxgen_main( igraph_t* );
void smxgen_network_create( igraph_t*, int );
void smxgen_network_destroy( igraph_t*, int );
void smxgen_network_run( igraph_t*, int );
void smxgen_network_wait_end( igraph_t*, int );

#endif /* ifndef SMXGEN_H */
