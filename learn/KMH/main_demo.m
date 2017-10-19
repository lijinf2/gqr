close all;
clear variables;
clc;
warning off; %#ok<WNOFF>

dataset_name = 'sift';

load data_sift\sift_training.mat;
load data_sift\sift_test.mat;
load data_sift\sift_groundtruth.mat;

use_mex = true; % use mex for evaluation (set false if the mex file is not compiled)

matlabpool; % train the subspaces in parallel

%%% this should reproduce Fig. 5 (b) in the paper.
num_bits = 64; num_bits_subspace = 4;
h_fig = demo(Xtraining, Xtest, gt, num_bits, num_bits_subspace, use_mex);
saveas(h_fig, ['.\fig\', dataset_name, int2str(use_mex), '_', int2str(num_bits), 'bits', int2str(num_bits_subspace), 'b.fig']);

%%% this should reproduce Fig. 5 (c) in the paper.
num_bits = 128; num_bits_subspace = 4;
h_fig = demo(Xtraining, Xtest, gt, num_bits, num_bits_subspace, use_mex);
saveas(h_fig, ['.\fig\', dataset_name, int2str(use_mex), '_', int2str(num_bits), 'bits', int2str(num_bits_subspace), 'b.fig']);

matlabpool close;