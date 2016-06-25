#include "igraph.h"
#include "smxgen.h"
#include <stdio.h>

int main( void )
{
    igraph_t g;
    FILE* ifile;
    igraph_i_set_attribute_table( &igraph_cattribute_table );

    ifile = fopen( "out.gml", "r" );
    igraph_read_graph_gml( &g, ifile );
    smxgen_main( &g );
    return 0;
}
