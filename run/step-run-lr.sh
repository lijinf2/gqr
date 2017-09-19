ls -al >> /dev/null
ls -al bin/ >> /dev/null
# cp bin/laitqlsh_topkquery tmp/step-lr-query
ls tmp/ >> /dev/null

# cifar
tmp/step-lr-query datasets/cifar-10/cifar-10-LSHBOX-60k-centered.bin datasets/cifar-10/1-cifar10-12bit-50iter.itq datasets/cifar-10/1000q-top10.ben 1 65536

# tmp/step-lr-query datasets/cifar-10/cifar-10-LSHBOX-60k-centered.bin datasets/cifar-10/1-cifar10-12bit-50iter-2tables.itq datasets/cifar-10/1000q-top10.ben 1 65536
#
# tmp/step-lr-query datasets/cifar-10/cifar-10-LSHBOX-60k-centered.bin datasets/cifar-10/1-cifar10-12bit-50iter-4tables.itq datasets/cifar-10/1000q-top10.ben 1 65536

# 1 gist
tmp/step-lr-query datasets/gist/gist1m-centered.bin datasets/gist/1-gist-16bit-50iter.itq datasets/gist/gist-1000q-top10.ben 1 1048576 

# tmp/step-lr-query datasets/gist/gist1m-centered.bin datasets/gist/1-gist-16bit-50iter-4tables.itq datasets/gist/gist-1000q-top10.ben 1 1048576 
#
# tmp/step-lr-query datasets/gist/gist1m-centered.bin datasets/gist/1-gist-16bit-50iter-8tables.itq datasets/gist/gist-1000q-top10.ben 1 1048576 

# tiny5m 
tmp/step-lr-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top10.ben 1 8388608

# tmp/step-lr-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter-4tables.itq datasets/tiny5m/1000q-top10.ben 1 8388608
#
# tmp/step-lr-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter-8tables.itq datasets/tiny5m/1000q-top10.ben 1 8388608

# sift10m
tmp/step-lr-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top10.ben 1 16777216

# tmp/step-lr-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter-8tables.itq datasets/sift10m/sift10m-1000q-top10.ben 1 16777216
#
# tmp/step-lr-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter-16tables.itq datasets/sift10m/sift10m-1000q-top10.ben 1 16777216

