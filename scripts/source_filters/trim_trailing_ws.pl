#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use Cwd;


# Command line arguments
my $check = '';
my $root = getcwd();
GetOptions("check" => \$check,
           "root=s" => \$root);


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
    $file or die "ERROR! missing input file: $!";
    open my $fh, '<', $file or die "ERROR! failed to open $file: $!";

    # Process file
    my $mflag = 0;  # set if at least match is found
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
        open my $fh, '>', $file or die "ERROR! failed to open $file: $!";
        foreach (@lines) {
            print $fh $_;
        }
        close $fh;
    }
}


# Main
# Traverse entire directory tree
# Select source files only for processing
my @dirs = ($root);
while (my $dir = shift @dirs) {
    opendir DIR, $dir or die "ERROR! failed to open $dir: $!";
    my @files = readdir DIR;
    closedir DIR;

    foreach (@files) {
        next if /^\..*$/;  # ignore anything starting with '.'

        my $file = "$dir/$_";
        if (-d $file) {
            push @dirs, $file;
        }
        elsif (-f $file and $file =~ /\.cpp$|\.h$/) {
            processFile $file;
        }
    }
}

