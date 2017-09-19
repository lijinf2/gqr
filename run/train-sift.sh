ls -al >> /dev/null
ls -al bin/ >> /dev/null
# # train multiple tables

# # one table
tmp/train-sift datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/1000q-top1000.ben 0 1000000000 1
