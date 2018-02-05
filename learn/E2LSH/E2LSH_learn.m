function [model, V, elapse] = LSH_learn(dataset, maxbits, w)
%   This is a function of E2LSH learning.
%
%	Usage:
%	[model, B,elapse] = LSH_learn(A, maxbits)
%
%	      A: Rows of vectors of data points. Each row is sample point
%   maxbits: Code length
%
%     model: Used for encoding a test sample point.
%	      B: The binary code of the input data A. Each row is sample point
%    elapse: The coding time (training time).
%
%
%   version 2.0 --Nov/2016 
%   version 1.0 --Jan/2010 
%
%   Written by  Yue Lin (linyue29@gmail.com)
%               Deng Cai (dengcai AT gmail DOT com) 
%   version 3.0 -- Jan/2018
%   Modified by LI Jinfeng (lijinf8@gmail.com)
%                                             

tmp_T = tic;

[~,Nfeatures] = size(dataset);
k = maxbits;

A = normrnd(0, 1, Nfeatures, k);
B = rand(1, maxbits) * w; 

model.A = A;
model.B = B;
V = E2LSH_compress(dataset, model, w);

elapse = toc(tmp_T);
end
