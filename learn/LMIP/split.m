function [prct] = split(norms, normInterval)
%   This is a function of split norms into normInterval
%
%	Usage:
%	[prct] = split(norms, normInteval)
%
%	      norms  : all data's array
%   normInterval : 
%
%   version 1.0 --Feb/2018 
%
%   Written by  Xinyan Dai (xinyan.dai@outlook.com)
%                                             
intervals = prctile(norms, linspace(0, 100, normInterval+1));
prct = intervals;
