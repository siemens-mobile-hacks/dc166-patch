#!/usr/bin/env perl

use strict;
use warnings;

(@ARGV == 2 || @ARGV == 3)
    or die "usage: $0 REFERENCE.lib TARGET.lib [ALLOWED_MISSING.txt]\n";
my ($reference_path, $target_path, $allowed_missing_path) = @ARGV;

sub u16 {
    return unpack 'v', substr($_[0], $_[1], 2);
}

sub u32 {
    return unpack 'V', substr($_[0], $_[1], 4);
}

sub archive_members {
    my ($path) = @_;
    open my $fh, '<:raw', $path or die "$path: $!\n";
    local $/;
    my $archive = <$fh>;
    close $fh or die "$path: $!\n";
    substr($archive, 0, 7) eq "!<ar>!\n"
        or die "$path: invalid ar166 header\n";

    my @members;
    pos($archive) = 7;
    while (pos($archive) < length($archive)) {
        $archive =~ /\G!<ar:(.*?)\s+(\d+)>!\n/gcs
            or die "$path: invalid member header\n";
        my ($name, $size) = ($1, $2);
        $name =~ s/\s+$//;
        my $object = substr($archive, pos($archive), $size);
        length($object) == $size or die "$path: truncated $name\n";
        pos($archive) += $size;
        push @members, [$name, $object];
    }
    return @members;
}

sub public_symbols {
    my ($path) = @_;
    my %public;
    for my $member (archive_members($path)) {
        my ($member_name, $object) = @$member;
        length($object) >= 22 or die "$path($member_name): short object\n";
        my $magic = u16($object, 0);
        ($magic == 0x0201 || $magic == 0x0202)
            or die sprintf "%s(%s): invalid object magic 0x%04x\n",
                $path, $member_name, $magic;
        my $section_count = u16($object, 6);
        my $relocation_count = u16($object, 8);
        my $name_count = u16($object, 10);
        my $emitted_size = u32($object, 12);
        my $extended_counts = u16($object, 20);
        $relocation_count |= ($extended_counts & 0xff00) << 8;
        $name_count |= ($extended_counts & 0x00ff) << 16;

        my $name_offset = 22 + $section_count * 20 + $emitted_size
                        + $relocation_count * 10;
        my $string_offset = $name_offset + $name_count * 14;
        $string_offset <= length($object)
            or die "$path($member_name): invalid symbol table bounds\n";

        for (my $index = 0; $index < $name_count; ++$index) {
            my $record = $name_offset + $index * 14;
            my $symbol_offset = u32($object, $record);
            my $type = u16($object, $record + 4);
            next unless ($type & 0x0040) && !($type & 0x0080);
            ($symbol_offset >= $string_offset &&
             $symbol_offset < length($object))
                or die "$path($member_name): invalid public name offset\n";
            my $end = index($object, "\0", $symbol_offset);
            $end >= 0 or die "$path($member_name): unterminated public name\n";
            my $name = substr($object, $symbol_offset,
                              $end - $symbol_offset);
            push @{$public{$name}}, $member_name;
        }
    }
    return %public;
}

my %reference = public_symbols($reference_path);
my %target = public_symbols($target_path);
my %allowed_missing;
if (defined $allowed_missing_path) {
    open my $allowed_fh, '<', $allowed_missing_path
        or die "$allowed_missing_path: $!\n";
    while (my $line = <$allowed_fh>) {
        $line =~ s/#.*//;
        $line =~ s/^\s+|\s+$//g;
        $allowed_missing{$line} = 1 if length $line;
    }
    close $allowed_fh or die "$allowed_missing_path: $!\n";
}

my @missing = grep { !exists $target{$_} } sort keys %reference;
my @unexpected_missing = grep { !$allowed_missing{$_} } @missing;
@unexpected_missing and die "Target archive is missing reference public symbols:\n  "
    . join("\n  ", map {
        "$_: " . join(', ', @{$reference{$_}})
    } @unexpected_missing) . "\n";

my @stale_allowance = grep { exists $target{$_} || !exists $reference{$_} }
    sort keys %allowed_missing;
@stale_allowance and die "Allowed-missing list contains stale symbols:\n  "
    . join("\n  ", @stale_allowance) . "\n";

my @duplicate = grep { @{$target{$_}} > 1 } sort keys %target;
@duplicate and die "Target archive has duplicate public symbols:\n  "
    . join("\n  ", map {
        "$_: " . join(', ', @{$target{$_}})
    } @duplicate) . "\n";

print "reference_public_symbols=" . scalar(keys %reference) . "\n";
print "target_public_symbols=" . scalar(keys %target) . "\n";
print "allowed_missing_public_symbols=" . scalar(@missing) . "\n";
print "unexpected_missing_public_symbols=0\n";
print "duplicate_target_public_symbols=0\n";
