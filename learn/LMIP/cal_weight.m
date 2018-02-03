function [result] = cal_weight(prct, norms, k, normInteval, maxbits)
%   This is a function of calculate weight
%
%	Usage:
%	[result] = cal_weight(prct, norms, k, normInteval, maxbits)
%
%
%   version 1.0 --Feb/2018 
%
%   Written by  Xinyan Dai (xinyan.dai@outlook.com)
%    
intervalIndex = find(prct>norms(k), 1) - 2; 
% 
if isempty(intervalIndex)
	intervalIndex = size(prct, 2) - 2;
end
if(intervalIndex<0)
	intervalIndex = 0;
end

result = intervalIndex + maxbits - normInteval;
