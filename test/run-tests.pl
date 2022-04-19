#!/usr/bin/perl -w

# Test for jsw-lib containers
#
#   > Created (Patrick Pelletier): April 17, 2022
#
# This code is in the public domain. Anyone may
# use it or change it in any way that they see
# fit. The author assumes no responsibility for
# damages incurred through use of the original
# code or any variations thereof.
#
# It is requested, but not required, that due
# credit is given to the original author and
# anyone who has modified the code through
# a header comment, such as this one.

use strict;
use FindBin;

chdir ($FindBin::Bin) or die;

my $cc = "gcc";
my $valgrind = "valgrind";

my @libs = qw(atree avltree rbtree slib hlib);

my $red = "\e[31m";
my $off = "\e[0m";
my $bold = "\e[1m";

sub mysystem {
    my @cmd = @_;
    print $bold, join(" ", @cmd), $off, "\n";
    if (system (@cmd) != 0) {
        if ($? == -1) {
            die "$red*** fatal: $!$off\n";
        } elsif ($? & 127) {
            die (sprintf ("$red*** fatal: signal %d$off\n", $? & 127));
        } else {
            die (sprintf ("$red*** fatal: exit code %d$off\n", $? >> 8));
        }
    }
}

foreach my $lib (@libs) {
    my $libdir = "../src/jsw_$lib";
    my $testname = "test-$lib";
    my @cmd = ($cc, "-Wall", "-g", "-o", $testname, "-I$libdir");
    if ($lib eq "slib") {
        push @cmd, "-I../src/jsw_rand";
        push @cmd, "../src/jsw_rand/jsw_rand.c";
    }
    push @cmd, "$libdir/jsw_$lib.c";
    push @cmd, "$testname.c";
    push @cmd, "test-main.c";

    mysystem (@cmd);
}

my $seed = int (rand (4294967296));

foreach my $lib (@libs) {
    my $testname = "test-$lib";
    my @cmd = ($valgrind, "-q", "--error-exitcode=99",
               "--exit-on-first-error=yes", "--leak-check=yes",
               "./$testname", $seed);

    mysystem (@cmd);
}
