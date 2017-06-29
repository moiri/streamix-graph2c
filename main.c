#include "igraph.h"
#include "smxgen.h"
#include "siagen.h"
#include "defines.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

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
    sia_t* symbols = NULL;
    const char* src_file_name;
    char* out_path = NULL;
    char* out_path_sia;
    char* file_name;
    char* path_main;
    char* path_boxh;
    char* path_boxc;
    char* format;
    FILE* ifile;
    int path_size, name_size;
    int c;
    igraph_i_set_attribute_table( &igraph_cattribute_table );

    while( ( c = getopt( argc, argv, "hvp:f:" ) ) != -1 )
        switch( c ) {
            case 'h':
                printf( "Usage:\n  %s [OPTION...] FILE\n\n", argv[0] );
                printf( "Options:\n" );
                printf( "  -h            This message\n" );
                printf( "  -v            Version\n" );
                printf( "  -p 'path'     Path to store the generated files\n" );
                printf( "  -f 'format'   Format of the graph either 'gml' or 'graphml'\n" );
                return 0;
            case 'v':
                printf( "gml2c-v0.0.1\n" );
                return 0;
            case 'p':
                out_path = optarg;
                break;
            case 'f':
                format = optarg;
                break;
            case '?':
                if( ( optopt == 'p' ) || ( optopt == 'f' ) )
                    fprintf ( stderr, "Option -%c requires an argument.\n",
                            optopt );
                else
                    fprintf ( stderr, "Unknown option character `\\x%x'.\n",
                            optopt );
                return 1;
            default:
                abort();
        }
    if( argc <= optind ) {
        fprintf( stderr, "Missing argument!\n" );
        return -1;
    }
    src_file_name = argv[ optind ];
    if( out_path == NULL ) out_path = "./build";
    mkdir( out_path, 0755 );
    if( format == NULL ) format = "graphml";

    path_size = get_path_size( src_file_name );
    name_size = get_name_size( src_file_name );
    file_name = malloc( name_size + 1 );
    memcpy( file_name, &src_file_name[ path_size ], name_size );
    file_name[ name_size ] = '\0';

    path_main = malloc( strlen( file_name ) + strlen( out_path ) + 4 );
    sprintf( path_main, "%s/%s.c", out_path, file_name );
    path_boxh = malloc( strlen( FILE_BOX_H ) + strlen( out_path ) + 2 );
    sprintf( path_boxh, "%s/%s", out_path, FILE_BOX_H );
    path_boxc = malloc( strlen( FILE_BOX_C ) + strlen( out_path ) + 2 );
    sprintf( path_boxc, "%s/%s", out_path, FILE_BOX_C );

    ifile = fopen( src_file_name, "r" );
    if( strcmp( format, "gml" ) == 0 ) {
        igraph_read_graph_gml( &g, ifile );
    }
    else if( strcmp( format, "graphml" ) == 0 ) {
        igraph_read_graph_graphml( &g, ifile, 0 );
    }
    else {
        printf( "Unknown format '%s'!\n", format );
        return -1;
    }
    fclose( ifile );

    // GENERATE RTS C CODE
    __src_file = fopen( path_main, "w" );
    smxgen_main( file_name, &g );
    fclose( __src_file );

    __src_file = fopen( path_boxh, "w" );
    smxgen_boxes_h( &g );
    fclose( __src_file );

    __src_file = fopen( path_boxc, "w" );
    smxgen_boxes_c( &g );
    fclose( __src_file );

    // GENERATE RTS SIA CODE
    siagen( &g, &symbols );
    out_path_sia = malloc( strlen( out_path ) + 5 );
    sprintf( out_path_sia, "%s/sia", out_path );
    siagen_write( &symbols, out_path_sia, format );

    /* printf( "str( %lu ): %s\n", strlen( src_file_name ), src_file_name ); */
    /* printf( "name( %lu/%d ): %s\n", strlen( file_name ), name_size, file_name ); */
    /* printf( "path( %lu/%d ): %s\n", strlen( out_path ), path_size, out_path ); */

    free( file_name );
    free( path_main );
    free( path_boxh );
    free( path_boxc );
    free( out_path_sia );

    igraph_destroy( &g );
    siagen_destroy( &symbols );

    return 0;
}
