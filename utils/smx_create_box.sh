#!/bin/sh
#http://agateau.com/2014/template-for-shell-based-command-line-scripts/
set -e

PROGNAME=$(basename $0)

die() {
    echo "$PROGNAME: $*" >&2
    exit 1
}

usage() {
    if [ "$*" != "" ] ; then
        echo "Error: $*"
    fi

    cat << EOF

Usage: $PROGNAME name type
Generate the necessary files for a Streamix box library.

Use a unique but simple name which describes the new type of box well.
The name will be prefixed with the type and the standard Streamix nomenclature.
It is recommended to use 'src', 'snk', 'prc', or 'lib' as a box type.

Options:
-h, --help          display this usage message and exit
-i, --tpl [PATH]    path to the streamix msg template folder [/opt/smx/tpl/box]

EOF

    exit 1
}

name=""
type=""
tpl="/opt/smx/tpl/box"
while [ $# -gt 0 ] ; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -i|--tpl)
            tpl="$2"
            shift
            ;;
        -*)
            usage "Unknown option '$1'"
            ;;
        *)
            if [ -z "$name" ] ; then
                name="$1"
            elif [ -z "$type" ] ; then
                type="$1"
            else
                usage "Too many arguments"
            fi
            ;;
    esac
    shift
done

if [ -z "$name" ] ; then
    usage "Not enough arguments"
fi
if [ -z "$type" ] ; then
    usage "Not enough arguments"
fi

name_box=smx_${type}_${name}
name_lib=smx$type$name
output=git-$name_box

mkdir $output
mkdir $output/src
mkdir $output/include
mkdir $output/debian
cp $tpl/config_mk $output/config.mk
cp $tpl/_gitignore $output/.gitignore
cp $tpl/README_md $output/README.md
sed -i "s/<box_name>/$name_box/g" $output/README.md
sed -i "s/<lib_name>/$name_lib/g" $output/README.md
cp $tpl/box_mk $output/Makefile
sed -i "s/<box_name>/$name_box/g" $output/Makefile
sed -i "s/<lib_name>/$name_lib/g" $output/Makefile
cp $tpl/box_json $output/box.json
sed -i "s/<box_name>/$name_box/g" $output/box.json
cp $tpl/src/box_c $output/src/$name_box.c
sed -i "s/<box_name>/$name_box/g" $output/src/$name_box.c
cp $tpl/include/box_h $output/include/$name_box.h
sed -i "s/<box_name>/$name_box/g" $output/include/$name_box.h
cp $tpl/include/box_sig_h $output/include/${name_box}_sig.h
sed -i "s/<box_name>/$name_box/g" $output/include/${name_box}_sig.h
port_in=$(cat $tpl/include/tpl_port_sig_h | sed "s/<box_name>/$name_box/g" | sed "s/<port_mode>/in/g" | sed "s/<port_name>/EMPTY/g")
port_out=$(cat $tpl/include/tpl_port_sig_h | sed "s/<box_name>/$name_box/g" | sed "s/<port_mode>/out/g" | sed "s/<port_name>/EMPTY/g")
sed -i "s/<box_ports_in>/$port_in/g" $output/include/${name_box}_sig.h
sed -i "s/<box_ports_out>/$port_out/g" $output/include/${name_box}_sig.h
cp $tpl/debian/control $output/debian/.
sed -i "s/<lib_name>/$name_lib/g" $output/debian/control
sed -i "s/<box_name>/$name_box/g" $output/debian/control
cp $tpl/debian/control-dev $output/debian/.
sed -i "s/<lib_name>/$name_lib/g" $output/debian/control-dev
sed -i "s/<box_name>/$name_box/g" $output/debian/control-dev

cat <<EOF

To change compilation options edit the file
 - '$output/config_mk'

To implement the box functionality edit the files
 - '$output/src/$name_box.c'.
 - '$output/include/$name_box.h'.
 - '$output/include/${name_box}_sig.h'.

To document your implementation edit the file
 - '$output/README.md'

Prepare a default configuration by editing the file
 - '$output/box.json'

To prepare a debian package change the files in the folder 'debian'.
DO NOT TOUCH THE VERSION TAGS! They will be automatically set when running
the command 'make dpkg'.

EOF
