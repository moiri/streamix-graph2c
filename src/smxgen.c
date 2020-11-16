/**
 * Generates the Streamix C code
 *
 * @file    smxgen.c
 * @author  Simon Maurer
 */

#include "smxgen.h"
#include "codegen.h"
#include <defines.h>
#include <igraph.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <fts.h>
#include <errno.h>
#include "smxconfgen.h"

extern FILE* __src_file;

/******************************************************************************/
void smxgen_app_file( igraph_t* g, const char* tpl_path,
        const char* tgt_path )
{
    FILE* ftpl;
    FILE* ftgt;
    char buffer[BUFFER_SIZE];
    const char* name = igraph_cattribute_GAS( g, "name" );
    char year[10];
    smxgen_get_year_str( year );

    // return if item exists
    if( access( tgt_path, F_OK ) == 0 )
        return;

    ftpl = fopen( tpl_path, "r" );

    if( ftpl == NULL )
    {
        fprintf( stderr, "cannot open source file '%s'\n", tpl_path );
        return;
    }

    ftgt = fopen( tgt_path, "w" );

    if( ftgt == NULL )
    {
        fprintf( stderr, "cannot create target file '%s'\n", tpl_path );
        return;
    }
    while( ( fgets( buffer, BUFFER_SIZE, ftpl ) ) != NULL )
    {
        smxgen_replace_path( buffer );
        smxgen_replace( buffer, YEAR_PATTERN, year );
        smxgen_replace( buffer, AUTHOR_PATTERN,
                igraph_cattribute_GAS( g, "author" ) );
        smxgen_replace( buffer, APP_NAME_PATTERN, name );
        smxgen_replace( buffer, APP_DEP_PATTERN,
                igraph_cattribute_GAS( g, "deps" ) );
        smxgen_replace( buffer, APP_REL_PATTERN,
                igraph_cattribute_GAS( g, "rels" ) );
        smxgen_replace( buffer, RTS_DEP_PATTERN,
                igraph_cattribute_GAS( g, "rts_dep" ) );
        smxgen_replace( buffer, BIN_NAME_PATTERN,
                igraph_cattribute_GAS( g, "binname" ) );
        fputs( buffer, ftgt );
    }

    fprintf( stdout, "(*) created file '%s'\n", tgt_path );

    fclose( ftgt );

}

/******************************************************************************/
int smxgen_app_file_tree( igraph_t* g, char* src_path, const char* tgt_path )
{
    FTS* ftsp;
    FTSENT* p;
    FTSENT* chp;
    int fts_options = FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR;
    int pathlen = strlen( src_path );
    char* path;
    char tmp_path[1000];
    char* paths[] = { src_path, NULL };

    if( ( ftsp = fts_open( paths, fts_options, NULL ) ) == NULL )
    {
        return -1;
    }
    chp = fts_children( ftsp, 0 );
    if( chp == NULL )
    {
        return 0;
    }

    while( ( p = fts_read( ftsp ) ) != NULL )
    {
        switch( p->fts_info ) {
            case FTS_D:
                if( p->fts_level > 0 )
                {
                    path = p->fts_path + pathlen + 1;
                    sprintf( tmp_path, "%s/%s", tgt_path, path );
                    mkdir( tmp_path, 0755 );
                }
                break;
            case FTS_F:
                path = p->fts_path + pathlen + 1;
                sprintf( tmp_path, "%s/%s", tgt_path, path );
                smxgen_app_file( g, p->fts_path, tmp_path );
                break;
            default:
                break;
        }
    }
    fts_close(ftsp);
    return 0;
}

/******************************************************************************/
void smxgen_box_file( igraph_t* g, int id, const char* name,
        const char* tpl_path, FILE* ftgt )
{
    FILE* ftpl;
    char buffer[BUFFER_SIZE];
    int cnt;
    char* libname;
    char indeg[10];
    char outdeg[10];
    igraph_vs_t v_cp;
    igraph_vector_t indegree, outdegree;
    char year[10];

    smxgen_get_year_str( year );
    igraph_vs_1( &v_cp, id );
    igraph_vector_init( &indegree, 1 );
    igraph_degree( g, &indegree, v_cp, IGRAPH_IN, 1 );
    sprintf( indeg, "%d", ( int )VECTOR( indegree )[0] );
    igraph_vector_init( &outdegree, 1 );
    igraph_degree( g, &outdegree, v_cp, IGRAPH_OUT, 1 );
    sprintf( outdeg, "%d", ( int )VECTOR( outdegree )[0] );

    ftpl = fopen( tpl_path, "r" );

    if( ftpl == NULL )
    {
        fprintf( stderr, "cannot open source file '%s'\n", tpl_path );
        return;
    }
    while( ( fgets( buffer, BUFFER_SIZE, ftpl ) ) != NULL )
    {
        smxgen_replace_path( buffer );
        smxgen_replace( buffer, RTS_DEP_PATTERN,
                igraph_cattribute_GAS( g, "rts_dep" ) );
        smxgen_replace( buffer, YEAR_PATTERN, year );
        smxgen_replace( buffer, BOX_NAME_PATTERN, name );
        smxgen_replace( buffer, AUTHOR_PATTERN,
                igraph_cattribute_GAS( g, "author" ) );
        libname = malloc( strlen( name ) + 1 );
        smxgen_to_alnum( libname, name );
        smxgen_replace( buffer, BOX_LIB_PATTERN, libname );
        free( libname );
        smxgen_replace( buffer, INDEGREE_PATTERN, indeg );
        smxgen_replace( buffer, OUTDEGREE_PATTERN, outdeg );
        if( strstr( buffer, BOX_SIG_PATTERN ) != NULL )
        {
            smxgen_insert_sig( g, id, name, ftgt );
        }
        else if( strstr( buffer, BOX_PORTS_IN_PATTERN ) != NULL )
        {
            cnt = smxgen_insert_ports( g, id, IGRAPH_IN, name, TPL_BOX_SIG_PORT, ftgt );
            if( cnt == 0 )
                smxgen_port_file( 0, name, "EMPTY", MODE_IN, TPL_BOX_SIG_PORT, ftgt );
        }
        else if( strstr( buffer, BOX_PORTS_OUT_PATTERN ) != NULL )
        {
            cnt = smxgen_insert_ports( g, id, IGRAPH_OUT, name, TPL_BOX_SIG_PORT, ftgt );
            if( cnt == 0 )
                smxgen_port_file( 0, name, "EMPTY", MODE_OUT, TPL_BOX_SIG_PORT, ftgt );
        }
        else if( strstr( buffer, BOX_IN_CASE_PATTERN ) != NULL )
        {
            smxgen_insert_ports( g, id, IGRAPH_IN, name, TPL_BOX_TEST_PORT, ftgt );
        }
        else if( strstr( buffer, BOX_OUT_CASE_PATTERN ) != NULL )
        {
            smxgen_insert_ports( g, id, IGRAPH_OUT, name, TPL_BOX_TEST_PORT, ftgt );
        }
        else if( strstr( buffer, BOX_IN_CONF_PATTERN ) != NULL )
        {
            smxgen_insert_ports( g, id, IGRAPH_IN, name, TPL_BOX_TEST_P_JSON, ftgt );
        }
        else if( strstr( buffer, BOX_OUT_CONF_PATTERN ) != NULL )
        {
            smxgen_insert_ports( g, id, IGRAPH_OUT, name, TPL_BOX_TEST_P_JSON, ftgt );
        }
        else if( strstr( buffer, NET_IN_CH_PATTERN ) != NULL )
        {
            smxgen_insert_ports( g, id, IGRAPH_IN, name, TPL_BOX_TEST_CH, ftgt );
        }
        else if( strstr( buffer, NET_OUT_CH_PATTERN ) != NULL )
        {
            smxgen_insert_ports( g, id, IGRAPH_OUT, name, TPL_BOX_TEST_CH, ftgt );
        }
        else if( strstr( buffer, NET_IN_CH_RM_PATTERN ) != NULL )
        {
            smxgen_insert_ports( g, id, IGRAPH_IN, name, TPL_BOX_TEST_CH_RM, ftgt );
        }
        else if( strstr( buffer, NET_OUT_CH_RM_PATTERN ) != NULL )
        {
            smxgen_insert_ports( g, id, IGRAPH_OUT, name, TPL_BOX_TEST_CH_RM, ftgt );
        }
        else
            fputs( buffer, ftgt );
    }

    fclose( ftpl );
}

/******************************************************************************/
void smxgen_box_file_path( igraph_t* g, int id, const char* name,
        const char* tpl_path, const char* tgt_path, bool append )
{
    FILE* ftgt;
    const char* mode = append ? "a" : "w";

    ftgt = fopen( tgt_path, mode );

    if( ftgt == NULL )
    {
        fprintf( stderr, "cannot create target file '%s'\n", tpl_path );
        return;
    }
    smxgen_box_file( g, id, name, tpl_path, ftgt );
    if( !append )
        fprintf( stdout, "created file '%s'\n", tgt_path );
    fclose( ftgt );
}

/******************************************************************************/
int smxgen_box_file_tree( igraph_t* g, int id, const char* name,
        char* src_path, const char* tgt_path )
{
    FTS* ftsp;
    FTSENT* p;
    FTSENT* chp;
    int fts_options = FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR;
    int pathlen = strlen( src_path );
    char* path;
    char tmp_path[1000];
    char* paths[] = { src_path, NULL };

    if( ( ftsp = fts_open( paths, fts_options, NULL ) ) == NULL )
    {
        return -1;
    }
    chp = fts_children( ftsp, 0 );
    if( chp == NULL )
    {
        return 0;
    }

    while( ( p = fts_read( ftsp ) ) != NULL )
    {
        switch( p->fts_info ) {
            case FTS_D:
                if( p->fts_level > 0 )
                {
                    path = p->fts_path + pathlen + 1;
                    sprintf( tmp_path, "%s/%s", tgt_path, path );
                    mkdir( tmp_path, 0755 );
                }
                break;
            case FTS_F:
                path = p->fts_path + pathlen + 1;
                sprintf( tmp_path, "%s/%s", tgt_path, path );
                smxgen_box_file_path( g, id, name, p->fts_path, tmp_path,
                        false );
                break;
            default:
                break;
        }
    }
    fts_close(ftsp);
    return 0;
}

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
int smxgen_box_is_duplicate_id( int id, int* ids, int len )
{
    int i;
    for( i=0; i<len; i++ )
        if( ids[i] == id )
            return 1;
    return 0;
}

/******************************************************************************/
void smxgen_connect( igraph_t* g, int ident, int eid, int vid, int mode,
        bool is_dyn )
{
    const char* mode_str = MODE_OUT;
    const char* dyn_str = GE_DYNSRC;
    if( mode == IGRAPH_IN )
    {
        mode_str = MODE_IN;
        dyn_str = GE_DYNDST;
    }
    bool is_dyn_attr = ( smxgen_net_is_type( g, vid, TEXT_CP )
            || igraph_cattribute_EAN( g, dyn_str, eid ) );
    if( is_dyn && is_dyn_attr )
    {
        cgen_connect_idx( ident, eid, vid, mode_str );
    }
    else if( !is_dyn && !is_dyn_attr )
    {
        cgen_connect_name( ident, eid, vid,
                    igraph_cattribute_VAS( g, GV_IMPL, vid ),
                    smxgen_get_port_name( g, eid, mode ),
                    mode_str );
    }
}

/******************************************************************************/
int smxgen_cp_file( const char* src, const char* tgt )
{
    char ch;
    FILE *source, *target;

    // return if item exists
    if( access( tgt, F_OK ) == 0 )
        return 0;

    source = fopen( src, "r" );
    if( source == NULL )
    {
        return -1;
    }

    target = fopen( tgt, "w" );
    if( target == NULL )
    {
        return -1;
    }

    while( ( ch = fgetc( source ) ) != EOF )
        fputc(ch, target);

    fclose(source);
    fclose(target);

    fprintf( stdout, "created file '%s'\n", tgt );

    return 0;
}

/******************************************************************************/
void smxgen_get_box_deps( igraph_t* g, char* deps, char* rels )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, idx;
    const char* name;
    int net_count = igraph_vcount( g );
    const char* names[net_count];
    char libname[1000];
    char dep[1000];
    bool first = true;

    for( idx = 0; idx < net_count; idx++ )
    {
        names[idx] = NULL;
    }
    idx = 0;

    // for all non-external boxes
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate code to run boxes
        vid = IGRAPH_VIT_GET( v_it );
        name = igraph_cattribute_VAS( g, GV_IMPL, vid );
        smxgen_to_alnum( libname, name );
        if( smxgen_box_is_duplicate( name, names, net_count )
                || !smxgen_net_is_extern( g, vid )
                || smxgen_net_is_type( g, vid, TEXT_CP )
                || smxgen_net_is_type( g, vid, TEXT_TF ) ) {
            IGRAPH_VIT_NEXT( v_it );
            continue;
        }
        names[idx++] = name;
        smxgen_read_dep( libname, dep );
        strcat( deps, dep );
        if( rels != NULL )
        {
            if( first )
            {
                first = false;
            }
            else
            {
                strcat( rels, ", " );
            }
            strcat( rels, name ); // trim ' -l'
            strcat( rels, ".h(3)" );
        }
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
void smxgen_get_year_str( char* year )
{
    time_t t = time( NULL );
    struct tm tm = *localtime( &t );
    sprintf( year, "%d", tm.tm_year + 1900 );
}

/******************************************************************************/
int smxgen_insert_ports( igraph_t* g, int id, igraph_neimode_t mode,
    const char* name, const char* tpl_path, FILE* ftgt )
{
    int eid, cnt = 0;
    const char* mode_str = ( mode == IGRAPH_IN ) ? MODE_IN : MODE_OUT;
    igraph_es_t e_sel;
    igraph_eit_t e_it;

    igraph_es_incident( &e_sel, id, mode );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate port code
        eid = IGRAPH_EIT_GET( e_it );
        smxgen_port_file( eid, name, smxgen_get_port_name( g, eid, mode ),
                mode_str, tpl_path, ftgt );
        IGRAPH_EIT_NEXT( e_it );
        cnt++;
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );
    return cnt;
}

/******************************************************************************/
void smxgen_insert_sig( igraph_t* g, int id, const char* box_name, FILE* ftgt )
{
    int eid;
    char buffer[BUFFER_SIZE];
    const char* port_name;
    igraph_es_t e_sel;
    igraph_eit_t e_it;

    igraph_es_incident( &e_sel, id, IGRAPH_IN );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate port code
        eid = IGRAPH_EIT_GET( e_it );
        port_name = smxgen_get_port_name( g, eid, IGRAPH_IN ),
        sprintf( buffer, " *   - in %s (use SMX_CHANNEL_READ( h, %s, %s ) to read)\n",
                port_name, box_name, port_name );
        fputs( buffer, ftgt );
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_es_incident( &e_sel, id, IGRAPH_OUT );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate port code
        eid = IGRAPH_EIT_GET( e_it );
        port_name = smxgen_get_port_name( g, eid, IGRAPH_OUT ),
        sprintf( buffer, " *   - out %s (use SMX_CHANNEL_WRITE( h, %s, %s, msg ) to write)\n",
                port_name, box_name, port_name );
        fputs( buffer, ftgt );
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );
}

/******************************************************************************/
void smxgen_network_create( igraph_t* g, int ident, int* tt_vcnt, int* tt_ecnt )
{
    const char* port_name;
    igraph_vs_t v_sel;
    igraph_vs_t v_cp;
    igraph_vit_t v_it;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    int edge_cnt = igraph_ecount( g );
    struct timespec tt[2*edge_cnt];
    int eid, vid1, vid2, ch_len, tt_type, i;
    int net_cnt = igraph_vcount( g );
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
        cgen_net_create( ident, vid1,
                igraph_cattribute_VAS( g, GV_LABEL, vid1 ),
                igraph_cattribute_VAS( g, GV_IMPL, vid1 ),
                igraph_cattribute_VAN( g, GV_TT, vid1 ) );
        igraph_vs_1( &v_cp, vid1 );
        cgen_net_init( ident, vid1,
                smxgen_newtork_get_dyn_degree( g, vid1,  IGRAPH_IN ),
                smxgen_newtork_get_dyn_degree( g, vid1,  IGRAPH_OUT ),
                igraph_cattribute_VAS( g, GV_IMPL, vid1 ) );
        if( smxgen_net_is_type( g, vid1, TEXT_CP ) )
            cgen_net_init_rn( ident, vid1 );
        igraph_vs_destroy( &v_cp );
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
        }
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );

    // connect static channels
    e_sel = igraph_ess_all( IGRAPH_EDGEORDER_ID );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate channel creation code
        eid = IGRAPH_EIT_GET( e_it );
        tt_type = igraph_cattribute_EAN( g, GE_TYPE, eid );
        if( tt_type != TIME_TT ) {
            // generate connection code for a channel and its connecting boxes
            igraph_edge( g, eid, &vid1, &vid2 );
            smxgen_connect( g, ident, eid, vid1, IGRAPH_OUT, false );
            smxgen_connect( g, ident, eid, vid2, IGRAPH_IN, false );
        }
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );

    // acccommodate open ports
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        vid1 = IGRAPH_VIT_GET( v_it );
        cgen_connect_open( ident, vid1,
                igraph_cattribute_VAS( g, GV_IMPL, vid1 ), "IN" );
        cgen_connect_open( ident, vid1,
                igraph_cattribute_VAS( g, GV_IMPL, vid1 ), "OUT" );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );

    // connect dynamic channels
    e_sel = igraph_ess_all( IGRAPH_EDGEORDER_ID );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate channel creation code
        eid = IGRAPH_EIT_GET( e_it );
        tt_type = igraph_cattribute_EAN( g, GE_TYPE, eid );
        if( tt_type != TIME_TT ) {
            // generate connection code for a channel and its connecting boxes
            igraph_edge( g, eid, &vid1, &vid2 );
            smxgen_connect( g, ident, eid, vid1, IGRAPH_OUT, true );
            smxgen_connect( g, ident, eid, vid2, IGRAPH_IN, true );
            if( smxgen_net_is_type( g, vid2, TEXT_CP ) )
                cgen_connect_rn( ident, eid, vid2 );
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


    // init all timers
    for( i = 0; i < *tt_vcnt; i++ ) {
        cgen_net_finalize_tf( ident, i + net_cnt );
    }
}

/******************************************************************************/
void smxgen_network_create_timer( igraph_t* g, int ident, int eid, int edge_cnt,
        int ch_len, struct timespec* tt, int* tt_vcnt, int* tt_ecnt )
{
    struct timespec stt;
    struct timespec dtt;
    int net_cnt = igraph_vcount( g );
    const char* port_name = igraph_cattribute_EAS( g, GE_LABEL, eid );
    int stt_idx, dtt_idx, ch_idx1, ch_idx2, ch_idx3, vid1, vid2, stt_id, dtt_id;
    stt.tv_sec = igraph_cattribute_EAN( g, GE_STS, eid );
    stt.tv_nsec = igraph_cattribute_EAN( g, GE_STNS, eid );
    dtt.tv_sec = igraph_cattribute_EAN( g, GE_DTS, eid );
    dtt.tv_nsec = igraph_cattribute_EAN( g, GE_DTNS, eid );
    // create timer instance
    stt_idx = smxgen_timer_is_duplicate( stt, tt, 2*edge_cnt );
    stt_id = stt_idx + net_cnt;
    if( stt_idx < 0 ) {
        stt_idx = *tt_vcnt;
        stt_id = stt_idx + net_cnt;
        tt[stt_idx].tv_sec = stt.tv_sec;
        tt[stt_idx].tv_nsec = stt.tv_nsec;
        cgen_net_create( ident, stt_id, TEXT_TF, TEXT_TF, 3 );
        cgen_net_init_tf( ident, stt_id, stt.tv_sec, stt.tv_nsec );
        ( *tt_vcnt )++;
    }
    dtt_idx = smxgen_timer_is_duplicate( dtt, tt, 2*edge_cnt );
    dtt_id = dtt_idx + net_cnt;
    if( dtt_idx < 0 ) {
        dtt_idx = *tt_vcnt;
        dtt_id = dtt_idx + net_cnt;
        tt[dtt_idx].tv_sec = dtt.tv_sec;
        tt[dtt_idx].tv_nsec = dtt.tv_nsec;
        cgen_net_create( ident, dtt_id, TEXT_TF, TEXT_TF, 3 );
        cgen_net_init_tf( ident, dtt_id, dtt.tv_sec, dtt.tv_nsec );
        ( *tt_vcnt )++;
    }
    ch_idx1 = eid;
    ch_idx2 = edge_cnt + *tt_ecnt;
    // create time-triggered channels
    if( ( stt.tv_sec == dtt.tv_sec )
            && ( stt.tv_nsec == dtt.tv_nsec ) ) {
        // both veryices are time-triggered at the same rate
        cgen_channel_create( ident, ch_idx1,
                igraph_cattribute_EAN( g, GE_DSRC, eid ), 2, 1, port_name );
        cgen_channel_create( ident, ch_idx2, 1,
                igraph_cattribute_EAN( g, GE_DDST, eid ), 1, port_name );
        cgen_connect_tf( ident, stt_id, ch_idx1, ch_idx2, port_name );
        ( *tt_ecnt )++;
    }
    else if( ( ( stt.tv_sec != 0 ) || ( stt.tv_nsec != 0 ) )
            && ( ( dtt.tv_sec != 0 ) || ( dtt.tv_nsec != 0 ) ) ) {
        // both vertices are time-timetriggered at a different rate
        cgen_channel_create( ident, ch_idx1,
                igraph_cattribute_EAN( g, GE_DSRC, eid ), 2, 1, port_name );
        cgen_channel_create( ident, ch_idx2, 1, 2, 1, port_name );
        ( *tt_ecnt )++;
        cgen_connect_tf( ident, stt_id, ch_idx1, ch_idx2, port_name );
        ch_idx3 = edge_cnt + *tt_ecnt;
        cgen_channel_create( ident, ch_idx3, 1,
                igraph_cattribute_EAN( g, GE_DDST, eid ), 1, port_name );
        cgen_connect_tf( ident, dtt_id, ch_idx2, ch_idx3, port_name );
        ch_idx2 = ch_idx3;
        ( *tt_ecnt )++;
    }
    else if( ( stt.tv_sec != 0 ) || ( stt.tv_nsec != 0 ) ) {
        // only the source vertex is time-triggered
        cgen_channel_create( ident, ch_idx1,
                igraph_cattribute_EAN( g, GE_DSRC, eid ), 2, 1, port_name );
        cgen_channel_create( ident, ch_idx2, 1,
                igraph_cattribute_EAN( g, GE_DDST, eid ), ch_len, port_name );
        cgen_connect_tf( ident, stt_id, ch_idx1, ch_idx2, port_name );
        ( *tt_ecnt )++;
    }
    else if( ( dtt.tv_sec != 0 ) || ( dtt.tv_nsec != 0 ) ) {
        // only the target vertex is time-triggered
        cgen_channel_create( ident, ch_idx1,
                igraph_cattribute_EAN( g, GE_DSRC, eid ), 2, ch_len,
                port_name );
        cgen_channel_create( ident, ch_idx2, 1,
                igraph_cattribute_EAN( g, GE_DDST, eid ), 1, port_name );
        cgen_connect_tf( ident, dtt_id, ch_idx1, ch_idx2, port_name );
        ( *tt_ecnt )++;
    }
    else {
        // there is a problem, one vertex should be time-triggered
    }
    igraph_edge( g, eid, &vid1, &vid2 );
    cgen_connect_idx( ident, ch_idx1, vid1, MODE_OUT );
    cgen_connect_idx( ident, ch_idx2, vid2, MODE_IN );
    if( smxgen_net_is_type( g, vid2, TEXT_CP ) )
        cgen_connect_rn( ident, eid, vid2 );
}

/******************************************************************************/
void smxgen_network_destroy( igraph_t* g, int ident, int tt_vcnt, int tt_ecnt )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    int eid, vid1, vid2, edge_cnt = igraph_ecount( g ), i;
    int net_cnt = igraph_vcount( g );
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
        if( smxgen_net_is_type( g, vid1, TEXT_CP ) )
            cgen_net_destroy_rn( ident, vid1 );
        cgen_net_destroy( ident, vid1 );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
    // for all timers
    for( i = 0; i < tt_vcnt; i++ ) {
        cgen_net_destroy_tf( ident, i + net_cnt );
        cgen_net_destroy( ident, i + net_cnt );
    }
}

/******************************************************************************/
int smxgen_newtork_get_dyn_degree( igraph_t* g, int vid, igraph_neimode_t mode )
{
    int count = 0;
    int eid;
    igraph_es_t e_sel;
    igraph_eit_t e_it;
    igraph_es_incident( &e_sel, vid, mode );
    igraph_eit_create( g, e_sel, &e_it );
    while( !IGRAPH_EIT_END( e_it ) ) {
        // generate channel creation code
        eid = IGRAPH_EIT_GET( e_it );
        if( smxgen_net_is_type( g, vid, TEXT_CP )
                || igraph_cattribute_EAN( g, GE_DYNSRC, eid ) )
        {
            count++;
        }

        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );

    return count;
}

/******************************************************************************/
void smxgen_network_run( igraph_t* g, int ident, int tt_vcnt )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, i;
    int net_cnt = igraph_vcount( g );
    // for all timers
    for( i = 0; i < tt_vcnt; i++ ) {
        cgen_net_run( ident, i + net_cnt, TEXT_TF );
    }
    // for all boxes
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate code to run boxes
        vid = IGRAPH_VIT_GET( v_it );
        cgen_net_run( ident, vid,
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
    int net_cnt = igraph_vcount( g );
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
        cgen_net_wait_end( ident, i + net_cnt );
    }
}

/******************************************************************************/
int smxgen_net_is_extern( igraph_t* g, int vid )
{
    return igraph_cattribute_VAN( g, GV_EXT, vid );
}

/******************************************************************************/
int smxgen_net_is_type( igraph_t* g, int vid, const char* type )
{
    const char* name = igraph_cattribute_VAS( g, GV_LABEL, vid );
    if( strcmp( name , type ) == 0 )
        return 1;
    return 0;
}

/******************************************************************************/
void smxgen_main_includes( igraph_t* g )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, idx = 0, net_count = igraph_vcount( g ), i;
    const char* box_names[net_count];
    const char* box_name;

    for( i=0; i<net_count; i++ )
        box_names[i] = NULL;
    // for all boxes in the scope
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        vid = IGRAPH_VIT_GET( v_it );
        // store name of vertex to avoid duplicates
        box_name = igraph_cattribute_VAS( g, GV_IMPL, vid );
        if( smxgen_box_is_duplicate( box_name, box_names, net_count )
                || smxgen_net_is_type( g, vid, TEXT_CP )
                || smxgen_net_is_type( g, vid, TEXT_TF ) ) {
            IGRAPH_VIT_NEXT( v_it );
            continue;
        }
        cgen_include_local( box_name );
        box_names[idx++] = box_name;
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}

/******************************************************************************/
void smxgen_port_file( int eid, const char* box_name, const char* port_name,
        const char* port_mode, const char* tpl_path, FILE* ftgt )
{
    FILE* ftpl;
    char buffer[BUFFER_SIZE];
    char id[10];

    sprintf( id, "%d", eid );

    ftpl = fopen( tpl_path, "r" );

    if( ftpl == NULL )
    {
        fprintf( stderr, "cannot open source file '%s'\n", tpl_path );
        return;
    }

    while( ( fgets( buffer, BUFFER_SIZE, ftpl ) ) != NULL )
    {
        smxgen_replace( buffer, BOX_NAME_PATTERN, box_name );
        smxgen_replace( buffer, PORT_NAME_PATTERN, port_name );
        smxgen_replace( buffer, PORT_MODE_PATTERN, port_mode );
        smxgen_replace( buffer, CH_ID_PATTERN, id );
        fputs( buffer, ftgt );
    }

    fclose( ftpl );
}

/******************************************************************************/
void smxgen_read_dep( const char* libname, char* dep )
{
    char version[1000];
    smxgen_read_dep_version( libname, version );
    sprintf( dep, " -l%s-%s", libname, version );
}

/******************************************************************************/
void smxgen_read_dep_version( const char* libname, char* version )
{
    char link[1000];
    char buf[1000];
    ssize_t buf_size;
    sprintf( link, "%s/lib%s.so", TPL_LIB_PATH, libname );
    buf_size = readlink( link, buf, 1000 );
    if( buf_size < 0 )
    {
        printf( "failed to parse version number of library '%s': '%s'\n", link,
                strerror( errno ) );
        sprintf( version, "%s", "" );
    }
    else
    {
        buf[buf_size] = '\0';
        sprintf( version, "%s", buf + strlen( libname ) + 7 );
    }
}

/******************************************************************************/
void smxgen_replace( char* str, const char* old_word, const char* new_word )
{
    char* pos, temp[BUFFER_SIZE];
    int index = 0;
    int owlen;

    owlen = strlen( old_word );


    /*
     * Repeat till all occurrences are replaced.
     */
    while( ( pos = strstr( str, old_word ) ) != NULL )
    {
        // Bakup current line
        strcpy( temp, str );

        // Index of current found word
        index = pos - str;

        // Terminate str after word found index
        str[index] = '\0';

        // Concatenate str with new word
        strcat( str, new_word );

        // Concatenate str with remaining words after
        // oldword found index.
        strcat( str, temp + index + owlen );
    }
}

/******************************************************************************/
void smxgen_replace_path( char* buffer )
{
    smxgen_replace( buffer, PATH_INCLUDE_PATTERN, TPL_INCLUDE_PATH );
    smxgen_replace( buffer, PATH_LIB_PATTERN, TPL_LIB_PATH );
    smxgen_replace( buffer, PATH_BIN_PATTERN, TPL_BIN_PATH );
    smxgen_replace( buffer, PATH_DOC_PATTERN, TPL_DOC_PATH );
    smxgen_replace( buffer, PATH_CONF_PATTERN, TPL_CONF_PATH );
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

/******************************************************************************/
void smxgen_to_alnum( char* dst, const char* src )
{
    unsigned long i = 0, c = 0;
    for(i = 0; i < strlen( src ); i++)
    {
        if( isalnum( src[i] ) )
        {
            dst[c] = src[i];
            c++;
        }
    }
    dst[c] = '\0';
}

/******************************************************************************/
void smxgen_tpl_box( igraph_t* g, char* box_path, char* build_path )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, idx;
    const char* name;
    int net_count = igraph_vcount( g );
    const char* names[net_count];
    char path_tmp[1000];
    char path_file[1000];
    char path[1000];
    char deps[1000] = "";
    ( void )( build_path );

    for( idx = 0; idx < net_count; idx++ )
    {
        names[idx] = NULL;
    }
    idx = 0;

    smxgen_read_dep( "smxrts", deps );
    igraph_cattribute_GAS_set( g, "rts_dep", deps );

    // for all non-external boxes
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate code to run boxes
        vid = IGRAPH_VIT_GET( v_it );
        name = igraph_cattribute_VAS( g, GV_IMPL, vid );
        if( smxgen_box_is_duplicate( name, names, net_count )
                || smxgen_net_is_extern( g, vid )
                || smxgen_net_is_type( g, vid, TEXT_CP )
                || smxgen_net_is_type( g, vid, TEXT_TF ) ) {
            IGRAPH_VIT_NEXT( v_it );
            continue;
        }
        names[idx++] = name;
        // create boxes directory
        sprintf( path_tmp, "%s/%s", box_path, name );
        strcpy( path, path_tmp );
        mkdir( path, 0755 );
        // create box Makefile
        sprintf( path_file, "%s/config.mk", path );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_CONF_MK, path_file, false );
        sprintf( path_file, "%s/Makefile", path );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_MK, path_file, false );
        // create box gitignore file
        sprintf( path_file, "%s/.gitignore", path );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_GITIGNORE, path_file, false );
        // create box config file
        sprintf( path_file, "%s/box.json", path );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_CONF, path_file, false );
        // create box README file
        sprintf( path_file, "%s/README.md", path );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_README, path_file, false );
        // create box package control files
        sprintf( path_tmp, "%s/tpl", path );
        mkdir( path_tmp, 0755 );
        sprintf( path_file, "%s/.doxygen", path_tmp );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_DOXYGEN, path_file, false );
        sprintf( path_tmp, "%s/tpl/debian", path );
        mkdir( path_tmp, 0755 );
        smxgen_box_file_tree( g, vid, name, TPL_BOX_DPKG, path_tmp );
        // create test files
        sprintf( path_tmp, "%s/test", path );
        mkdir( path_tmp, 0755 );
        sprintf( path_file, "%s/.gitignore", path_tmp );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_TEST_GI, path_file, false );
        sprintf( path_file, "%s/Makefile", path_tmp );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_TEST_MK, path_file, false );
        sprintf( path_file, "%s/test.c", path_tmp );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_TEST_MAIN_C, path_file, false );
        sprintf( path_file, "%s/test.json", path_tmp );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_TEST_JSON, path_file, false );
        sprintf( path_file, "%s/test.zlog", path_tmp );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_TEST_ZLOG, path_file, false );
        sprintf( path_file, "%s/test_%s.c", path_tmp, name );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_TEST_C, path_file, false );
        sprintf( path_file, "%s/test_%s.h", path_tmp, name );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_TEST_H, path_file, false );
        sprintf( path_tmp, "%s/test/log", path );
        mkdir( path_tmp, 0755 );
        // create box.c file
        sprintf( path_tmp, "%s/src", path );
        mkdir( path_tmp, 0755 );
        sprintf( path_file, "%s/%s.c", path_tmp, name );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_C, path_file, false );
        // create h files
        sprintf( path_tmp, "%s/include", path );
        mkdir( path_tmp, 0755 );
        sprintf( path_file, "%s/%s.h", path_tmp, name );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_H, path_file, false );
        fprintf( stdout, "(*) " );
        sprintf( path_file, "%s/%s_sig.h", path_tmp, name );
        smxgen_box_file_path( g, vid, name, TPL_BOX_SIG_H, path_file, false );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}

/******************************************************************************/
void smxgen_tpl_main( igraph_t* g, char* build_path )
{
    char path_tmp[1000];
    char file[1000];
    char deps[1000] = "";
    char rels[1000] = "";
    char binname[1000] = "";
    FILE* ftpl;
    char buffer[BUFFER_SIZE];
    int tt_vcnt = 0;
    int tt_ecnt = 0;
    int ident = 1;
    int rc;

    ftpl = fopen( TPL_APP_MAIN, "r" );

    if( ftpl == NULL )
    {
        fprintf( stderr, "cannot open source file '%s'\n", TPL_APP_MAIN );
        return;
    }

    smxgen_get_box_deps( g, deps, rels );
    igraph_cattribute_GAS_set( g, "deps", deps );
    igraph_cattribute_GAS_set( g, "rels", rels );
    smxgen_to_alnum( binname, igraph_cattribute_GAS( g, "name" ) );
    igraph_cattribute_GAS_set( g, "binname", binname );

    sprintf( file, "%s/app.c", build_path );
    __src_file = fopen( file, "w" );

    if( __src_file == NULL )
    {
        fprintf( stderr, "cannot create target file '%s'\n", file );
        return;
    }
    while( ( fgets( buffer, BUFFER_SIZE, ftpl ) ) != NULL )
    {
        smxgen_replace_path( buffer );
        smxgen_replace( buffer, BIN_NAME_PATTERN, binname );
        smxgen_replace( buffer, APP_NAME_PATTERN,
                igraph_cattribute_GAS( g, "name" ) );
        if( strstr( buffer, APP_NW_PATTERN ) != NULL )
        {
            smxgen_network_create( g, ident, &tt_vcnt, &tt_ecnt );
            cgen_print( "\n" );
            cgen_program_init_run( ident );
            cgen_print( "\n" );
            smxgen_network_run( g, ident, tt_vcnt );
            cgen_print( "\n" );
            smxgen_network_wait_end( g, ident, tt_vcnt );
            cgen_print( "\n" );
            smxgen_network_destroy( g, ident, tt_vcnt, tt_ecnt );
        }
        else if( strstr( buffer, APP_INC_PATTERN ) != NULL )
        {
            smxgen_main_includes( g );
        }
        else
            fputs( buffer, __src_file );
    }

    fprintf( stdout, "(*) created file '%s'\n", file );

    fclose( __src_file );

    mkdir( DIR_LOG, 0755 );
    sprintf( path_tmp, "%s/tpl", build_path );
    mkdir( path_tmp, 0755 );

    sprintf( file, "%s/Makefile", path_tmp );
    smxgen_app_file( g, TPL_APP_MK, file );
    smxgen_cp_file( file, "Makefile" );

    sprintf( file, "%s/config.mk", path_tmp );
    smxgen_app_file( g, TPL_APP_CONF_MK, file );
    smxgen_cp_file( file, "config.mk" );

    sprintf( file, "%s/README.md", path_tmp );
    smxgen_app_file( g, TPL_APP_README, file );
    smxgen_cp_file( file, "README.md" );

    rc = smxconfgen_generate_file( g, igraph_cattribute_GAS( g, "name" ),
            "<maj_version>", false, path_tmp, "app.json" );
    if( rc == 0 )
    {
        fprintf( stdout, "(*) created file '%s/%s'\n", path_tmp, "app.json" );
    }

    smxconfgen_generate_file( g, igraph_cattribute_GAS( g, "name" ),
            "<maj_version>", true, path_tmp, "app.schema.json" );
    if( rc == 0 )
    {
        fprintf( stdout, "(*) created file '%s/%s'\n", path_tmp,
                "app.schema.json" );
    }

    sprintf( file, "%s/app.zlog", path_tmp );
    smxgen_app_file( g, TPL_APP_LOG, file );
    smxgen_cp_file( file, "app.zlog" );

    sprintf( file, "%s/.gitignore", path_tmp );
    smxgen_app_file( g, TPL_APP_GITIGNORE, file );
    smxgen_cp_file( file, ".gitignore" );

    sprintf( path_tmp, "%s/tpl/debian", build_path );
    mkdir( path_tmp, 0755 );
    smxgen_app_file_tree( g, TPL_PATH_APP_DPKG, path_tmp );
}
