function [dirtyS] = insEchos( s )
%INSECHOS Will insert echos from a trading signal
% insEchos will effectively transform a SIGNAL to a STATE. One must be vigilant in
% its use as the logical that might otherwise indicate a given state may no longer
% be true.
%
% When aggregating SIGNALS or STATES it is sometimes necessary to artificially
% normalize the inputs. For example, consider the following logical condition:
%
%	"What is the most recent SIGNAL from function_SIG?  ...was it a BUY or SELL?"
%
% insEchos will replicate the last non-zero value until the next non-zero value in a 
% serial fashion.
%
%	Example:
%				in	[0 0 0 1 0 0 0 -1  0  0  0 -1  0  0  0 1 0 0]
%				out	[0 0 0 1 1 1 1 -1 -1 -1 -1 -1 -1 -1 -1 1 1 1]
%
%	Note:	Matrix inputs are permitted
%
% Author:           Mark Tompkins
% Revision:			4903.17854
% All rights reserved.

dirtyS = s;
[rows,cols] = size(dirtyS);

% Iterate columns
for jj = 1:cols
    % Iterate rows
    for ii = 2:rows
        if  dirtyS(ii) == 0
            dirtyS(ii) = dirtyS(ii-1);
        end; %if
    end; %for ii
end; %for jj

end

