echo "There can be some minor differences due to rounding errors"
echo "(for example: x87 vs. sse2)"

mv 0-cmp.log 0-cmp-.log 2>/dev/null
diff -b . .. | grep -v ^Only | tee 0-cmp.log

