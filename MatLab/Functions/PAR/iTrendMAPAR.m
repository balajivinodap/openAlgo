function sh = iTrendMaPAR(x,data,scaling,cost,bigPoint)
% iTrendMA wrapper
%
% Author: Mark Tompkins
%

row = size(x,1);
sh = zeros(row,1);

%% Progress Bar
try % Initialization
      ppm = ParforProgressStarter2('Parametric Sweep: marsiPARMETS', row, 0.1);
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


parfor ii = 1:row
    [~,~,sh(ii)] = iTrendMaSIG(data,x(ii,1),x(ii,2),scaling,cost,bigPoint);
    ppm.increment(ii); %#ok<PFBNS>
end

%% Destroy progress bar
  try % use try / catch here, since delete(struct) will raise an error.
      delete(ppm);
  catch me %#ok<NASGU>
  end
  
  