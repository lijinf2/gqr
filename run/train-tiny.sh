ls -al >> /dev/null
ls -al bin/ >> /dev/null
# train

# different hash tables
tmp/train-tiny datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter-10tables.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 10 18

tmp/train-tiny datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter-20tables.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 20 18



# different bits
# 8 bits
# tmp/train-tiny datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-12bit-50iter.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 1 8

# 12 bits
# tmp/train-tiny datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-12bit-50iter.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 1 12
#
# # 16 bits
# tmp/train-tiny datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-16bit-50iter.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 1 16
#
# # 20 bits
# tmp/train-tiny datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-20bit-50iter.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 1 20
#
# # 24 bits
# tmp/train-tiny datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-24bit-50iter.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 1 24
#
# # 28 bits
# tmp/train-tiny datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-28bit-50iter.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 1 28
#
# # 32 bits
# tmp/train-tiny datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-32bit-50iter.itq datasets/tiny5m/1000q-top1000.ben 0 1000000000 1 32
