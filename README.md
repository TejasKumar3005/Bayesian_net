# Bayesian Network Parameter Learning - IIT Delhi COL333

Learn Conditional Probability Tables (CPTs) for a Bayesian network from **incomplete observational data** (Alarm network / BIF format). Coursework from **COL333 (Artificial Intelligence)** at IIT Delhi.

## Problem

Given:

- a Bayesian network structure in **BIF** (`alarm.bif`)
- a dataset with missing values

estimate each node's CPT so the completed network explains the data well (and passes the course format checker).

## Approach (`medical.cpp`)

- Parse BIF into an in-memory graph (`Graph_Node`: parents, children, values, CPT, Markov blanket)
- Use the Markov blanket + observed evidence for local estimation
- Iterative / EM-style refinement to fill missing entries and smooth sparse counts
- Emit a solved BIF compatible with `Format_Checker.cpp`

Alternate / experimental variants live in `soft.cpp` and `startup_code.cpp`.

## Build & run

```bash
bash compile.sh
bash run.sh
# or manually:
g++ -O2 -std=c++17 medical.cpp -o medical
./medical
```

## Course

**COL333 - Artificial Intelligence**, IIT Delhi  
Assignment 4: Learning Bayesian network parameters with missing data

## License

Coursework / educational use.
