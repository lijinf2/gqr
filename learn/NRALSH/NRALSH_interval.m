function [lens] = NRALSH_interval (norms, prct)

numItems = size(norms, 1)
lens = zeros(numItems, 1);

for k=1:numItems
    index = find(prct>=norms(k), 1) - 1;
    lens(k) = index;     
end
end