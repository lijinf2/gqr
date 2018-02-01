addpath('../../MatlabFunc/Tools')
addpath('../../MatlabFunc/ANNS/Hashing/Unsupervised')

dataset = 'audio';

% zero-centered dataset
trainset = double(fvecs_read (['../../data/',dataset,'/',dataset,'_base.fvecs']));
trainset = trainset';

testset = fvecs_read (['../../data/',dataset,'/',dataset,'_query.fvecs']);
testset = testset' ;

norms = sum(trainset.^2, 2);
testNorms = sum(testset.^2, 2);
histogram(norms);
bits = 16;
prc = prctile(norms, linspace(0, 100, bits+1));
