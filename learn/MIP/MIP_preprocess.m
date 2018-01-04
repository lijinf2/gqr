function [ max_norm ] = process(train, test)
% max_norm
% train input data
% test query data

norm_row = sum(train.^2, 2);
max_norm = max(norm_row);

train(:, end+1) = sqrt(max_norm - norm_row);
test(:, end+1) = 0;