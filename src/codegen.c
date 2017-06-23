#include "codegen.h"
#include <stdarg.h>
#include <stdio.h>
#include "zlog.h"

extern FILE* __src_file;

void cgen_box_cp_init( int ident, int id, int indegree, int outdegree )
{
    cgen_ident( ident );
    cgen_print( "SMX_BOX_CP_INIT( box_%d, %d, %d );\n", id, indegree,
            outdegree );
}

/******************************************************************************/
void cgen_box_create( int ident, int id, const char* name )
{
    cgen_ident( ident );
    cgen_print( "box_%s_t* box_%d = SMX_BOX_CREATE( %s );\n", name, id, name );
}

/******************************************************************************/
void cgen_box_destroy( int ident, int id, int is_sync )
{
    if( is_sync ) {
        cgen_ident( ident );
        cgen_print( "SMX_BOX_CP_DESTROY( box_%d );\n", id );
    }
    cgen_ident( ident );
    cgen_print( "SMX_BOX_DESTROY( box_%d );\n", id );
}

/******************************************************************************/
void cgen_box_fct_call( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "%s( handler );\n", name );
}

/******************************************************************************/
void cgen_box_fct_ext( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "extern void* %s( void* handler );\n", name );
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
void cgen_box_fct_ret( int ident )
{
    cgen_ident( ident );
    cgen_print( "return NULL;\n" );
}

/******************************************************************************/
void cgen_box_port( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "smx_channel_t*  port_%s;\n", name );
}

/******************************************************************************/
void cgen_box_run( int ident, int id, const char* name )
{
    cgen_ident( ident );
    cgen_print( "SMX_BOX_RUN( box_%d, %s );\n", id, name );
}

/******************************************************************************/
void cgen_box_struct_head( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "typedef struct box_%s_s {\n", name );
}

/******************************************************************************/
void cgen_box_struct_tail( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "} box_%s_t;\n\n", name );
}

/******************************************************************************/
void cgen_box_wait_end( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_BOX_WAIT_END( box_%d );\n", id );
}

/******************************************************************************/
void cgen_box_zlog_end( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "dzlog_info( \"end box %s\" );\n", name );
}

/******************************************************************************/
void cgen_box_zlog_start( int ident, const char* name )
{
    cgen_ident( ident );
    cgen_print( "dzlog_info( \"start box %s\" );\n", name );
}

/******************************************************************************/
void cgen_channel_create( int ident, int id, int decoupled )
{
    cgen_ident( ident );
    cgen_print( "smx_channel_t* ch_%d = SMX_CHANNEL_CREATE( ", id );
    if( decoupled )
        cgen_print( "1, SMX_BLACKBOARD" );
    else
        cgen_print( "1, SMX_FIFO" );
    cgen_print( " );\n" );
}

/******************************************************************************/
void cgen_channel_destroy( int ident, int id )
{
    cgen_ident( ident );
    cgen_print( "SMX_CHANNEL_DESTROY( ch_%d );\n", id );
}

/******************************************************************************/
void cgen_connect( int ident, int id_ch, int id_box, const char* box_name,
        const char* ch_name, const char* mode, int is_sync )
{
    cgen_ident( ident );
    if( is_sync )
        cgen_print( "SMX_CONNECT_CP( box_%d, ch_%d, %s );\n", id_box,
                id_ch, mode );
    else
        cgen_print( "SMX_CONNECT( box_%d, ch_%d, %s, %s, %s );\n", id_box,
                id_ch, box_name, ch_name, mode );
}

/******************************************************************************/
void cgen_endif( const char* name )
{
    cgen_print( "#endif // %s\n", name );
}

/******************************************************************************/
void cgen_function_end( int ident )
{
    cgen_ident( ident );
    cgen_print( "}\n\n" );
}

/******************************************************************************/
void cgen_function_start( int ident )
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
