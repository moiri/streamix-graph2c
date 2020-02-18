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
Depending on the current version of a box, do one of the following
 - create a new debian package
 - update an existing debian package

The version number given in the file 'config.mk' is used.
IMPOTANT: when updating a package you MUST fix the version number in the
changelog. Currently a wrong number is generated.

When creating a new debian package the followng tasks are berformed:
 1. Get the current version number.
 2. Create a new packeting branch (it is called debian/maj.min.rev).
 3. Create and checkout a new upstream branch (it is called maj.min).
 4. Generate all required debian files.
 5. Add and commit the new files to the release branch.
 6. Create a tag with the current version.
 7. Checkout the packeting branch (debian/maj.min.rev).
 8. Merge with the tag created in 6.
 9. Update debian/changelog.
 10. Build the package.

When updating a debian package the followng tasks are berformed:
 1. Get the current version number.
 2. Create a new packeting branch (it is called debian/maj.min.rev).
 3. Checkout the upstream branch (it is called maj.min).
 4. Merge head (before switching to upstream branch) into upstream branch.
 5. Generate manpages.
 6. Add and commit the changed files to the release branch.
 7. Create a tag with the current version.
 8. Checkout the packeting branch (debian/maj.min.rev).
 9. Merge with the tag created in 6.
 10. Update debian/changelog.
 11. Build the package.

Options:
-h, --help                    display this usage message and exit
-C, --directory [DIRECTORY]   specify the path where to execute the script
-d, --doxygen                 do not generate manpages with doxygen
-v, --verbose                 generate more output on what is going on

EOF

    exit 1
}

directory="."
verbose=0
doxygen=1
new=0
while [ $# -gt 0 ] ; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -C|--directory)
            directory="$2"
            shift
            ;;
        -d|--doxygen)
            doxygen=0
            ;;
        -v|--verbose)
            verbose=1
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

# Define branch and tag names
branch=$VMAJ.$VMIN
branch_p=debian/$branch.$VREV
tag=v$branch.$VREV
pristine_branch=pristine-tar
pristine_branch_v=${pristine_branch}_$branch

# Check if a package must be updated or a new package is required
if ! git -C $path show-ref --verify --quiet refs/heads/$branch; then
    new=1
fi

# Create build branch
git -C $path branch $branch_p
if [ $new -ne 0 ]; then
    # Create upstream branch
    git -C $path checkout -b $branch

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
else
    head=$(git rev-parse HEAD)
    git -C $path checkout $branch
    git -C $path merge $head
    # Rename pristine-tar branch
    if ! git -C $path branch -m $pristine_branch_v $pristine_branch; then
        echo "Branch $pristine_branch_v does not exist on working directory $path, aborting"
        exit 1
    fi
fi

# Generate Manpages with Doxygen
if [ -f "$path/tpl/.doxygen" ] && [ $doxygen -ne 0 ]
then
    cp $path/tpl/.doxygen $path/.doxygen
    echo "replacing version"
    sed -i "s/<version>/$VMAJ.$VMIN.$VREV/g" $path/.doxygen

    ( cd $path && doxygen .doxygen )
    # remove all links not including 'smx' or 'SMX' (i.e. all struct memebers)
    find $path/doc/man/man3/ -type f ! -name '*smx*' ! -name '*SMX*' -delete
    ( cd $path && ls -1 doc/man/man3/* > debian/$LIBNAME-dev.manpages )
fi

# Commit changes to upstream branch
git add .
git commit -m "Create package files for new release $tag"

# Create version tag
if ! git -C $path tag $tag; then
    echo "Failed to create tag $tag in working directory $path, aborting"
    exit 1
fi

# Merge changes to build branch
if ! git -C $path checkout $branch_p; then
    echo "Branch $branch_p does not exist on working directory $path, aborting"
    exit 1
fi
git -C $path merge $tag

# Generate changelog and commit changes
gbp dch --git-author --release --debian-branch=$branch_p --upstream-tag='v%(version)s'

if ! git commit -m "Release $tag" debian/changelog; then
    echo "No changes to initial changelog, using default"
fi

# Build the package
gbp buildpackage --git-export-dir=dpkg --git-pristine-tar --git-pristine-tar-commit --git-upstream-tag='v%(version)s' --git-debian-branch=$branch_p --git-submodules -uc -us

# Rename the pristine-tar branch
git -C $path branch -m $pristine_branch $pristine_branch_v

# Checkout master
git -C $path checkout master
