#!/usr/bin/env perl

use strict;
use warnings;

@ARGV == 4 or die "usage: $0 SMALL.lib IMPORTED.manifest REPLACED.manifest PENDING.manifest\n";
my ($archive_path, $imported_path, $replaced_path, $pending_path) = @ARGV;

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

open my $archive_fh, '<:raw', $archive_path or die "$archive_path: $!\n";
my $archive;
{
    local $/;
    $archive = <$archive_fh>;
}
close $archive_fh or die "$archive_path: $!\n";
substr($archive, 0, 7) eq "!<ar>!\n"
    or die "$archive_path: invalid ar166 header\n";

my @original;
pos($archive) = 7;
while (pos($archive) < length($archive)) {
    $archive =~ /\G!<ar:(.*?)\s+(\d+)>!\n/gcs
        or die "$archive_path: invalid member header\n";
    my ($name, $size) = ($1, $2);
    $name =~ s/\s+$//;
    push @original, $name;
    pos($archive) += $size;
}

my %category;
for my $entry (
    ['imported', $imported_path],
    ['replaced', $replaced_path],
    ['pending', $pending_path],
) {
    my ($kind, $path) = @$entry;
    for my $name (manifest_names($path)) {
        exists $category{$name}
            and die "$name occurs in both $category{$name} and $kind\n";
        $category{$name} = $kind;
    }
}

my %original = map { $_ => 1 } @original;
for my $name (sort keys %category) {
    exists $original{$name}
        or die "$name is classified but absent from $archive_path\n";
}
my @unclassified = grep { !exists $category{$_} } @original;
@unclassified and die "unclassified modules:\n  " . join("\n  ", @unclassified) . "\n";

my %count;
++ $count{$category{$_}} for @original;
print "libc_modules=" . scalar(@original) . "\n";
print "libc_imported=" . ($count{imported} // 0) . "\n";
print "libc_replaced=" . ($count{replaced} // 0) . "\n";
print "libc_pending=" . ($count{pending} // 0) . "\n";
