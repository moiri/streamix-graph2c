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

Usage: $PROGNAME [OPTION ...]
Automatically build a new revision of a package with the current version.

The version number given in the file 'config.mk' is used.

The script performs the following tasks:
 1. Get the current version number.
 2. Checkout the latest upstream branch (it is called maj.min).
 3. Create a tag with the current version.
 4. Checkout the packeting branch (debian/maj.min).
 5. Merge with the tag created in 3.
 6. Update debian/changelog.
 7. Build the package.

Options:
-h, --help                    display this usage message and exit
-C, --directory [DIRECTORY]   specify the path where to execute the script

EOF

    exit 1
}

directory="./"
while [ $# -gt 0 ] ; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -C|--directory)
            directory="$2"
            shift
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

path=$directory
VMAJ=$(gawk 'match($0, /VMAJ = ([0-9]+)/, a) {print a[1]}' $path/config.mk)
VMIN=$(gawk 'match($0, /VMIN = ([0-9]+)/, a) {print a[1]}' $path/config.mk)
VREV=$(gawk 'match($0, /VREV = ([0-9]+)/, a) {print a[1]}' $path/config.mk)

if ! [ -z "$(git status --porcelain)" ]; then
    echo "Working directory $path has uncommitted changes, aborting"
    exit 1
fi

branch=$VMAJ.$VMIN
if ! git -C $path checkout $branch; then
    echo "Branch $branch does not exist in working directory $path, aborting"
    exit 1
fi

tag=v$VMAJ.$VMIN.$VREV
if ! git -C $path tag $tag; then
    echo "Failed to create tag $tag in working directory $path, aborting"
    exit 1
fi

branch_p=debian/$branch
if ! git -C $path checkout $branch_p; then
    echo "Branch $branch_p does not exist on working directory $path, aborting"
    exit 1
fi

git -C $path merge $tag

gbp dch --git-author --release --auto --debian-branch=$branch_p
git commit -m "Release $tag" debian/changelog
gbp buildpackage --git-export-dir=dpkg --git-pristine-tar --git-pristine-tar-commit --git-upstream-tag='v%(version)s' --git-debian-branch=$branch_p --git-submodules -uc -us
