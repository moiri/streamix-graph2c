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
This is a wrapper script and invokes the command gbp buildpackage in order to
build a debian package of a specific version. make sure to checkout a debian
branch of your choice before starting this script

Options:
-h, --help                    display this usage message and exit

EOF

    exit 1
}

directory="."
doxygen=""
while [ $# -gt 0 ] ; do
    case "$1" in
        -h|--help)
            usage
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

branch=$(git rev-parse --abbrev-ref HEAD)

if ! [[ $branch == debian* ]]; then
    echo "Please checkout a debian branch. You are on branch $branch, aboring"
    exit 1
fi

gbp buildpackage --git-export-dir=dpkg --git-upstream-tag='v%(version)s' --git-debian-branch=$branch --git-submodules -uc -us
