#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use File::Basename;


# Command line arguments
my $invalid = 0;
my $help = '';
my $check = '';
my $remove = '';
my $license = '';
my @infiles;
my @roots;
GetOptions("help"      => \$help,
           "check"     => \$check,
           "remove"    => \$remove,
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
    basename($0), " [--help] [--check] [--remove] [--license=file.lic] [--file=input.cpp | --dir=input/]\n\n",
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
        warn "$file: $.: $_" if $mflag;
        last if ! $mflag and $. == 5;
        last if ! /^\/\//;  # all license lines are single line comments
        $lcount++;
    }
    $lcount = 0 if ! $mflag;
    close $fh;

    if (! $check) {
        if (! $mflag) {
            # Load license
            open my $lh, '<', $license or die "ERROR! failed to open $license: $!\n";
            my @lines = <$lh>;
            close $lh;

            # Load file
            open $fh, '<', $file or die "ERROR! failed to open $file: $!\n";
            while (<$fh>) {
                push @lines, $_;
            }
            close $fh;

            # Overwrite file
            open $fh, '>', $file or die "ERROR! failed to open $file: $!\n";
            foreach (@lines) {
                print $fh $_;
            }
            close $fh;
        }
        elsif ($mflag and $remove) {
            # Load file
            open $fh, '<', $file or die "ERROR! failed to open $file: $!\n";
            my @lines;
            while (<$fh>) {
                next if $. <= $lcount;  # skip existing license 
                push @lines, $_;
            }
            close $fh;

            # Overwrite file
            open $fh, '>', $file or die "ERROR! failed to open $file: $!\n";
            foreach (@lines) {
                print $fh $_;
            }
            close $fh;
        }
    }
}


########
# Main #
########
if ($license and -f $license) {
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
}
else {
    usage();
}

exit 0;

