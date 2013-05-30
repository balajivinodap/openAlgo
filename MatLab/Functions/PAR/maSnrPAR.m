function shMETS = maSnrPAR(x,data,scaling,cost,bigPoint)
% define ma+rsi to accept vectorized inputs and return only sharpe ratio
%% 
% marsiMETS(price,N,M,typeMA,Mrsi,thresh,typeRSI,scaling,cost,bigPoint)
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

%% Progress Bar
try % Initialization
      ppm = ParforProgressStarter2('Parametric Sweep: maSnrPAR', row, 0.1);
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
      end %if
end %try


%% Parallel iterations
parfor ii = 1:row
    if x(ii,1) > x(ii,2)
        shTest(ii) = NaN;
        shVal(ii) = NaN;
    else
        [~,~,shTest(ii)] =	maSnr(vBarsTest,x(ii,1),x(ii,2),x(ii,3),x(ii,4),x(ii,5),...
                                                scaling,cost,bigPoint); 
        [~,~,shVal(ii)] =	maSnr(vBarsVal,x(ii,1),x(ii,2),x(ii,3),x(ii,4),x(ii,5),...
                                                scaling,cost,bigPoint);  %#ok<PFBNS>
    end
    ppm.increment(ii); %#ok<PFBNS>
end

%% Destroy progress bar
  try % use try / catch here, since delete(struct) will raise an error.
      delete(ppm);
  catch me %#ok<NASGU>
  end

%% Aggregate sharpe ratios
shMETS = ((shTest*2)+shVal)/3;
