function [varargout] = signalNoiseRatio_DIS(price,iMult,qMult,hSub)

%% Error check
rows = size(price,1);
if rows < 8
    error('SIGNALNOISERATIO:dataSizeFailure','signalNoiseRatio requires a minimum of 8 observations. Exiting.');
end;

%% Defaults
if ~exist('iMult','var'), iMult=.635; end;
if ~exist('qMult','var'), qMult=.338; end;

amp = snr_mex(price,iMult,qMult);

%% Parse
[~,~,~,fClose] = OHLCSplitter(price);

 %% If no assignment to variable, show the averages in a chart
    if (nargout == 0) && (~exist('hSub','var'))% Plot
        
    % Plot results
    ax(1) = subplot(2,1,1);
    plot(fClose); 
    axis (ax(1),'tight');
    grid on
    legend('Close','Location','NorthWest')
    title('Closing Price')
    
    ax(2) = subplot(2,1,2);
    plot(amp); grid on
    legend('SNR','Location','North')
    title('Signal To Noise Ratio')
    linkaxes(ax,'x')
    
    elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
        % We pass hSub as a string so we can have asymmetrical graphs
        % The call to char() parses the passed cell array
        ax(1) = subplot(str2num(char(hSub(1))), str2num(char(hSub(2))), str2num(char(hSub(3)))); %#ok<ST2NM>
        plot(fClose);
        axis (ax(1),'tight');
        grid on
        legend('Close','Location','NorthWest')
        title('Closing Price')
        
        ax(2) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))), str2num(char(hSub(4)))); %#ok<ST2NM>
        plot(amp); grid on
        legend('SNR','Location','North')
        title('Signal To Noise Ratio')
        linkaxes(ax,'x') 
    else
        for ii = 1:nargout
            switch ii
                case 1
                    varargout{1} = amp;
                otherwise
                    warning('SIGNALNOISERATIO:OutputArg',...
                        'Too many output arguments requested, ignoring last ones');
            end %switch
        end %for
    end %if
    


