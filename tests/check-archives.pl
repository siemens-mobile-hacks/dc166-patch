#!/usr/bin/env perl

use strict;
use warnings;
use File::Spec;

@ARGV == 1 or die "usage: $0 PROJECT_ROOT\n";
my ($root) = @ARGV;

sub manifest_names {
    my ($path) = @_;
    open my $fh, '<', $path or die "$path: $!\n";
    my @names;
    while (my $line = <$fh>) {
        $line =~ s/#.*//;
        $line =~ s/^\s+|\s+$//g;
        push @names, $line if length $line;
    }
    close $fh or die "$path: $!\n";
    return @names;
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
        my $member = substr($archive, pos($archive), $size);
        length($member) == $size or die "$path: truncated $name\n";
        pos($archive) += $size;
        push @members, [$name, $member];
    }
    return @members;
}

my %tags = (
    t => "\x00\x00\x10\x02\x0f",
    s => "\x00\x00\x20\x02\x0f",
    m => "\x00\x00\x30\x03\x0f",
    l => "\x00\x00\x40\x03\x0f",
    h => "\x00\x00\x50\x03\x0f",
);

for my $variant (qw(ext ext2)) {
  for my $model (qw(t m l h)) {
    for my $family (qw(c166 fp166 rt166)) {
        my $library = "$family$model";
        my $manifest = File::Spec->catfile(
            $root, 'runtime', 'manifests', "${family}l.modules");
        my $archive = File::Spec->catfile(
            $root, 'lib', $variant, "$library.lib");
        my @expected = manifest_names($manifest);
        if ($family eq 'c166') {
            push @expected, manifest_names(File::Spec->catfile(
                $root, 'runtime', 'manifests', 'c166l-generated.modules'));
            if ($model eq 't') {
                my %unsupported = map { $_ => 1 } manifest_names(
                    File::Spec->catfile($root, 'runtime', 'manifests',
                                       'c166t-excluded.modules'));
                @expected = grep { !$unsupported{$_} } @expected;
            }
        } elsif ($family eq 'rt166') {
            push @expected, 'cstart.obj';
        }
        my @members = archive_members($archive);
        my @actual = map { $_->[0] } @members;
        my %actual;
        for my $name (@actual) {
            !$actual{$name}++ or die "$archive: duplicate member $name\n";
        }
        for my $name (@expected) {
            $actual{$name} or die "$archive: missing member $name\n";
        }
        if ($model eq 'l' || $model eq 'h') {
            join("\n", @actual) eq join("\n", @expected)
                or die "$archive: unexpected member list\n";
        }

        for my $entry (@members) {
            my ($name, $data) = @$entry;
            for my $other (qw(t s m l h)) {
                next if $other eq $model;
                index($data, $tags{$other}) < 0
                    or die "$archive($name): $other model tag remains\n";
            }
            (index($data, $tags{$model}) >= 0 ||
             index($data, $model eq 't' ? "\x02\x01\x70\x7d" :
                                          "\x02\x01\x70\x7a") >= 0)
                or die "$archive($name): $model model tag missing\n";
        }
    }
  }
}

print "archive audit passed\n";
