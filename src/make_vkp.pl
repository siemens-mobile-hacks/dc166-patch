#!/usr/bin/env perl
use strict;
use warnings;

my ($old_path, $new_path, $vkp_path) = @ARGV;
die "usage: $0 OLD_FILE NEW_FILE OUTPUT.vkp\n" unless defined $vkp_path;

sub slurp {
    my ($path) = @_;
    open my $fh, '<:raw', $path or die "open $path: $!\n";
    local $/;
    my $data = <$fh>;
    close $fh;
    return $data;
}

my $old = slurp($old_path);
my $new = slurp($new_path);
die "file sizes differ\n" unless length($old) == length($new);

open my $out, '>', $vkp_path or die "open $vkp_path: $!\n";
for (my $offset = 0; $offset < length($old); $offset += 16) {
    my $old_block = substr($old, $offset, 16);
    my $new_block = substr($new, $offset, 16);
    next if $old_block eq $new_block;
    printf {$out} "%08X: %s %s\n", $offset,
        uc(unpack('H*', $old_block)), uc(unpack('H*', $new_block));
}
close $out;
