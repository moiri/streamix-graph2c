#include <defines.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "smxgen.h"
#include "codegen.h"
#include "tpl.h"

extern FILE* __src_file;

/******************************************************************************/
void smxgen_app_file( igraph_t* g, const char* name, const char* tpl_path,
        const char* tgt_path )
{
    FILE* ftpl;
    FILE* ftgt;
    char buffer[BUFFER_SIZE];

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
        smxgen_replace( buffer, APP_NAME_PATTERN, name );
        if( strstr( buffer, APP_CONF_PATTERN ) != NULL )
        {
            smxgen_insert_conf( g, ftgt );
        }
        else
            fputs( buffer, ftgt );
    }

    fprintf( stdout, "created file '%s'\n", tgt_path );

    fclose( ftgt );

}

/******************************************************************************/
void smxgen_box_file( igraph_t* g, int id, const char* name,
        const char* tpl_path, FILE* ftgt )
{
    FILE* ftpl;
    char buffer[BUFFER_SIZE];
    int cnt;

    ftpl = fopen( tpl_path, "r" );

    if( ftpl == NULL )
    {
        fprintf( stderr, "cannot open source file '%s'\n", tpl_path );
        return;
    }
    while( ( fgets( buffer, BUFFER_SIZE, ftpl ) ) != NULL )
    {
        smxgen_replace( buffer, BOX_NAME_PATTERN, name );
        if( strstr( buffer, BOX_PORTS_PATTERN ) != NULL )
        {
            smxgen_insert_ports( g, id, IGRAPH_IN, name, TPL_BOX_PORT, ftgt );
            smxgen_insert_ports( g, id, IGRAPH_OUT, name, TPL_BOX_PORT, ftgt );
        }
        else if( strstr( buffer, BOX_SIG_PATTERN ) != NULL )
        {
            smxgen_insert_sig( g, id, name, ftgt );
        }
        else if( strstr( buffer, BOX_PORTS_IN_PATTERN ) != NULL )
        {
            cnt = smxgen_insert_ports( g, id, IGRAPH_IN, name, TPL_BOX_SIG_PORT, ftgt );
            if( cnt == 0 )
                smxgen_port_file( name, "EMPTY", MODE_IN, TPL_BOX_SIG_PORT, ftgt );
        }
        else if( strstr( buffer, BOX_PORTS_OUT_PATTERN ) != NULL )
        {
            cnt = smxgen_insert_ports( g, id, IGRAPH_OUT, name, TPL_BOX_SIG_PORT, ftgt );
            if( cnt == 0 )
                smxgen_port_file( name, "EMPTY", MODE_OUT, TPL_BOX_SIG_PORT, ftgt );
        }
        else
            fputs( buffer, ftgt );
    }

    fclose( ftpl );
}

/******************************************************************************/
void smxgen_box_file_path( igraph_t* g, int id, const char* name,
        const char* tpl_path, const char* tgt_path )
{
    FILE* ftgt;

    ftgt = fopen( tgt_path, "w" );

    if( ftgt == NULL )
    {
        fprintf( stderr, "cannot create target file '%s'\n", tpl_path );
        return;
    }
    smxgen_box_file( g, id, name, tpl_path, ftgt );
    fprintf( stdout, "created file '%s'\n", tgt_path );
    fclose( ftgt );
}

/******************************************************************************/
void smxgen_insert_conf( igraph_t* g, FILE* ftgt )
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
                || smxgen_net_is_type( g, vid, TEXT_PROFILER ) ) {
            IGRAPH_VIT_NEXT( v_it );
            continue;
        }
        box_names[idx++] = box_name;
        smxgen_box_file( g, vid, box_name, TPL_APP_XML_BOX, ftgt );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );

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
        smxgen_port_file( name, smxgen_get_port_name( g, eid, mode ), mode_str,
                tpl_path, ftgt );
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
        port_name = smxgen_get_port_name( g, eid, IGRAPH_IN ),
        sprintf( buffer, " *   - out %s (use SMX_CHANNEL_WRITE( h, %s, %s, msg ) to write)\n",
                port_name, box_name, port_name );
        fputs( buffer, ftgt );
        IGRAPH_EIT_NEXT( e_it );
    }
    igraph_eit_destroy( &e_it );
    igraph_es_destroy( &e_sel );
}

/******************************************************************************/
void smxgen_port_file( const char* box_name, const char* port_name,
        const char* port_mode, const char* tpl_path, FILE* ftgt )
{
    FILE* ftpl;
    char buffer[BUFFER_SIZE];

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
        fputs( buffer, ftgt );
    }

    fclose( ftpl );
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
void smxgen_tpl_box( igraph_t* g )
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

    for( idx = 0; idx < net_count; idx++ )
        names[idx] = NULL;
    idx = 0;
    mkdir( DIR_BOXES, 0755 );

    // for all boxes
    v_sel = igraph_vss_all();
    igraph_vit_create( g, v_sel, &v_it );
    while( !IGRAPH_VIT_END( v_it ) ) {
        // generate code to run boxes
        vid = IGRAPH_VIT_GET( v_it );
        name = igraph_cattribute_VAS( g, GV_IMPL, vid );
        if( smxgen_box_is_duplicate( name, names, net_count )
                || smxgen_net_is_extern( g, vid )
                || smxgen_net_is_type( g, vid, TEXT_CP )
                || smxgen_net_is_type( g, vid, TEXT_PROFILER )
                || smxgen_net_is_type( g, vid, TEXT_TF ) ) {
            IGRAPH_VIT_NEXT( v_it );
            continue;
        }
        names[idx++] = name;
        // create boxes directory
        sprintf( path_tmp, "%s/%s", DIR_BOXES, name );
        strcpy( path, path_tmp );
        mkdir( path, 0755 );
        // create box Makefile
        sprintf( path_file, "%s/Makefile", path );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_MK, path_file );
        // create box gitignore file
        sprintf( path_file, "%s/.gitignore", path );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_GITIGNORE, path_file );
        // create box.c file
        sprintf( path_tmp, "%s/src", path );
        mkdir( path_tmp, 0755 );
        sprintf( path_file, "%s/%s.c", path_tmp, name );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_C, path_file );
        // create h files
        sprintf( path_tmp, "%s/include", path );
        mkdir( path_tmp, 0755 );
        sprintf( path_file, "%s/%s.h", path_tmp, name );
        if( access( path_file, F_OK ) < 0 )
            smxgen_box_file_path( g, vid, name, TPL_BOX_H, path_file );
        fprintf( stdout, "(*) " );
        sprintf( path_file, "%s/%s_sig.h", path_tmp, name );
        smxgen_box_file_path( g, vid, name, TPL_BOX_SIG, path_file );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}

/******************************************************************************/
void smxgen_tpl_main( const char* name, igraph_t* g )
{
    char file[1000];
    FILE* ftpl;
    char buffer[BUFFER_SIZE];
    int tt_vcnt = 0;
    int tt_ecnt = 0;
    int ident = 1;

    ftpl = fopen( TPL_APP_MAIN, "r" );

    if( ftpl == NULL )
    {
        fprintf( stderr, "cannot open source file '%s'\n", TPL_APP_MAIN );
        return;
    }

    sprintf( file, "%s.c", name );
    __src_file = fopen( file, "w" );

    if( __src_file == NULL )
    {
        fprintf( stderr, "cannot create target file '%s'\n", file );
        return;
    }
    while( ( fgets( buffer, BUFFER_SIZE, ftpl ) ) != NULL )
    {
        smxgen_replace( buffer, APP_NAME_PATTERN, name );
        if( strstr( buffer, APP_NW_PATTERN ) != NULL )
        {
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

    smxgen_app_file( g, name, TPL_APP_MK, "Makefile" );
    smxgen_app_file( g, name, TPL_APP_XML, "app.xml" );
    smxgen_app_file( g, name, TPL_APP_LOG, "app.zlog" );
}
