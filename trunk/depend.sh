#!/bin/sh
# $Id$

set -e

TMPFILE=/tmp/$0.$$.out

function die {
	rm -f $TMPFILE
	echo "** $0 exiting abnormally"
	exit 1
}

CC=$1
shift

$CC -MM $* > $TMPFILE || die

#sed -e 's@^\(.*\)\.o:@deps/\1.d objs/\1.o:@' $TMPFILE || die
sed -e 's@^\(.*\)\.o:@objs/\1.o:@' $TMPFILE || die

rm -f $TMPFILE
