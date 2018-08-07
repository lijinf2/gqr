# shell scripts
mkdir ../build
cd ../build 
# cmake ../ -DCMAKE_BUILD_TYPE=Debug
cmake ../ -DCMAKE_BUILD_TYPE=Release
make search 2>&1 | tee ../reproduce/log.txt
cd ../reproduce

# Figure 6
sh runExperiment.sh "ITQH" "GQR" 12 1 "cifar60k" 
sh runExperiment.sh "ITQH" "QR" 12 1 "cifar60k" 

sh runExperiment.sh "ITQH" "GQR" 16 1 "gist" 
sh runExperiment.sh "ITQH" "QR" 16 1 "gist" 

sh runExperiment.sh "ITQH" "GQR" 18 1 "tiny5m" 
sh runExperiment.sh "ITQH" "QR" 18 1 "tiny5m" 

sh runExperiment.sh "ITQH" "GQR" 20 1 "sift10m" 
sh runExperiment.sh "ITQH" "QR" 20 1 "sift10m" 

# Figure 7 & 8 & 9
sh runExperiment.sh "ITQH" "GQR" 12 1 "cifar60k" 
sh runExperiment.sh "ITQH" "GHR" 12 1 "cifar60k" 
sh runExperiment.sh "ITQH" "HR" 12 1 "cifar60k" 

sh runExperiment.sh "ITQH" "GQR" 16 1 "gist" 
sh runExperiment.sh "ITQH" "GHR" 16 1 "gist" 
sh runExperiment.sh "ITQH" "HR" 16 1 "gist" 

sh runExperiment.sh "ITQH" "GQR" 18 1 "tiny5m" 
sh runExperiment.sh "ITQH" "GHR" 18 1 "tiny5m" 
sh runExperiment.sh "ITQH" "HR" 18 1 "tiny5m"

sh runExperiment.sh "ITQH" "GQR" 20 1 "sift10m" 
sh runExperiment.sh "ITQH" "GHR" 20 1 "sift10m" 
sh runExperiment.sh "ITQH" "HR" 20 1 "sift10m"

# Figure 13 & 14
sh runExperiment.sh "PCAH" "GQR" 12 1 "cifar60k" 
sh runExperiment.sh "PCAH" "GHR" 12 1 "cifar60k" 
sh runExperiment.sh "PCAH" "HR" 12 1 "cifar60k" 

sh runExperiment.sh "PCAH" "GQR" 16 1 "gist" 
sh runExperiment.sh "PCAH" "GHR" 16 1 "gist" 
sh runExperiment.sh "PCAH" "HR" 16 1 "gist" 

sh runExperiment.sh "PCAH" "GQR" 18 1 "tiny5m" 
sh runExperiment.sh "PCAH" "GHR" 18 1 "tiny5m" 
sh runExperiment.sh "PCAH" "HR" 18 1 "tiny5m"

sh runExperiment.sh "PCAH" "GQR" 20 1 "sift10m" 
sh runExperiment.sh "PCAH" "GHR" 20 1 "sift10m" 
sh runExperiment.sh "PCAH" "HR" 20 1 "sift10m"

# Figure 15 & 16
sh runExperiment.sh "SH" "GQR" 12 1 "cifar60k" 
sh runExperiment.sh "SH" "GHR" 12 1 "cifar60k" 
sh runExperiment.sh "SH" "HR" 12 1 "cifar60k" 

sh runExperiment.sh "SH" "GQR" 16 1 "gist" 
sh runExperiment.sh "SH" "GHR" 16 1 "gist" 
sh runExperiment.sh "SH" "HR" 16 1 "gist" 

sh runExperiment.sh "SH" "GQR" 18 1 "tiny5m" 
sh runExperiment.sh "SH" "GHR" 18 1 "tiny5m" 
sh runExperiment.sh "SH" "HR" 18 1 "tiny5m"

sh runExperiment.sh "SH" "GQR" 20 1 "sift10m" 
sh runExperiment.sh "SH" "GHR" 20 1 "sift10m" 
sh runExperiment.sh "SH" "HR" 20 1 "sift10m"
