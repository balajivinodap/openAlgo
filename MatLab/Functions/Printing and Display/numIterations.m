function [I] = numIterations(range)
%NUMITERATIONS A function that takes a parametric sweep range and returns the number of iterations
%   Parametric sweeps are passed ordinarily as a 'range' varable for the parameter(s) it will test.
%   numIterations will parse this value, often given in colon notation, and calculate the number of
%   iterations in the sweep
%
%   I = NUMITERATIONS(RANGE) returns the number of iterations from a given range variable
%
% Author: Mark Tompkins
%

N = length(range);

I = length(range{1});
for ii = 2:N
    I = I * length(range{ii});
end %for

end

