#!/usr/bin/env perl

use strict;
use warnings;

@ARGV == 3 or die "usage: $0 INPUT.lib CFF48.obj OUTPUT.lib\n";
my ($input_path, $replacement_path, $output_path) = @ARGV;

sub read_raw {
    my ($path) = @_;
    open my $fh, '<:raw', $path or die "$path: $!\n";
    local $/;
    my $data = <$fh>;
    close $fh or die "$path: $!\n";
    return $data;
}

my $archive = read_raw($input_path);
my $replacement = read_raw($replacement_path);
substr($archive, 0, 7) eq "!<ar>!\n"
    or die "$input_path: invalid ar166 header\n";

my $result = "!<ar>!\n";
my $patched = 0;
pos($archive) = 7;
while (pos($archive) < length($archive)) {
    $archive =~ /\G!<ar:(.*?)\s+(\d+)>!\n/gcs
        or die "$input_path: invalid member header\n";
    my ($name, $size) = ($1, $2);
    $name =~ s/\s+$//;
    my $member = substr($archive, pos($archive), $size);
    length($member) == $size or die "$input_path: truncated $name\n";
    pos($archive) += $size;

    if ($name eq 'cff48.obj') {
        $member = $replacement;
        $patched++;
    }
    $result .= sprintf("!<ar:%-20s %d>!\n", $name, length($member));
    $result .= $member;
}
$patched == 1 or die "$input_path: missing cff48.obj\n";

open my $output_fh, '>:raw', $output_path or die "$output_path: $!\n";
print {$output_fh} $result or die "$output_path: $!\n";
close $output_fh or die "$output_path: $!\n";
