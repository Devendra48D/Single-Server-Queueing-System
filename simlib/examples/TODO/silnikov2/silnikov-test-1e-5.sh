
step="1e-5"

for i in "abm4" "euler" "rke" "rkf3" "rkf5" "rkf8" "fw" ; do
    echo ./silnikov-m $i $step
    time ./silnikov-m $i $step
done

