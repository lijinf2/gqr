addpath('../../MatlabFunc/Tools')
addpath('../../MatlabFunc/ANNS/Hashing/Unsupervised')

dataset = 'audio';

% zero-centered dataset
trainset = double(fvecs_read (['../../data/',dataset,'/',dataset,'_base.fvecs']));
trainset = trainset';

size(trainset)

norms = sum(trainset.^2, 2);
histogram(norms);
bits = 12;
prc = prctile(norms, linspace(0, 100, bits+1));
