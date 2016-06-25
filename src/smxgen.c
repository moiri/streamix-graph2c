#include "smxgen.h"
#include "codegen.h"
#include <igraph.h>

void smxgen_main( igraph_t* g )
{
    int ident = 0;
    cgen_header_c_file( "todo" );
    cgen_include_local( "smxrts.h" );
    cgen_print( "\n" );
    cgen_main_head();
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
        vid1 = IGRAPH_EIT_GET( v_it );
        cgen_box_create( ident, vid1,
                igraph_cattribute_VAS( g, "label", vid1 ) );
        IGRAPH_VIT_NEXT( v_it );
    }
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
        vid = IGRAPH_EIT_GET( v_it );
        cgen_box_destroy( ident, vid );
        IGRAPH_VIT_NEXT( v_it );
    }
    // for all channels in the scope
    e_sel = igraph_ess_all( IGRAPH_EDGEORDER_ID );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate channel destruction code
        eid = IGRAPH_EIT_GET( e_it );
        cgen_channel_destroy( ident, eid );
        IGRAPH_EIT_NEXT( e_it );
    }
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
        vid = IGRAPH_EIT_GET( v_it );
        cgen_box_run( ident, vid,
                igraph_cattribute_VAS( g, "label", vid ) );
        IGRAPH_VIT_NEXT( v_it );
    }
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
        vid = IGRAPH_EIT_GET( v_it );
        cgen_box_wait_end( ident,
                igraph_cattribute_VAS( g, "label", vid ) );
        IGRAPH_VIT_NEXT( v_it );
    }
}
