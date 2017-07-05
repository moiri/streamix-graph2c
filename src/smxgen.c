#include "smxgen.h"
#include "codegen.h"
#include <defines.h>
#include <igraph.h>
#include <string.h>

int smxgen_box_is_duplicate( const char* name, const char** names, int len )
{
    int i;
    for( i=0; i<len; i++ )
        if( ( names[i] != NULL ) && ( strcmp( names[i], name ) == 0 ) )
            return 1;
    return 0;
}

int smxgen_box_is_cp_sync( igraph_t* g, int vid )
{
    const char* impl_name = igraph_cattribute_VAS( g, GV_IMPL, vid );
    if( strcmp( impl_name , CP_SYNC_STR ) == 0 )
        return 1;
    return 0;
}

/******************************************************************************/
void smxgen_box_structs( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, idx = 0, box_count = igraph_vcount( g ), i;
    const char* box_names[box_count];
    const char* box_name;
    for( i=0; i<box_count; i++ )
        box_names[i] = NULL;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        vid = IGRAPH_VIT_GET( v_it );
        // store name of vertex to avoid duplicates
        box_name = igraph_cattribute_VAS( g, GV_IMPL, vid );
        if( smxgen_box_is_cp_sync( g, vid )
                || smxgen_box_is_duplicate( box_name, box_names, box_count ) ) {
            IGRAPH_VIT_NEXT( v_it );
            continue;
        }
        box_names[idx++] = box_name;
        // generate box structure code
        cgen_box_struct_head( ident, box_name );
        ident++;
        // for all incident channels of this box
        smxgen_box_structs_ports( g, ident, vid, IGRAPH_IN );
        smxgen_box_structs_ports( g, ident, vid, IGRAPH_OUT );
        ident--;
        cgen_box_struct_tail( ident,
                igraph_cattribute_VAS( g, GV_IMPL, vid ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}

/******************************************************************************/
const char* smxgen_get_port_name( igraph_t* g, int eid, int mode )
{
    const char* name = NULL;
    if( mode == IGRAPH_IN )
        name = igraph_cattribute_EAS( g, GE_NDST, eid );
    else if( mode == IGRAPH_OUT )
        name = igraph_cattribute_EAS( g, GE_NSRC, eid );
    if( strcmp( name, TEXT_NULL ) == 0 )
        name = igraph_cattribute_EAS( g, GE_LABEL, eid );
    return name;
}

/******************************************************************************/
void smxgen_box_structs_ports( igraph_t* g, int ident, int vid, int mode )
{
    int eid;
    const char* mode_str = ( mode == IGRAPH_IN ) ? MODE_IN : MODE_OUT;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    cgen_struct_head( ident );
    ident++;

    igraph_es_incident( &e_sel, vid, mode );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate port code
        eid = IGRAPH_EIT_GET( e_it );
        cgen_box_port( ident, smxgen_get_port_name( g, eid, mode ) );
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );
    cgen_box_ports( ident );
    ident--;
    cgen_struct_tail( ident, mode_str );
}

/******************************************************************************/
void smxgen_boxes_c( igraph_t* g )
{
    int ident = 0;
    cgen_header_c_file( FILE_BOX );
    cgen_include_local( FILE_BOX_H );
    cgen_include( FILE_ZLOG_H );
    cgen_print( "\n" );
    smxgen_box_fct_defs( g, ident );
}

/******************************************************************************/
void smxgen_boxes_h( igraph_t* g )
{
    int ident = 0;
    cgen_header_h_file( FILE_BOX );
    cgen_ifndef( FILE_BOX_IH );
    cgen_print( "\n" );
    cgen_include_local( FILE_SMX_H );
    cgen_print( "\n" );
    smxgen_box_structs( g, ident );
    smxgen_box_fct_prots( g, ident );
    cgen_print( "\n" );
    cgen_endif( FILE_BOX_IH );
}

/******************************************************************************/
void smxgen_box_fct_defs( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, idx = 0, box_count = igraph_vcount( g ), i;
    const char* box_names[box_count];
    const char* box_name;
    for( i=0; i<box_count; i++ )
        box_names[i] = NULL;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        vid = IGRAPH_VIT_GET( v_it );
        // store name of vertex to avoid duplicates
        box_name = igraph_cattribute_VAS( g, GV_IMPL, vid );
        if( smxgen_box_is_cp_sync( g, vid )
                || smxgen_box_is_duplicate( box_name, box_names, box_count ) ) {
            IGRAPH_VIT_NEXT( v_it );
            continue;
        }
        box_names[idx++] = box_name;
        // generate box function definitions
        cgen_box_fct_ext( ident, box_name );
        cgen_box_fct_head( ident, box_name );
        cgen_function_start( ident );
        ident++;
        cgen_box_zlog_start( ident, box_name );
        cgen_box_fct_call( ident, box_name );
        cgen_channels_terminate( ident, box_name );
        cgen_box_zlog_end( ident, box_name );
        cgen_box_fct_ret( ident );
        ident--;
        cgen_function_end( ident );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}

/******************************************************************************/
void smxgen_box_fct_prots( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, idx = 0, box_count = igraph_vcount( g ), i;
    const char* box_names[box_count];
    const char* box_name;
    for( i=0; i<box_count; i++ )
        box_names[i] = NULL;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        vid = IGRAPH_VIT_GET( v_it );
        // store name of vertex to avoid duplicates
        box_name = igraph_cattribute_VAS( g, GV_IMPL, vid );
        if( smxgen_box_is_cp_sync( g, vid )
                || smxgen_box_is_duplicate( box_name, box_names, box_count ) ) {
            IGRAPH_VIT_NEXT( v_it );
            continue;
        }
        box_names[idx++] = box_name;
        // generate box function prototypes
        cgen_box_fct_proto( ident, box_name );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}

/******************************************************************************/
void smxgen_main( const char* file_name, igraph_t* g )
{
    int ident = 0;
    cgen_header_c_file( file_name );
    cgen_include_local( FILE_SMX_H );
    cgen_include_local( FILE_BOX_H );
    cgen_print( "\n" );
    cgen_main_head();
    cgen_function_start( ident );
    ident++;
    cgen_program_init( ident );
    cgen_print( "\n" );
    smxgen_network_create( g, ident );
    cgen_print( "\n" );
    smxgen_network_run( g, ident );
    cgen_print( "\n" );
    smxgen_network_wait_end( g, ident );
    cgen_print( "\n" );
    smxgen_network_destroy( g, ident );
    cgen_print( "\n" );
    cgen_program_cleanup( ident );
    ident--;
    cgen_function_end( ident );
}

/******************************************************************************/
void smxgen_network_create( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vs_t v_cp;
    igraph_vit_t v_it;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    int eid, vid1, vid2, ch_len;
    igraph_vector_t indegree, outdegree;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate box creation code
        vid1 = IGRAPH_VIT_GET( v_it );
        cgen_box_create( ident, vid1,
                igraph_cattribute_VAS( g, GV_IMPL, vid1 ) );
        igraph_vs_1( &v_cp, vid1 );
        igraph_vector_init( &indegree, 1 );
        igraph_vector_init( &outdegree, 1 );
        igraph_degree( g, &indegree, v_cp, IGRAPH_IN, 1 );
        igraph_degree( g, &outdegree, v_cp, IGRAPH_OUT, 1 );
        cgen_box_init( ident, igraph_cattribute_VAS( g, GV_IMPL, vid1 ), vid1,
                VECTOR( indegree )[0], VECTOR( outdegree )[0] );
        if( smxgen_box_is_cp_sync( g, vid1 ) ) {
            cgen_box_cp_init( ident, vid1 );
        }
        igraph_vs_destroy( &v_cp );
        igraph_vector_destroy( &indegree );
        igraph_vector_destroy( &outdegree );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
    // for all channels in the scope
    e_sel = igraph_ess_all( IGRAPH_EDGEORDER_ID );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate channel creation code
        eid = IGRAPH_EIT_GET( e_it );
        ch_len = igraph_cattribute_EAN( g, GE_LEN, eid );
        cgen_channel_create( ident, eid,
                igraph_cattribute_EAN( g, GE_DSRC, eid ),
                igraph_cattribute_EAN( g, GE_DDST, eid ), ch_len );
        // generate connection code for a channel and its connecting boxes
        igraph_edge( g, eid, &vid1, &vid2 );
        cgen_connect( ident, eid, vid1,
                igraph_cattribute_VAS( g, GV_IMPL, vid1 ),
                smxgen_get_port_name( g, eid, IGRAPH_OUT ), MODE_OUT,
                smxgen_box_is_cp_sync( g, vid1 ) );
        cgen_connect( ident, eid, vid2,
                igraph_cattribute_VAS( g, GV_IMPL, vid2 ),
                smxgen_get_port_name( g, eid, IGRAPH_IN ), MODE_IN,
                smxgen_box_is_cp_sync( g, vid2 ) );
        if( smxgen_box_is_cp_sync( g, vid2 ) )
            cgen_connect_cp( ident, eid, vid2 );
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );
}

/******************************************************************************/
void smxgen_network_destroy( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    int eid, vid1, vid2;
    // for all channels in the scope
    e_sel = igraph_ess_all( IGRAPH_EDGEORDER_ID );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate channel destruction code
        eid = IGRAPH_EIT_GET( e_it );
        igraph_edge( g, eid, &vid1, &vid2 );
        cgen_channel_destroy( ident, eid );
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate box destruction code
        vid1 = IGRAPH_VIT_GET( v_it );
        cgen_box_destroy( ident, igraph_cattribute_VAS( g, GV_IMPL, vid1 ),
                vid1, smxgen_box_is_cp_sync( g, vid1 ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}

/******************************************************************************/
void smxgen_network_run( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate code to run boxes
        vid = IGRAPH_VIT_GET( v_it );
        cgen_box_run( ident, vid, igraph_cattribute_VAS( g, GV_IMPL, vid ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}

/******************************************************************************/
void smxgen_network_wait_end( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate box waiting code
        vid = IGRAPH_VIT_GET( v_it );
        cgen_box_wait_end( ident, vid );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}
