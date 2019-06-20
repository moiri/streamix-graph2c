#include <igraph.h>

#ifndef TPL_H
#define TPL_H

#define APP_NAME_PATTERN      "<app_name>"
#define APP_CONF_PATTERN      "<box_configs>"
#define APP_INC_PATTERN       "<app_includes>"
#define APP_NW_PATTERN        "<smx_network>"
#define BOX_NAME_PATTERN      "<box_name>"
#define BOX_PORTS_PATTERN     "<box_ports_custom>"
#define BOX_PORTS_IN_PATTERN  "<box_ports_in>"
#define BOX_PORTS_OUT_PATTERN "<box_ports_out>"
#define BOX_SIG_PATTERN       "<box_signature>"
#define PORT_NAME_PATTERN     "<port_name>"
#define PORT_MODE_PATTERN     "<port_mode>"

#define TPL_PATH            "/opt/smx/tpl"
#define TPL_APP_MK          TPL_PATH "/app_mk"
#define TPL_APP_XML         TPL_PATH "/app_xml"
#define TPL_APP_XML_BOX     TPL_PATH "/tpl_box_xml"
#define TPL_APP_LOG         TPL_PATH "/app_zlog"
#define TPL_APP_MAIN        TPL_PATH "/main_c"
#define TPL_BOX_PATH        TPL_PATH "/box"
#define TPL_BOX_MK          TPL_BOX_PATH "/box_mk"
#define TPL_BOX_GITIGNORE   TPL_BOX_PATH "/.gitignore"
#define TPL_BOX_H           TPL_BOX_PATH "/box_h"
#define TPL_BOX_PORT        TPL_BOX_PATH "/tpl_port_h"
#define TPL_BOX_C           TPL_BOX_PATH "/box_c"
#define TPL_BOX_CONF        TPL_BOX_PATH "/box_xml"
#define TPL_BOX_DEB         TPL_BOX_PATH "/box_deb"
#define TPL_BOX_DEB_DEV     TPL_BOX_PATH "/box-dev_deb"
#define TPL_BOX_SIG         TPL_BOX_PATH "/box_sig_h"
#define TPL_BOX_SIG_PORT    TPL_BOX_PATH "/tpl_port_sig_h"

/**
 * Read an app template file, replace the generic patterns and insert it to the
 * target file.
 *
 * @param g         pointer to the dependency graph
 * @param name      name of the app
 * @param tpl_path  path to the template file
 * @param tgt_path  path to the target file
 */
void smxgen_app_file( igraph_t* g, const char* name, const char* tpl_path,
        const char* tgt_path );

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
 */
void smxgen_box_file_path( igraph_t* g, int id, const char* name,
        const char* tpl_path, const char* tgt_path );

/**
 * Insert the box configurations to the configuration file.
 *
 * @param g         pointer to the dependency graph
 * @param ftgt      file descriptor to the target file
 */
void smxgen_insert_conf( igraph_t* g, FILE* ftgt );

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
 * Read a port template file, replace the generic patterns and insert it to the
 * target file
 *
 * @param box_name  name of the box
 * @param port_name name of the port
 * @param port_mode port direction string
 * @param tpl_path  path to the template file
 * @param ftgt      file descriptor to the target file
 */
void smxgen_port_file( const char* box_name, const char* port_name,
        const char* port_mode, const char* tpl_path, FILE* ftgt );

/**
 * Replace all occurrences of a given a word in string.
 *
 * @param str       the string to be modified
 * @param old_word  the word to be replaced
 * @param new_word  the replacing word
 */
void smxgen_replace( char* str, const char* old_word, const char* new_word );

/**
 * Copies the box template files to the output and replaces the generic patterns
 * with concrete content.
 *
 * @param g         pointer to the dependency graph
 */
void smxgen_tpl_box( igraph_t* g );

/**
 * Copies the main c file to the output and replaces the generic patterns.
 *
 * @param name      the app name
 * @param g         pointer to the dependency graph
 */
void smxgen_tpl_main( const char* name, igraph_t* g );

#endif
