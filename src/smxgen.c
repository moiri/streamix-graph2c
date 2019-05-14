#include "smxgen.h"
#include "codegen.h"
#include <defines.h>
#include <igraph.h>
#include <string.h>
#include <time.h>

/******************************************************************************/
int smxgen_box_is_duplicate( const char* name, const char** names, int len )
{
    int i;
    for( i=0; i<len; i++ )
        if( ( names[i] != NULL ) && ( strcmp( names[i], name ) == 0 ) )
            return 1;
    return 0;
}

/******************************************************************************/
int smxgen_box_is_type( igraph_t* g, int vid, const char* type )
{
    const char* name = igraph_cattribute_VAS( g, GV_LABEL, vid );
    if( strcmp( name , type ) == 0 )
        return 1;
    return 0;
}

/******************************************************************************/
void smxgen_net_structs( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, idx = 0, net_count = igraph_vcount( g ), i;
    const char* net_names[net_count];
    const char* net_name;
    for( i=0; i<net_count; i++ )
        net_names[i] = NULL;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        vid = IGRAPH_VIT_GET( v_it );
        // store name of vertex to avoid duplicates
        net_name = igraph_cattribute_VAS( g, GV_LABEL, vid );
        if( smxgen_box_is_duplicate( net_name, net_names, net_count )
                || smxgen_box_is_type( g, vid, TEXT_CP )
                || smxgen_box_is_type( g, vid, TEXT_TF ) ) {
            IGRAPH_VIT_NEXT( v_it );
            continue;
        }
        net_names[idx++] = net_name;
        // generate box structure code
        cgen_net_struct_head( ident, net_name );
        ident++;
        // for all incident channels of this box
        smxgen_box_structs_ports( g, ident, vid, IGRAPH_IN );
        smxgen_box_structs_ports( g, ident, vid, IGRAPH_OUT );
        cgen_net_tt( ident );
        ident--;
        cgen_net_struct_tail( ident,
                igraph_cattribute_VAS( g, GV_LABEL, vid ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
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
        cgen_net_port( ident, smxgen_get_port_name( g, eid, mode ) );
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );
    cgen_net_ports( ident );
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
    smxgen_net_structs( g, ident );
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
        if( smxgen_box_is_duplicate( box_name, box_names, box_count ) ) {
            IGRAPH_VIT_NEXT( v_it );
            continue;
        }
        box_names[idx++] = box_name;
        // generate box function definitions
        cgen_box_fct_ext( ident, box_name );
        cgen_box_fct_head( ident, box_name );
        cgen_block_start( ident );
        ident++;
        cgen_box_body( ident, igraph_cattribute_VAS( g, GV_LABEL, vid ),
                box_name );
        ident--;
        cgen_block_end( ident );
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
        if( smxgen_box_is_duplicate( box_name, box_names, box_count ) ) {
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
void smxgen_main( const char* file_name, igraph_t* g )
{
    int ident = 0;
    int tt_vcnt = 0;
    int tt_ecnt = 0;
    cgen_header_c_file( file_name );
    cgen_include_local( FILE_SMX_H );
    cgen_include_local( FILE_BOX_H );
    cgen_print( "\n" );
    cgen_main_head();
    cgen_block_start( ident );
    ident++;
    cgen_program_init( ident );
    cgen_print( "\n" );
    smxgen_network_create( g, ident, &tt_vcnt, &tt_ecnt );
    cgen_print( "\n" );
    cgen_program_init_run( ident );
    cgen_print( "\n" );
    smxgen_network_run( g, ident, tt_vcnt );
    cgen_print( "\n" );
    smxgen_network_wait_end( g, ident, tt_vcnt );
    cgen_print( "\n" );
    smxgen_network_destroy( g, ident, tt_vcnt, tt_ecnt );
    cgen_print( "\n" );
    cgen_program_cleanup( ident );
    ident--;
    cgen_block_end( ident );
}

/******************************************************************************/
void smxgen_network_create( igraph_t* g, int ident, int* tt_vcnt, int* tt_ecnt )
{
    const char* name;
    const char* port_name;
    igraph_vs_t v_sel;
    igraph_vs_t v_cp;
    igraph_vit_t v_it;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    int edge_cnt = igraph_ecount( g );
    struct timespec tt[2*edge_cnt];
    int eid, vid1, vid2, ch_len, tt_type, i;
    igraph_vector_t indegree, outdegree;
    // init timer structures
    for( i=0; i<edge_cnt; i++ ) {
        tt[i].tv_sec = 0;
        tt[i].tv_nsec = 0;
    }
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate net creation code
        vid1 = IGRAPH_VIT_GET( v_it );
        name = igraph_cattribute_VAS( g, GV_LABEL, vid1 );
        cgen_net_create( ident, vid1, name );
        igraph_vs_1( &v_cp, vid1 );
        igraph_vector_init( &indegree, 1 );
        igraph_vector_init( &outdegree, 1 );
        igraph_degree( g, &indegree, v_cp, IGRAPH_IN, 1 );
        igraph_degree( g, &outdegree, v_cp, IGRAPH_OUT, 1 );
        cgen_net_init( ident, igraph_cattribute_VAS( g, GV_LABEL, vid1 ), vid1,
                VECTOR( indegree )[0], VECTOR( outdegree )[0] );
        if( smxgen_box_is_type( g, vid1, TEXT_CP ) ) {
            cgen_net_cp_init( ident, vid1 );
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
        tt_type = igraph_cattribute_EAN( g, GE_TYPE, eid );
        if( tt_type == TIME_TT ) {
            smxgen_network_create_timer( g, ident, eid, edge_cnt, ch_len, tt,
                    tt_vcnt, tt_ecnt );
        }
        else {
            port_name = smxgen_get_port_name( g, eid, IGRAPH_OUT );
            cgen_channel_create( ident, eid,
                    igraph_cattribute_EAN( g, GE_DSRC, eid ),
                    igraph_cattribute_EAN( g, GE_DDST, eid ),
                    ch_len, port_name );
            // generate connection code for a channel and its connecting boxes
            igraph_edge( g, eid, &vid1, &vid2 );
            cgen_connect( ident, eid, vid1,
                    igraph_cattribute_VAS( g, GV_LABEL, vid1 ),
                    smxgen_get_port_name( g, eid, IGRAPH_OUT ),
                    MODE_OUT, smxgen_box_is_type( g, vid1, TEXT_CP ) );
            cgen_connect( ident, eid, vid2,
                    igraph_cattribute_VAS( g, GV_LABEL, vid2 ),
                    smxgen_get_port_name( g, eid, IGRAPH_IN ),
                    MODE_IN, smxgen_box_is_type( g, vid2, TEXT_CP ) );
            if( smxgen_box_is_type( g, vid2, TEXT_CP ) )
                cgen_connect_cp( ident, eid, vid2 );
            if( tt_type == TIME_TB ) {
                cgen_connect_guard( ident, eid,
                        igraph_cattribute_EAN( g, GE_DTS, eid ),
                        igraph_cattribute_EAN( g, GE_DTNS, eid ) );
            }
        }
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );
}

/******************************************************************************/
void smxgen_network_create_timer( igraph_t* g, int ident, int eid, int edge_cnt,
        int ch_len, struct timespec* tt, int* tt_vcnt, int* tt_ecnt )
{
    struct timespec stt;
    struct timespec dtt;
    const char* port_name = igraph_cattribute_EAS( g, GE_LABEL, eid );
    int stt_idx, dtt_idx, ch_idx1, ch_idx2, ch_idx3, vid1, vid2;
    stt.tv_sec = igraph_cattribute_EAN( g, GE_STS, eid );
    stt.tv_nsec = igraph_cattribute_EAN( g, GE_STNS, eid );
    dtt.tv_sec = igraph_cattribute_EAN( g, GE_DTS, eid );
    dtt.tv_nsec = igraph_cattribute_EAN( g, GE_DTNS, eid );
    // create timer instance
    stt_idx = smxgen_timer_is_duplicate( stt, tt, 2*edge_cnt );
    if( stt_idx < 0 ) {
        stt_idx = *tt_vcnt;
        tt[stt_idx].tv_sec = stt.tv_sec;
        tt[stt_idx].tv_nsec = stt.tv_nsec;
        cgen_timer_create( ident, stt_idx, stt.tv_sec, stt.tv_nsec );
        ( *tt_vcnt )++;
    }
    dtt_idx = smxgen_timer_is_duplicate( dtt, tt, 2*edge_cnt );
    if( dtt_idx < 0 ) {
        dtt_idx = *tt_vcnt;
        tt[dtt_idx].tv_sec = dtt.tv_sec;
        tt[dtt_idx].tv_nsec = dtt.tv_nsec;
        cgen_timer_create( ident, dtt_idx, dtt.tv_sec, dtt.tv_nsec );
        ( *tt_vcnt )++;
    }
    ch_idx1 = eid;
    ch_idx2 = edge_cnt + *tt_ecnt;
    // create time-triggered channels
    if( ( stt.tv_sec == dtt.tv_sec )
            && ( stt.tv_nsec == dtt.tv_nsec ) ) {
        // both veryices are time-triggered at the same rate
        cgen_channel_create( ident, ch_idx1,
                igraph_cattribute_EAN( g, GE_DSRC, eid ), 1, 1, port_name );
        cgen_channel_create( ident, ch_idx2, 1,
                igraph_cattribute_EAN( g, GE_DDST, eid ), 1, port_name );
        cgen_connect_tt( ident, stt_idx, ch_idx1, ch_idx2, port_name );
        ( *tt_ecnt )++;
    }
    else if( ( ( stt.tv_sec != 0 ) || ( stt.tv_nsec != 0 ) )
            && ( ( dtt.tv_sec != 0 ) || ( dtt.tv_nsec != 0 ) ) ) {
        // both vertices are time-timetriggered at a different rate
        cgen_channel_create( ident, ch_idx1,
                igraph_cattribute_EAN( g, GE_DSRC, eid ), 1, 1, port_name );
        cgen_channel_create( ident, ch_idx2, 1, 1, 1, port_name );
        ( *tt_ecnt )++;
        cgen_connect_tt( ident, stt_idx, ch_idx1, ch_idx2, port_name );
        ch_idx3 = edge_cnt + *tt_ecnt;
        cgen_channel_create( ident, ch_idx3, 1,
                igraph_cattribute_EAN( g, GE_DDST, eid ), 1, port_name );
        cgen_connect_tt( ident, dtt_idx, ch_idx2, ch_idx3, port_name );
        ch_idx2 = ch_idx3;
        ( *tt_ecnt )++;
    }
    else if( ( stt.tv_sec != 0 ) || ( stt.tv_nsec != 0 ) ) {
        // only the source vertex is time-triggered
        cgen_channel_create( ident, ch_idx1,
                igraph_cattribute_EAN( g, GE_DSRC, eid ), 1, 1, port_name );
        cgen_channel_create( ident, ch_idx2, 1,
                igraph_cattribute_EAN( g, GE_DDST, eid ), ch_len, port_name );
        cgen_connect_tt( ident, stt_idx, ch_idx1, ch_idx2, port_name );
        ( *tt_ecnt )++;
    }
    else if( ( dtt.tv_sec != 0 ) || ( dtt.tv_nsec != 0 ) ) {
        // only the target vertex is time-triggered
        cgen_channel_create( ident, ch_idx1,
                igraph_cattribute_EAN( g, GE_DSRC, eid ), 1, ch_len,
                port_name );
        cgen_channel_create( ident, ch_idx2, 1,
                igraph_cattribute_EAN( g, GE_DDST, eid ), 1, port_name );
        cgen_connect_tt( ident, dtt_idx, ch_idx1, ch_idx2, port_name );
        ( *tt_ecnt )++;
    }
    else {
        // there is a problem, one vertex should be time-triggered
    }
    igraph_edge( g, eid, &vid1, &vid2 );
    cgen_connect( ident, ch_idx1, vid1,
            igraph_cattribute_VAS( g, GV_LABEL, vid1 ),
            smxgen_get_port_name( g, eid, IGRAPH_OUT ), MODE_OUT,
            smxgen_box_is_type( g, vid1, TEXT_CP ) );
    cgen_connect( ident, ch_idx2, vid2,
            igraph_cattribute_VAS( g, GV_LABEL, vid2 ),
            smxgen_get_port_name( g, eid, IGRAPH_IN ), MODE_IN,
            smxgen_box_is_type( g, vid2, TEXT_CP ) );
    if( smxgen_box_is_type( g, vid2, TEXT_CP ) )
        cgen_connect_cp( ident, eid, vid2 );
}

/******************************************************************************/
void smxgen_network_destroy( igraph_t* g, int ident, int tt_vcnt, int tt_ecnt )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    int eid, vid1, vid2, edge_cnt = igraph_ecount( g ), i;
    // for all channels
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
    // for all timer channels
    for( i = edge_cnt; i < edge_cnt + tt_ecnt; i++ ) {
        cgen_channel_destroy( ident, i );
    }
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate box destruction code
        vid1 = IGRAPH_VIT_GET( v_it );
        cgen_net_destroy( ident, igraph_cattribute_VAS( g, GV_LABEL, vid1 ),
                vid1, smxgen_box_is_type( g, vid1, TEXT_CP ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
    // for all timers
    for( i = 0; i < tt_vcnt; i++ ) {
        cgen_timer_destroy( ident, i );
    }
}

/******************************************************************************/
void smxgen_network_run( igraph_t* g, int ident, int tt_vcnt )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, i;
    // for all timers
    for( i = 0; i < tt_vcnt; i++ ) {
        cgen_timer_run( ident, i );
    }
    // for all boxes
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate code to run boxes
        vid = IGRAPH_VIT_GET( v_it );
        cgen_net_run( ident, vid, igraph_cattribute_VAS( g, GV_LABEL, vid ),
                igraph_cattribute_VAS( g, GV_IMPL, vid ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}

/******************************************************************************/
void smxgen_network_wait_end( igraph_t* g, int ident, int tt_vcnt )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, i;
    // for all boxes
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate box waiting code
        vid = IGRAPH_VIT_GET( v_it );
        cgen_net_wait_end( ident, vid );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
    // for all timers
    for( i = 0; i < tt_vcnt; i++ ) {
        cgen_timer_wait_end( ident, i );
    }
}

/******************************************************************************/
int smxgen_timer_is_duplicate( struct timespec tt_elem, struct timespec* tt,
        int len )
{
    int i;
    for( i=0; i<len; i++ )
        if( ( tt_elem.tv_sec == tt[i].tv_sec )
                && ( tt_elem.tv_nsec == tt[i].tv_nsec ) )
            return i;
    return -1;
}
