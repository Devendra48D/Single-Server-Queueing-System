#!/bin/sh
OF=0-differences
mv -f $OF.log ${OF}-.log
OF=$OF.log
echo "Comparing results..."
date >$OF
for i in *.out *.dat ; do
    echo "===== $i ====="  >>$OF
    if [ -f "../$i" ]; then
        diff -sb "$i" "../$i"  >>$OF
    else
        echo "../$i" does not exist
        echo "***** NOT COMPARED ***** " >>$OF
    fi
done
echo "Results stored in: $OF"
echo "Done."

