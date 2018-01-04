function [ max_norm, trainset, testset ] = preprocess(train, test)
% max_norm 
% train input data
% test query data

train_norm = sum(train.^2, 2);
test_norm = sum(test.^2, 2);
max_norm = max(train_norm);

train(:, end+1) = sqrt(max_norm - train_norm);
train(:, end+1) = 0;

test(:, end+1) = 0;
test(:, end+1) = sqrt(max_norm - test_norm);

trainset = train;
testset = test;