function sh = movAvg2inputsPARMEX(x,data,scaling,cost,bigPoint)
% define leadlag to accept vectorized inputs and return only sharpe ratio
%%
% Vectorized input:
%   x(i,1) = lead
%   x(i,2) = lag
%   x(i,3) = average type
%
% Author: Mark Tompkins
%


%% MEX code to be skipped
coder.extrinsic('virtualBarsMEX_mex','movAvg2inputsMEX_mex')

[row,col] = size(x);
sh  = zeros(row,1);
x = round(x);
disp ('Number of rows:')
disp (row)
% run parallel iterations

parfor i = 1:row
%for i = drange(1:row)
	if col > 2
        type = x(i,3);
    else
        type = 0;
	end;
	if col > 3
        error('No longer handling vBars at the function level.  Address the passed in ''range''');
	end; %if

    if x(i,1) >= x(i,2)
        sh(i) = NaN;
    else
        [~,~,sh(i)] = movAvg2inputsMEX_mex(data, x(i,1), x(i,2),type,scaling,cost,bigPoint);  %#ok<PFBNS>
    end;

end; %parfor



