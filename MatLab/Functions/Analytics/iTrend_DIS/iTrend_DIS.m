function [ varargout ] = iTrend_DIS(price,hSub)
%ITREND_DIS An indicator based on the work of John Elhers
%   ITREND_DIS returns an instantaneous trend and a tLine value.
%
%   Input 'price' should be of an O | H | L | C form as we use the average 
%   of the Open & Close when passed to iTrend.m
%
%   [iTrend, instT] = ITRENDSIGDIS(PRICE)       returns an instantaneous trend and 
%                                               an accompanying trendline as simple 
%                                               average over the measured dominant 
%                                               cycle period based upon a 14-period iTrend. 
%
%   iTrend, instT] = ITRENDSIGDIS(PRICE,hSub)   includes the hSub variable for asymetrical
%                                               graphic output
%
% Author:           Mark Tompkins
% Revision:			4905.34976
% All rights reserved.

%% Error check
rows = size(price,1);
if rows < 55
    error('iTrend2inputs:dataSizeFailure','iTrend2inputs requires a minimum of 55 observations. Exiting.');
end;

%% Parse
[~,fHigh,fLow,fClose] = OHLCSplitter(price);
HighLow = (fHigh+fLow)/2;


%% iTrend signal generation using dominant cycle crossing
if nargin > 0
    %% Preallocate
    
    [tLine,instT] = iTrend_mex(HighLow);
    
    %% If no assignment to variable, show the averages in a chart
    if (nargout == 0) && (~exist('hSub','var'))% Plot
        
    % Plot results
    plot([fClose,tLine,instT]); 
    grid on
    legend('Close','tLine','instT','Location','NorthWest')
    title('Instantaneous Trend')
    
    elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
        % We pass hSub as a string so we can have asymmetrical graphs
        % The call to char() parses the passed cell array
        ax(1) = subplot(str2num(char(hSub(1))), str2num(char(hSub(2))), str2num(char(hSub(3)))); %#ok<ST2NM>
        plot([fClose,tLine,instT]); 
        axis (ax(1),'tight');
        grid on
        legend('Close','tLine','instT','Location','NorthWest')
        title('Instantaneous Trend')
        
    else
        for i = 1:nargout
            switch i
                case 1
                    varargout{1} = tLine;
                case 2
                    varargout{2} = instT;
                otherwise
                    warning('ITREND_DIS:OutputArg',...
                        'Too many output arguments requested, ignoring last ones');
            end %switch
        end %for
    end %if
    
end; %if

end

