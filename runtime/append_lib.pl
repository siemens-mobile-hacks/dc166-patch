#!/usr/bin/env perl

use strict;
use warnings;

@ARGV >= 4 or die "usage: $0 BASE.lib OUTPUT.lib MODEL MEMBER.obj...\n";
my ($base_path, $output_path, $model, @objects) = @ARGV;

my %model_tags = (
    t => "\x00\x00\x10\x02\x0f",
    m => "\x00\x00\x30\x03\x0f",
    l => "\x00\x00\x40\x03\x0f",
    h => "\x00\x00\x50\x03\x0f",
);
exists $model_tags{$model} or die "$0: invalid model '$model'\n";

open my $base_fh, '<:raw', $base_path or die "$base_path: $!\n";
local $/;
my $archive = <$base_fh>;
close $base_fh or die "$base_path: $!\n";
substr($archive, 0, 7) eq "!<ar>!\n"
    or die "$base_path: invalid ar166 header\n";

my %all_tags = (%model_tags, s => "\x00\x00\x20\x02\x0f");
my $target_tag = $model_tags{$model};
my $target_user_stack = $model eq 't' ?
    "\x02\x01\x70\x7d" : "\x02\x01\x70\x7a";

for my $path (@objects) {
    open my $object_fh, '<:raw', $path or die "$path: $!\n";
    my $object = <$object_fh>;
    close $object_fh or die "$path: $!\n";

    for my $candidate (sort keys %all_tags) {
        next if $candidate eq $model;
        index($object, $all_tags{$candidate}) < 0
            or die "$path: $candidate model tag present in $model library\n";
    }
    (index($object, $target_tag) >= 0 ||
     index($object, $target_user_stack) >= 0)
        or die "$path: $model model tag missing\n";

    my $name = $path;
    $name =~ s{.*[/\\]}{};
    length($name) <= 20 or die "$path: member name is longer than 20 bytes\n";
    $archive .= sprintf("!<ar:%-20s %d>!\n", $name, length($object));
    $archive .= $object;
}

open my $output_fh, '>:raw', $output_path or die "$output_path: $!\n";
print {$output_fh} $archive or die "$output_path: $!\n";
close $output_fh or die "$output_path: $!\n";
