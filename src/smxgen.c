#include "smxgen.h"
#include "codegen.h"
#include <igraph.h>

/******************************************************************************/
void smxgen_box_structs( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    int vid, eid;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate box structure code
        vid = IGRAPH_VIT_GET( v_it );
        cgen_box_struct_head( ident, igraph_cattribute_VAS( g, "label", vid ) );
        ident++;
        // for all incident channels of this box
        igraph_es_incident( &e_sel, vid, IGRAPH_ALL );
        igraph_eit_create( g, e_sel, &e_it );
        while( !IGRAPH_EIT_END( e_it ) ) {
            // generate port code
            eid = IGRAPH_EIT_GET( e_it );
            cgen_box_port( ident, igraph_cattribute_EAS( g, "label", eid ) );
            IGRAPH_EIT_NEXT( e_it );
        }
        igraph_eit_destroy( &e_it );
        ident--;
        cgen_box_struct_tail( ident, igraph_cattribute_VAS( g, "label", vid ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );

}

/******************************************************************************/
void smxgen_boxes_c( igraph_t* g )
{
    int ident = 0;
    cgen_header_c_file( "boxgen" );
    cgen_include_local( "boxgen.h" );
    cgen_include_local( "boximpl.h" );
    cgen_include_local( "smxrts.h" );
    cgen_include( "zlog.h" );
    cgen_print( "\n" );
    smxgen_box_fct_defs( g, ident );
}

/******************************************************************************/
void smxgen_boxes_h( igraph_t* g )
{
    int ident = 0;
    cgen_header_h_file( "boxgen" );
    cgen_ifndef( "BOXGEN_H" );
    cgen_print( "\n" );
    smxgen_box_structs( g, ident );
    smxgen_box_fct_prots( g, ident );
    cgen_print( "\n" );
    cgen_endif( "BOXGEN_H" );
}

/******************************************************************************/
void smxgen_box_fct_defs( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate box function definitions
        vid = IGRAPH_VIT_GET( v_it );
        cgen_box_fct_head( ident, igraph_cattribute_VAS( g, "label", vid ) );
        cgen_function_start( ident );
        ident++;
        cgen_box_zlog_start( ident, igraph_cattribute_VAS( g, "label", vid ) );
        cgen_box_fct_call( ident, igraph_cattribute_VAS( g, "func", vid ) );
        cgen_box_zlog_end( ident, igraph_cattribute_VAS( g, "label", vid ) );
        cgen_box_fct_ret( ident );
        ident--;
        cgen_function_end( ident );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
}

/******************************************************************************/
void smxgen_box_fct_prots( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate box function prototypes
        vid = IGRAPH_VIT_GET( v_it );
        cgen_box_fct_proto( ident, igraph_cattribute_VAS( g, "label", vid ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
}

/******************************************************************************/
void smxgen_main( igraph_t* g )
{
    int ident = 0;
    cgen_header_c_file( "main" );
    cgen_include_local( "smxrts.h" );
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
    igraph_vit_t v_it;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    int eid, vid1, vid2;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate box creation code
        vid1 = IGRAPH_VIT_GET( v_it );
        cgen_box_create( ident, vid1,
                igraph_cattribute_VAS( g, "label", vid1 ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    // for all channels in the scope
    e_sel = igraph_ess_all( IGRAPH_EDGEORDER_ID );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate channel creation code
        eid = IGRAPH_EIT_GET( e_it );
        cgen_channel_create( ident, eid );
        // generate connection code for a channel and its connecting boxes
        igraph_edge( g, eid, &vid1, &vid2 );
        cgen_connect( ident, eid, vid1,
                igraph_cattribute_VAS( g, "label", vid1 ),
                igraph_cattribute_EAS( g, "label", eid ) );
        cgen_connect( ident, eid, vid2,
                igraph_cattribute_VAS( g, "label", vid2 ),
                igraph_cattribute_EAS( g, "label", eid ) );
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
}

/******************************************************************************/
void smxgen_network_destroy( igraph_t* g, int ident )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    int eid, vid;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate box destruction code
        vid = IGRAPH_VIT_GET( v_it );
        cgen_box_destroy( ident, vid );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    // for all channels in the scope
    e_sel = igraph_ess_all( IGRAPH_EDGEORDER_ID );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate channel destruction code
        eid = IGRAPH_EIT_GET( e_it );
        cgen_channel_destroy( ident, eid );
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
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
        cgen_box_run( ident, vid, igraph_cattribute_VAS( g, "label", vid ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
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
        cgen_box_wait_end( ident, igraph_cattribute_VAS( g, "label", vid ) );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
}
