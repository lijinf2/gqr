function [model, V, elapse] = ALSH_learn(dataset, maxbits, w, m, U)
%   This is a function of E2LSH learning.
%
%	Usage:
%	[model, B,elapse] = ALSH_learn(A, maxbits)
%
%	      A: Rows of vectors of data points. Each row is sample point
%   maxbits: Code length
%
%     model: Used for encoding a test sample point.
%	      B: The binary code of the input data A. Each row is sample point
%    elapse: The coding time (training time).
%         m: add m extra term (see detail in ALSH.)
%         U: scale train data's max norm equals U. 
%
%                                             
%
%   Reference:
%       Anshumali Shrivastava, Ping Li
%	Asymmetric LSH (ALSH) for Sublinear Time Maximum Inner Product Search (MIPS)
%
%
%   version 1.0 --Feb/2018 
%
%   Written by xinyan Dai (xinyanshawn@gmail.com)




tmp_T = tic;


[Nitems, Nfeatures] = size(dataset);

norms = sum(dataset.^2,  2).^0.5;
dataset = dataset ./ (max(norms)) .* U;
norms = sum(dataset.^2,  2).^0.5;

normTerm = zeros(Nitems, m);
for k=1:Nitems
   	for i=1:m 
		normTerm(k, i) = norms(k)^(2^i);
    end
end

normalized_data = [dataset normTerm];

first_U = normrnd(0, 1, Nfeatures, maxbits);
lastbit_U = normrnd(0, 1, m, maxbits);
A = [first_U; lastbit_U];

B = rand(1, maxbits) * w; 

model.A = A;
model.B = B;

model.norms = norms;
model.m = m;

expandedB = repmat(model.B, size(normalized_data, 1), 1);
Ym = (normalized_data * model.A + expandedB) / w ;
V = floor(Ym);


elapse = toc(tmp_T);
end
