ls -al >> /dev/null

ls -al bin/ >> /dev/null

ls -al tmp/ >> /dev/null

# tiny5m hr
tmp/hr-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top20.ben 1 8388608

# tiny5m tl
tmp/tl-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top20.ben 1 8388608

# sift10m hr
tmp/hr-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top20.ben 1 16777216

# sift10m tl
tmp/tl-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top20.ben 1 16777216

