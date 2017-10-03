
addpath('../../MatlabFunc/Tools')
addpath('../../MatlabFunc/ANNS/Hashing/Unsupervised')
datasetCandi = {'audio'};
% datasetCandi = {'siftsmall'};
% datasetCandi = {'sift', 'gist'};
% datasetCandi = {'sift'};
% datasetCandi = {'gist'};
% datasetCandi = {'imagenet'};

% methodCandi = {'LSH','ITQ'};
% methodCandi = {'LSH'};
% methodCandi = {'ITQ'};
% methodCandi = {'SH'};
% methodCandi = {'SpH'};
% methodCandi = {'BRE'};
% methodCandi = {'USPLH'};
% methodCandi = {'KLSH'};
% methodCandi = {'IsoH'};
% methodCandi = {'DSH'};

% methodCandi = {'LSH','SH','BRE','USPLH','ITQ','SpH','IsoH','DSH'};
methodCandi = {'ITQ'};

% codelengthCandi = [24 28 36 40 44 48];
codelengthCandi = [16];

nHashTable = 1;

for d=1:length(datasetCandi)
    dataset = datasetCandi{d};
    
    for m=1:length(methodCandi)
        method = methodCandi{m};
        
        for l=1:length(codelengthCandi)
            codelength = codelengthCandi(l);
            
            
            if codelength > 128
                disp(['codelenth ',num2str(codelength),' not supported yet!']);
                return;
            end
            
            disp('==============================');
            disp([method,' ',num2str(codelength),'bit ',dataset,' nTable=',num2str(nHashTable)]);
            disp('==============================');
            
            trainset = double(fvecs_read (['../../data/',dataset,'/',dataset,'_base.fvecs']));
            testset = fvecs_read (['../../data/',dataset,'/',dataset,'_query.fvecs']);
            trainset = trainset';
            testset = testset';
            
            train_all = 0;
            test_all = 0;
            for j =1:nHashTable                
                % train and test model
                trainStr = ['[model, trainB ,train_elapse] = ',method,'_learn(trainset, codelength);'];
                testStr = ['[testB,test_elapse] = ',method,'_compress(testset, model);'];
                eval(trainStr);
                eval(testStr);
                                
                train_all = train_all + train_elapse;
                test_all = test_all + test_elapse;
               
                % save model and data table
                ResultFile = ['./hashingCodeTXT/',method,'table',upper(dataset),num2str(codelength),'b_',num2str(j),'.txt'];              
                                          
                fid = fopen(ResultFile,'wt');
                
                % #of tables, dimension, codelength, #data points
                [numPoints, dimension] = size(trainset);
                fprintf(fid, '%d %d %d %d\n', nHashTable, dimension, codelength, numPoints);
                
                % save model
                for i = 1 : size(model.pc, 1);
                    fprintf(fid, '%g ', model.pc(i, :));
                    fprintf(fid, '\n');
                end
                
                for i = 1 : size(model.R, 1);
                    fprintf(fid, '%g ', model.R(i, :));
                    fprintf(fid, '\n');
                end
                
                % save table                
                for i = 1 : size(trainB,1);
                    fprintf(fid,'%g ',trainB(i,:));
                    fprintf(fid,'\n');
                end
                fclose(fid);
                
                % save query table
                ResultFile = ['./hashingCodeTXT/',method,'query',upper(dataset),num2str(codelength),'b_',num2str(j),'.txt'];
                fid = fopen(ResultFile,'wt');
                for i = 1 : size(testB,1);
                    fprintf(fid,'%g ',testB(i,:));
                    fprintf(fid,'\n');
                end
                fclose(fid);
            end
            disp('==============================');
            disp(['Total training time: ',num2str(train_all)]);
            disp(['Total testing time: ',num2str(test_all)]);
        end
    end
end
