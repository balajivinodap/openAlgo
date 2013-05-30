function sh = rsiPAR(x,data,scaling,cost,bigPoint)
% define rsi to accept vectorized inputs and return only sharpe ratio
%
% Author: Mark Tompkins
%

row = size(x,1);
sh = zeros(row,1);
parfor i = 1:row
    [~,~,sh(i)] = rsiMETS(data,[x(i,1),x(i,2)],x(i,3),scaling,cost,bigPoint);
end
