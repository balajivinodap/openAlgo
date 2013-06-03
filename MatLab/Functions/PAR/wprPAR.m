function sh = wprPAR(x,data,scaling,cost,bigPoint)
% WPR wrapper
%
% Author:           Mark Tompkins
% Revision:			4902.23682

row = size(x,1);
sh = zeros(row,1);
parfor ii = 1:row
    [~,~,sh(ii)] = wprSIG(data,x(ii,1),scaling,cost,bigPoint);
end