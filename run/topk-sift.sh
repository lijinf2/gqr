ls -al >> /dev/null

ls -al bin/ >> /dev/null

# cp bin/laitqlsh_topkquery tmp/step-hr-query

ls -al tmp/ >> /dev/null

# top1
tmp/step-hr-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top1.ben 1 16777216

tmp/step-hl-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top1.ben 1 16777216

tmp/step-tl-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top1.ben 1 16777216


# top10
tmp/step-hr-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top10.ben 1 16777216

tmp/step-hl-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top10.ben 1 16777216

tmp/step-tl-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top10.ben 1 16777216


# top50
tmp/step-hr-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top50.ben 1 16777216

tmp/step-hl-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top50.ben 1 16777216

tmp/step-tl-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top50.ben 1 16777216

# top100
tmp/step-hr-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top100.ben 1 16777216

tmp/step-hl-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top100.ben 1 16777216

tmp/step-tl-query datasets/sift10m/sift10m-centered.bin datasets/sift10m/1-sift10m-20bit-50iter.itq datasets/sift10m/sift10m-1000q-top100.ben 1 16777216
