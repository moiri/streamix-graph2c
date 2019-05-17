#include "codegen.h"
#include <stdarg.h>
#include <stdio.h>
#include "zlog.h"

extern FILE* __src_file;

/******************************************************************************/
void cgen_box_body( int ident, const char* net_name, const char* box_name )
{
    cgen_ident( ident );
    cgen_print( "return START_ROUTINE_NET( handler, %s, %s );\n", net_name,
            box_name );
}

/******************************************************************************/
void cgen_box_fct_ext( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_EXTERN( %s );\n", name );
}

/******************************************************************************/
void cgen_box_fct_head( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "void* box_%s( void* handler )\n", name );
}

/******************************************************************************/
void cgen_box_fct_proto( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "void *box_%s( void* );\n", name );
}

/******************************************************************************/
void cgen_net_init( int ident, const char* name, int id, int indegree,
        int outdegree )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_INIT( %d, %s, %d, %d );\n", id, name, indegree,
            outdegree );
}

/******************************************************************************/
void cgen_net_cp_init( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_RN_INIT( %d );\n", id );
}

/******************************************************************************/
void cgen_net_create( int ident, int id, const char* net_name,
        const char* box_name )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_CREATE( %d, %s, %s );\n", id, net_name, box_name );
}

/******************************************************************************/
void cgen_net_destroy( int ident, const char* name, int id, int is_sync )
{
    if( is_sync ) {
        cgen_ident( ident );
        cgen_print( "SMX_NET_RN_DESTROY( %d );\n", id );
    }
    cgen_ident( ident );
    cgen_print( "SMX_NET_DESTROY( %d, %s );\n", id, name );
}

/******************************************************************************/
void cgen_net_port( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "smx_channel_t*  port_%s;\n", name );
}

/******************************************************************************/
void cgen_net_ports( int ident )
{
    cgen_ident( ident );
    cgen_print( "smx_channel_t**  ports;\n" );
    cgen_ident( ident );
    cgen_print( "int              count;\n" );
}

/******************************************************************************/
void cgen_net_run( int ident, int id, const char* net_name,
        const char* box_name )
{
    cgen_ident( ident );
    cgen_print( "SMX_NET_RUN( %d, %s, %s );\n", id, net_name, box_name );
}

/******************************************************************************/
void cgen_net_struct_head( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "typedef struct net_%s_s {\n", name );
}

/******************************************************************************/
void cgen_net_struct_tail( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "} net_%s_t;\n\n", name );
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
void cgen_channel_create( int ident, int id, int dsrc, int ddst, int len,
        const char* name )
{
    cgen_ident( ident );
    cgen_print( "SMX_CHANNEL_CREATE( %d, %d, ", id, len );
    if( dsrc && ddst == 1 )
        cgen_print( "SMX_D_FIFO_D" );
    else if( dsrc && ddst == 2 )
        cgen_print( "SMX_D_FIFO_DD" );
    else if( dsrc )
        cgen_print( "SMX_D_FIFO" );
    else if( ddst == 1 )
        cgen_print( "SMX_FIFO_D" );
    else if( ddst == 2 )
        cgen_print( "SMX_FIFO_DD" );
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
void cgen_connect( int ident, int id_ch, int id_box, const char* net_name,
        const char* box_name, const char* ch_name, const char* mode,
        int is_sync )
{
    if( !is_sync ) {
        cgen_ident( ident );
        cgen_print( "SMX_CONNECT( %d, %d, %s, %s, %s, %s );\n", id_box,
                id_ch, net_name, box_name, ch_name, mode );
    }
    cgen_ident( ident );
    cgen_print( "SMX_CONNECT_ARR( %d, %d, %s, %s, %s, %s );\n", id_box,
            id_ch, net_name, box_name, ch_name, mode );
}

/******************************************************************************/
void cgen_connect_cp( int ident, int id_ch, int id_box )
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
void cgen_connect_tt( int ident, int vid, int eid1, int eid2,
        const char* ch_name )
{
    cgen_ident( ident );
    cgen_print( "SMX_CONNECT_TF( %d, %d, %d, %s );\n", vid, eid1,
            eid2, ch_name );
}

/******************************************************************************/
void cgen_endif( const char* name )
{
    cgen_print( "#endif // %s\n", name );
}

/******************************************************************************/
void cgen_block_end( int ident )
{
    cgen_ident( ident );
    cgen_print( "}\n\n" );
}

/******************************************************************************/
void cgen_block_start( int ident )
{
    cgen_ident( ident );
    cgen_print( "{\n" );
}

/******************************************************************************/
void cgen_header_c_file( const char* name )
{
  cgen_print( "/**\n" );
  cgen_print( " * @file %s.h\n", name );
  cgen_print( " *\n" );
  cgen_print( " * Source code of compiled streamix-file for runtime.\n" );
  cgen_print( " *\n" );
  cgen_print( " * THIS FILE HAS BEEN GENERATED.\n" );
  cgen_print( " * DO NOT EDIT THIS FILE.\n" );
  cgen_print( " * EDIT THE ORIGINAL STREAMIX-SOURCE FILE %s.smx INSTEAD!\n",
          name );
  cgen_print( " *\n" );
  cgen_print( " * ALL CHANGES MADE TO THIS FILE WILL BE OVERWRITTEN!\n" );
  cgen_print( " */\n\n" );
}

/******************************************************************************/
void cgen_header_h_file( const char* name )
{
  cgen_print( "/**\n" );
  cgen_print( " * @file %s.c\n", name );
  cgen_print( " *\n" );
  cgen_print( " * Header definitions of compiled streamix-file for "
          "runtime.\n" );
  cgen_print( " *\n" );
  cgen_print( " * THIS FILE HAS BEEN GENERATED.\n" );
  cgen_print( " * DO NOT EDIT THIS FILE.\n" );
  cgen_print( " * EDIT THE ORIGINAL STREAMIX-SOURCE FILE %s.smx INSTEAD!\n",
          name );
  cgen_print( " *\n" );
  cgen_print( " * ALL CHANGES MADE TO THIS FILE WILL BE OVERWRITTEN!\n" );
  cgen_print( " */\n\n" );
}

/******************************************************************************/
void cgen_ident( int ident )
{
    int i;
    for( i = 0; i < ident; i++ ) cgen_print( "    " );
}

/******************************************************************************/
void cgen_ifndef( const char* name )
{
    cgen_print( "#ifndef %s\n", name );
    cgen_print( "#define %s\n\n", name );
}

/******************************************************************************/
void cgen_include( const char* file )
{
    cgen_print( "#include <%s>\n", file );
}

/******************************************************************************/
void cgen_include_local( const char* file )
{
    cgen_print( "#include \"%s\"\n", file );
}

/******************************************************************************/
void cgen_main_head()
{
    cgen_print( "int main( void )\n" );
}

/******************************************************************************/
int cgen_print( const char* format, ... )
{
    int res;
    va_list args;
    va_start( args, format );
    res = vfprintf( __src_file, format, args );
    va_end( args );
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

/******************************************************************************/
void cgen_struct_head( int ident )
{
    cgen_ident( ident );
    cgen_print( "struct {\n" );
}

/******************************************************************************/
void cgen_struct_tail( int ident, const char* mode )
{
    cgen_ident( ident );
    cgen_print( "} %s;\n", mode );
}

/******************************************************************************/
void cgen_timer_create( int ident, int id, int sec, int nsec )
{
    cgen_ident( ident );
    cgen_print( "SMX_TF_CREATE( %d, %d, %d );\n", id,
            sec, nsec );
}

/******************************************************************************/
void cgen_timer_destroy( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_TF_DESTROY( %d );\n", id );
}

/******************************************************************************/
void cgen_timer_run( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_TF_RUN( %d );\n", id );
}

/******************************************************************************/
void cgen_timer_wait_end( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_TF_WAIT_END( %d );\n", id );
}
