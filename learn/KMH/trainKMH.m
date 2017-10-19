function [centers_table, R, sample_mean] = trainKMH(Xtraining, M, b, num_iter, lambda)

%%% projection

[R, sample_mean] = projection_optimized_product_quantization(Xtraining, M);

Xtraining_proj = project(Xtraining, R, sample_mean);

%%% affinity_preserving_PQ

centers_table = affinity_preserving_PQ(Xtraining_proj, M, b, num_iter, lambda);

end