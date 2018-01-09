addpath('../../MatlabFunc/Tools')
addpath('../../MatlabFunc/ANNS/Hashing/Unsupervised')

% single PCA and multiple rotations
dataset = 'audio';
method = 'IsoH'
codelength = 12;
nHashTable = 1;

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


[cardinality, dimension] = size(trainset)
numQueries = size(testset, 1)

modelFid = fopen(modelFile,'wt');
% #of tables, dimension, codelength, #data points, #num queries
fprintf(modelFid,'%d %d %d %d %d\n' , nHashTable, dimension, codelength, cardinality, numQueries);
fprintf(modelFid, '%f ', meanTrainset);
fprintf(modelFid, '\n');

% pca and save pca
[pc, pv] = eigs(trainset'*trainset, codelength);
meanPV = mean(diag(pv));

model.pc = pc;
for i = 1 : size(model.pc, 1);
    fprintf(modelFid,'%f ',model.pc(i,:));
    fprintf(modelFid,'\n');
end

baseCodeFid = fopen(baseCodeFile,'wt');
queryCodeFid = fopen(queryCodeFile,'wt');
for j =1:nHashTable

    % Rotation

    n_iter = 100;	
    R = randn(codelength,codelength);
    [U11, ~, ~] = svd(R);
    R = U11(:,1:codelength);
    Z = R'*pv*R;
    for iter=1:n_iter
        T = Z;
        for i = 1:codelength
            T(i,i) = meanPV;
        end
        [R, ~] = eig(T);
        Z = R'*pv*R;
    end    

    % save model
    for i = 1 : size(R,1);
        fprintf(modelFid,'%f ',R(i,:));
        fprintf(modelFid,'\n');
    end

    % save base codes
    trainB = (trainset * model.pc * R > 0);
    for i = 1 : size(trainB,1);
        fprintf(baseCodeFid,'%g ',trainB(i,:));
        fprintf(baseCodeFid,'\n');
    end

    % save query codes
    testB = (testset * model.pc * R > 0);
    for i = 1 : size(testB,1);
        fprintf(queryCodeFid,'%g ',testB(i,:));
        fprintf(queryCodeFid,'\n');
    end
end
fclose(modelFid)
fclose(baseCodeFid);
fclose(queryCodeFid);
disp('==============================');

