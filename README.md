GQR
==========
# A General and Efficient Querying Method for Learning to Hash
-----------------------------------------------------------------------------------------------------------------

# Dependences
- CMake
- Matlab

# Run
Use the command "git clone --recursive" to include the submodules.
```
git clone --recursive https://github.com/$user/gqr.git
```
```
cd learn/PCAH
mkdir hashingCodeTXT && matlab < pcah.m
cd ../../
mkdir build && cd ./build && cmake ../ -DCMAKE_BUILD_TYPE=Release
make search
cd ../script && sh search.sh
```
You can refer to script/readme.txt for detailed explanation of configuration in search.sh.
  

# Acknowledgement
GQR project is developed based on LSHBOX (https://github.com/RSIA-LIESMARS-WHU/LSHBOX) and MatlabFunc (https://github.com/dengcai78/MatlabFunc). Great appreciation to the contributors of LSHBOX and MatlabFunc. 
