#ifndef CODEGEN_H
#define CODEGEN_H

void cgen_block_end( int );
void cgen_block_start( int );
void cgen_box_body( int ident, const char* box_name );
void cgen_box_enum_head( int ident, const char* name, const char* mode );
void cgen_box_enum_tail( int ident );
void cgen_box_fct_ext( int, const char* );
void cgen_box_fct_head( int, const char* );
void cgen_box_fct_proto( int, const char* );
void cgen_box_port( int ident, const char* box_name, const char* port_name,
        const char* mode );
void cgen_channel_create( int ident, int id, int dsrc, int ddst, int len,
        const char* name );
void cgen_channel_destroy( int, int );
void cgen_connect( int ident, int id_ch, int id_box, const char* box_name,
        const char* ch_name, const char* mode, int is_sync );
void cgen_connect_guard( int, int, int, int );
void cgen_connect_rn( int ident, int id_ch, int id_box );
void cgen_connect_tf( int ident, int vid, int eid1, int eid2,
        const char* ch_name );
void cgen_endif( const char* );
void cgen_header_c_file( const char* );
void cgen_header_h_file( const char* );
void cgen_ident( int );
void cgen_ifndef( const char* );
void cgen_include( const char* );
void cgen_include_local( const char* );
void cgen_main_head();
void cgen_net_init( int, int, int, int );
void cgen_net_init_profiler( int ident, int id );
void cgen_net_init_rn( int ident, int id );
void cgen_net_init_tf( int ident, int id, int sec, int nsec );
void cgen_net_create( int ident, int id, const char* net_name,
        const char* box_name );
void cgen_net_destroy( int ident, int id );
void cgen_net_destroy_profiler( int ident, int id );
void cgen_net_destroy_rn( int ident, int id );
void cgen_net_destroy_tf( int ident, int id );
void cgen_net_finalize_profiler( int ident, int id );
void cgen_net_finalize_tf( int ident, int id );
void cgen_net_run( int ident, int id, const char* box_name, int prio );
void cgen_net_tt( int );
void cgen_net_wait_end( int, int );
int cgen_print( const char*, ... );
void cgen_program_init( int );
void cgen_program_init_run( int ident );
void cgen_program_cleanup( int );
void cgen_struct_head( int );
void cgen_struct_tail( int, const char* );

#endif /* ifndef CODEGEN_H */
