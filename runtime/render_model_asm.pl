#!/usr/bin/env perl

use strict;
use warnings;

(@ARGV == 3 || @ARGV == 4)
    or die "usage: $0 MODEL INPUT.asm OUTPUT.asm [ext|ext2]\n";
my ($model, $input, $output, $architecture) = @ARGV;
$architecture //= 'ext';
my %name = (t => 'TINY', s => 'SMALL', m => 'MEDIUM', l => 'LARGE', h => 'HUGE');
exists $name{$model} or die "$0: invalid model '$model'\n";
($architecture eq 'ext' || $architecture eq 'ext2')
    or die "$0: invalid architecture '$architecture'\n";

open my $in, '<', $input or die "$input: $!\n";
local $/;
my $source = <$in>;
close $in or die "$input: $!\n";

$source =~ s/^\$EXTEND\s*$/\$EXTEND2/m if $architecture eq 'ext2';
$source =~ s/^\$(?:NON)?SEGMENTED\s*$/\$@{[$model eq 't' || $model eq 's' ? 'NONSEGMENTED' : 'SEGMENTED']}/m;
$source =~ s/^\$MODEL\([^)]*\)\s*$/\$MODEL($name{$model})/m;
if ($model eq 't' || $model eq 'm') {
    $source =~ s/ PROC FAR\b/ PROC NEAR/g;
    $source =~ s/^[ \t]*CALLS[ \t]+SEG[ \t]+([^,\s]+),\1[ \t]*$/        CALL $1/mg;
    $source =~ s/^([ \t]*EXTERN[ \t]+[^:\s]+):FAR[ \t]*$/$1:NEAR/mg;
    $source =~ s/^\s*RETS\s*$/        RET/mg;
}
# Tiny passes the near jmp_buf pointer in R13 and the longjmp value in R13;
# the other models use the far-pointer/value registers emitted by this source.
if ($model eq 't' && $input =~ m{(?:^|[/\\])setjmp\.asm$}) {
    $source =~ s/^\s*EXTP R13,#01h\s*\n//mg;
    $source =~ s/MOV R4,R14/MOV R4,R13/;
}

open my $out, '>', $output or die "$output: $!\n";
print {$out} $source or die "$output: $!\n";
close $out or die "$output: $!\n";
