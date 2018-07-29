% only compress queries
function [V,elapse] = ALSH_compress(querySet, model, w, m)

tmp_T = tic;

[Nqueries, Nfeatures] = size(querySet);
normTerm = zeros(Nqueries, m);

queryNorms = sum(querySet.^2, 2).^0.5;
for k=1:Nqueries
    querySet(k, :) = querySet(k, :) ./ queryNorms(k);
end


for k=1:Nqueries
   	for i=1:m
		normTerm(k, i) = 0.5 ;
	end
end

normalized_data = [querySet normTerm];

expandedB = repmat(model.B, size(normalized_data, 1), 1);
Ym = (normalized_data * model.A + expandedB) / w ;
V = floor(Ym);

elapse = toc(tmp_T);
end
