function K = mal_dot_product(in, cfg)
%
% MALHEUR - Automatic Malware Analysis on Steroids
% Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
% Berlin Institute of Technology (TU Berlin).
% 
% Synopsis:
%     K = mal_dot_product(in, cfg);
%
% Arguments:
%     in:       Directory or archive containing reports
%     cfg:      Malheur configuration file 
%
% Returns:
%     K:	Matrix of dot products (kernel matrix)
%

% Call native function
K = malheur('dot_product', in, cfg);
