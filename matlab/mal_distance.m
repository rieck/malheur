function [dist, dx, dy, t1, t2] = mal_distance(ix, iy, cfg)
%
% MALHEUR - Automatic Malware Analysis on Steroids
% Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
% Berlin Institute of Technology (TU Berlin).
% 
% Synopsis:
%     [dist, dx, dy] = mal_distance(ix, iy, cfg);
%
% Arguments:
%     ix:       Directory or archive containing reports (n)
%     iy:       Directory or archive containing reports (m)
%     cfg:      Malheur configuration file 
%
% Returns:
%     dist:     Distance matrix (n x m)
%     dx:       Data structure   
%               .labels    Label vector (1 x n)
%               .names     Label to class names structure
%               .sources   Sources of feature vectors (1 x n)
%     dy:       [...]
%

% Call native function
[dist, dx, dy, t1, t2] = malheur('distance', ix, iy, cfg);
