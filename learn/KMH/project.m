function Xproj = project(X, R, sample_mean)

X = bsxfun(@minus, X, sample_mean);
Xproj = X * R;

return;