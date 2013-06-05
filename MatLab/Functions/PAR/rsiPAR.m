function sh = rsiPAR(x,data,scaling,cost,bigPoint)
% define rsi to accept vectorized inputs and return only sharpe ratio
%
% Author:           Mark Tompkins
% Revision:			4902.23658

row = size(x,1);
sh = zeros(row,1);
parfor ii = 1:row
    [~,~,sh(ii)] = rsiSIG(data,[x(ii,1),x(ii,2)],x(ii,3),scaling,cost,bigPoint);
end
