function [ varargout ] = iTrend_v2METS(price,iMult,qMult,hSub)
%ITREND_v2METS An indicator based on the work of John Elhers
%   instantaneousTrend returns a trading signal for a given iTrend and MA cross as well as a
%   technical indicator.
%
%   T = ITREND_v2METS(PRICE) returns the instantaneous trend
%
%   [ITREND] = ITREND_v2METS(...) returns the Instantaneous Trend
%% Error check
rows = size(price,1);
if rows < 41
    error('ITREND_v2METS:dataSizeFailure','iTrend_v2METS requires a minimum of 41 observations. Exiting.');
end;

%% Defaults
if ~exist('iMult','var'), iMult=.635; end;
if ~exist('qMult','var'), qMult=.338; end;

%% Parse
[~,fHigh,fLow,fClose] = OHLCSplitter(price);
HighLow = (fHigh+fLow)/2;

[tLine] = iTrend_v2MEX_mex(HighLow,iMult,qMult);

  %% If no assignment to variable, show the averages in a chart
    if (nargout == 0) && (~exist('hSub','var'))% Plot
        % Plot results
        plot([fClose,tLine]); 
        grid on
        legend('Close','iTrend','Location','NorthWest')
        title('Instantaneous Trend')
    elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
        % We pass hSub as a string so we can have asymmetrical graphs
        % The call to char() parses the passed cell array
        ax(1) = subplot(str2num(char(hSub(1))), str2num(char(hSub(2))), str2num(char(hSub(3)))); %#ok<ST2NM>
        plot([fClose,tLine]); 
        axis (ax(1),'tight');
        grid on
        legend('Close','iTrend','Location','NorthWest')
        title('Instantaneous Trend')
    else
        for i = 1:nargout
            switch i
                case 1
                    varargout{1} = tLine;
                otherwise
                    warning('ITREND_v2METS:OutputArg',...
                        'Too many output arguments requested, ignoring last ones');
            end %switch
        end %for
    end %if
end

