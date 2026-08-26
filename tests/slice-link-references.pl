#!/usr/bin/env perl

use strict;
use warnings;

@ARGV == 4 or die "usage: $0 INDEX COUNT INPUT.asm OUTPUT.asm\n";
my ($index, $count, $input, $output) = @ARGV;
$index =~ /\A\d+\z/ && $count =~ /\A\d+\z/ && $count > 0 && $index < $count
    or die "$0: invalid slice $index/$count\n";

open my $input_fh, '<', $input or die "$input: $!\n";
my @lines = <$input_fh>;
close $input_fh or die "$input: $!\n";

my @references;
for my $line (@lines) {
    if ($line =~ /^\s*CALL(?:S\s+SEG)?\s+([^,\s]+)/ ||
        $line =~ /^\s*MOV\s+R1,#SOF\s+([^,\s]+)/) {
        push @references, $1;
    }
}
@references or die "$input: no link references found\n";

my %selected;
for my $ordinal (0 .. $#references) {
    $selected{$references[$ordinal]} = 1 if $ordinal % $count == $index;
}

open my $output_fh, '>', $output or die "$output: $!\n";
for my $line (@lines) {
    my $symbol;
    if ($line =~ /^\s*CALL(?:S\s+SEG)?\s+([^,\s]+)/ ||
        $line =~ /^\s*MOV\s+R1,#SOF\s+([^,\s]+)/ ||
        $line =~ /^\s*EXTERN\s+([^:\s]+):/) {
        $symbol = $1;
    }
    print {$output_fh} $line
        if !defined($symbol) || $selected{$symbol};
}
close $output_fh or die "$output: $!\n";

print "link_references_total=" . scalar(@references) . "\n";
print "link_references_selected=" . scalar(keys %selected) . "\n";
