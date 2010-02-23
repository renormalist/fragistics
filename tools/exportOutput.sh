#! /bin/sh

##########################################################################
#
# This shell script converts the output/ directory to a directory
# by copying and removing all CVS-administrative files
#
# Use:
#
#   ./exportOutput.sh newDirName
#
#
##########################################################################
#
# Copyright (c) 2000 Steffen Schwigon
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# 
##########################################################################

############################# config section #############################

# The destination directory file
DEST=${1}

############################# function section ###########################

function usage () {
  echo "Usage:"
  echo "  $0 newDirName"
}

function die () {
  echo "error: $1"
  exit 1;
}

function warn () {
  echo "$1"
}

################################# go #####################################

function exportOutputDir () {
  echo "Convert output into ${DEST} ..."

  if [ -r "${DEST}" ] ; then
    die "${DEST} already exists."
  fi

  # copy and remove CVS-files
  cp -r output ${DEST}
  rm -fr ${DEST}/CVS
}


# --- main ---

# check against common mistakes

if [ $# != 1 ] ; then
  usage
  exit 0
fi

exportOutputDir "$1"

