#include <igraph.h>

/**
 * Read a box template file, replace the generic patterns and insert it to the
 * target file.
 *
 * @param g         pointer to the dependency graph
 * @param id        id of a box
 * @param name      name of a box
 * @param tpl_path  path to the template file
 * @param tgt_path  path to the target file
 */
void smxgen_box_file( igraph_t* g, int id, const char* name,
        const char* tpl_path, const char* tgt_path );

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
 * Copies the template files to the output and replaces the generic patterns
 * with concrete content.
 *
 * @param g         pointer to the dependency graph
 */
void smxgen_tpl( igraph_t* g );
