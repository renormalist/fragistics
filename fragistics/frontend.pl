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
use Shell;

use strict;
use warnings;

# constants
my $MYNAME   = 'fragistics';
my $TPLTNAME = 'templates';
my $MAXLEN   = 1024;
my $CFGNAME  = "$MYNAME.conf";

# global cmdline options
my $CFGFILE         = undef;
my $LOGFILE         = undef;
my $DESTDIR         = undef;
my $VERBOSE         = 1;
my $HELP            = 0;
my $MAN             = 0;
my $FORCE           = 0;
my $FRAGISTICS_HOME = undef;
my $TPLTPATH         = undef;

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
    or die "Can't open config file $CFGFILE\n";
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
    or die "Can't open log file $logfile\n";
  open DF, "> $destfile"
    or die "Can't open destination file $destfile\n";
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
    or die "Can't open log file $logfile\n";
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

  # -- defaults --
  my @alternatives;
  my $param_cfgfile  = undef;
  my $param_homedir  = undef;
  my $param_logfile  = undef;
  my $param_tpltpath = undef;

  # cmdline options
  GetOptions ('verbose!'   => \$VERBOSE,
	      'cfgfile=s'  => \$param_cfgfile,
	      'logfile=s'  => \$param_logfile,
	      'destdir=s'  => \$DESTDIR,
	      'homedir=s'  => \$param_homedir,
	      'tpltpath=s' => \$param_tpltpath,
	      'force!'     => \$FORCE,
	      'help'       => \$HELP,
	      'man'        => \$MAN)
    or pod2usage(2);

  # homedir
  @alternatives = ();
  push (@alternatives, $param_homedir) if $param_homedir;
  push (@alternatives, "$ENV{'FRAGISTICS_HOME'}") if $ENV{'FRAGISTICS_HOME'};
  push (@alternatives, "$ENV{'HOME'}/.$MYNAME") if $ENV{'HOME'};
  push (@alternatives, "./.$MYNAME");
  $FRAGISTICS_HOME = $alternatives[0];
  printf "Homedir $FRAGISTICS_HOME\n" if $VERBOSE;

  # source path
  @alternatives = ();
  push (@alternatives, $param_tpltpath) if $param_tpltpath;
  push (@alternatives, "$FRAGISTICS_HOME/$TPLTNAME") if $FRAGISTICS_HOME && -d "$FRAGISTICS_HOME/$TPLTNAME";
  push (@alternatives, "/etc/$MYNAME/$TPLTNAME") if -d "/etc/$MYNAME/$TPLTNAME";
  $TPLTPATH = $alternatives[0]
    or die "Can't find $TPLTNAME directory in $FRAGISTICS_HOME /etc/$MYNAME\n";
  printf "Template path $TPLTPATH\n" if $VERBOSE;

  # cfg file
  @alternatives = ();
  push (@alternatives, $param_cfgfile) if $param_cfgfile;
  push (@alternatives, "$FRAGISTICS_HOME/$CFGNAME") if $FRAGISTICS_HOME && -R "$FRAGISTICS_HOME/$CFGNAME";
  push (@alternatives, "$ENV{'HOME'}/$CFGNAME") if ($ENV{'HOME'} and -R "$ENV{'HOME'}/$CFGNAME");
  push (@alternatives, "/etc/$MYNAME/$CFGNAME") if -R "/etc/$MYNAME/$CFGNAME";
  push (@alternatives, "/etc/$CFGNAME") if -R "/etc/$CFGNAME";
  push (@alternatives, "./$CFGNAME") if -R "./$CFGNAME";
  $CFGFILE = $alternatives[0]
    or die "Can't find config file $CFGNAME in $FRAGISTICS_HOME $ENV{'HOME'} /etc/$MYNAME /etc .\n";
  printf "Config file $CFGFILE\n" if $VERBOSE;

  # logfile
  @alternatives = ();
  my $cfglogfile = get_cfg_value ('LOGS', 'logpath0');
  push (@alternatives, $param_logfile) if $param_logfile;
  push (@alternatives, $cfglogfile) if $cfglogfile;
  push (@alternatives, "games.log");
  $LOGFILE = $alternatives[0];
  print "Log file $LOGFILE\n" if $VERBOSE;

  # homedir
  unless (-e $FRAGISTICS_HOME) {
    printf "Create homedir $FRAGISTICS_HOME\n" if $VERBOSE;
    system ('mkdir', $FRAGISTICS_HOME) # returns 0 if ok
      and die "Can't create homedir $FRAGISTICS_HOME\n";
  }

}


# void main();
#
# The frontend to the fragistics engine.
# It takes command line options:
#   -f logfile
#   -o outputdir
#
# If no logfile given, it takes LOGS/logpath0 from fragistics.conf.
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

  # time format
  $timeformat = get_timeformat ($LOGFILE);
  if ($timeformat eq ':') {
    print "Time format 'min:sec' ... good\n" if $VERBOSE;
  }

  # convert time format if necessary
  if ($timeformat eq '.') {
    print "Time format 'sec.tenthsec' ... convert to format 'min:sec'\n" if $VERBOSE;
    $convertedlog = basename ($LOGFILE);
    chomp $convertedlog;
    $convertedlog =~ s/\.[^.]*$//;
    $convertedlog .= '.convertedlog';
    $convertedlog = "$FRAGISTICS_HOME/$convertedlog";
    if (-e $convertedlog and !$FORCE) {
      print "Converted log $convertedlog already exists,\n" if $VERBOSE;
      print "  so we use it. Use --force to recreate.\n" if $VERBOSE;
    } else {
      print "Converting $LOGFILE -> $convertedlog ...\n" if $VERBOSE;
      convert_timeformat ($LOGFILE, $convertedlog);
    }
    $okfile = $convertedlog;
  } else {
    $okfile = $LOGFILE;
  }

  # dest dir
  $destdir = $DESTDIR;
  unless ($destdir) {
    $destdir = $LOGFILE;
    $destdir =~ s/\.[^.]*$//;
    $destdir = "$FRAGISTICS_HOME/stats_".$destdir;
  }
  if (-e $destdir and not $FORCE) {
    die "Destination dir $destdir already exists,\n  use --force to overwrite.\n";
  } else {
    print "Create $destdir\n" if $VERBOSE;
    system ('mkdir', '-p', $destdir)  # returns 0 if ok
      and die "Can't create $destdir\n";
  }

  # call parameterized fragistics
  print "Execute $MYNAME $okfile -> $destdir) ...\n" if $VERBOSE;
  system ('fragistics',
	  '\LOGS/number_of_logs', 1,
	  '\LOGS/logpath0', $okfile,
	  '\MAIN/dest_path', $destdir,
	  '\MAIN/cfgfile', $CFGFILE,
	  '\MAIN/src_path', $TPLTPATH
	 )
    and die "$@";
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

  --homedir   ... Homedir, used as default for input/output files
                  (default: $FRAGISTICS_HOME or $HOME/.fragistics)

  --cfgfile   ... Which configfile to use
                  (default: "fragistics.conf" in $HOME/.fragistics:/etc/fragistics)

  --logfile   ... Which logfile to parse
                  (default: read from config file)

  --destdir   ... Which directory to write
                  (default: "<homedir>/stats_<logfilebasename>")


=head1 DESCRIPTION

B<This program> prepares everything expected by the fragistics engine. It
mainly converts the time format and solves known artefacts in broken logfiles,
e.g., incomplete lines due to Q3 server restarts.

=cut
