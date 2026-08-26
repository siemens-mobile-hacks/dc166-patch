#!/usr/bin/env perl
use strict;
use warnings;

my $reverse = @ARGV && $ARGV[0] eq '--reverse';
shift @ARGV if $reverse;
my ($patch_path, $input_path, $output_path) = @ARGV;
die "usage: $0 [--reverse] PATCH.vkp INPUT OUTPUT\n"
    unless defined $output_path;

open my $in, '<:raw', $input_path or die "open $input_path: $!\n";
my $image;
{
    local $/;
    $image = <$in>;
}
close $in;

open my $pf, '<', $patch_path or die "open $patch_path: $!\n";
my $count = 0;
while (my $line = <$pf>) {
    $line =~ s/[;#].*$//;
    next if $line =~ /^\s*$/;
    $line =~ /^\s*([0-9a-fA-F]+):\s*([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s*$/
        or die "invalid VKP line $.: $line";
    my ($offset, $old_hex, $new_hex) = (hex($1), $2, $3);
    ($old_hex, $new_hex) = ($new_hex, $old_hex) if $reverse;
    die "odd hex length at line $.\n"
        if length($old_hex) & 1 || length($new_hex) & 1;
    die "length mismatch at line $.\n"
        unless length($old_hex) == length($new_hex);
    my $old = pack('H*', $old_hex);
    my $new = pack('H*', $new_hex);
    die sprintf("old bytes mismatch at %08X (line %u)\n", $offset, $.)
        unless substr($image, $offset, length($old)) eq $old;
    substr($image, $offset, length($new), $new);
    ++$count;
}
close $pf;

open my $out, '>:raw', $output_path or die "open $output_path: $!\n";
print {$out} $image;
close $out;
printf "%s %u records: %s -> %s\n",
    $reverse ? 'reversed' : 'applied', $count, $input_path, $output_path;
