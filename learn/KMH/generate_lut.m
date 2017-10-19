function lut = generate_lut(b)

% generate a lookup table for binary code

k = 2^b;
lut = zeros(k,b);

for i=1:k
    for j=1:b
        lut(i,j) = bitand(uint8(2^(j-1)),uint8(i-1))/uint8(2^(j-1));
    end
end

end