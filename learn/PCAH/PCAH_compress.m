function [B,elapse] = PCAH_compress(A, model)

tmp_T = tic;

Ym = A * model.pc;

%Ym = Ym - repmat(model.codemean, size(Ym, 1), 1);

B = (Ym > 0);

elapse = toc(tmp_T);
end

