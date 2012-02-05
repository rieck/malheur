function [dist, dx, dy, t1, t2] = mal_distance(ix, iy, cfg)
%
% MALHEUR - Automatic Analysis of Malware Behavior
% Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)
% University of Goettingen, Berlin Institute of Technology
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
