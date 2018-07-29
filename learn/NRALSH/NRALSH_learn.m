function [model, V, elapse] = NRALSH_learn(dataset, maxbits, w, m, U, normInteval, norms, prct, lengthBits)

tmp_T = tic;
[NormIndex] = NRALSH_interval(norms, prct);
[Nitems, Nfeatures] = size(dataset);

% normalize by scaling factor
for k = 1:Nitems
    % get scaling factor
    norm = norms(k);
    index = find(prct >= norm, 1);
    scaler = prct(index);
    % scaling
    dataset(k, :) = dataset(k, :) / scaler * U;
end

% add normTerm in the end
normTerm = zeros(Nitems, m);
for k=1:Nitems
    newNorm = sum(dataset(k, :).^2).^0.5;
   	for i=1:m 
		normTerm(k, i) = newNorm^(2^i);
    end
end

normalized_data = [dataset normTerm];

A = normrnd(0, 1, Nfeatures + m, maxbits);
B = rand(1, maxbits) * w; 
model.A = A;
model.B = B;

expandedB = repmat(model.B, size(normalized_data, 1), 1);
Ym = (normalized_data * model.A + expandedB) / w ;
V = floor(Ym);

V = [V NormIndex];
elapse = toc(tmp_T);
end
