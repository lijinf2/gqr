function show_clusters(X,centers)

figure();
hold on;
axis equal;

k=size(centers,1);

dist=sqdist(X',centers');
[not_used,idx_centers]=min(dist,[],2);

color_table =  lines(16);
color_table(8,:) = [0,0,0];

for i=1:k
    Xsubset = X(idx_centers==i,:);
    plot3(Xsubset(:,1),Xsubset(:,2),Xsubset(:,3),'.','color',color_table(i,:),'MarkerSize',1);
end

for i=1:k
    plot3(centers(i,1),centers(i,2),centers(i,3),'bo');
end

num_bits = log2(k);
lut = generate_lut(num_bits);

for i=1:k
    for j=1:k
        h_dist = sum(abs(lut(i,:) - lut(j,:)));
        if h_dist == 1
            plot3([centers(i,1),centers(j,1)],[centers(i,2),centers(j,2)],[centers(i,3),centers(j,3)],'b-');
        end
    end
end

view(3);

end