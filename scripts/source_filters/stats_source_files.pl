#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use File::Basename;


# Command line arguments
my $invalid = 0;
my $help = '';
my @roots;
GetOptions("help"  => \$help,
           "dir=s" => \@roots
          ) or $invalid = 1;
$invalid = 1 if scalar @ARGV;  # catch invalid options without '-' or '--'
if ($invalid) {
    warn "Unknown option: $_\n" for @ARGV;
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
    basename($0), " [--help] --dir=input/\n\n",
    "\tCount the number of C++ source files in given directory.\n",
    "\tDirectories are traversed recursively.\n",
    "\tMultiple directories are allowed by repeating the option pair.\n",
    "\n";
}


########
# Main #
########
my $numfiles = 0;
if (@roots) {
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
                    $numfiles++ if $file =~ /(\.cpp|\.h)$/;
                }
            }
        }
    }

    print "Number of C++ files: $numfiles\n";
}
else {
    usage();
}

exit 0;

