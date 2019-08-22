#include "codegen.h"
#include "defines.h"
#include <stdarg.h>
#include <stdio.h>

extern FILE* __src_file;

/******************************************************************************/
void cgen_channel_create( int ident, int id, int dsrc, int ddst, int len,
        const char* name )
{
    cgen_ident( ident );
    cgen_print( "SMX_CHANNEL_CREATE( %d, %d, ", id, len );
    if( dsrc && ddst )
        cgen_print( "SMX_D_FIFO_D" );
    else if( dsrc )
        cgen_print( "SMX_D_FIFO" );
    else if( ddst )
        cgen_print( "SMX_FIFO_D" );
    else
        cgen_print( "SMX_FIFO" );
    cgen_print( ", %s );\n", name );
}

/******************************************************************************/
void cgen_channel_destroy( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_CHANNEL_DESTROY( %d );\n", id );
}

/******************************************************************************/
void cgen_connect( int ident, int id_ch, int id_box, const char* box_name,
        const char* ch_name, const char* mode, int is_sync )
{
    if( is_sync )
    {
        cgen_ident( ident );
        cgen_print( "SMX_CONNECT_ARR( %d, %d, %s );\n", id_box,
                id_ch, mode );
    }
    else {
        cgen_ident( ident );
        cgen_print( "SMX_CONNECT( %d, %d, %s, %s, %s );\n", id_box,
                id_ch, box_name, ch_name, mode );
    }
}

/******************************************************************************/
void cgen_connect_rn( int ident, int id_ch, int id_box )
{
    cgen_ident( ident );
    cgen_print( "SMX_CONNECT_RN( %d, %d );\n", id_box, id_ch );
}

/******************************************************************************/
void cgen_connect_guard( int ident, int id_ch, int iats, int iatns )
{
    cgen_ident( ident );
    cgen_print( "SMX_CONNECT_GUARD( %d, %d, %d );\n", id_ch, iats, iatns );
}

/******************************************************************************/
void cgen_connect_tf( int ident, int vid, int eid1, int eid2,
        const char* ch_name )
{
    cgen_ident( ident );
    cgen_print( "SMX_CONNECT_TF( %d, %d, %d, %s );\n", vid, eid1,
            eid2, ch_name );
}

/******************************************************************************/
void cgen_ident( int ident )
{
    int i;
    for( i = 0; i < ident; i++ ) cgen_print( "    " );
}

/******************************************************************************/
void cgen_include_local( const char* file )
{
    cgen_print( "#include \"%s.h\"\n", file );
}

/******************************************************************************/
void cgen_net_init( int ident, int id, int indegree, int outdegree )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_INIT( %d, %d, %d );\n", id, indegree,
            outdegree );
}

/******************************************************************************/
void cgen_net_init_rn( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_INIT_RN( %d );\n", id );
}

/******************************************************************************/
void cgen_net_init_tf( int ident, int id, int sec, int nsec )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_INIT_TF( %d, %d, %d );\n", id, sec, nsec );
}

/******************************************************************************/
void cgen_net_create( int ident, int id, const char* net_name,
        const char* box_name, int prio )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_CREATE( %d, %s, %s, %d );\n", id, net_name, box_name,
            prio );
}

/******************************************************************************/
void cgen_net_destroy( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_DESTROY( %d );\n", id );
}

/******************************************************************************/
void cgen_net_destroy_rn( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_DESTROY_RN( %d );\n", id );
}

/******************************************************************************/
void cgen_net_destroy_tf( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_DESTROY_TF( %d );\n", id );
}

/******************************************************************************/
void cgen_net_finalize_tf( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_FINALIZE_TF( %d );\n", id );
}

/******************************************************************************/
void cgen_net_run( int ident, int id, const char* box_name )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_RUN( %d, %s );\n", id, box_name );
}

/******************************************************************************/
void cgen_net_tt( int ident )
{
    cgen_ident( ident );
    cgen_print( "smx_timer_t* timer;\n" );
}

/******************************************************************************/
void cgen_net_wait_end( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_WAIT_END( %d );\n", id );
}

/******************************************************************************/
int cgen_print( const char* format, ... )
{
    char buffer[BUFFER_SIZE];
    int res;
    va_list args;
    va_start( args, format );
    res = vsprintf( buffer, format, args );
    va_end( args );
    fputs( buffer, __src_file );
    return res;
}

/******************************************************************************/
void cgen_program_init( int ident )
{
    cgen_ident( ident );
    cgen_print( "SMX_PROGRAM_INIT();\n" );
}

/******************************************************************************/
void cgen_program_init_run( int ident )
{
    cgen_ident( ident );
    cgen_print( "SMX_PROGRAM_INIT_RUN();\n" );
}

/******************************************************************************/
void cgen_program_cleanup( int ident )
{
    cgen_ident( ident );
    cgen_print( "SMX_PROGRAM_CLEANUP();\n" );
}
