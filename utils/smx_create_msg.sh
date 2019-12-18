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

Usage: $PROGNAME name
Generate the necessary files for a Streamix message library.

Use a unique but simple name which describes the new type of message well.
The name will be prefixed with the standard Streamix nomenclature.

Options:
-h, --help          display this usage message and exit
-i, --tpl [PATH]    path to the streamix msg template folder [/opt/smx/tpl/msg]

EOF

    exit 1
}

name=""
tpl="/opt/smx/tpl/msg"
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

name_msg=smx_msg_$name
output=git-$name_msg

mkdir $output
mkdir $output/src
mkdir $output/include
mkdir $output/debian
cp $tpl/config_mk $output/config.mk
cp $tpl/_gitignore $output/.gitignore
cp $tpl/README_md $output/README.md
sed -i "s/<msg_name>/$name/g" $output/README.md
cp $tpl/msg_mk $output/Makefile
sed -i "s/<msg_name>/$name/g" $output/Makefile
cp $tpl/src/msg_c $output/src/$name_msg.c
sed -i "s/<msg_name>/$name/g" $output/src/$name_msg.c
cp $tpl/include/msg_h $output/include/$name_msg.h
sed -i "s/<msg_name>/$name/g" $output/include/$name_msg.h
cp $tpl/debian/control $output/debian/.
sed -i "s/<msg_name>/$name/g" $output/debian/control
cp $tpl/debian/control-dev $output/debian/.
sed -i "s/<msg_name>/$name/g" $output/debian/control-dev

cat <<EOF

To change compilation options edit the file
 - $output/config_mk

To adapt the message structure and create new function prototypes edit the file
 - $output/include/$name_msg.h

To define copy, destroy, unpack, and other functions edit the file
 - $output/src/$name_msg.c

To document your implementation edit the file
 - $output/README.md

To prepare a debian package change the files in the folder 'debian'.
DO NOT TOUCH THE VERSION TAGS! They will be automatically set when running
the command 'make dpkg'.

EOF
