GQR
==========
# A General and Efficient Querying Method for Learning to Hash
-----------------------------------------------------------------------------------------------------------------

# Dependences
- CMake
- Matlab

# Run
Download the latest source code of GQR, and use the command "git clone --recursive" to include the submodules.
```
git clone --recursive https://github.com/lijinf2/gqr.git
```
Create a build directory, and do a out-of-source build using CMake:
```
cd gqr
mkdir build && cd ./build && cmake ../ -DCMAKE_BUILD_TYPE=Release
make search   # we will use it later
```
Create model file and base-bits file using Matlab:
```
cd ../learn/PCAH 
mkdir hashingCodeTXT && matlab < pcah.m
```
Run the search program (search.sh contains all the configurations)
```
cd ../script && sh search.sh
```

# Configuration

Configurations are integrated into **search.sh** in the **script** folder to facilitate the modification of parameters.    


* hash_method  
  - PCAH   - PCA hashing
  - ITQ    - Iterative Quantization
  
* query_method   
  - GQR    - Generate-to-probe Quantization Ranking   
  - GHR/HL - Generate-to-probe Hamming Ranking    
  - QR     - Quantization Ranking  
  - HR     - Hamming Ranking
  
* codelength    
  - Default code length is 12, 16, 18 and 20 for CIFAR60K, GIST1M, TINY5M and SIFT10M, respectively. We experimentally verify that the above settings is almost optimal.
  
* base_format
  - For now, fvecs is the only format we supported.
  
* num_queries
  - For each dataset and querying method, we usually randomly sample 1000 items as queries (query items are not used in hash function learning).
 
* topk
  - By default, we do 20-nearest neighbors search.
  
* model_file & base_bits_file
  - hash functions learned from dataset using hash_method mentioned above.
  
# Experiments

  
