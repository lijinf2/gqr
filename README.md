GQR
==========
# A General and Efficient Querying Method for Learning to Hash
-----------------------------------------------------------------------------------------------------------------

# Dependences
- CMake
- Matlab

# Run
-Step 1: clone the repository

    git clone --recursive https://github.com/lijinf2/gqr.git

    cd gqr

-Step 2: train models, which are stored under folder hashingCodeTXT

    cd learn/PCAH

    mkdir hashingCodeTXT && matlab < pcah.m

    cd ../../

-Step 3: make app and run 

    mkdir build && cd ./build && cmake ../ -DCMAKE_BUILD_TYPE=Release

    make search

    cd ../script && sh search.sh

You may refer to script/readme.txt for detailed explanations and more instructions.
  
# Support
## Hashing Algorithms
- ITQ: Iterative quantization
- PCAH: PCA Hashing
- PCARR: PCA hashing with random rotation
- SH: Spectral Hashing
- SpH: Spherical Hashing
- KMH: Kmeans Hashing
- IsoH: isotropic Hashing

## Query Algorithms
- HR: Hamming Ranking
- HL: Hash Lookup
- GQR: Generate-to-probe quantization ranking

# Acknowledgement
GQR project is developed based on LSHBOX (https://github.com/RSIA-LIESMARS-WHU/LSHBOX) and MatlabFunc (https://github.com/dengcai78/MatlabFunc). Great appreciation to the contributors of LSHBOX and MatlabFunc. 
