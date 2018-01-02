addpath('../../MatlabFunc/Tools')
addpath('../../MatlabFunc/ANNS/Hashing/Unsupervised')

dataset = 'audio';
codelength = 12;            
nHashTable = 1; % multiple hash tables do not help accuracy, but only slow down anns


method = 'SIM'
baseCodeFile = ['./hashingCodeTXT/',method,'table',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];              
queryCodeFile = ['./hashingCodeTXT/',method,'query',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];
modelFile = ['./hashingCodeTXT/',method,'model',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];
        
% zero-centered dataset
trainset = double(fvecs_read (['../../data/',dataset,'/',dataset,'_base.fvecs']));
testset = fvecs_read (['../../data/',dataset,'/',dataset,'_query.fvecs']);
trainset = trainset';
testset = testset';

% add a normlize term for every row
% [max_norm] = precess(trainset, testset)

if codelength > 128
    disp(['codelenth ',num2str(codelength),' not supported yet!']);
    return;
end

disp('==============================');
disp([method,' ',num2str(codelength),'bit ',dataset,' nTable=',num2str(nHashTable)]);
disp('==============================');



[cardinality, dimension] = size(trainset)
numQueries = size(testset, 1)

modelFid = fopen(modelFile,'wt');
% #of tables, dimension, codelength, #data points, #num queries
fprintf(modelFid,'%d %d %d %d %d %d\n' , nHashTable, dimension, codelength, cardinality, numQueries);
fprintf(modelFid, '\n');
baseCodeFid = fopen(baseCodeFile,'wt');
queryCodeFid = fopen(queryCodeFile,'wt');
for j =1:nHashTable                
    % train and test model
    trainStr = ['[model, trainB ,train_elapse] = ',method,'_learn(trainset, codelength);'];
    testStr = ['[testB,test_elapse] = ',method,'_compress(testset, model);'];
    eval(trainStr);
    eval(testStr);
   
    % save model
    for i = 1 : size(model.U, 1);
        fprintf(modelFid,'%f ',model.U(i,:));
        fprintf(modelFid,'\n');
    end

    % save base codes 
    for i = 1 : size(trainB,1);
        fprintf(baseCodeFid,'%g ',trainB(i,:));
        fprintf(baseCodeFid,'\n');
    end
    
    % save query codes
    for i = 1 : size(testB,1);
        fprintf(queryCodeFid,'%g ',testB(i,:));
        fprintf(queryCodeFid,'\n');
    end
end
fclose(modelFid)
fclose(baseCodeFid);
fclose(queryCodeFid);
disp('==============================');

