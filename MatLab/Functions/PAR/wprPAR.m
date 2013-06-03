function sh = wprPAR(x,data,scaling,cost,bigPoint)
% WPR wrapper
%%
% Copyright 2010, The MathWorks, Inc.
% All rights reserved.% define wpr to accept vectorized inputs and return only sharpe ratio

row = size(x,1);
sh = zeros(row,1);
parfor ii = 1:row
    [~,~,sh(ii)] = wprSIG(data,x(ii,1),scaling,cost,bigPoint);
end