#!/bin/bash

for k in 1000 100000; do
for i in $(seq 20); do
if [ -f castle/castle-8-5-$i-cards.pddl ]; then
    echo "instance $i with k=$k"
    PATTERN=$(python3 fast-downward/fast-downward.py castle/castle-8-5-$i-cards.pddl --search "astar(pdb(pattern=greedy($k)), bound=0)" | grep "Greedy generator pattern:")
    PATTERN=${PATTERN##Greedy generator pattern: }
    echo $PATTERN
    echo "Built-in implementation"
    (
        ulimit -t 60
        ulimit -v 2000000
        python3 fast-downward/fast-downward.py castle/castle-8-5-$i-cards.pddl --search "astar(pdb(pattern=greedy($k)))" | grep "Expanded until last jump\|Total time\|Search time"
    )
    echo "PlanOpt implementation"
    (
        ulimit -t 60
        ulimit -v 2000000
        python3 fast-downward/fast-downward.py castle/castle-8-5-$i-cards.pddl --search "astar(planopt_pdb(pattern=$PATTERN))" | grep "Expanded until last jump\|Total time\|Search time"
    )
    echo
fi
done
done
