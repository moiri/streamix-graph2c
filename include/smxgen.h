/**
 * Generates the Streamix C code
 *
 * @file    smxgen.h
 * @author  Simon Maurer
 */

#ifndef SMXGEN_H
#define SMXGEN_H

#include <igraph.h>

/**
 * Checks wether a box name already exists
 *
 * @param name  the name of the box to check
 * @param names the list of existing box names
 * @param len   the length of the existing box array
 * @return      1 if suplicate exists, 0 otherwise
 */
int smxgen_box_is_duplicate( const char* name, const char** names, int len );

/**
 * Checks whether a box is externally defined
 *
 * @param g     pointer to the dependency graph
 * @param vid   id of a vertex of the dependency graph
 * @return      1 if the box is externally defined, 0 otherwise
 */

int smxgen_box_is_extern( igraph_t* g, int vid );
/**
 * @brief checks whether a net is a certain type of net
 *
 * @param g     pointer to the dependency graph
 * @param vid   id of a vertex of the dependency graph
 * @param type  type string to check against
 * @return      1 if the type matches, 0 otherwise
 */
int smxgen_net_is_type( igraph_t* g, int vid, const char* type );

/**
 * Generates the include in the main file.
 *
 * @param g     pointer to the dependency graph
 */
void smxgen_main_includes( igraph_t* g );

/**
 * Generate the signatures of all nets
 *
 * @param g     pointer to the dependency graph
 * @param ident identation string
 */
void smxgen_net_structs( igraph_t* g, int ident );

/**
 * Generate the port entries of the net signatures for a specified mode
 *
 * @param g     pointer to the dependency graph
 * @param ident identation string
 * @param vid   the net id
 * @param mode  the mode of the ports
 */
void smxgen_box_structs_ports( igraph_t* g, int ident, int vid, int mode );

/**
 * Generate the box function definitions.
 *
 * @param g     pointer to the dependency graph
 * @param ident identation string
 */
void smxgen_box_fct_defs( igraph_t* g, int ident );

/**
 * Generate the box function prototypes.
 *
 * @param g     pointer to the dependency graph
 * @param ident identation string
 */
void smxgen_box_fct_prots( igraph_t* g, int ident );

/**
 * Generate the C file holding the startup routines of the nets.
 *
 * @param g     pointer to the dependency graph
 */
void smxgen_boxes_c( igraph_t* );

/**
 * Generate the H file holding the startup routines of the nets.
 *
 * @param g     pointer to the dependency graph
 */
void smxgen_boxes_h( igraph_t* );

/**
 * Get the name of a port given the edge id and the mode of the port.
 *
 * @param g     pointer to the dependency graph
 * @param eid   id of the graph edge
 * @param mode  the mode of the port
 * @return      the name of the port or NULL
 */
const char* smxgen_get_port_name( igraph_t* g, int eid, int mode );

/**
 * Generate the main application C file
 *
 * @param file_name name of the main app file
 * @param g         pointer to the dependency graph
 */
void smxgen_main( const char* file_name, igraph_t* g );

/**
 * Generate the smx network of the application.
 *
 * @param g         pointer to the dependency graph
 * @param ident     identation string
 * @param tt_vcnt   pointer to the tt vertice counter
 * @param tt_ecnt   pointer to the tt edge counter
 */
void smxgen_network_create( igraph_t* g, int ident, int* tt_vcnt, int* tt_ecnt );

/**
 * Generate the temporal firewalls
 *
 * @param g         pointer to the dependency graph
 * @param ident     identation string
 * @param eid       the id of the edge with timing annotation
 * @param edge_cnt  the number of edges connected
 * @param ch_len    the length of the annotated edge
 * @param tt        pointer to an array of all timer structures
 * @param tt_vcnt   pointer to the tt vertice counter
 * @param tt_ecnt   pointer to the tt edge counter
 */
void smxgen_network_create_timer( igraph_t* g, int ident, int eid, int edge_cnt,
        int ch_len, struct timespec* tt, int* tt_vcnt, int* tt_ecnt );

/**
 * Generate thi instructions to destroy the complete smx network
 *
 * @param g         pointer to the dependency graph
 * @param ident     identation string
 * @param tt_vcnt   the tt vertice counter
 * @param tt_ecnt   the tt edge counter
 */
void smxgen_network_destroy( igraph_t* g, int ident, int tt_vcnt, int tt_ecnt );

/**
 * Generate the instructions to run the network
 *
 * @param g         pointer to the dependency graph
 * @param ident     identation string
 * @param tt_vcnt   the tt vertice counter
 */
void smxgen_network_run( igraph_t* g, int ident, int tt_vcnt );

/**
 * Generate the instructions to sync the network on termination
 *
 * @param g         pointer to the dependency graph
 * @param ident     identation string
 * @param tt_vcnt   the tt vertice counter
 */
void smxgen_network_wait_end( igraph_t* g, int ident, int tt_vcnt );

/**
 * Checks wether a timer with the same settings already exists
 *
 * @param tt_elem   the timer structure to check
 * @param tt        pointer to an array of all timer structures
 * @param len       the number of existing tt elements
 * @return          1 if a timer already exists, 0 otherwise
 */
int smxgen_timer_is_duplicate( struct timespec tt_elem, struct timespec* tt,
        int len );

#endif /* SMXGEN_H */
