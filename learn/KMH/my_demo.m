function [centers_table, R, sample_mean, codeXtrainingKMH] = demo(Xtraining, num_bits, num_bits_subspace, use_mex)

%num_bits = 64; % num of bits per vector

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% training and encoding
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% KMH
B = num_bits;  
b = num_bits_subspace;   % num of bits per subspace
         % recommended:
         % SIFT-32bits: b=2; SIFT-64,128bits: b=4;
         % GIST: b=4 or 8; (b=8 is slightly better but slower for training)
         
num_iter = 50; % num of iterations, 50 to 200. Usually 50 is enough.
lambda = 10;
nSamples = size(Xtraining,1); % num of samples to train affinity preserving KM. Better to use all samples. 
M = B / b;  % num of subspaces

if nSamples == size(Xtraining,1)
    training_id = 1:nSamples;
else
    training_id = randperm(size(Xtraining,1));
    training_id = training_id(1:nSamples);
end

tic
[centers_table, R, sample_mean] = trainKMH(Xtraining(training_id, :), M, b, num_iter, lambda);
toc

tic
[BincodeXtrainingKMH, codeXtrainingKMH] = encode_KMH(Xtraining, R, sample_mean, M, b, centers_table);
toc

fprintf('KMH done.\n\n');
