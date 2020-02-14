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
Automatically prepare a new package using the current version of a box.

The version number given in the file 'config.mk' is used.

The script performs the following tasks:
 1. Get the current version number.
 2. Create and checkout a new upstream branch (it is called maj.min).
 3. Create a new packeting branch (it is called debian/maj.min).
 4. Generate all required debian files.
 5. Add and commit the new files to the release branch.

Options:
-h, --help                    display this usage message and exit
-C, --directory [DIRECTORY]   specify the path where to execute the script
-d, --doxygen                 use doxygen to generate manpages
-v, --verbose                 generate more output on what is going on

EOF

    exit 1
}

directory="."
verbose=0
doxygen=0
while [ $# -gt 0 ] ; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -C|--directory)
            directory="$2"
            shift
            ;;
        -v|--verbose)
            verbose=1
            ;;
        -d|--doxygen)
        doxygen=1
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

# Read Metatdata

VMAJ=$(gawk 'match($0, /VMAJ = ([0-9]+)/, a) {print a[1]}' $path/config.mk)
VMIN=$(gawk 'match($0, /VMIN = ([0-9]+)/, a) {print a[1]}' $path/config.mk)
VREV=$(gawk 'match($0, /VREV = ([0-9]+)/, a) {print a[1]}' $path/config.mk)
VDEB=$(gawk 'match($0, /VDEB = ([0-9]+)/, a) {print a[1]}' $path/config.mk)
LIBNAME=$(gawk 'match($0, /LIBNAME = (.+)/, a) {print a[1]}' $path/config.mk)
LIBNAME=lib$LIBNAME
DATE_R=$(date -R | sed -e 's/[\/&]/\\&/g')

# Create Branches

branch=$VMAJ.$VMIN
git -C $path checkout -b $branch
branch_p=debian/$branch
git -C $path branch $branch_p

# Copy Debian Templates

rm -rf $path/debian
cp -R $path/tpl/debian $path/debian
mv $path/debian/install $path/debian/$LIBNAME$VMAJ.$VMIN.install
mv $path/debian/install-dev $path/debian/$LIBNAME-dev.install
mv $path/debian/docs $path/debian/$LIBNAME$VMAJ.$VMIN.docs

# Fill Debian Templates with Meaningful Information

files=$(find $path/debian -type f -exec echo "{}" \;)

for file in $files
do
    if [ -d "$file" ]; then
        continue
    fi
    echo "changing file $file"

    echo "replacing maj_version"
    sed -i "s/<maj_version>/$VMAJ.$VMIN/g" $file

    echo "replacing version"
    sed -i "s/<version>/$VMAJ.$VMIN.$VREV-$VDEB/g" $file

    echo "replacing date_r: $DATE_R"
    sed -i "s/<date_r>/$DATE_R/g" $file
done

# Generate Manpages with Doxygen
if [test -f "$path/tpl/.doxygen"] && [ $doxygen -ne 0 ]
then
    cp $path/tpl/.doxygen $path/.doxygen
    echo "replacing version"
    sed -i "s/<version>/$VMAJ.$VMIN.$VREV/g" $path/.doxygen
    echo "replacing libname"
    sed -i "s/<lib_name>/$LIBNAME/g" $path/.doxygen

    ( cd $path && doxygen .doxygen )
fi

git add .
git commit -m "Create package files for new release"
