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

    cd ../script && bash search.sh

You may refer to folder `./script` for detailed explanations and more instructions.
  
# Support
## Hashing Algorithms
- ITQ: Iterative quantization
- PCAH: PCA Hashing
- PCARR: PCA hashing with random rotation
- SH: Spectral Hashing
- SpH: Spherical Hashing
- KMH: KMeans Hashing
- IsoH: Isotropic Hashing
- SIM: Random Projecting Hashing.
- LMIP: Length Marked (variant length) Inner Product
    - only work with query method LM
    - based on SIM which generate random projecting bits, and extra bits is generated for representing NORM(Length)

## Query Algorithms
- HR: Hamming Ranking
- HL: Hash Lookup
- GQR: Generate-to-probe quantization ranking
- LM: Length Marked ranking(work the LMIP)
    - use both random projecting bits and extra bis generated in LMIP to rank

# Acknowledgement
GQR project is developed based on LSHBOX (https://github.com/RSIA-LIESMARS-WHU/LSHBOX) and MatlabFunc (https://github.com/dengcai78/MatlabFunc). Great appreciation to the contributors of LSHBOX and MatlabFunc. 
