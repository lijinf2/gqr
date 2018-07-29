function [model, V, elapse] = NRALSH_learn(dataset, maxbits, w, m, U, normInteval, norms, prct)

tmp_T = tic;

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
   	for i=1:m 
		normTerm(k, i) = norms(k)^(2^i);
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

% tha above is original ALSH, below adds lengthIndex

[lens] = NRALSH_interval(norms, prct);

V = [V lens];
elapse = toc(tmp_T);
end
