/**
 * Generates the Streamix SIA code
 *
 * @file    siagen.c
 * @author  Simon Maurer
 *
 */

#include "siagen.h"
#include "defines.h"

/******************************************************************************/
void siagen( igraph_t* g, sia_t** symbols )
{
    int vid;
    igraph_vs_t vs;
    igraph_vit_t vit;
    sia_t* sia;
    const char* impl_name;
    char* name;

    vs = igraph_vss_all();
    igraph_vit_create( g, vs, &vit );
    // iterate through all net instances of the graph
    while( !IGRAPH_VIT_END( vit ) ) {
        vid = IGRAPH_VIT_GET( vit );
        impl_name = igraph_cattribute_VAS( g, GV_IMPL, vid );
        if( strcmp( impl_name, CP_SYNC_STR ) == 0 ) {
            sia = siagen_cpsync( g, vid );
            name = malloc( strlen( CP_SYNC_STR ) + CONST_ID_LEN + 2 );
            sprintf( name, "%s_%d", CP_SYNC_STR, vid );
            sia->smx_name = name;
            HASH_ADD( hh_smx, *symbols, smx_name, strlen( sia->smx_name ),
                    sia );
        }
        IGRAPH_VIT_NEXT( vit );
    }
    igraph_vit_destroy( &vit );
    igraph_vs_destroy( &vs );
}

/******************************************************************************/
sia_t* siagen_cpsync( igraph_t* nw, int vid )
{
    int eid, smx_id, id_dst, id_src = 1;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    const char* name;
    char* edge_id;
    sia_t* sia = sia_create( NULL, NULL );
    igraph_add_vertices( &sia->g, 2, NULL );

    // handle all input actions
    igraph_es_incident( &e_sel, vid, IGRAPH_IN );
    igraph_eit_create( nw, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        eid = IGRAPH_EIT_GET( e_it );
        smx_id = igraph_cattribute_EAN( nw, GE_SIA, eid );
        name = igraph_cattribute_EAS( nw, GE_LABEL, eid );
        edge_id = malloc( CONST_ID_LEN + 1 );
        sprintf( edge_id, "%d", smx_id );
        sia_add_edge( &sia->g, 0, 1, edge_id, name, G_SIA_MODE_IN );
        free( edge_id );
        IGRAPH_EIT_NEXT( e_it );
    }

    // handle all output actions
    igraph_es_incident( &e_sel, vid, IGRAPH_OUT );
    igraph_eit_create( nw, e_sel, &e_it );
    igraph_add_vertices( &sia->g, IGRAPH_EIT_SIZE( e_it ) - 1, NULL );
    while( !IGRAPH_EIT_END( e_it ) ) {
        eid = IGRAPH_EIT_GET( e_it );
        smx_id = igraph_cattribute_EAN( nw, GE_SIA, eid );
        name = igraph_cattribute_EAS( nw, GE_LABEL, eid );
        id_dst = id_src + 1;
        if( id_dst == igraph_vcount( &sia->g ) ) id_dst = 0;
        edge_id = malloc( CONST_ID_LEN + 1 );
        sprintf( edge_id, "%d", smx_id );
        sia_add_edge( &sia->g, id_src, id_dst, edge_id, name,
                G_SIA_MODE_OUT );
        free( edge_id );
        id_src++;
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );
    return sia;
}

/******************************************************************************/
void siagen_destroy( sia_t** smx_symbols )
{
    sia_t* sia;
    sia_t* sia_tmp;
    HASH_ITER( hh_smx, *smx_symbols, sia, sia_tmp ) {
        HASH_DELETE( hh_smx, *smx_symbols, sia );
        sia_destroy( sia );
    }
}

/******************************************************************************/
void siagen_write( sia_t** symbols, const char* out_path, const char* format )
{
    sia_t* sia;
    sia_t* tmp;

    HASH_ITER( hh_smx, *symbols, sia, tmp ) {
        sia_write( sia, sia->smx_name, out_path, format );
    }
}
