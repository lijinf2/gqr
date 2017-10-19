function [centers,scale] = init_PCAH(X,b)

n = size(X,1);
dim = size(X,2);
k = 2^b;

% generate a k-by-b lookup table for binary codes
lut = generate_lut(b);

% do PCA Hashing
meanX = mean(X,1);
X = bsxfun(@minus, X, meanX);
covX = X' * X / n;
[eigVectors,eigValues] = eigs(covX,size(covX,1),'LM');

Xproj = X * eigVectors;

code_PCAH = zeros(size(Xproj(:,1:b)));
code_PCAH(Xproj(:,1:b) > 0) = 1;
code_PCAH = code_PCAH * 2 - 1;

% find scale
scale = sum(sum(Xproj(:,1:b).*code_PCAH)) / sum(sum(code_PCAH.^2));

% generate centers in the PCA space
centers_proj = zeros(k,dim);
centers_proj(:,1:b) = 2*lut(:,end:-1:1)-1;
centers_proj = centers_proj * scale;

% project the centers back

centers = centers_proj * eigVectors';
centers = centers + repmat(meanX, [k,1]);
end