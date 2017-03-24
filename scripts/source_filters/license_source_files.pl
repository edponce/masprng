#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use File::Basename;


# Command line arguments
my $invalid = 0;
my $help = '';
my $add = '';
my $remove = '';
my $overwrite = '';
my $license = '';
my @infiles;
my @roots;
GetOptions("help"      => \$help,
           "add"       => \$add,
           "remove"    => \$remove,
           "overwrite" => \$overwrite,
           "license=s" => \$license,
           "file=s"    => \@infiles,
           "dir=s"     => \@roots
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
    basename($0), " [--help] [--add] [--remove] [--overwrite] [--license=file.lic] [--file=input.cpp | --dir=input/]\n\n",
    "\tAdd/remove license to/from source files.\n",
    "\tLicense should be given as a file.\n",
    "\tIf a file is provided, it can be of any type.\n",
    "\tIf a directory is provided, only C++ source files are processed.\n",
    "\tDirectories are traversed recursively.\n",
    "\tA provided file has priority over a provided directory.\n",
    "\tMultiple files or directories are allowed by repeating the option pair.\n",
    "\n";
}


# Check if file has license keyword
sub hasLicense {
    return /.*COPYRIGHT.*/ ? 1 : 0;
}


# Add licensing to file 
sub licenseFile {
    my $file = shift;

    # Process file
    $file or die "ERROR! missing input file\n";
    open my $fh, '<', $file or die "ERROR! failed to open $file: $!\n";
    my $mflag = 0;  # set if a match is found
    my $lcount = 0;  # count lines used by license, if existing
    while (<$fh>) {
        $mflag |= hasLicense $_;
        last if ! $mflag and $. == 5;
        last if ! /^\/\//;  # all license lines are single line comments
        $lcount++;
    }
    close $fh;

    $lcount = 0 if ! $mflag;

    # Print file status
    print "$file: ", ($mflag ? "YES" : "NO"), " license with $lcount lines.\n";

    return if (! $add and ! $remove and ! $overwrite);
    return if ($mflag and $add);
    return if (! $mflag and $remove);

    # Load new license
    my @lines;
    #if ((! $mflag and $add) or $overwrite) {
    if ($add or $overwrite) {
        open my $lh, '<', $license or die "ERROR! failed to open $license: $!\n";
        while (<$lh>) {
            push @lines, $_;
        }
        close $lh;
    }

    # Remove existing license
    # Load file
    #if ($mflag and ($remove or $overwrite)) {
    #if ($remove or $overwrite) {
        open $fh, '<', $file or die "ERROR! failed to open $file: $!\n";
        while (<$fh>) {
            next if $mflag and $. <= $lcount;  # skip existing license 
            push @lines, $_;
        }
        close $fh;
    #}

    # Overwrite file
    open $fh, '>', $file or die "ERROR! failed to open $file: $!\n";
    foreach (@lines) {
        print $fh $_;
    }
    close $fh;
}


########
# Main #
########
if (@infiles) {
    # Process files
    foreach (@infiles) {
        next if /^\./;  # ignore anything that starts with '.'
        licenseFile $_ if -f;
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
                    licenseFile $file if $file =~ /(\.cpp|\.h)$/;
                }
            }
        }
    }
}
else {
    usage();
}

exit 0;

