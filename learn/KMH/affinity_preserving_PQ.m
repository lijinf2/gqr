function centers_table = affinity_preserving_PQ(X, M, b, num_iter, lambda)

D = size(X,2);
d = D / M;

centers_table = cell(M,1);

cell_X = cell(M, 1);
for m=1:M
    cell_X{m} = X(:,(1:d)+(m-1)*d);
end

%%% train the subspaces in parallel
parfor m = 1:M   
    fprintf('subspace #: %d\n', m);
    
    Xsub = cell_X{m};
    
    warning off; %#ok<WNOFF>
    centers = affinity_preserving_KM(Xsub,b,num_iter,lambda);
    
    centers_table{m} = centers;
end

fprintf('affinity_preserving_PQ done: M: %d, b: %d, num_iter: %d, lambda: %d, nSamples: %d\n', M, b, num_iter, lambda, size(X, 1));
end




