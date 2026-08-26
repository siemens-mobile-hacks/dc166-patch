#!/usr/bin/env perl
use strict;
use warnings;

my ($input, $output, $payload_path) = @ARGV;
die "usage: $0 INPUT_EXE OUTPUT_EXE PAYLOAD_BIN\n" unless defined $payload_path;

open my $in, '<:raw', $input or die "open $input: $!\n";
local $/;
my $exe = <$in>;
close $in;

open my $pf, '<:raw', $payload_path or die "open $payload_path: $!\n";
my $payload = <$pf>;
close $pf;

my $stub_off = 0x00022ce0;
my $stub_va  = 0x00422ce0;
my $cave_off = 0x0010e0c0;
my $cave_va  = 0x0050e0c0;

my @fixed_patches = (
    [0x0000914a,
     '83ff06740a83ff08740583ff05750b',
     '83ff04721583ff06760583ff08750b'],
    [0x00009f03,
     '83f806741b83f808741683f8057411',
     '83f804720a83f806761683f8087411'],
    [0x00048d55,
     '68c8125100680f220000e8fc36050083c408',
     'c705707f5b0002000000e903000000909090'],
    [0x00046899, '68c8125100', 'eb17909090'],
    [0x00029537, '0f841e060000', '909090909090'],
);

my $old_stub = pack('H*', '68c8125100680f220000e87197070083c408c3');
die "unexpected FP stub bytes\n"
    unless substr($exe, $stub_off, length($old_stub)) eq $old_stub;
die "payload is too large\n" if length($payload) > 0x0f40;
die "code cave is not empty\n"
    unless substr($exe, $cave_off, length($payload)) eq "\0" x length($payload);

for my $patch (@fixed_patches) {
    my ($off, $old_hex, $new_hex) = @$patch;
    my $old = pack('H*', $old_hex);
    my $new = pack('H*', $new_hex);
    die sprintf("unexpected bytes at %08X\n", $off)
        unless substr($exe, $off, length($old)) eq $old;
    die sprintf("patch length mismatch at %08X\n", $off)
        unless length($old) == length($new);
    substr($exe, $off, length($new), $new);
}

my $rel = $cave_va - ($stub_va + 5);
my $hook = "\xE9" . pack('l<', $rel) . ("\x90" x (length($old_stub) - 5));
substr($exe, $stub_off, length($hook), $hook);
substr($exe, $cave_off, length($payload), $payload);

open my $out, '>:raw', $output or die "open $output: $!\n";
print {$out} $exe;
close $out;

printf "patched %s: stub=%08X cave=%08X payload=%u bytes\n",
    $output, $stub_off, $cave_off, length($payload);
