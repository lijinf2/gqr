function [model, B, elapse] = LMIP_learn(A, maxbits, normInteval, norms, prct, lengthBits)
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
%   version 1.0 --Jan/2018 
%
%   Written by  Xinyan Dai (xinyan.dai@outlook.com)
%                                             


tmp_T = tic;

[Nitems, Nfeatures] = size(A);


U = normrnd(0, 1, Nfeatures, maxbits);
Z = A * U;

B = (Z > 0);

lens = zeros(Nitems, lengthBits);

% put vector norm(length) in lens matrix in binary form
for k=1:Nitems
	% vector whose norms is not greater than prct[2] belongs to the 0 group
	currentLength = cal_weight(prct, norms, k, normInteval,  maxbits);

	mask = 1;
	for bitIndex=1:lengthBits
		lens(k, lengthBits+1-bitIndex) = bitand(currentLength, mask)>0;
		mask = 2 * mask;
    	end

end
% lens = lens - lens;
B = [B lens];

model.U = U;
model.prct = prct;
model.lengthBits = lengthBits;
model.maxbits = maxbits
model.normInteval = normInteval

elapse = toc(tmp_T);
end
