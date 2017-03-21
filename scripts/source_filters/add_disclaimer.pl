#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use File::Basename;
use Cwd;


# Command line arguments
my $invalid = 0;
my $help = '';
my $check = '';
my @infiles;
my @roots;
GetOptions("help"    => \$help,
           "check"   => \$check,
           "file=s"  => \@infiles,
           "dir=s"   => \@roots
          ) or $invalid = 1;
$invalid = 1 if scalar @ARGV;
if ($invalid) {
    print "Unknown option: $_\n" for @ARGV;
    usage();
    exit 0;
}
elsif ($help) {
    usage();
    exit 0;
}


# Print script usage
sub usage {
    print "\n",
    basename($0), " [--help] [--check] [--file=input.txt | --dir=input/]\n\n",
    "\tRemove trailing whitespaces for each line of input files.\n",
    "\tIf a file is provided, it can be of any type.\n",
    "\tIf a directory is provided, only C++ source files are processed.\n",
    "\tDirectories are traversed recursively.\n",
    "\tA provided file has priority over a provided directory.\n",
    "\tMultiple files or directories are allowed by repeating the option pair.\n",
    "\n";
}


# Trim trailing whitespaces
sub trim {
    if (/[^\S\r\n]$/) {
        s/\s+$/\n/;
        return 1;
    }
    return 0;
}


# Process a file based on a regex
sub processFile {
    my $file = shift;

    # Open file
    $file or die "ERROR! missing input file\n";
    open my $fh, '<', $file or die "ERROR! failed to open $file: $!\n";

    # Process file
    my $mflag = 0;  # set if at least a match is found
    my @lines;
    while (<$fh>) {
        my $flag = trim $_;
        $mflag |= $flag;
        warn "$file: $.: $_" if $flag;
        push @lines, $_;
    }
    close $fh;

    # If changes made, overwrite file
    if ($mflag and ! $check) {
        open my $fh, '>', $file or die "ERROR! failed to open $file: $!\n";
        foreach (@lines) {
            print $fh $_;
        }
        close $fh;
    }
}


########
# Main #
########
if (@infiles) {
    # Process files
    foreach (@infiles) {
        next if /^\./;  # ignore anything that starts with '.'
        processFile $_ if -f;
    }
}
elsif (@roots) {
    # Traverse entire directory trees
    # Select source files only for processing
    foreach my $root (@roots) {
        my @dirs = ($root);
        while (my $dir = shift @dirs) {
            opendir DIR, $dir or die "ERROR! failed to open $dir: $!\n";
            my @files = readdir DIR;
            closedir DIR;

            foreach (@files) {
                next if /^\./;  # ignore anything that starts with '.'

                my $file = "$dir/$_";
                if (-d $file) {
                    push @dirs, $file;
                }
                elsif (-f $file) {
                    processFile $file if $file =~ /(\.cpp|\.h)$/;
                }
            }
        }
    }
}
else {
    usage();
}

