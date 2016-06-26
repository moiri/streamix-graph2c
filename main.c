#include "igraph.h"
#include "smxgen.h"
#include "defines.h"
#include <stdio.h>
#include <string.h>

FILE* __src_file;

char* extract_base_name( const char* path )
{
    const char* dot;
    const char* slash;
    char* file_name;
    int path_size, name_size, ext_size;
    slash = strrchr( path, '/' );
    if( slash == NULL ) {
        path_size = 0;
        slash = path;
    }
    else if ( slash == path ) {
        path_size = 1;
        slash++;
    }
    else {
        slash++;
        path_size = slash - path;
    }

    dot = strrchr( slash, '.' );
    if( dot == NULL || dot == slash ) ext_size = 0;
    else ext_size = strlen( dot );

    name_size = strlen( path ) - path_size - ext_size;

    file_name = malloc( name_size + 1 );
    memcpy( file_name, &path[ path_size ], name_size );
    file_name[ name_size ] = '\0';
    return file_name;
}

int main( int argc, char **argv )
{
    igraph_t g;
    const char* src_file_name;
    char* file_name;
    char* file_name_target;
    FILE* ifile;
    igraph_i_set_attribute_table( &igraph_cattribute_table );

    if( argc != 2 ) {
        printf( "Missing argument!\n" );
        return -1;
    }
    src_file_name = argv[ 1 ];

    file_name = extract_base_name( src_file_name );

    file_name_target = malloc( strlen( file_name ) + 3 );
    sprintf( file_name_target, "%s.c", file_name );

    ifile = fopen( src_file_name, "r" );
    igraph_read_graph_gml( &g, ifile );
    fclose( ifile );

    __src_file = fopen( file_name_target, "w" );
    smxgen_main( file_name, &g );
    fclose( __src_file );

    __src_file = fopen( FILE_BOX_H, "w" );
    smxgen_boxes_h( &g );
    fclose( __src_file );

    __src_file = fopen( FILE_BOX_C, "w" );
    smxgen_boxes_c( &g );
    fclose( __src_file );

    free( file_name );
    free( file_name_target );

    igraph_destroy( &g );

    return 0;
}
