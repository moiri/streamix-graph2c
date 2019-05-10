#ifndef CODEGEN_H
#define CODEGEN_H

void cgen_box_body( int, const char* );
void cgen_box_init( int, const char*, int, int, int );
void cgen_box_cp_init( int, int );
void cgen_box_create( int, int, const char* );
void cgen_box_destroy( int, const char*, int, int );
void cgen_box_fct_ext( int, const char* );
void cgen_box_fct_head( int, const char* );
void cgen_box_fct_proto( int, const char* );
void cgen_box_port( int, const char* );
void cgen_box_ports( int );
void cgen_box_run( int, int, const char* );
void cgen_box_struct_head( int, const char* );
void cgen_box_struct_tail( int, const char* );
void cgen_box_tt( int );
void cgen_box_wait_end( int, int );
void cgen_channel_create( int, int, int, int, int, const char* );
void cgen_channel_destroy( int, int );
void cgen_connect( int, int, int, const char*, const char*, const char*, int );
void cgen_connect_cp( int, int, int );
void cgen_connect_guard( int, int, int, int );
void cgen_connect_tt( int ident, int vid, int eid1, int eid2 );
void cgen_endif( const char* );
void cgen_block_end( int );
void cgen_block_start( int );
void cgen_header_c_file( const char* );
void cgen_header_h_file( const char* );
void cgen_ident( int );
void cgen_ifndef( const char* );
void cgen_include( const char* );
void cgen_include_local( const char* );
void cgen_main_head();
int cgen_print( const char*, ... );
void cgen_program_init( int );
void cgen_program_init_run( int ident );
void cgen_program_cleanup( int );
void cgen_struct_head( int );
void cgen_struct_tail( int, const char* );
void cgen_timer_create( int ident, int id, int sec, int nsec );
void cgen_timer_destroy( int ident, int id );
void cgen_timer_run( int ident, int id );
void cgen_timer_wait_end( int ident, int id );

#endif /* ifndef CODEGEN_H */
