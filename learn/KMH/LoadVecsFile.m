function dataMat = LoadVecsFile(filePath)
% INTRO
%   load a data matrix from the *vecs file
% INPUT
%   filePath: string (file path)
% OUTPUT
%   dataMat: D x N (data matrix)

% determine the value length and conversion string
if ~isempty(strfind(filePath, 'bvecs'))
  valLen = 1;
  cvtStr = 'uint8=>uint8';
elseif ~isempty(strfind(filePath, 'ivecs'))
  valLen = 4;
  cvtStr = 'uint32=>uint32';
elseif ~isempty(strfind(filePath, 'fvecs'))
  valLen = 4;
  cvtStr = 'float=>single';
end

% open file
inFile = fopen(filePath, 'rb');

% obtain number of vectors/dimensions
rowCnt = fread(inFile, 1, 'uint32');
colLen = 4 + valLen * rowCnt;
fseek(inFile, 0, 'eof');
byteCnt = ftell(inFile);
colCnt = byteCnt / colLen;
fprintf('[INFO] loading a %d x %d matrix from %s\n', rowCnt, colCnt, filePath);

% load data matrix
fseek(inFile, 0, 'bof');
skipCnt = 4 / valLen;
dataMat = fread(inFile, [rowCnt + skipCnt, colCnt], cvtStr);
dataMat = dataMat(skipCnt + 1 : end, :);

% close file
fclose(inFile);

end
