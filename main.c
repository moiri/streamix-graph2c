#include "igraph.h"
#include "smxgen.h"
#include "defines.h"
#include <stdio.h>
#include <string.h>

FILE* __src_file;
char* extract_path( const char* str )
{
    const char* slash;
    char* path;
    int path_size;
    slash = strrchr( str, '/' );
    if( slash == NULL ) {
        return "";
    }
    else {
        path_size = slash - str + 1;
    }
    path = malloc( path_size + 1 );
    memcpy( path, str, path_size );
    path[ path_size ] = '\0';
    return path;
}

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
    char* path;
    char* file_name;
    char* path_main;
    char* path_boxh;
    char* path_boxc;
    FILE* ifile;
    igraph_i_set_attribute_table( &igraph_cattribute_table );

    if( argc != 2 ) {
        printf( "Missing argument!\n" );
        return -1;
    }
    src_file_name = argv[ 1 ];

    path = extract_path( src_file_name );

    file_name = extract_base_name( src_file_name );

    path_main = malloc( strlen( file_name ) + strlen( path ) + 3 );
    sprintf( path_main, "%s%s.c", path, file_name );
    path_boxh = malloc( strlen( FILE_BOX_H ) + strlen( path ) + 1 );
    sprintf( path_boxh, "%s%s", path, FILE_BOX_H );
    path_boxc = malloc( strlen( FILE_BOX_C ) + strlen( path ) + 1 );
    sprintf( path_boxc, "%s%s", path, FILE_BOX_C );

    ifile = fopen( src_file_name, "r" );
    igraph_read_graph_gml( &g, ifile );
    fclose( ifile );

    __src_file = fopen( path_main, "w" );
    smxgen_main( file_name, &g );
    fclose( __src_file );

    __src_file = fopen( path_boxh, "w" );
    smxgen_boxes_h( &g );
    fclose( __src_file );

    __src_file = fopen( path_boxc, "w" );
    smxgen_boxes_c( &g );
    fclose( __src_file );

    free( file_name );
    free( path_main );
    free( path_boxh );
    free( path_boxc );

    igraph_destroy( &g );

    return 0;
}
