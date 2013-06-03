function sh = ma2inputsPAR(x,data,scaling,cost,range,bigPoint,showBar)
% define leadlag to accept vectorized inputs and return only sharpe ratio
%
% Author:           Mark Tompkins
% Revision:			4902.23582

[row,col] = size(x);
sh  = zeros(row,1);
%t   = length(data);
x = round(x);

if ~exist('scaling','var')
    scaling = 1;
end
if ~exist('cost','var')
    cost = 0;
end
if ~exist('bigPoint','var')
    bigPoint = 1;
end;

if ~exist('showBar','var')
    showBar = 0;
end;

% We can't estimate in a parfor.  Variables are temporary and destroyed.
% We'll perform the first 10 iterations on one CPU then divide by the number
% of active cores

% To prevent an ambigous error in parfor caused by an uninitialed variable
% we initialize an empty ppm.

ppm = [];

if showBar
% Progress bar initialization
  try 
      ppm = ParforProgressStarter2('Moving Average 2 Input Parameter Sweep', row, 0.1);
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
end %if

ii=1; smpl=1; contRow = 1;

if (row > 10)
    smplToc = zeros(10,1); 
    while smpl < 11 && ii <= row
        if x(ii,1) >= x(ii,2)                  % ensures we don't dupe the optimizations.  Checks Fast > Slow
            sh(ii) = NaN;
        else
            if col > 2
                type = x(i,3);
            else
                type = 0;
            end;
            if col > 3
                error('No longer handling vBars at the function level.  Address the passed in ''range''');
            end; %if
            
            % We are not calling MEX because we want user feedback like progress bar.
            % Use movAvg2inputsPARMEX to run without feedback
            [~,~,sh(ii)] = ma2inputsSIG(data,x(ii,1),x(ii,2),type,scaling,cost,bigPoint);
            
            smplToc(smpl) = toc; 
            formatSpec = '          Iteration %d\n';
            fprintf(formatSpec,smpl)
            smpl = smpl + 1;
        end; %if
        if showBar
            ppm.increment();
        end %if
    ii = ii + 1;
    end; %while
    contRow=i;
end; %if

% 10 iterations sampled.  Present feedback. First variable '1' indicates parfor usage in calculation.
smplToc(2:end,2)=diff(smplToc);
top = sort(smplToc(:,2),'descend'); 
topS = (sum(top(1:5))*2); 

optInfo(1,range,topS,contRow-1) 

if contRow <= row
    % run parallel iterations
    parfor ii = contRow:row

	if col > 2
        type = x(ii,3);
    else
        type = 0;
	end;
	
	if col > 3
        error('No longer handling vBars at the function level.  Address the passed in ''range''');
	end; %if
    
    if x(ii,1) >= x(ii,2)
        sh(ii) = NaN;
    else
        % We are not calling MEX because we want user feedback like progress bar.
        % Use movAvg2inputsPARMEX to run without feedback
        [~,~,sh(ii)] = ma2inputsSIG(data,x(ii,1),x(ii,2),type,scaling,cost,bigPoint); %#ok<PFBNS>
    end;

    if showBar
        ppm.increment(); %#ok<PFBNS> % update progressbar
    end; %if
    
    end; %parfor
end; %if

if showBar
  try % use try / catch here, since delete(struct) will raise an error.
      delete(ppm);
  catch me %#ok<NASGU>
  end;
end %if
