ls -al >> /dev/null
ls -al bin/ >> /dev/null
# train
# one table for tiny
tmp/train-pca-tiny datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/pca/1-pca-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 1

# one table for sift
tmp/train-pca-sift datasets/sift10m/sift10m-centered.bin datasets/sift10m/pca/1-pca-sift10m-20bit-50iter.itq datasets/sift10m/1000q-top1000.ben 0 1000000000 1
