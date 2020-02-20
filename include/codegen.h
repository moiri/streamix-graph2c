#ifndef CODEGEN_H
#define CODEGEN_H

void cgen_channel_create( int ident, int id, int dsrc, int ddst, int len,
        const char* name );
void cgen_channel_destroy( int, int );
void cgen_connect_name( int ident, int id_ch, int id_box, const char* box_name,
        const char* ch_name, const char* mode );
void cgen_connect_idx( int ident, int id_ch, int id_box, const char* mode );
void cgen_connect_guard( int, int, int, int );
void cgen_connect_rn( int ident, int id_ch, int id_box );
void cgen_connect_tf( int ident, int vid, int eid1, int eid2,
        const char* ch_name );
void cgen_ident( int );
void cgen_include_local( const char* );
void cgen_net_init( int ident, int id, int indegree, int outdegree,
        const char* box_name );
void cgen_net_init_profiler( int ident, int id );
void cgen_net_init_rn( int ident, int id );
void cgen_net_init_tf( int ident, int id, int sec, int nsec );
void cgen_net_create( int ident, int id, const char* net_name,
        const char* box_name, int prio );
void cgen_net_destroy( int ident, int id );
void cgen_net_destroy_profiler( int ident, int id );
void cgen_net_destroy_rn( int ident, int id );
void cgen_net_destroy_tf( int ident, int id );
void cgen_net_finalize_profiler( int ident, int id );
void cgen_net_finalize_tf( int ident, int id );
void cgen_net_run( int ident, int id, const char* box_name );
void cgen_net_tt( int );
void cgen_net_wait_end( int, int );
int cgen_print( const char*, ... );
void cgen_program_init( int );
void cgen_program_init_run( int ident );
void cgen_program_cleanup( int );

#endif /* ifndef CODEGEN_H */
