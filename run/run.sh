ls -al >> /dev/null
ls -al bin/ >> /dev/null
# train
# bin/laitqlsh_topkquery datasets/cifar-10/cifar-10-LSHBOX-60k-centered.bin datasets/cifar-10/1-cifar10-10bit-50iter.itq datasets/cifar-10/1000q-top20.ben 0 65536
# bin/laitqlsh_topkquery datasets/gist/gist1m-centered.bin datasets/gist/1-gist-8bit-50iter.itq datasets/gist/gist-1000q-top1000.ben 0 1048576 1
# bin/laitqlsh_topkquery datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-28bit-50iter-2.itq datasets/sift10m/1000q-top1000.ben 0 1000000000
# bin/laitqlsh_topkquery datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter-tmp.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 1
# ./laitqlsh_topkquery datasets/cifar-10/cifar-10-LSHBOX-60k-centered.bin datasets/cifar-10/tmp-19tables.itq datasets/cifar-10/1000q-top100.ben 0 65536

# 1 cifar
# bin/laitqlsh_topkquery datasets/cifar-10/cifar-10-LSHBOX-60k-centered.bin datasets/cifar-10/1-cifar10-12bit-50iter.itq datasets/cifar-10/1000q-top20.ben 1 65536
# bin/laitqlsh_topkquery datasets/cifar-10/cifar-10-LSHBOX-60k-centered.bin datasets/cifar-10/1-cifar10-12bit-50iter.itq datasets/cifar-10/1000q-top100.ben 1 32768

# 1 gist
# bin/laitqlsh_topkquery datasets/gist/gist1m-centered.bin datasets/gist/1-gist-16bit-50iter-4tables.itq datasets/gist/gist-1000q-top20.ben 1 1048576 

# tiny5m 
# bin/laitqlsh_topkquery datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter-8tables.itq datasets/tiny5m/1000q-top100.ben 1 8388608


# sift10m
bin/laitqlsh_topkquery datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-28bit-50iter.itq datasets/sift10m/sift10m-1000q-top20.ben 1 16777216
# bench
# ./create_benchmark datasets/cifar-10/cifar-10-LSHBOX-60k-centered.bin cifar-10.bench 1000 100


