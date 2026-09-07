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
my $argument_slot_call_off = 0x000295e3;
my $argument_slot_call_va  = 0x004295e3;
my $argument_slot_hook_va  = $cave_va + 0x0f00;
my $argument_slot_check_call_off = 0x00029ba3;
my $argument_slot_check_call_va  = 0x00429ba3;
my $argument_slot_check_hook_va  = $cave_va + 0x0f20;

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
    # Preserve the sign bit when the internal FP value is zero.  The original
    # float/double serializers explicitly wrote an all-zero exponent word.
    [0x0008a928,
     'c6470200c64703005f5e83c40cc390909090909090909090',
     'c64702008a460ad0c88847035f5e83c40cc3909090909090'],
    [0x0008aa8d,
     '5fc6460600c64607005e83c40cc39090909090',
     'c64606008a470ad0c88846075f5e83c40cc390'],
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

my $old_argument_slot_call = pack('H*', 'e8d8acfdff');
die "unexpected argument slot call bytes\n"
    unless substr($exe, $argument_slot_call_off,
                  length($old_argument_slot_call)) eq $old_argument_slot_call;
my $argument_slot_rel =
    $argument_slot_hook_va - ($argument_slot_call_va + 5);
substr($exe, $argument_slot_call_off, length($old_argument_slot_call),
       "\xE8" . pack('l<', $argument_slot_rel));

my $old_argument_slot_check_call = pack('H*', 'e8e8a8fdff');
die "unexpected argument slot check call bytes\n"
    unless substr($exe, $argument_slot_check_call_off,
                  length($old_argument_slot_check_call)) eq
           $old_argument_slot_check_call;
my $argument_slot_check_rel =
    $argument_slot_check_hook_va - ($argument_slot_check_call_va + 5);
substr($exe, $argument_slot_check_call_off,
       length($old_argument_slot_check_call),
       "\xE8" . pack('l<', $argument_slot_check_rel));

my $rel = $cave_va - ($stub_va + 5);
my $hook = "\xE9" . pack('l<', $rel) . ("\x90" x (length($old_stub) - 5));
substr($exe, $stub_off, length($hook), $hook);
substr($exe, $cave_off, length($payload), $payload);

open my $out, '>:raw', $output or die "open $output: $!\n";
print {$out} $exe;
close $out;

printf "patched %s: stub=%08X cave=%08X payload=%u bytes\n",
    $output, $stub_off, $cave_off, length($payload);
