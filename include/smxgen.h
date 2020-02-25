/**
 * Generates the Streamix C code
 *
 * @file    smxgen.h
 * @author  Simon Maurer
 */

#ifndef SMXGEN_H
#define SMXGEN_H

#include <igraph.h>
#include <stdbool.h>

#define PATH_INCLUDE_PATTERN  "<include_path>"
#define PATH_LIB_PATTERN      "<lib_path>"
#define PATH_BIN_PATTERN      "<bin_path>"
#define PATH_DOC_PATTERN      "<doc_path>"
#define PATH_CONF_PATTERN     "<conf_path>"
#define YEAR_PATTERN          "<date_y>"
#define AUTHOR_PATTERN        "<author>"
#define APP_NAME_PATTERN      "<app_name>"
#define BIN_NAME_PATTERN      "<bin_name>"
#define APP_DEP_PATTERN       "<box_dependencies>"
#define APP_REL_PATTERN       "<box_relations>"
#define RTS_DEP_PATTERN       "<rts_dependency>"
#define APP_CONF_PATTERN      "<box_configs>"
#define APP_CONF_NET_PATTERN  "<net_configs>"
#define APP_CONF_INST_PATTERN "<net_inst_configs>"
#define APP_INC_PATTERN       "<app_includes>"
#define APP_NW_PATTERN        "<smx_network>"
#define BOX_NAME_PATTERN      "<box_name>"
#define NET_NAME_PATTERN      "<net_name>"
#define NET_ID_PATTERN        "<net_id>"
#define BOX_LIB_PATTERN       "<lib_name>"
#define BOX_MSG_PATTERN       "<box_msg_types>"
#define BOX_PORTS_IN_PATTERN  "<box_ports_in>"
#define BOX_PORTS_OUT_PATTERN "<box_ports_out>"
#define BOX_SIG_PATTERN       "<box_signature>"
#define PORT_NAME_PATTERN     "<port_name>"
#define PORT_MODE_PATTERN     "<port_mode>"
#define CH_ID_PATTERN         "<ch_id>"
#define BOX_IN_CASE_PATTERN   "<box_ports_in_case>"
#define BOX_OUT_CASE_PATTERN  "<box_ports_out_case>"
#define BOX_IN_CONF_PATTERN   "<box_ports_in_conf>"
#define BOX_OUT_CONF_PATTERN  "<box_ports_out_conf>"
#define NET_IN_CH_PATTERN     "<net_ch_in>"
#define NET_OUT_CH_PATTERN    "<net_ch_out>"
#define NET_IN_CH_RM_PATTERN  "<net_ch_rm_in>"
#define NET_OUT_CH_RM_PATTERN "<net_ch_rm_out>"
#define INDEGREE_PATTERN      "<in_degree>"
#define OUTDEGREE_PATTERN     "<out_degree>"

#define TPL_INCLUDE_PATH    "/usr/include/smx"
#define TPL_LIB_PATH        "/usr/lib/x86_64-linux-gnu"
#define TPL_BIN_PATH        "/usr/bin"
#define TPL_DOC_PATH        "/usr/share/doc"
#define TPL_CONF_PATH       "/etc/smx"
#define TPL_PATH            "/opt/smx/tpl"

#define TPL_PATH_APP        TPL_PATH "/app"
#define TPL_PATH_APP_DPKG   TPL_PATH_APP "/debian"
#define TPL_APP_GITIGNORE   TPL_PATH_APP "/_gitignore"
#define TPL_APP_README      TPL_PATH_APP "/README_md"
#define TPL_APP_MK          TPL_PATH_APP "/app_mk"
#define TPL_APP_CONF_MK     TPL_PATH_APP "/config_mk"
#define TPL_APP_JSON        TPL_PATH_APP "/app_json"
#define TPL_IMPL_JSON       TPL_PATH_APP "/tpl_impl_json"
#define TPL_NET_JSON        TPL_PATH_APP "/tpl_net_json"
#define TPL_INST_JSON       TPL_PATH_APP "/tpl_inst_json"
#define TPL_APP_LOG         TPL_PATH_APP "/app_zlog"
#define TPL_APP_MAIN        TPL_PATH_APP "/main_c"
#define TPL_BOX_PATH        TPL_PATH "/box"
#define TPL_BOX_GITIGNORE   TPL_BOX_PATH "/_gitignore"
#define TPL_BOX_DOXYGEN     TPL_BOX_PATH "/_doxygen"
#define TPL_BOX_CONF        TPL_BOX_PATH "/box_json"
#define TPL_BOX_MK          TPL_BOX_PATH "/box_mk"
#define TPL_BOX_CONF_MK     TPL_BOX_PATH "/config_mk"
#define TPL_BOX_README      TPL_BOX_PATH "/README_md"
#define TPL_BOX_DPKG        TPL_BOX_PATH "/debian"
#define TPL_BOX_C           TPL_BOX_PATH "/src/box_c"
#define TPL_BOX_H           TPL_BOX_PATH "/include/box_h"
#define TPL_BOX_SIG_H       TPL_BOX_PATH "/include/box_sig_h"
#define TPL_BOX_SIG_PORT    TPL_BOX_PATH "/include/tpl_port_sig_h"
#define TPL_BOX_TEST_PATH   TPL_BOX_PATH "/test"
#define TPL_BOX_TEST_MK     TPL_BOX_TEST_PATH "/test_mk"
#define TPL_BOX_TEST_H      TPL_BOX_TEST_PATH "/test_h"
#define TPL_BOX_TEST_C      TPL_BOX_TEST_PATH "/test_c"
#define TPL_BOX_TEST_MAIN_C TPL_BOX_TEST_PATH "/main_c"
#define TPL_BOX_TEST_PORT   TPL_BOX_TEST_PATH "/tpl_port_c"
#define TPL_BOX_TEST_CH     TPL_BOX_TEST_PATH "/tpl_ch_c"
#define TPL_BOX_TEST_CH_RM  TPL_BOX_TEST_PATH "/tpl_ch_rm_c"
#define TPL_BOX_TEST_JSON   TPL_BOX_TEST_PATH "/test_json"
#define TPL_BOX_TEST_ZLOG   TPL_BOX_TEST_PATH "/test_zlog"
#define TPL_BOX_TEST_P_JSON TPL_BOX_TEST_PATH "/tpl_port_json"

/**
 * Read an app template file, replace the generic patterns and insert it to the
 * target file.
 *
 * @param g         pointer to the dependency graph
 * @param tpl_path  path to the template file
 * @param tgt_path  path to the target file
 */
void smxgen_app_file( igraph_t* g, const char* tpl_path, const char* tgt_path );

/**
 * Traverse a directory tree and apply smxgen_app_file() on each file.
 *
 * @param g
 *  a pointer to the dependency graph
 * @param src_path
 *  the path to the source directory
 * @param tgt_path
 *  the path to the target directory
 * @return
 *  0 on success, -1 on failure.
 */
int smxgen_app_file_tree( igraph_t* g, char* src_path, const char* tgt_path );

/**
 * Read a box template file, replace the generic patterns and insert it to the
 * target file.
 *
 * @param g         pointer to the dependency graph
 * @param id        id of a box
 * @param name      name of a box
 * @param tpl_path  path to the template file
 * @param ftgt      pointer to the target file
 */
void smxgen_box_file( igraph_t* g, int id, const char* name,
        const char* tpl_path, FILE* ftgt );

/**
 * Wrapper for smxgen_box_file which opend a target file descriptor.
 *
 * @param g         pointer to the dependency graph
 * @param id        id of a box
 * @param name      name of a box
 * @param tpl_path  path to the template file
 * @param tgt_path  path to the target file
 * @param append    if true abbend to file, if false create new file
 */
void smxgen_box_file_path( igraph_t* g, int id, const char* name,
        const char* tpl_path, const char* tgt_path, bool append );

/**
 * Traverse a directory tree and apply smxgen_box_file_path() on each file.
 *
 * @param g
 *  a pointer to the dependency graph
 * @param id        id of a box
 * @param name      name of a box
 * @param src_path
 *  the path to the source directory
 * @param tgt_path
 *  the path to the target directory
 * @return
 *  0 on success, -1 on failure.
 */
int smxgen_box_file_tree( igraph_t* g, int id, const char* name,
        char* src_path, const char* tgt_path );

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
 * Generic wrapper file to replace instance or net tpl configuration files.
 *
 * @param g
 *  A pointer to the depedency graph
 * @param id
 *  The id of the net
 * @param impl
 *  The name of the box implementation
 * @param net
 *  The name of the net
 * @param tpl_path
 *  The path to the source tpl file
 * @param ftgt
 *  A file pointer to the target file
 */
void smxgen_conf_file( igraph_t* g, int id, const char* impl, const char* net,
        const char* tpl_path, FILE* ftgt );

/**
 * Generate the code to conncet an edge to a vertex given a mode.
 *
 * @param g         pointer to the dependency graph
 * @param ident     identation string
 * @param eid       id of the graph edge
 * @param vid       id of the vertex
 * @param mode      the mode of the port
 */
void smxgen_connect( igraph_t* g, int ident, int eid, int vid, int mode );

/**
 * Copy a file
 *
 * @param src
 *  The source file path
 * @param tgt
 *  The target file path
 * @return
 *  0 on success, -1 on failure
 */
int smxgen_cp_file( const char* src, const char* tgt );

/**
 *
 */
void smxgen_get_box_deps( igraph_t* g, char* deps, char* rels );

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
 * Get the curren year as string.
 *
 * @param year
 *  An allocated output buffer to store the year.
 */
void smxgen_get_year_str( char* year );

/**
 * Insert the box configurations to the configuration file.
 *
 * @param g         pointer to the dependency graph
 * @param ftgt      file descriptor to the target file
 */
void smxgen_insert_conf_impl( igraph_t* g, FILE* ftgt );

/**
 * Insert the net configurations to the implementation configuration entry.
 *
 * @param g         pointer to the dependency graph
 * @param ftgt      file descriptor to the target file
 * @param impl      the name of the implementation
 */
void smxgen_insert_conf_net( igraph_t* g, FILE* ftgt, const char* impl );

/**
 * Insert the net instance configurations to the net configuration entry.
 *
 * @param g         pointer to the dependency graph
 * @param ftgt      file descriptor to the target file
 * @param impl      the name of the implementation
 * @param net       the name of the net
 */
void smxgen_insert_conf_inst( igraph_t* g, FILE* ftgt, const char* impl,
        const char* net );

/**
 * Insert port templates to the target file.
 *
 * @param g         pointer to the dependency graph
 * @param id        id of a box
 * @param mode      the port direction
 * @param name      name of a box
 * @param tpl_path  path to the template file
 * @param ftgt      file descriptor to the target file
 * @return          the number of generated ports
 */
int smxgen_insert_ports( igraph_t* g, int id, igraph_neimode_t mode,
        const char* name, const char* tpl_path, FILE* ftgt );

/**
 * Insert the box signature to the target file.
 *
 * @param g        pointer to the dependency graph
 * @param id       id of the box
 * @param box_name name of the box
 * @param ftgt     file descriptor to the target file
 */
void smxgen_insert_sig( igraph_t* g, int id, const char* box_name, FILE* ftgt );

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
 * Checks whether a box is externally defined
 *
 * @param g     pointer to the dependency graph
 * @param vid   id of a vertex of the dependency graph
 * @return      1 if the box is externally defined, 0 otherwise
 */
int smxgen_net_is_extern( igraph_t* g, int vid );

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
 * Read a port template file, replace the generic patterns and insert it to the
 * target file
 *
 * @param eid       the edge id
 * @param box_name  name of the box
 * @param port_name name of the port
 * @param port_mode port direction string
 * @param tpl_path  path to the template file
 * @param ftgt      file descriptor to the target file
 */
void smxgen_port_file( int eid, const char* box_name, const char* port_name,
        const char* port_mode, const char* tpl_path, FILE* ftgt );

/**
 *
 */
void smxgen_read_dep( const char* libname, char* dep );

/**
 * Replace all occurrences of a given a word in string.
 *
 * @param str       the string to be modified
 * @param old_word  the word to be replaced
 * @param new_word  the replacing word
 */
void smxgen_replace( char* str, const char* old_word, const char* new_word );

/**
 * Replace all path patterns
 *
 * @param buffer
 *  pointer to the current line buffer
 */
void smxgen_replace_path( char* buffer );

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

/**
 * Convert a string to an alphanumeric string.
 *
 * @param dst   The target string
 * @param src   The source string
 */
void smxgen_to_alnum( char* dst, const char* src );

/**
 * Copies the box template files to the output and replaces the generic patterns
 * with concrete content.
 *
 * @param g          pointer to the dependency graph
 * @param box_path   path to the folder holding the box implementations
 * @param build_path path to the build folder
 */
void smxgen_tpl_box( igraph_t* g, char* box_path, char* build_path );

/**
 * Copies the main c file to the output and replaces the generic patterns.
 *
 * @param g          pointer to the dependency graph
 * @param build_path path to the build folder
 */
void smxgen_tpl_main( igraph_t* g, char* build_path );

#endif /* SMXGEN_H */
