addpath('../../MatlabFunc/Tools')
addpath('../../MatlabFunc/ANNS/Hashing/Unsupervised')

dataset = 'sift1m';
method = 'PCARR'
codelength = 16;            
nHashTable = 16;
    
baseCodeFile = ['./hashingCodeTXT/',method,'table',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];              
queryCodeFile = ['./hashingCodeTXT/',method,'query',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];
modelFile = ['./hashingCodeTXT/',method,'model',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];
        
% zero-centered dataset
trainset = double(fvecs_read (['../../data/',dataset,'/',dataset,'_base.fvecs']));
testset = fvecs_read (['../../data/',dataset,'/',dataset,'_query.fvecs']);
trainset = trainset';
meanTrainset = mean(trainset);
trainset = trainset - repmat(meanTrainset, size(trainset, 1), 1);
testset = testset';
testset = testset - repmat(meanTrainset, size(testset, 1), 1);

if codelength > 128
    disp(['codelenth ',num2str(codelength),' not supported yet!']);
    return;
end

disp('==============================');
disp([method,' ',num2str(codelength),'bit ',dataset,' nTable=',num2str(nHashTable)]);
disp('==============================');


time_train = 0;
time_test = 0;            

[cardinality, dimension] = size(trainset)
numQueries = size(testset, 1)

modelFid = fopen(modelFile,'wt');
% #of tables, dimension, codelength, #data points, #num queries
fprintf(modelFid,'%d %d %d %d %d\n' , nHashTable, dimension, codelength, cardinality, numQueries);
fprintf(modelFid, '%f ', meanTrainset);
fprintf(modelFid, '\n');
% pca and save pca 
[pc, ~] = eigs(cov(trainset), codelength);
model.pc = pc;
trainset = trainset * model.pc;
testset = testset * model.pc;
for i = 1 : size(model.pc, 1);
    fprintf(modelFid,'%f ',model.pc(i,:));
    fprintf(modelFid,'\n');
end

baseCodeFid = fopen(baseCodeFile,'wt');
queryCodeFid = fopen(queryCodeFile,'wt');
for j =1:nHashTable                

    % Rotation
    R = randn(codelength, codelength);
    [U11 S2 V2] = svd(R);
    R = U11(:,1:codelength);
    
    % save model
    for i = 1 : size(R,1);
        fprintf(modelFid,'%f ',R(i,:));
        fprintf(modelFid,'\n');
    end
    
    % save base codes 
    trainB = (trainset * R > 0);    
    for i = 1 : size(trainB,1);
        fprintf(baseCodeFid,'%g ',trainB(i,:));
        fprintf(baseCodeFid,'\n');
    end
    
    % save query codes
    testB = (testset * R > 0); 
    for i = 1 : size(testB,1);
        fprintf(queryCodeFid,'%g ',testB(i,:));
        fprintf(queryCodeFid,'\n');
    end
end
fclose(modelFid)
fclose(baseCodeFid);
fclose(queryCodeFid);
disp('==============================');
disp(['Total training time: ',num2str(time_train)]);
disp(['Total testing time: ',num2str(time_test)]);
