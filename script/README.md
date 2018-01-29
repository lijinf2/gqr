***************************************************************************************

## Usages

### Introduction

```
sample_queries.sh
    input: $base_file, $num_queries
    output: $query_file

cal_groundtruth.sh
    input: $base_file, $query_file, $topk, $numThreads
    output: $benchmark_file
```

### Procedure to run an application

- Prepare base_file (fvecs as defined in TEXMEX(http://corpus-texmex.irisa.fr/))
- Run sample_queries.sh on base_file to produce query_file
- Run cal_groundtruth.sh (using base_file & query_file as parameters) to produce benchmark_file
- Train model using Matlab code. Do not forget to "mkdir hashingCodeTXT" first, otherwise error would occur.  Make sure that the parameters in Matlab code is consistent with configuration in search.sh. Otherwise you may get "Invalid file identifier" error in MATLAB
- Run search.sh

***************************************************************************************

## Configurations in search.sh

### hash_method
    - PCAH - PCA hashing
    - PCARR - PCA hashing with random rotation
    - ITQ - Iterative Quantization
    - SH - Spectral Hashing
    - SpH - Spherical Hashing
    - IsoH - Isotropic Hashing
    - KMH - Kmeans Hashing
    - SIM - Random Projecting Hashing.
    - LMIP: length Marked (variant length) Inner Product
        - only work with query method LM
        - based on SIM which generate random projecting bits, and extra bits is generated for representing NORM(Length)

### query_method
    - GQR - Generate-to-probe Quantization Ranking
    - GHR or HL - Generate-to-probe Hamming Ranking, or Hash Lookup
    - QR - Quantization Ranking
    - HR - Hamming Ranking
    - MIH - Multi-index Hashing
    - LM: Length Marked ranking(work the LMIP)
        - use both random projecting bits and extra bis generated in LMIP to rank
    
### codelength
    - Default code length is 12, 16, 18 and 20 for CIFAR60K, GIST1M, TINY5M and SIFT10M, respectively. We experimentally verify that the above settings is almost optimal.
    - For LMIP, a extra parameter normInteval is needed. Default value equals codeLength.

### base_format
    - Fvecs is currently the only format we supported. See TEXMEX(http://corpus-texmex.irisa.fr/) for details.

### model_file & base_bits_file
    - model learned from dataset using hash_method mentioned above.
    

***************************************************************************************

## More scripts

### Data generation or transformation

GQR only takes fvecs as input formats. We can generate random datasets or transform existing datasets under folder `./data_to_fvecs`.

### K-Means hashing

K-Means Hashing requires other scirpts to run, please refer to folder `../learn/KMH` for details.

## Limitations

### Data scale: current implementation supports at most 2^27 (i.e. about 100,000,000) data items, to process larger datasets please refer to distributed computing frameworks LoSHa (https://dl.acm.org/citation.cfm?id=3080800). 
