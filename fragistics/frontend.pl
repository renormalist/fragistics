#! /usr/bin/perl

# ###########################################################
# This perl script converts a Q3 logfile format based
# on time values with "seconds.tenthseconds" to a format
# understood by Fragistics (i.e., time is "minutes:seconds").
#
# It also tries to correct various artefacts that might
# break lines of a logfile due to Q3 server restarts
#
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

use Getopt::Long;
use Pod::Usage;

use strict;
use warnings;

# constants
my $MAXLEN  = 1024;

# global cmdline options
my $CFGFILE  = 'fragistics.cfg';
my $LOGFILE  = undef;
my $DESTDIR  = undef;
my $VERBOSE  = 1;
my $HELP     = 0;
my $MAN      = 0;

# string get_cfg_value (section, key);
# Reads the value for a given section and key.
# Config file format:
#
#   [SECTION]
#   key = value
#   // comment
#
sub get_cfg_value {
  my ($srchsec,$srchkey) = @_;
  my $section = '';
  my $key = '';
  my $val = undef;
  open CF, "< $CFGFILE"
    or die "Can't open config file $CFGFILE.\n";
  while (<CF>) {
    next if m{^//}; # comment
    $section = $1 if /^\[([^\]]*)\]/; # [SECTION]
    next unless $section eq $srchsec;
    $key=$1, $val=$2, last
      if /^([^=\s]*)\s*=\s*(.*)/ and $1 eq $srchkey; # key = val
  }
  close CF;
  return $val;
}

# loop through the log file and call callbacks
sub convert_timeformat {
  my $logfile = shift;
  my $destfile = shift;
  open LF, "< $logfile"
    or die "Can't open log file $logfile.\n";
  open DF, "> $destfile"
    or die "Can't open destination file $destfile.\n";
  while (<LF>) {
    # match time format "seconds.tenthseconds"
    if (/^([0-9]+\.[0-9])( .*$)/) {
      my $timeSec = $1;
      my $rest = $2;
      my $timeMin = $timeSec / 60;
      my $minutes = sprintf ("%d", $timeMin);
      my $seconds = sprintf ("%02d", ($timeMin - $minutes) * 60);
      my $convertedtime = sprintf ("%d:%02d", $minutes, $seconds);
      # print with time format "minutes:seconds"
      print DF $convertedtime,$rest, "\n";
    } else {
      # print unchanged line
      print DF;
    }
  } # while
  close DF;
  close LF;
}

sub get_timeformat {
  my $logfile = shift;
  my $tf = undef;
  open LF, "< $logfile"
    or die "Can't open log file $logfile.\n";
  # search for a '------...'-line
  while (<LF>) {
    next unless /^\s*\d*(:|\.)\d*.*-/;
    #$VERBOSE and print "Found: $_";
    $tf = ":" if /^\d*:\d*/;
    $tf = "." if /^\d*\.\d*/;
    last;
  }
  close LF;
  return $tf;
}

sub read_cmdline_args {
  GetOptions ('verbose!'  => \$VERBOSE,
	      'cfgfile=s' => \$CFGFILE,
	      'logfile=s' => \$LOGFILE,
	      'destdir=s' => \$DESTDIR,
	      'help'      => \$HELP,
	      'man'       => \$MAN)
    or pod2usage(2);
}


# void main();
#
# The frontend to the fragistics engine.
# It takes command line options:
#   -f logfile
#   -o outputdir
#
# If no logfile given, it takes LOGS/logpath0 from fragistics.cfg.
# If no outputdir given, it create 'stats_LOGFILEBASENAME' (according
# to a logfile 'LOGFILEBASENAME.ext').
#
sub main {
  my $okfile;
  my $logfile;
  my $convertedlog;
  my $destdir;
  my $timeformat;

  # cmd line args and usage
  read_cmdline_args();
  pod2usage(1) if $HELP;
  pod2usage(-exitstatus => 0, -verbose => 2) if $MAN;

  # logfile
  $logfile = $LOGFILE;
  $logfile = get_cfg_value ('LOGS', 'logpath0') unless $logfile;
  print "Log file: $logfile\n" if $VERBOSE;

  # time format
  $timeformat = get_timeformat ($logfile);
  if ($timeformat eq ':') {
    print "Time format 'min:sec' ... good.\n" if $VERBOSE;
  }

  # convert time format if necessary
  if ($timeformat eq '.') {
    print "Time format 'sec.tenthsec' ... convert to 'min:sec'.\n" if $VERBOSE;
    $convertedlog = $logfile;
    chomp $convertedlog;
    $convertedlog =~ s/\.[^.]*$//;
    $convertedlog .= '.convertedlog';
    if (-e $convertedlog) {
      print "A converted logfile $convertedlog already exists, use that.\n" if $VERBOSE;
    } else {
      print "Converting $logfile -> $convertedlog ...\n" if $VERBOSE;
      convert_timeformat ($logfile, $convertedlog);
    }
    $okfile = $convertedlog;
  } else {
    $okfile = $logfile;
  }

  # call parameterized fragistics
  $destdir = $DESTDIR;
  unless ($destdir) {
    $destdir = $logfile;
    $destdir =~ s/\.[^.]*$//;
    $destdir = './stats_'.$destdir;
  }
  print "Execute fragistics (FILE: $okfile, DESTDIR: $destdir) ...\n" if $VERBOSE;
  system ('mkdir', $destdir) unless -e $destdir;
  system ('./fragistics',
	  '\LOGS/number_of_logs', 1,
	  '\LOGS/logpath0', $okfile,
	  '\MAIN/dest_path', $destdir);
}

main;

__END__

=head1 NAME

frontend - Frontend to fragistics.

=head1 SYNOPSIS

frontend [options]

 Options:
  --help      ... print this help text.

  --verbose
  --noverbose ... [Do not] describe what is going on.

  --cfgfile   ... Which configfile to use
                  (default: "fragistics.cfg")

  --logfile   ... Which logfile to parse
                  (default: read from config file)

  --destdir   ... Which directory to write
                  (default: "./stats_LOGBASENAME")

=head1 DESCRIPTION

B<This program> prepares everything expected by the fragistics engine. It
mainly converts the time format and solves known artefacts in broken logfiles,
e.g., incomplete lines due to Q3 server restarts.

=cut
