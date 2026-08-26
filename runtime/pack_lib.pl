#!/usr/bin/env perl

use strict;
use warnings;
use File::Spec;

@ARGV == 3 or die "usage: $0 WORK_DIR ORDER.txt OUTPUT.lib\n";
my ($work_dir, $order_path, $output_path) = @ARGV;

open my $order, '<', $order_path or die "$order_path: $!\n";
my $archive = "!<ar>!\n";
while (my $line = <$order>) {
    chomp $line;
    next unless length $line;
    my ($name, $relative) = split /\t/, $line, 2;
    defined $relative or die "$order_path: invalid line '$line'\n";
    length($name) <= 20 or die "$name: archive name too long\n";
    my $path = File::Spec->catfile($work_dir, $relative);
    open my $object_fh, '<:raw', $path or die "$path: $!\n";
    local $/;
    my $object = <$object_fh>;
    close $object_fh or die "$path: $!\n";
    $archive .= sprintf("!<ar:%-20s %d>!\n", $name, length($object));
    $archive .= $object;
}
close $order or die "$order_path: $!\n";

open my $output, '>:raw', $output_path or die "$output_path: $!\n";
print {$output} $archive or die "$output_path: $!\n";
close $output or die "$output_path: $!\n";
