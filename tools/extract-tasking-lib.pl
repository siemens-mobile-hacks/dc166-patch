#!/usr/bin/env perl

use strict;
use warnings;
use File::Path qw(make_path);
use File::Spec;

@ARGV == 2 or die "usage: $0 INPUT.lib OUTPUT_DIR\n";
my ($input, $output_dir) = @ARGV;

open my $input_fh, '<:raw', $input or die "$input: $!\n";
local $/;
my $archive = <$input_fh>;
close $input_fh or die "$input: $!\n";
substr($archive, 0, 7) eq "!<ar>!\n"
    or die "$input: invalid ar166 header\n";

make_path($output_dir);
pos($archive) = 7;
while (pos($archive) < length($archive)) {
    $archive =~ /\G!<ar:(.*?)\s+(\d+)>!\n/gcs
        or die "$input: invalid member header at offset " . pos($archive) . "\n";
    my ($name, $size) = ($1, $2);
    $name =~ s/\s+$//;
    $name =~ /\A[A-Za-z0-9_.-]+\z/
        or die "$input: unsafe member name '$name'\n";
    pos($archive) + $size <= length($archive)
        or die "$input: truncated member $name\n";

    my $path = File::Spec->catfile($output_dir, $name);
    open my $output_fh, '>:raw', $path or die "$path: $!\n";
    print {$output_fh} substr($archive, pos($archive), $size)
        or die "$path: $!\n";
    close $output_fh or die "$path: $!\n";
    pos($archive) += $size;
}
