close all;
clear variables;
clc;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% this is a geometric illustration of KMH (like Fig. 3)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

load data_sift\sift_training.mat;

nSamples = 10000;
training_id = randperm(size(Xtraining,1));
training_id = training_id(1:nSamples);
Xtraining = Xtraining(training_id, :);

%%% decompose into M subspaces
M = 8;
[R, sample_mean] = projection_optimized_product_quantization(Xtraining, M);
X = project(Xtraining, R, sample_mean);
clear Xtraining;

X = X(:, 1:3); % show the first subspace;
%X = X(:, (1:3)+size(X,2)/M); % show the second subspace;

b = 3; %%% 3 bits per subspace
num_iter = 20;
lambda = 10;
affinity_preserving_KM_display(X,b,num_iter,lambda);



