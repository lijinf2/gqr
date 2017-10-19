function recall_vs_sel = eval_recall_vs_sel(BincodeXtraining, BincodeXtest, gt, K, selectivity)

num_test = size(BincodeXtest, 1);

ham = hammingDist(BincodeXtest, BincodeXtraining);
[not_used,index] = sort(ham,2);
index = index(:,1:selectivity);

clear ham;

% find the sucess items
recall_vs_sel = zeros(num_test, selectivity);

for i=1:num_test
    index_one_query = index(i, :);
    gt_one_query = gt(i, :);
    
    % find correct items
    hit = (sqdist(gt_one_query, index_one_query) == 0);
    recall_vs_sel(i, :) = cumsum(sum(hit, 1)) / K;
end
recall_vs_sel = mean(recall_vs_sel, 1);

end