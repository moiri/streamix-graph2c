#include "igraph.h"
#include "smxgen.h"
#include <stdio.h>

FILE* __src_file;

int main( void )
{
    igraph_t g;
    FILE* ifile;
    igraph_i_set_attribute_table( &igraph_cattribute_table );

    ifile = fopen( "test/out.gml", "r" );
    igraph_read_graph_gml( &g, ifile );
    fclose( ifile );
    __src_file = fopen( "test/main.c", "w" );
    smxgen_main( &g );
    fclose( __src_file );
    __src_file = fopen( "test/boxes.h", "w" );
    smxgen_boxes_h( &g );
    fclose( __src_file );
    /* __src_file = fopen( "test/boxes.c", "w" ); */
    /* smxgen_boxes_c( &g ); */
    /* fclose( __src_file ); */

    return 0;
}
