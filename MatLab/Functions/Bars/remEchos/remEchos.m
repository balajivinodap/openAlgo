function [cleanS] = remEchos( s )
%REMECHOS Will remove echos froms a trading signal
% Trading signals may have an echo when they are generated from some strategies.
% For example, when generating signals from a lead / lag moving average calculation
% the following method is used:
% s(lead>lag) = 2;                                
% s(lead<lag) = -2;  
% In this instance we only want to have a new signal trigger when there is a cross.
% NOT in the case of every check
% This function removes the echos and makes it a 'On Cross'

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

