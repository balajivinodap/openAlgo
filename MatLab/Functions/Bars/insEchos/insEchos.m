function [dirtyS] = insEchos( s )
%INSECHOS Will insert echos from a trading signal
% Trading signals may have an echo when they are generated from some strategies.
% For example, when generating signals from a lead / lag moving average calculation
% the following method is used:
% s(lead>lag) = 2;                                
% s(lead<lag) = -2;  
% Functions currently clean these echos with remEchosMEX
% When aggregating signals we often needs these echoes reintroduced

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

