#!/usr/bin/env perl

use strict;
use warnings;

@ARGV == 4 or die "usage: $0 INPUT.lib CFF48.obj CFF84.obj OUTPUT.lib\n";
my ($input_path, $cff48_path, $cff84_path, $output_path) = @ARGV;

sub read_raw {
    my ($path) = @_;
    open my $fh, '<:raw', $path or die "$path: $!\n";
    local $/;
    my $data = <$fh>;
    close $fh or die "$path: $!\n";
    return $data;
}

my $archive = read_raw($input_path);
my %replacement = (
    'cff48.obj' => read_raw($cff48_path),
    'cff84.obj' => read_raw($cff84_path),
);
substr($archive, 0, 7) eq "!<ar>!\n"
    or die "$input_path: invalid ar166 header\n";

my $result = "!<ar>!\n";
my %patched;
pos($archive) = 7;
while (pos($archive) < length($archive)) {
    $archive =~ /\G!<ar:(.*?)\s+(\d+)>!\n/gcs
        or die "$input_path: invalid member header\n";
    my ($name, $size) = ($1, $2);
    $name =~ s/\s+$//;
    my $member = substr($archive, pos($archive), $size);
    length($member) == $size or die "$input_path: truncated $name\n";
    pos($archive) += $size;

    if (exists $replacement{$name}) {
        $member = $replacement{$name};
        $patched{$name}++;
    }
    $result .= sprintf("!<ar:%-20s %d>!\n", $name, length($member));
    $result .= $member;
}
for my $name (sort keys %replacement) {
    ($patched{$name} // 0) == 1 or die "$input_path: missing $name\n";
}

open my $output_fh, '>:raw', $output_path or die "$output_path: $!\n";
print {$output_fh} $result or die "$output_path: $!\n";
close $output_fh or die "$output_path: $!\n";
