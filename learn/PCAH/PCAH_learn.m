function [model, B,elapse] = PCAH_learn(A, maxbits)

tmp_T = tic;

[pc, ~] = eigs(cov(A), maxbits);
A = A * pc;
model.pc = pc;

%model.codemean = mean(A);
%A = A - repmat(model.codemean, size(A, 1), 1);

B = (A > 0);

elapse = toc(tmp_T);
end