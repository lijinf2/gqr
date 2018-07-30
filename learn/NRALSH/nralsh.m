addpath('../../MatlabFunc/Tools')
% addpath('../../MatlabFunc/ANNS/Hashing/Unsupervised')

dataset = 'imagenet';
W = 2.5;
codelength = 26;
nHashTable = 1; % multiple hash tables do not help accuracy, but only slow down anns

m = 3;
U = 0.83;
normInteval = 64; % number of intervals

method = 'NRALSH'
baseCodeFile = ['./hashingCodeTXT/',method,'table',upper(dataset),num2str(codelength),'b_',num2str(normInteval),'i_',num2str(nHashTable),'tb.txt'];              
queryCodeFile = ['./hashingCodeTXT/',method,'query',upper(dataset),num2str(codelength),'b_',num2str(normInteval),'i_',num2str(nHashTable),'tb.txt'];
modelFile = ['./hashingCodeTXT/',method,'model',upper(dataset),num2str(codelength),'b_',num2str(normInteval),'i_',num2str(nHashTable),'tb.txt'];
        
% zero-centered dataset
trainset = double(fvecs_read (['../../data/',dataset,'/',dataset,'_base.fvecs']));
testset = fvecs_read (['../../data/',dataset,'/',dataset,'_query.fvecs']);
trainset = trainset';
testset = testset';

[cardinality, dimension] = size(trainset)
numQueries = size(testset, 1)

% cal multiple scaling factors
l2norms = sum(trainset.^2,  2).^0.5;
prct = prctile(l2norms, linspace(0, 100, normInteval));
prct(end) = max(l2norms);

% write data statistics into model file
modelFid = fopen(modelFile,'wt');
% #of tables, dimension, codelength, #data points, #num queries
fprintf(modelFid,'%d %d %d %d %d\n' , nHashTable, dimension, codelength, cardinality, numQueries);
% model parameters
fprintf(modelFid, '%f %d %f %d\n', W, m, U, normInteval);
fprintf(modelFid, '%f ', prct);
fprintf(modelFid, '\n');


baseCodeFid = fopen(baseCodeFile,'wt');
queryCodeFid = fopen(queryCodeFile,'wt');
for j =1:nHashTable                
    % train and test model
    [model, trainB, train_elapse] = NRALSH_learn(trainset, codelength, W, m, U, normInteval, l2norms, prct);
    [testB, test_elapse] = NRALSH_query_compress(testset, model, W, m);

   
    % save model
    for i = 1 : size(model.A, 1);
        fprintf(modelFid,'%f ',model.A(i,:));
        fprintf(modelFid,'\n');
    end
    fprintf(modelFid,'%f ',model.B);
    
    % save base codes 
    for i = 1 : size(trainB,1);
        fprintf(baseCodeFid,'%d ',trainB(i,:));
        fprintf(baseCodeFid,'\n');
    end
    
    % save query codes
    for i = 1 : size(testB,1);
        fprintf(queryCodeFid,'%d ',testB(i,:));
        fprintf(queryCodeFid,'\n');
    end
end
fclose(modelFid)
fclose(baseCodeFid);
fclose(queryCodeFid);
disp('===========finished===================');
