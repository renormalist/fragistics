#! /bin/sh

##########################################################################
#
# This shell script extracts all images needed for Fragistics 
# out of a given .pk3 file, converts them as needed by Fragistics and 
# puts the resulting images into the output/images directory.
#
# Use:
#
#   ./extract_pk3images.sh /path/to/pak0.pk3
#
#
# External programs needed:
#
#   unzip
#   convert    (from ImageMagick)
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

# The pk3 file
PAK=${1}

# The image output directory
# (relative to Fragistics base dir)
###################################

IMAGEDIR=output/images


# executables
#############

UNZIP="unzip -oq"
CONVERT=convert
MV="mv -f"


############################# function section ###########################

function usage () {
  echo "Usage:"
  echo "  $0 /path/to/pak0.pk3"
  echo "  (requires unzip and convert from ImageMagick be installed)"
}

function die () {
  echo "error: $1"
  exit 1;
}

function warn () {
  echo "$1"
}

################################# go #####################################

function extract_pk3 () {
  echo "EXTRACT IMAGES FROM PK3 ARCHIVE."
  echo "--------------------------------"

  if [ ! -r "${PAK}" ] ; then
    die "can't read archive ${PAK}."
  fi

  # change into output directory

  cd ${IMAGEDIR} || die "can't change to output dir."

  # --- extract levelshots ---

  echo -n "Extracting levelshots ... "
  ${UNZIP} "${PAK}" 'levelshots/*' 2> /dev/null || warn "no levelshots in archive."
  echo done


  # --- convert basenames of levelshots to uppercase ---

  if [ -d levelshots ] ; then
    echo -n "Uppercasing levelshots ... "
    cd levelshots
    for i in `find -name "*.jpg" -maxdepth 1` ; do
      bname=`basename $i .jpg` 
      ${MV} $i `echo $bname|tr 'a-z' 'A-Z'`.jpg 2> /dev/null
    done
    echo "done"
    cd ..
  fi


  # --- extract original player icons ---

  echo -n "Extracting player icons ... "
  ${UNZIP} "${PAK}" 'models/players/*/icon_*.tga' 2> /dev/null || echo -n "(no player models in archive) "
  echo done

  # --- transform player images to Fragistics-usable images ---

  if [ -d models/players ] ; then
    cd models/players
    for i in `find -type d -mindepth 1` ; do
      player=`basename $i`
      cd ${player}
      echo -n "Transforming ${player} ... "
      for j in *.tga ; do
        source=${j}
        target=`basename ${j} .tga|sed "s/icon/${player}/"`.jpg
        targetdir=../..
        ${CONVERT} ${source} ${targetdir}/${target} && rm -f $j || die "can't convert image."
      done
      echo done
      cd ..
      rmdir ${player}
    done
    cd ..
    rmdir players
    cd ..
  fi
  cd ../..
}


# --- main ---

# check against common mistakes

if [ $# != 1 ] ; then
  usage
  exit 0
fi

extract_pk3 "$1"

