#!/usr/bin/env perl

use strict;
use warnings;

@ARGV == 4 or die "usage: $0 INPUT.lib MANIFEST MODEL OUTPUT.lib\n";
my ($input, $manifest, $model, $output) = @ARGV;

my %model_tags = (
    t => "\x00\x00\x10\x02\x0f",
    m => "\x00\x00\x30\x03\x0f",
    l => "\x00\x00\x40\x03\x0f",
    h => "\x00\x00\x50\x03\x0f",
);
exists $model_tags{$model} or die "$0: invalid model '$model'\n";

open my $manifest_fh, '<', $manifest or die "$manifest: $!\n";
my @wanted;
while (my $line = <$manifest_fh>) {
    $line =~ s/#.*//;
    $line =~ s/^\s+|\s+$//g;
    push @wanted, $line if length $line;
}
close $manifest_fh or die "$manifest: $!\n";

open my $input_fh, '<:raw', $input or die "$input: $!\n";
local $/;
my $archive = <$input_fh>;
close $input_fh or die "$input: $!\n";

substr($archive, 0, 7) eq "!<ar>!\n"
    or die "$input: invalid ar166 header\n";

my %members;
pos($archive) = 7;
while (pos($archive) < length($archive)) {
    $archive =~ /\G!<ar:(.*?)\s+(\d+)>!\n/gcs
        or die "$input: invalid member header at offset " . pos($archive) . "\n";
    my ($name, $size) = ($1, $2);
    $name =~ s/\s+$//;
    pos($archive) + $size <= length($archive)
        or die "$input: truncated member $name\n";
    exists $members{$name} and die "$input: duplicate member $name\n";
    $members{$name} = substr($archive, pos($archive), $size);
    pos($archive) += $size;
}

my $small_tag = "\x00\x00\x20\x02\x0f";
my $small_user_stack = "\x02\x01\x70\x7d";
my $paged_user_stack = "\x02\x01\x70\x7a";
my $target_tag = $model_tags{$model};
my $target_user_stack = $model eq 't' ? $small_user_stack : $paged_user_stack;
my $result = "!<ar>!\n";

for my $name (@wanted) {
    exists $members{$name} or die "$input: missing member $name\n";
    my $member = $members{$name};
    my $tags = ($member =~ s/\Q$small_tag\E/$target_tag/g);
    $tags == 1 or die "$input: expected one Small model tag in $name, found $tags\n";
    my $stack_sections = 0;
    if ($target_user_stack ne $small_user_stack) {
        $stack_sections =
            ($member =~ s/\Q$small_user_stack\E/$target_user_stack/g);
    } else {
        $stack_sections = () = $member =~ /\Q$small_user_stack\E/g;
    }
    $stack_sections <= 1
        or die "$input: found multiple user-stack sections in $name\n";
    $result .= sprintf("!<ar:%-20s %d>!\n", $name, length($member));
    $result .= $member;
}

open my $output_fh, '>:raw', $output or die "$output: $!\n";
print {$output_fh} $result or die "$output: $!\n";
close $output_fh or die "$output: $!\n";
