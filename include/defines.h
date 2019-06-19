#ifndef DEFINES_H
#define DEFINES_H

#include "graph_defines.h"

#ifndef FILE_BOX
#define FILE_BOX    "boxgen"
#endif
#define FILE_BOX_C  FILE_BOX ".c"
#define FILE_BOX_H  FILE_BOX ".h"
#define FILE_BOX_IH "BOXGEN_H"
#define FILE_SMX    "smxrts"
#define FILE_SMX_H  FILE_SMX ".h"
#define DIR_BOXES   "./boxes"
#define DIR_BUILD   "./build"

#define BUFFER_SIZE 1000

#define BOX_NAME_PATTERN      "<box_name>"
#define BOX_PORTS_PATTERN     "<box_ports_custom>"
#define BOX_PORTS_IN_PATTERN  "<box_ports_in>"
#define BOX_PORTS_OUT_PATTERN "<box_ports_out>"
#define BOX_SIG_PATTERN       "<box_signature>"
#define PORT_NAME_PATTERN     "<port_name>"
#define PORT_MODE_PATTERN     "<port_mode>"

#define TPL_PATH            "/opt/smx/tpl"
#define TPL_APP_MK          TPL_PATH "/app_mk"
#define TPL_BOX_PATH        TPL_PATH "/box"
#define TPL_BOX_MK          TPL_BOX_PATH "/box_mk"
#define TPL_BOX_H           TPL_BOX_PATH "/box_h"
#define TPL_BOX_PORT        TPL_BOX_PATH "/tpl_port_h"
#define TPL_BOX_C           TPL_BOX_PATH "/box_c"
#define TPL_BOX_CONF        TPL_BOX_PATH "/box_xml"
#define TPL_BOX_DEB         TPL_BOX_PATH "/box_deb"
#define TPL_BOX_DEB_DEV     TPL_BOX_PATH "/box-dev_deb"
#define TPL_BOX_SIG         TPL_BOX_PATH "/build/box_sig_h"
#define TPL_BOX_SIG_PORT    TPL_BOX_PATH "/build/tpl_port_sig_h"

#define CONST_ID_LEN 9

#define MODE_OUT    "out"
#define MODE_IN     "in"

#endif /* ifndef DEFINES_H */
