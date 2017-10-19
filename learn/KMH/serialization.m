function serialization(output_path_prefix, dataset, base_bin, num_bits_subspace, centers_table, R, sample_mean)

n = size(base_bin, 1);
d = size(sample_mean, 2);
num_bits = size(base_bin, 2);
num_subspace = num_bits / num_bits_subspace;
d_subspace = d / num_subspace;

base_bin_fid = fopen(strcat(output_path_prefix, 'KMHtable', dataset, int2str(num_bits), 'b', int2str(num_subspace), 'm_', '1tb.txt'), 'w');

for i=1:n
    for j=1:(num_bits-1)
        fprintf(base_bin_fid, '%d ', base_bin(i, j));
    end
    fprintf(base_bin_fid, '%d\n', base_bin(i, num_bits));
end

fclose(base_bin_fid);

model_fid = fopen(strcat(output_path_prefix, 'KMHmodel', dataset, int2str(num_bits), 'b', int2str(num_subspace), 'm_', '1tb.txt'), 'w');

fprintf(model_fid, '%d %d %d %d\n', n, d, num_bits, num_bits_subspace);

for i=1:(d-1)
    fprintf(model_fid, '%f ', sample_mean(i))
end
fprintf(model_fid, '%f\n', sample_mean(d))

for m=1:num_subspace
    for i=1:(2^num_bits_subspace)
        for j=1:(d_subspace-1)
            fprintf(model_fid, '%f ', centers_table{m}(i, j));
        end
        fprintf(model_fid, '%f\n', centers_table{m}(i, d_subspace));
    end
end

for i=1:d
    for j=1:(d-1)
        fprintf(model_fid, '%f ', R(i, j));
    end
    fprintf(model_fid, '%f\n', R(i, d));
end

fclose(model_fid);

