
step="0.001"

for i in "abm4" "euler" "fw" "rke" "rkf3" "rkf5" "rkf8" ; do
    echo ./silnikov-m $i $step
    time ./silnikov-m $i $step
done

