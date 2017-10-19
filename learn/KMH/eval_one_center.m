function cost = eval_one_center(c_j, sum_X_j, n_table, n, centers, j, ham_dist,lambda)

% this function computes the cost of Eqn. (8) given a center c_j

%E_quan = (-2*dotprod(sum_X_j, c_j') + n_table(j)*dotprod(c_j, c_j'))/n;    % the constant about X has been ignored
%E_quan = (-2*(sum_X_j*c_j') + n_table(j)*(c_j*c_j'))/n;    % the constant about X has been ignored
E_quan = (-2*sum_X_j + n_table(j)*c_j)*c_j'/n;    % the constant about X has been ignored

euc_dist = sum((bsxfun(@minus, centers, c_j)).^2,2).^.5;
euc_dist(j) = 0;

err_dist = (euc_dist - ham_dist).^2;
    
E_aff = n_table(j)*sum(n_table.*err_dist)/n/n;
    
cost = E_quan+2*lambda*E_aff; % at submission there was a typo here (cost = 2*E_quan+lambda*E_aff;)

end