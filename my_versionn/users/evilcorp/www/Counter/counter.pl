#!/usr/bin/perl

use strict;
use warnings;
use FindBin;

my $counter_file = "$FindBin::Bin/counter.txt";
my $fh;

# Probeer het bestand te openen voor lezen en schrijven
if (!open($fh, "+<", $counter_file)) {
    # Als het nog niet bestaat, maak het aan
    open($fh, "+>", $counter_file) or do {
        print "Content-type: text/plain\n\n";
        print "[fout bij openen tellerbestand]\n";
        exit;
    };
}

# Verhoog de teller
flock($fh, 2);
my $count = <$fh> || 0;
$count++;
seek($fh, 0, 0);
print $fh $count;
truncate($fh, tell($fh));
close($fh);

# Output naar de browser
print "Content-type: text/plain\n\n";
print "$count\n";
