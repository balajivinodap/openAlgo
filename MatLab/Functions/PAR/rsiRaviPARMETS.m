function shMETS = rsiRaviPARMETS(x,data,scaling,cost,bigPoint)
% define rsi + ravi to accept vectorized inputs and return only sharpe ratio
%% 
%                       rsiRavi(price,rsiM,rsiThresh,rsiType,raviF,raviS,raviD,raviM,raviE, ...
%                               raviThresh, scaling,cost,bigPoint)
%
% Author: Mark Tompkins
%

row = size(x,1);
shTest = zeros(row,1);
shVal = zeros(row,1);
shMETS = zeros(row,1); %#ok<NASGU>

testPts = floor(0.8*length(data(:,1)));
vBarsTest = data(1:testPts,:);
vBarsVal = data(testPts+1:end,:);

try 
      ppm = ParforProgressStarter2('RSI with RAVI Transformer Parameter Sweep', row, 0.1);
  catch me % make sure "ParforProgressStarter2" didn't get moved to a different directory
      if strcmp(me.message, 'Undefined function or method ''ParforProgressStarter2'' for input arguments of type ''char''.')
          error('ParforProgressStarter2 not in path.');
      else
          % this should NEVER EVER happen.
          msg{1} = 'Unknown error while initializing "ParforProgressStarter2":';
          msg{2} = me.message;
          print_error_red(msg);
          % backup solution so that we can still continue.
          ppm.increment = nan(1, nbr_files);
      end
end

parfor ii = 1:row
	[~,~,shTest(ii)] = rsiRaviSIG_DIS(vBarsTest,[x(ii,1) x(ii,2)],x(ii,3),x(ii,4),x(ii,5),x(ii,6),x(ii,7),x(ii,8),...
                                x(ii,9), x(ii,10),scaling,cost,bigPoint); 
	[~,~,shVal(ii)] = rsiRaviSIG_DIS(vBarsVal,[x(ii,1) x(ii,2)],x(ii,3),x(ii,4),x(ii,5),x(ii,6),x(ii,7),x(ii,8),...
                                x(ii,9), x(ii,10),scaling,cost,bigPoint); %#ok<PFBNS>
        ppm.increment(); %#ok<PFBNS> % update progressbar
end; %parfor

  try % use try / catch here, since delete(struct) will raise an error.
      delete(ppm);
  catch me %#ok<NASGU>
  end;
  
  %% Aggregate sharpe ratios
shMETS = ((shTest*2)+shVal)/3;
