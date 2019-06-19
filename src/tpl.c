#include <defines.h>
#include <string.h>
#include <sys/stat.h>
#include "smxgen.h"
#include "tpl.h"

/******************************************************************************/
void smxgen_box_file( igraph_t* g, int id, const char* name,
        const char* tpl_path, const char* tgt_path )
{
    FILE* ftpl;
    FILE* ftgt;
    char buffer[BUFFER_SIZE];
    int cnt;

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
    fclose( ftgt );
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
void smxgen_tpl( igraph_t* g )
{
    igraph_vs_t v_sel;
    igraph_vit_t v_it;
    int vid, idx;
    const char* name;
    int net_count = igraph_vcount( g );
    const char* names[net_count];
    char path_tmp[1000];
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
        sprintf( path_tmp, "%s/%s", DIR_BOXES, name );
        strcpy( path, path_tmp );
        mkdir( path, 0755 );
        sprintf( path_tmp, "%s/%s.c", path, name );
        smxgen_box_file( g, vid, name, TPL_BOX_C, path_tmp );
        sprintf( path_tmp, "%s/%s.h", path, name );
        smxgen_box_file( g, vid, name, TPL_BOX_H, path_tmp );
        sprintf( path_tmp, "%s/%s", path, DIR_BUILD );
        mkdir( path_tmp, 0755 );
        sprintf( path_tmp, "%s/%s/%s_sig.h", path, DIR_BUILD, name );
        smxgen_box_file( g, vid, name, TPL_BOX_SIG, path_tmp );
        IGRAPH_VIT_NEXT( v_it );
    }
    igraph_vit_destroy( &v_it );
    igraph_vs_destroy( &v_sel );
}
