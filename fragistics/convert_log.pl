#! /usr/bin/perl -w

# ###########################################################
# This perl script converts a q3 logfile format based
# on time values with "seconds.tenthseconds" to a format
# understood by Fragistics (i.e., time is "minutes:seconds")
# ###########################################################
#
# Copyright (c) 2002 Steffen Schwigon.  All rights reserved.
#
# This package is free software and is provided "as is"
# without express or implied warranty.  It may be used,
# redistributed and/or modified under the terms of the
# Perl Artistic License
# (see http://www.perl.com/perl/misc/Artistic.html).
#
# ###########################################################

use strict;

while (<>) {
  # match time format "seconds.tenthseconds"
  if (/^([0-9]+\.[0-9])( .*$)/) {
    my $timeSec = $1;
    my $rest = $2;
    my $timeMin = $timeSec / 60;
    my $minutes = sprintf ("%d", $timeMin);
    my $seconds = sprintf ("%02d", ($timeMin - $minutes) * 60);
    my $convertedtime = sprintf ("%d:%02d", $minutes, $seconds);
    # print with time format "minutes:seconds"
    print ($convertedtime,$rest, "\n");
  } else {
    # print unchanged line
    print ($_);
  }
}

