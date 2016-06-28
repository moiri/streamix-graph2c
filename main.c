#include "igraph.h"
#include "smxgen.h"
#include "defines.h"
#include <stdio.h>
#include <string.h>

FILE* __src_file;

int get_path_size( const char* str )
{
    const char* slash;
    int path_size;
    slash = strrchr( str, '/' );
    if( slash == NULL ) {
        return 0;
    }
    else {
        path_size = slash - str + 1;
    }
    return path_size;
}

int get_ext_size( const char* str )
{
    const char* dot;
    dot = strrchr( str, '.' );
    if( dot == NULL || dot == str ) return 0;
    else return strlen( dot );
}

int get_name_size( const char* str )
{
    int path_size, ext_size;
    path_size = get_path_size( str );
    ext_size = get_ext_size( &str[ path_size ] );
    return strlen( str ) - path_size - ext_size;
}

int main( int argc, char **argv )
{
    igraph_t g;
    const char* src_file_name;
    char* file_path;
    char* file_name;
    char* path_main;
    char* path_boxh;
    char* path_boxc;
    FILE* ifile;
    int path_size, name_size;
    igraph_i_set_attribute_table( &igraph_cattribute_table );

    if( argc != 2 ) {
        printf( "Missing argument!\n" );
        return -1;
    }
    src_file_name = argv[ 1 ];

    path_size = get_path_size( src_file_name );
    file_path = malloc( path_size + 1 );
    memcpy( file_path, src_file_name, path_size );
    file_path[ path_size ] = '\0';

    name_size = get_name_size( src_file_name );
    file_name = malloc( name_size + 1 );
    memcpy( file_name, &src_file_name[ path_size ], name_size );
    file_name[ name_size ] = '\0';

    path_main = malloc( strlen( file_name ) + strlen( file_path ) + 3 );
    sprintf( path_main, "%s%s.c", file_path, file_name );
    path_boxh = malloc( strlen( FILE_BOX_H ) + strlen( file_path ) + 1 );
    sprintf( path_boxh, "%s%s", file_path, FILE_BOX_H );
    path_boxc = malloc( strlen( FILE_BOX_C ) + strlen( file_path ) + 1 );
    sprintf( path_boxc, "%s%s", file_path, FILE_BOX_C );

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

    /* printf( "str( %lu ): %s\n", strlen( src_file_name ), src_file_name ); */
    /* printf( "name( %lu/%d ): %s\n", strlen( file_name ), name_size, file_name ); */
    /* printf( "path( %lu/%d ): %s\n", strlen( file_path ), path_size, file_path ); */

    free( file_name );
    free( file_path );
    free( path_main );
    free( path_boxh );
    free( path_boxc );

    igraph_destroy( &g );

    return 0;
}
