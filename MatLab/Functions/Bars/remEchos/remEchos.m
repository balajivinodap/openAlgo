function [cleanS] = remEchos( s )
%REMECHOS Will remove echos froms a provided vector
% The primary purpose of remEchos is to transform a STATE vector in to a SIGNAL vector.
% For example, when attempting to generating signals from a lead / lag moving average STATE function
% a logical output of whether a condition is true or not is provide.
%	Example (where s is a given STATE vector):
%		s(lead>lag) =  1
%		s(lead<=lag) = -1
% 	We can transform the output of this STATE condition as follows:
%		in	[1 1 1 1 1 1 -1 -1 -1 -1 -1 1 1 1 1]	<-- STATE INPUT
%		out	[1 0 0 0 0 0 -1  0  0  0  0 1 0 0 0]	<-- SIGNAL OUTPUT
%	This output is now "actionable" as a SIGNAL
%
% Author:           Mark Tompkins
% Revision:			4903.19069
% All rights reserved.


% Now that we've adjusted the first trade we need to remove all dupe information
% so that we'll be left with only singals and not echos

cleanS = s;
[rows,cols] = size(cleanS);

for jj=1:cols
    if(~isempty(cleanS(:,jj)))
        % Index of active trade - starts at first position
        actIdx = 1;
        actSig = cleanS(actIdx,jj);                     % Get the active signal so we can remove echos

        % Define the iterate range from first trades to last signal
        for ii=1:rows-1
            if (cleanS(ii+1,jj)==actSig)             	% If the next line is the same, it is an echo. Zero it out.
                cleanS(ii+1,jj) = 0;
            else                                       	% We have a new active signal.  Update.
                if (cleanS(ii+1,jj) ~= 0)              	% Zeros are not new signals
                    actSig = cleanS(ii+1,jj);                                      
                end; %if
            end; %if
        end; %for                                     	% Iterate until complete
    end; %if
end; %for
end

