#! /usr/bin/env perl

###################################################################
#         FILE: create_container_links.pl
# SW-COMPONENT: Software update
#  DESCRIPTION: Software update is responsible for updating the 
#               firmware of the targets.
#    COPYRIGHT: (C) Robert Bosch Car Multimedia GmbH
#       AUTHOR: Philipp Blanke (CM-CI1/ESW1) <philipp.blanke@de.bosch.com>
#
# The reproduction, distribution and utilization of this file as
# well as the communication of its contents to others without express
# authorization is prohibited. Offenders will be held liable for the
# payment of damages. All rights reserved in the event of the grant
# of a patent, utility model or design.
###################################################################

use strict;
use warnings;

use Carp;                     # warnings and errors

my $VERBOSE = 0;
my $RFS_IS_MOUNTED_RW = 0;
use constant USAGE =>
   "This script creates links according to an XML description.\n" .
   "Links should point from the parent RFS into a Linux Container root path.\n" .
   "\n" .
   "USAGE\n" .
   "  perl create_container_links.pl OPTIONS\n" .
   "\n" .
   "OPTIONS\n" .
   "  --list FILE  File with XML formatted list of links\n" .
   "  --force      [optional] Force linking, remove existing destination files.\n" .
   "  --dryrun     [optional] Only print what would be done, don't actually link.\n" .
   "                          This finds some possible errors, but not all.\n" .
   "  --verbose    [optional] Enable debugging output.\n" .
   "  --help       [optional] Display this text.\n";

# --- main program ---
__PACKAGE__->main() unless caller();

sub main {
   # parse command line arguments and setup variables
   my %options = (
      # -- Strings
      list    => '',
      # -- Flags
      force   => 0,
      dryrun  => 0,
   );
   read_options(\%options);

   if ($options{dryrun}) {
      chomp(my $user = qx(whoami));
      if ($user ne "root") {
         _error("RFS has to be remounted. Only root can do that. Running as: $user",
            $options{dryrun});
      }
   }
   else {
      _mount_rfs_rw();
   }

   open (my $links_fh, '<', $options{list})
      or _fatal("Can't open $options{list}");

   my $root_node_found = 0;
   my $root_node_closed = 0;
   while (<$links_fh>) {
      if (!$root_node_found) {
         if ($_ =~ /<links>/) {
            $root_node_found = 1;
            next;
         }
         next;
      }
      elsif ($_ =~ /<\/links>/) {
         $root_node_closed = 1;
         last;
      }

      my $source = '';
      my $dest = '';
      if ($_ =~ /<hardlink.* source="([^"]*)" .*\/>/) {
         $source = $1;
      }
      if ($_ =~ /<hardlink.* dest="([^"]*)" .*\/>/) {
         $dest = $1;
      }

      if (!$source or !$dest) {
         _error("Missing 'source' or 'dest' attribute.",
            $options{dryrun});
      }

      my $ln_opt = "";
      if ($options{force}) {
         $ln_opt = "-f";
      }

      _print("ln $ln_opt $source $dest");

      if ($options{dryrun}) {
         next;
      }

      # discard stdout, collect stderr
      # if the command fails, $err is undef
      my $err = qx( ln $ln_opt $source $dest 2>&1 1> /dev/null );
      if (!defined $err or $err) {
         _error("Can't link. Reason:\n$err");
      }
   }
   if (!$root_node_found) {
      _error("There was no <links> node in $options{list}");
   }

   if (!$options{dryrun}) {
      _mount_rfs_ro();
   }
}

sub _mount_rfs_rw {
   # re-mount the current RFS writable
   my $result = system('/bin/mount -o rw,remount /');
   if ($result != 0) {
      my $user = qx(whoami);
      _fatal("Can't remount the RFS writable. Running as: $user");
   }
   $RFS_IS_MOUNTED_RW = 1;
}
sub _mount_rfs_ro {
   return unless $RFS_IS_MOUNTED_RW;

   # re-mount the current RFS read-only
   my $result = system('/bin/mount -o ro,remount /');
   if ($result != 0) {
      my $user = qx(whoami);
      _fatal("Can't remount the RFS read-only. Running as: $user");
   }
}

### Functions to print formatted output ###
sub _print {
   my ( $msg ) = @_;
   print $msg . "\n";
}
sub _debug {
   return unless ($VERBOSE);
   my ( $msg ) = @_;
   print "(debug) $msg\n";
}
sub _warn {
   my ( $msg ) = @_;
   print "(warn ) $msg\n";
}
sub _error {
   my ( $msg, $continue ) = @_;
   if (!defined $continue) { $continue = 0; }

   if (!$continue) {
      _mount_rfs_ro();
      die "(error) $msg\n";
   }
   print "(error) $msg\n";
}
sub _fatal {
   my ( $msg ) = @_;
   croak "(fatal) $msg\n";
}

### Function ###
#    Purpose: Evaluate options given to the script.
# Parameters: HashRef - Options
#    Returns: None
#
sub read_options {
   my ( $options_ref ) = @_;

   while (my $arg = shift @ARGV) {
      if ($arg =~ /^--l/) {
         $options_ref->{list} = shift @ARGV;
      }
      elsif ($arg =~ /^--f/) {
         $options_ref->{force} = 1;
      }
      elsif ($arg =~ /^--dr/) {
         $options_ref->{dryrun} = 1;
      }
      elsif ($arg =~ /^--de/ or $arg =~ /^--v/) {
         $VERBOSE = 1;
      }
      elsif ($arg =~ /^--u/ or $arg =~ /^--h/) {
         _print(USAGE);
         exit 0;
      }
      else {
         _error("Can't parse options.\n\n" . USAGE);
      }
   }

   # verify mandatory options
   for my $name ( ('list') ) {
      if (!$options_ref->{$name}) {
         _print(USAGE);
         _error("Mandatory option --$name is missing.\n\n");
      }
   }

   if (!-f $options_ref->{list}) {
      _error("--list no such file: $options_ref->{list}\n");
   }
}

