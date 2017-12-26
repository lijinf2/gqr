function[max_norm] = precess(train, test)

norm_row = sqrt(sum(A.^2, 1))
max_norm = 31*max(norm_row)

train(:, end+1) = max_norm
train(:, end+1) = 0

test(:, end+1) = 0
test(:, end+1) = max_norm