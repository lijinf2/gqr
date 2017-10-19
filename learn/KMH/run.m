cifar60k_base = LoadVecsFile('/path/to/data/cifar60k_base.fvecs')';
[centers_table, R, sample_mean, codeXtrainingKMH] = my_demo(double(cifar60k_base), 8, 1, false);
serialization('hashingCodeTXT/', 'CIFAR60K', codeXtrainingKMH, 1, centers_table, R, sample_mean);
clear cifar60k_base;

sift1m_base = LoadVecsFile('/path/to/data/sift1m_base.fvecs')';
[centers_table, R, sample_mean, codeXtrainingKMH] = my_demo(double(sift1m_base), 16, 1, false);
serialization('hashingCodeTXT/', 'SIFT1M', codeXtrainingKMH, 1, centers_table, R, sample_mean);
clear sift1m_base;

gist_base = LoadVecsFile('/path/to/data/gist_base.fvecs')';
[centers_table, R, sample_mean, codeXtrainingKMH] = my_demo(double(gist_base), 16, 1, false);
serialization('hashingCodeTXT/', 'GIST', codeXtrainingKMH, 1, centers_table, R, sample_mean);
clear gist_base;

tiny5m_base = LoadVecsFile('/path/to/data/tiny5m_base.fvecs')';
[centers_table, R, sample_mean, codeXtrainingKMH] = my_demo(double(tiny5m_base), 16, 1, false);
serialization('hashingCodeTXT/', 'TINY5M', codeXtrainingKMH, 1, centers_table, R, sample_mean);
clear tiny5m_base;
