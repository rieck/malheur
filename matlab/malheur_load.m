function [rep,list] = malheur_load(in, level);  
%
% MALHEUR - Automatic Malware Analysis on Steroids
% Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
% Berlin Institute of Technology (TU Berlin).
% 
% Synopsis:
%     [rep,list] = malheur_load(in, level);
%
% Arguments:
%     in:       cell array of file name or directory name
%     level:    MIST instruction level
%
% Returns:
%     rep:      Cell array of MIST reports 
%     list:     Cell array of file names
%

% Check for input
if iscellstr(in) 
   list = in
elseif isstr(in)
   % Generate list of files
   d = dir(in);
   list = {};
   for i = 1:length(d)
      if d(i).isdir
         continue;
      end
      list{end+1} = sprintf('%s/%s', in, d(i).name);
   end
else
   error('first arguments needs to be cell array or string');
end

% Load data
rep = malheur('load_mist', list, level);
