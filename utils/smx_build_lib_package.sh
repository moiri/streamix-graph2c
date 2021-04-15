#!/bin/bash
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

Usage: $PROGNAME [OPTION ...]
This is a wrapper script and invokes the script smx_build_package with the -l
option in order to build a library.

Options:
-h, --help                    display this usage message and exit
-H, --help-parent             display the usage message of smx_build_package
                              and exit
-C, --directory [DIRECTORY]   specify the path where to execute the script
-d, --doxygen                 do not generate manpages with doxygen
-t, --dryrun                  only create a debian folder with the appripriate files

EOF

    exit 1
}

directory="."
doxygen=""
dryrun=""
while [ $# -gt 0 ] ; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -H|--help-parent)
            smx_build_package.sh -h
            exit 1
            ;;
        -C|--directory)
            directory="$2"
            shift
            ;;
        -d|--doxygen)
            doxygen="-d"
            ;;
        -t|--dryrun)
            dryrun=-"t"
            ;;
        -*)
            usage "Unknown option '$1'"
            ;;
        *)
            usage "Too many arguments"
            ;;
    esac
    shift
done

smx_build_package.sh -l -C $directory $doxygen $dryrun
