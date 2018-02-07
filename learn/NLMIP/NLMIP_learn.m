function [model, B, elapse] = NLMIP_learn(A, maxbits, normInteval, norms, prct, lengthBits)
%   This is a function of LMIP (Locality Sensitive Hashing) learning.
%
%	Usage:
%	[model, B,elapse] = LMIP_learn(A, maxbits)
%
%	      A: Rows of vectors of data points. Each row is sample point
%   maxbits: Code length
%
%     model: Used for encoding a test sample point.
%	      B: The binary code of the input data A. Each row is sample point
%    elapse: The coding time (training time).
%
%   version 1.0 --Feb/2018 
%
%   Written by  Xinyan Dai (xinyan.dai@outlook.com)
%                                             


tmp_T = tic;

[Nitems, Nfeatures] = size(A);

lens = zeros(Nitems, lengthBits);
max_norm = max(norms);
scale = 2^lengthBits / max_norm;

% put vector norm(length) in lens matrix in binary form
for k=1:Nitems
	% vector whose norms is not greater than prct[2] belongs to the 0 group
	currentLength = find(prct>=norms(k), 1) - 2; 
  	if isempty(currentLength)
       		currentLength = size(prct, 2) - 2;
    	end
	if(currentLength<0)
		currentLength = 0;
    	end
%	currentLength = currentLength + maxbits - normInteval;
	
	% currentLength = scale * norms(k);
	% currentLength = floor(currentLength);
	
	mask = 1;
    
	for bitIndex=1:lengthBits
        lens(k, lengthBits+1-bitIndex) = bitand(uint8(currentLength), uint8(mask))>0;
		mask = 2 * mask;
    	end
    
end

normTerm = zeros(Nitems, 1);
for k=1:Nitems
	normIntervalIndex = find(prct>norms(k), 1);
    	if isempty(normIntervalIndex)
        	normIntervalIndex = size(prct, 2);
    	end
    
	normIntervalItem = prct(normIntervalIndex);
	normTerm(k, 1) = sqrt(normIntervalItem - norms(k));
end

Normalized_A = [A normTerm];
first_U = normrnd(0, 1, Nfeatures, maxbits);
lastbit_U = normrnd(0, 1, 1, maxbits);
U = [first_U; lastbit_U];
Z = Normalized_A * U;

B = (Z > 0);
% lens = lens - lens;
B = [B lens];

model.U = U;
model.prct = prct;
model.lengthBits = lengthBits;
model.maxbits = maxbits
model.normInteval = normInteval

elapse = toc(tmp_T);
end
