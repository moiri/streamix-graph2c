#!/bin/sh

dpkg-scanpackages -m . | gzip -c > Packages.gz
