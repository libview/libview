#!/usr/bin/perl

$f = shift;

$past_includes = 0;

$in_comment = 0;
$found_docblock = 0;
$in_docblock_summary = 0;
$in_docblock_results = 0;
$in_docblock_sideeffects = 0;

$docblock_summary = "";
$docblock_results = "";
$docblock_sideeffects = "";

$comment_block = "";

open (IN, $f);
for (<IN>) {
    s#// (IN|OUT|IN/OUT):\s+(.+)$#//!< \2#;

    if (/^#include/) {
        $past_includes = 1;
    }

    if (/^\/\*$/) {
        $in_comment = 1;
    }
    elsif (/^ \*\/$/) {
        $comment_block .= $_;
        $in_comment = 0;

        &process_comment($comment_block);

        $comment_block = "";
        next;
    }

    if ($in_comment == 1) {
        $comment_block .= $_;
    }
    else {
        print $_;
    }
}
close IN;


sub process_comment($) {
    my $block = shift;

    if ($block =~ /^\/\*\n \*-----+\n \*\n \* ([A-Za-z0-9:~_]+) --\s*\n \*\n \*\s+(.*?) \*\n \* Results:\n \*      \s*(.+?)\n \* Side effects:\n \*      \s*(.+?) \*\n \*------+\n \*\//s) {

        $name = $1;
        $summary = $2;
        $returns = $3;
        $sideeffects = $4;

        $summary =~ s/[\t ]+/ /gs;
        $summary =~ s/\s\*$//g;
        $returns =~ s/[\t ]+/ /gs;
        $returns =~ s/\s\*$//g;
        $sideeffects =~ s/[\t ]+/ /gs;
        $sideeffects =~ s/\s\*$//g;

        chomp $summary;
        chomp $returns;
        chomp $sideeffects;

        print "/**\n";
        print " * $summary\n";

        if ($returns !~ /^[Nn]one\.?/) {
            print " *\n";
            print " * \@return $returns\n";
        }

        if ($sideeffects !~ /^[Nn]one\.?/) {
            print " *\n";
            print " * \@sideeffects $sideeffects\n";
        }

        print " */\n";
    }
    else {
        print $block;
    }
}
