function centers_new = optimize_one_iter(X, centers, sc, lambda)

n=size(X,1);
dim=size(X,2);
k=size(centers,1);

lut = generate_lut(log2(k));

code = (2*lut(:,end:-1:1)-1)*sc;

d_h = sqdist(code',code').^.5; % this is the k-by-k affinity matrix of Hamming-based distances

%%%%% 'E-step': update the index of each sample
dist=sqdist(X',centers');
[dist,idx_X]=min(dist,[],2);


%%%%% 'M-step': update the centers using Eqn. (8)
n_table = zeros(k,1); % num of samples belonging to each center
for i=1:k
    n_table(i)=numel(find(idx_X==i));
end

sum_X_table = zeros(k,dim); % sample sum of each center
for i=1:k
    sum_X_table(i,:)=sum(X(idx_X==i,:),1);
end

for j=1:k % update the centers sequentially
    c_j0 = double(centers(j,:)); % initial
    
    warning off; %#ok<WNOFF>
    
    % update one center minimizing Eqn. (8)
    options = optimset('Display','off');
    c_j = fminunc(@(c_j) eval_one_center(c_j, sum_X_table(j,:), n_table, n, centers, j, d_h(:,j), lambda), c_j0, options);
    
    centers(j,:)=c_j;
end

centers_new = centers;

end
