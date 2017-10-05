#!/bin/sh
OF=0-differences
mv -f $OF ${OF}-
echo "Comparing results..."
LC_ALL=C date >$OF
for i in *.out ; do
    echo "===== $i ====="  >>$OF
    if [ -f "../$i" ]; then
        diff -sb "$i" "../$i"  >>$OF
    else
        echo "../$i" does not exist
        echo "***** NOT COMPARED ***** " >>$OF
    fi
done

echo "Done."

