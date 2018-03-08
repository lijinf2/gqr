usage of search_mip
==================

# Run
-Step 1: clone the repository

    git clone --recursive https://github.com/lijinf2/gqr.git
    cd gqr

-Step 2: train models, which are stored under folder hashingCodeTXT

    cd learn/NLMIP
    mkdir hashingCodeTXT
    matlab < nlmip.m

    cd ../../

-Steo 3: calculate ground-truth

    cd ./script
    vim cal_groundtruth.sh
            # modify cal_groundtruth.sh
            # 1. metric=product
            # 2. for dataset in "movielens" (if we use movielens as default)

    bash cal_groundtruth.sh
    cd ../  # return to gqr/

-Step 4: make app and run

    cd ./script/mips/ && bash search_mip.sh

# MIPS Hashing method

## LMIP
- supported Query method
    * LM

## NLMIP
- our "Norm-Ranging LSH for Maximum Inner Product Search"
- supported Query method
    * NR: rank by norm and number of same bit between query's hash code and bucket's hash code
    * IMIP: generate to probe without full sort with heap based probe method in [The Inverted Multi-Index](https://www.google.com.hk/search?q=The+Inverted+Multi-Index&oq=The+Inverted+Multi-Index&aqs=chrome..69i57j69i61l2j69i60&sourceid=chrome&ie=UTF-8)
    * PRE_SORT: full sort in limited time, see details in our "Norm-Ranging LSH for Maximum Inner Product Search""

## ALSH
- see details in [Asymmetric LSH (ALSH) for Sublinear Time Maximum Inner Product Search](https://arxiv.org/pdf/1405.5869.pdf)
- supported Query method
    * no option for query method (default as IntRankALSH)

## SimpleLSH
- see details in [On Symmetric and Asymmetric LSHs for Inner Product Search](https://arxiv.org/pdf/1410.5518.pdf)
- supported Query method
    - HR: Hamming Ranking
    - HL: Hash Lookup
    - GQR: Generate-to-probe quantization ranking
