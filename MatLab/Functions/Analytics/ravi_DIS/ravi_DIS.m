function [ varargout ] = ravi_DIS(price,lead,lag,D,M,hSub)
%RAVI_DIS Indictor to detect a change between trending and ranging states
%   Detailed explanation goes here
%
% Author:		Mark Tompkins
% Revision: 	4902.18884

if size(price,2) < 4
    error('RAVI:tooFewInputs', ...
            'We call ATR to normalize price data therefore we need O | H | L | C as price input. Exiting.');
end; %if

ind=ravi(price,lead,lag,D,M);

% Determine divisor for measuring the rate of change
if D == 0
    div = ' Slow MA';
elseif D == 1
    div = ' ATR';
else
    error('RAVI:inputArg','Unknown input in value ''D''. Aborting.');
end;

%% Plot if requested
if nargout == 0 && (~exist('hSub','var'))% Plot
    
    ax(1) = subplot(2,1,1);
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    title('Price')
    
    plotTitle = strcat('Normalized RAVI - Using  ',div);
    
    ax(2) = subplot(2,1,2);
    ylim([0 100])
    axis manual;
    hold on
    plot(ind);
    axis (ax(1),'tight');
    grid on
    title(plotTitle)
    linkaxes(ax,'x')
    
elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
	% We pass hSub as a string so we can have asymmetrical graphs
	% The call to char() parses the passed cell array
    plotTitle = strcat('Normalized RAVI - Using  ',div);
    
    ax(1) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(3)))); %#ok<ST2NM>
	axis manual;
    plot(ind);
    axis (ax(1),'tight');
    ylim([0 100]);

    hold on
%     
    grid on
    title(plotTitle)
else    
    for ii = 1:nargout
        switch ii
            case 1
                varargout{1} = ind; 
            otherwise
                warning('RAVI:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end; %switch
    end; %for
end; %if
end

