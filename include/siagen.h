/**
 * Generates the Streamix SIA code
 *
 * @file    siagen.h
 * @author  Simon Maurer
 *
 */

#ifndef SIAGEN_H
#define SIAGEN_H

#include <igraph.h>
#include "sia.h"

/**
 * @brief Generate SIAs for all boxes
 *
 * @param igraph_t*     pointer to the streamix graph
 * @param sia_t**       pointer to the SIA symbol table
 */
void siagen( igraph_t*, igraph_t*, sia_t** );

/**
 * @brief Generate a SIA for a cannel
 *
 * @param igraph_t*     pointer to the dependency graph
 * @param int           id of the edge that is replaced by the channel
 * @param const char*   sia id of the producer vertex
 * @param const char*   sia id of the consumer vertex
 * @param int           id of the channel
 */
sia_t* siagen_channel( igraph_t*, int, const char*, const char*, int );

/**
 * @brief Generate a SIA for a cp-sync
 *
 * @param igraph_t* pointer to the dependency graph
 * @param int       vertex id of the current vertex on the dependency graph
 * @return sia_t*   pointer to the generated SIA structure
 */
sia_t* siagen_cpsync( igraph_t*, int );

/**
 * @brief destroy all sia structures and its corresponding sub structures
 *
 * @param sia_t**   pointer to the symbol table of sias
 */
void siagen_destroy( sia_t** );

/**
 *
 */
void siagen_insert_channels( igraph_t*, igraph_t*, sia_t** );

/**
 * @brief Write out the graph files of the sias
 *
 * @param sia_t**       pointer to the symbol table of sias
 * @param const char*   output path where the files will be stored
 * @param const char*   format string, either 'gml' or 'graphml'
 */
void siagen_write( sia_t**, const char*, const char* );

#endif /* SIAGEN_H */
