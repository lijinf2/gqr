function [ max_norm ] = precess(train, test)
% max_norm 
% train input data
% test query data

norm_row = sum(train.^2, 2);
max_norm = 31*max(norm_row);

% train(:, end+1) = max_norm - norm_row;
% train(:, end+1) = 0;

% test(:, end+1) = 0;
% test(:, end+1) = max_norm - sum(test.^2, 2);
