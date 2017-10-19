function centers = affinity_preserving_KM_display(X,b,num_iter,lambda)

n = size(X,1);

% PCA (in case X is not PCA-aligned)
meanX = mean(X,1);
X = bsxfun(@minus, X, meanX);
covX = X' * X / n;
[eigVectors, eigValues] = eigs(double(covX),size(covX,1),'LM');
X = X * eigVectors;

% initialize by PCAH


[centers, scale] = init_PCAH(X,b);

show_clusters(X,centers);
title(['initilization (PCAH)']);
    
for iter=1:num_iter
    if mod(iter,10)==0
        fprintf('iter: %d \n', iter);
    end
    centers = optimize_one_iter(X,centers,scale,lambda);
    show_clusters(X,centers);
    title(['iter: ', int2str(iter)]);
end

centers = centers * eigVectors';
centers = bsxfun(@plus, centers, meanX);
end