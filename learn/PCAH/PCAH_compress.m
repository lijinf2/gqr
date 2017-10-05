function [B,elapse] = PCAH_compress(A, model)

tmp_T = tic;

Ym = A * model.pc;
B = (Ym > 0);

elapse = toc(tmp_T);
end

