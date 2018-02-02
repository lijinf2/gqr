addpath('../../MatlabFunc/Tools')
addpath('../../MatlabFunc/ANNS/Hashing/Unsupervised')

dataset = 'movielens';

% zero-centered dataset
trainset = double(fvecs_read (['../../data/',dataset,'/',dataset,'_base.fvecs']));
trainset = trainset';

testset = fvecs_read (['../../data/',dataset,'/',dataset,'_query.fvecs']);
testset = testset' ;

norms = sum(trainset.^2, 2).^(0.5);
testNorms = sum(testset.^2, 2);
% histogram(norms);
bits = 63;
prc = prctile(norms, linspace(0, 100, bits+1));
% stem(prc)

norm_median = median(norms);
eps = 0.0001;
while length(find(norms<norm_median-eps)) + length(find(norms>norm_median+eps)) > 0.3*length(norms)
    eps = eps + 0.0001
end
