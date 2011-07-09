
name = '6_samples';
rfile = sprintf('/home/mlsec/malheur/%s.zip', name);
cfile = '/home/mlsec/malheur/malheur.cfg';
runs = 6;

[D, dx, dy] = mal_distance(rfile, rfile, cfile);

[files idx] = sort(dx.sources);
D = D(idx,idx);

colormap(bone);
imagesc(D);
colorbar;

xlabel('Runs'); ylabel('Runs');
set(gcf, 'Position', [200 200 300 300]);
print('-depsc', sprintf('%s-mat.eps', name));

n = size(D,1) / runs;

I = repmat(1:n, runs, 1);
I = I(:);

clear m s
for i = 1:n
   idx = find(I == i);
   d = D(idx,idx);
   m(i) = mean(d(:));
   s(i) = std(d(:));
end

errorbar(1:n, m, s, 'o');
xlabel('Samples');
ylabel('Distance between runs');
set(gca, 'YLim', [0 sqrt(2)]); grid on;
set(gcf, 'Position', [200 200 300 300]);
print('-depsc', sprintf('%s-std.eps', name));
