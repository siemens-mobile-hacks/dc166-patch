#!/usr/bin/env perl

use strict;
use warnings;

@ARGV == 18 or die "usage: $0 INPUT.lib ADF4.obj MLF4.obj DVF4.obj CIF44.obj " .
    "ADF8.obj MLF8.obj DVF8.obj CFI82.obj CFU82.obj " .
    "CFI84.obj CFU84.obj CIF48.obj " .
    "CFF48.obj CFF84.obj FP32.obj FP64.obj " .
    "OUTPUT.lib\n";
my ($input_path, $adf4_path, $mlf4_path, $dvf4_path, $cif44_path,
    $adf8_path, $mlf8_path, $dvf8_path, $cfi82_path, $cfu82_path,
    $cfi84_path, $cfu84_path,
    $cif48_path, $cff48_path, $cff84_path,
    $fp32_path, $fp64_path, $output_path) = @ARGV;

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
    'adf4.obj' => read_raw($adf4_path),
    'mlf4.obj' => read_raw($mlf4_path),
    'dvf4.obj' => read_raw($dvf4_path),
    'cif44.obj' => read_raw($cif44_path),
    'adf8.obj' => read_raw($adf8_path),
    'mlf8.obj' => read_raw($mlf8_path),
    'dvf8.obj' => read_raw($dvf8_path),
    'cfi82.obj' => read_raw($cfi82_path),
    'cfu82.obj' => read_raw($cfu82_path),
    'cfi84.obj' => read_raw($cfi84_path),
    'cfu84.obj' => read_raw($cfu84_path),
    'cif48.obj' => read_raw($cif48_path),
    'cff48.obj' => read_raw($cff48_path),
    'cff84.obj' => read_raw($cff84_path),
);
my $fp32 = read_raw($fp32_path);
my $fp64 = read_raw($fp64_path);
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
$result .= sprintf("!<ar:%-20s %d>!\n", 'fp32.obj', length($fp32));
$result .= $fp32;
$result .= sprintf("!<ar:%-20s %d>!\n", 'fp64.obj', length($fp64));
$result .= $fp64;

open my $output_fh, '>:raw', $output_path or die "$output_path: $!\n";
print {$output_fh} $result or die "$output_path: $!\n";
close $output_fh or die "$output_path: $!\n";
