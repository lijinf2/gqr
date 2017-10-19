
addpath('../../MatlabFunc/Tools')
addpath('../../MatlabFunc/ANNS/Hashing/Unsupervised')

dataset = 'audio';
method = 'SpH'
codelength = 12;            
nHashTable = 1;

baseCodeFile = ['./hashingCodeTXT/',method,'table',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];              
queryCodeFile = ['./hashingCodeTXT/',method,'query',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];
modelFile = ['./hashingCodeTXT/',method,'model',upper(dataset),num2str(codelength),'b_',num2str(nHashTable),'tb.txt'];

trainset = double(fvecs_read (['../../data/',dataset,'/',dataset,'_base.fvecs']));
testset = fvecs_read (['../../data/',dataset,'/',dataset,'_query.fvecs']);
trainset = trainset';
testset = testset';

time_train = 0;
time_test = 0;            

[cardinality, dimension] = size(trainset)
numQueries = size(testset, 1)

modelFid = fopen(modelFile,'wt');
% #of tables, dimension, codelength, #data points, #num queries
fprintf(modelFid,'%d %d %d %d %d\n' , nHashTable, dimension, codelength, cardinality, numQueries);
baseCodeFid = fopen(baseCodeFile,'wt');
queryCodeFid = fopen(queryCodeFile,'wt');
for j =1:nHashTable                
    % train and test model
    trainStr = ['[model, trainB ,train_elapse] = ',method,'_learn(trainset, codelength);'];
    testStr = ['[testB,test_elapse] = ',method,'_compress(testset, model);'];
    eval(trainStr);
    eval(testStr);
                    
    time_train = time_train + train_elapse;
    time_test = time_test + test_elapse;
   
    % save model
    for i = 1 : size(model.centers,1);
        fprintf(modelFid,'%f ',model.centers(i,:));
        fprintf(modelFid,'\n');
    end
    for i = 1 : size(model.radii,1);
        fprintf(modelFid,'%f ',model.radii(i,:));
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
disp(['Total training time: ',num2str(time_train)]);
disp(['Total testing time: ',num2str(time_test)]);
