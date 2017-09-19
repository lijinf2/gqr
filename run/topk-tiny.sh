ls -al >> /dev/null

ls -al bin/ >> /dev/null

# cp bin/laitqlsh_topkquery tmp/step-hr-query

ls -al tmp/ >> /dev/null

# top1
tmp/step-hr-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top1.ben 1 8388608

tmp/step-hl-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top1.ben 1 8388608

tmp/step-tl-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top1.ben 1 8388608

# top10
tmp/step-hr-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top10.ben 1 8388608

tmp/step-hl-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top10.ben 1 8388608

tmp/step-tl-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top10.ben 1 8388608

# top50
tmp/step-hr-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top50.ben 1 8388608

tmp/step-hl-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top50.ben 1 8388608

tmp/step-tl-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top50.ben 1 8388608

# top100
tmp/step-hr-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top50.ben 1 8388608

tmp/step-hl-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top50.ben 1 8388608

tmp/step-tl-query datasets/tiny5m/tinygist5m-centered.bin datasets/tiny5m/1-tiny5m-18bit-50iter.itq datasets/tiny5m/1000q-top50.ben 1 8388608
