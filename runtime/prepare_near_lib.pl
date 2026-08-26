#!/usr/bin/env perl

use strict;
use warnings;
use File::Path qw(make_path);
use File::Spec;

@ARGV == 4 or die "usage: $0 INPUT.lib MANIFEST MODEL WORK_DIR\n";
my ($input, $manifest, $model, $work_dir) = @ARGV;
$model eq 't' || $model eq 'm'
    or die "$0: Near adapters are only valid for t and m\n";

sub u16 { return unpack 'v', substr($_[0], $_[1], 2); }
sub u32 { return unpack 'V', substr($_[0], $_[1], 4); }

sub wanted_members {
    my ($path) = @_;
    open my $fh, '<', $path or die "$path: $!\n";
    my @members;
    while (my $line = <$fh>) {
        $line =~ s/#.*//;
        $line =~ s/^\s+|\s+$//g;
        push @members, $line if length $line;
    }
    close $fh or die "$path: $!\n";
    return @members;
}

sub archive_members {
    my ($path) = @_;
    open my $fh, '<:raw', $path or die "$path: $!\n";
    local $/;
    my $archive = <$fh>;
    close $fh or die "$path: $!\n";
    substr($archive, 0, 7) eq "!<ar>!\n"
        or die "$path: invalid ar166 header\n";
    my %members;
    pos($archive) = 7;
    while (pos($archive) < length($archive)) {
        $archive =~ /\G!<ar:(.*?)\s+(\d+)>!\n/gcs
            or die "$path: invalid member header\n";
        my ($name, $size) = ($1, $2);
        $name =~ s/\s+$//;
        $members{$name} = substr($archive, pos($archive), $size);
        length($members{$name}) == $size or die "$path: truncated $name\n";
        pos($archive) += $size;
    }
    return %members;
}

sub symbol_records {
    my ($object) = @_;
    my $section_count = u16($object, 6);
    my $relocation_count = u16($object, 8);
    my $name_count = u16($object, 10);
    my $emitted_size = u32($object, 12);
    my $extended = u16($object, 20);
    $relocation_count |= ($extended & 0xff00) << 8;
    $name_count |= ($extended & 0x00ff) << 16;
    my $name_table = 22 + $section_count * 20 + $emitted_size
                   + $relocation_count * 10;
    my $string_table = $name_table + $name_count * 14;
    my @records;
    for my $index (0 .. $name_count - 1) {
        my $record = $name_table + $index * 14;
        my $type = u16($object, $record + 4);
        next unless $type == 0x005e || $type == 0x009c;
        my $offset = u32($object, $record);
        $offset >= $string_table && $offset < length($object)
            or die "invalid symbol name offset\n";
        my $end = index($object, "\0", $offset);
        $end >= 0 or die "unterminated symbol name\n";
        push @records, {
            type => $type,
            offset => $offset,
            name => substr($object, $offset, $end - $offset),
        };
    }
    return @records;
}

sub alternate_name {
    my ($name) = @_;
    $name =~ s/([a-z])/uc($1)/e
        or die "cannot create alternate name for $name\n";
    return $name;
}

sub write_raw {
    my ($path, $data) = @_;
    open my $fh, '>:raw', $path or die "$path: $!\n";
    print {$fh} $data or die "$path: $!\n";
    close $fh or die "$path: $!\n";
}

my @wanted = wanted_members($manifest);
my %archive = archive_members($input);
my %objects;
my %public_functions;
my %member_public;
for my $member (@wanted) {
    exists $archive{$member} or die "$input: missing $member\n";
    my $object = $archive{$member};
    my @public = map { $_->{name} }
                 grep { $_->{type} == 0x005e } symbol_records($object);
    $objects{$member} = $object;
    $member_public{$member} = \@public;
    for my $name (@public) {
        exists $public_functions{$name}
            and die "$input: duplicate public function $name\n";
        $public_functions{$name} = alternate_name($name);
    }
}
for my $name (keys %public_functions) {
    exists $public_functions{$public_functions{$name}}
        and die "$input: alternate symbol collision for $name\n";
}

my %model_tag = (
    t => "\x00\x00\x10\x02\x0f",
    m => "\x00\x00\x30\x03\x0f",
);
my $small_tag = "\x00\x00\x20\x02\x0f";
my $small_stack = "\x02\x01\x70\x7d";
my $paged_stack = "\x02\x01\x70\x7a";
my $segmented = $model eq 'm' ? '$SEGMENTED' : '$NONSEGMENTED';
my $model_name = $model eq 'm' ? 'MEDIUM' : 'TINY';

make_path($work_dir);
my $order_path = File::Spec->catfile($work_dir, 'order.txt');
open my $order, '>', $order_path or die "$order_path: $!\n";

for my $member (@wanted) {
    my $object = $objects{$member};
    my %patched_offset;
    for my $record (symbol_records($object)) {
        next unless exists $public_functions{$record->{name}};
        next if $patched_offset{$record->{offset}}++;
        my $replacement = $public_functions{$record->{name}};
        length($replacement) == length($record->{name})
            or die "symbol length changed for $record->{name}\n";
        substr($object, $record->{offset}, length($replacement), $replacement);
    }
    my $tag_count = ($object =~ s/\Q$small_tag\E/$model_tag{$model}/g);
    $tag_count == 1 or die "$input($member): expected one Small model tag\n";
    if ($model eq 'm') {
        my $stack_count = ($object =~ s/\Q$small_stack\E/$paged_stack/g);
        $stack_count <= 1 or die "$input($member): multiple stack sections\n";
    }

    (my $stem = $member) =~ s/\.obj$//;
    my $far_object = "$stem-far.obj";
    write_raw(File::Spec->catfile($work_dir, $far_object), $object);
    my @public = @{$member_public{$member}};
    if (!@public) {
        print {$order} "$member\t$far_object\n";
        next;
    }

    my $module = uc("${stem}_NEAR_ADAPTER");
    $module =~ s/[^A-Z0-9_]/_/g;
    my $section = "${module}_PR";
    my $assembly = join("\n",
        '$EXTEND',
        '$NOMOD166',
        '$STDNAMES(reg.def)',
        $segmented,
        '$CASE',
        '$NOEXPANDREGBANK',
        "\$MODEL($model_name)",
        '',
        "        NAME $module",
        "$section SECTION CODE WORD PUBLIC 'SHAREDRTLIB'",
        (map { "        PUBLIC $_" } @public),
        (map { "        EXTERN $public_functions{$_}:FAR" } @public),
        '',
        (map {
            my $near = $_;
            my $far = $public_functions{$_};
            ("$near PROC NEAR",
             "        CALLS SEG $far,$far",
             '        RET',
             "$near ENDP",
             '')
        } @public),
        "$section ENDS",
        '        REGDEF R0-R15',
        '        END',
        '');
    my $adapter_asm = "$stem-adapter.asm";
    my $adapter_obj = "$stem-adapter.obj";
    write_raw(File::Spec->catfile($work_dir, $adapter_asm), $assembly);
    print {$order} "$member\t$adapter_obj\n";
    my $far_member = "${stem}f.obj";
    length($far_member) <= 20 or die "$far_member: archive name too long\n";
    print {$order} "$far_member\t$far_object\n";
}
close $order or die "$order_path: $!\n";
