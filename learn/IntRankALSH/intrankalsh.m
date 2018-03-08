addpath('../../MatlabFunc/Tools')
% addpath('../../MatlabFunc/ANNS/Hashing/Unsupervised')

dataset = 'movielens';
W = 2.5;
codelength = 16;
nHashTable = 1; % multiple hash tables do not help accuracy, but only slow down anns

m = 3;
U = 0.83;

method = 'IntRankALSH'
baseCodeFile = ['./hashingCodeTXT/',method,'table',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];              
queryCodeFile = ['./hashingCodeTXT/',method,'query',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];
modelFile = ['./hashingCodeTXT/',method,'model',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];
        
% zero-centered dataset
trainset = double(fvecs_read (['../../data/',dataset,'/',dataset,'_base.fvecs']));
testset = fvecs_read (['../../data/',dataset,'/',dataset,'_query.fvecs']);
trainset = trainset';
meanTrainset = mean(trainset);
meanTrainset = meanTrainset-meanTrainset;

trainset = trainset - repmat(meanTrainset, size(trainset, 1), 1);
testset = testset';
testset = testset - repmat(meanTrainset, size(testset, 1), 1);


disp('==============================');
disp([method,' ',num2str(codelength),'bit ',dataset,' nTable=',num2str(nHashTable)]);
disp('==============================');



[cardinality, dimension] = size(trainset)
numQueries = size(testset, 1)

modelFid = fopen(modelFile,'wt');
% #of tables, dimension, codelength, #data points, #num queries
fprintf(modelFid,'%d %d %d %d %d %f\n' , nHashTable, dimension, codelength, cardinality, numQueries, W);
% m and U in alsh 
fprintf(modelFid, '%d %f\n', m, U);

fprintf(modelFid, '%f ', meanTrainset);
fprintf(modelFid, '\n');
baseCodeFid = fopen(baseCodeFile,'wt');
queryCodeFid = fopen(queryCodeFile,'wt');
for j =1:nHashTable             dataset   
    % train and test model
    trainStr = ['[model, trainB ,train_elapse] = ',method,'_learn(trainset, codelength, W, m, U);'];
    testStr = ['[testB,test_elapse] = ',method,'_compress(testset, model, W);'];
    eval(trainStr);
    eval(testStr);
   
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
disp('==============================');
