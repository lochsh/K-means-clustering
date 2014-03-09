%Read data
d = csvread('input.csv');
N = length(d);

%Read assignments
r = csvread('outputAssignments.csv');
iter = length(r)/N;
r = reshape(r, N, iter) + 1; %need to add 1 because MATLAB indexes from 1, C indexes from 0

%Read centroids
cc_0 = csvread('initialCentroids.csv');
K = length(cc_0);
cc = csvread('outputCentroids.csv');
cc = [cc_0; cc(1:K*(iter-1),:)];

%Colours for plots
col = [0.25 0 1;  0 1 0.3;  1 0.1 1; 0 1 1; 0.7 0 1];


%Plot assignment each iteration
for i = 1:iter
    evol = figure;
    hold on;
    
    for k = 1:K
        for n = 1:N
            if r(n,i) == k
               scatter(d(n,1), d(n,2), '.', 'MarkerEdgeColor', col(k,:));
            end
        end

        scatter(cc(k + 5*(i-1), 1), cc(k + 5*(i-1), 2), 'MarkerFaceColor', col(k, :),'Linewidth', 2, 'MarkerEdgeColor', 'k');
    end
    
    titleStr = sprintf('Iteration %i', i-1);
    title(titleStr, 'fontsize', 16);
    
    pngStr = sprintf('iter%02i', i-1);
    saveas(evol, pngStr, 'png');
    
   close all;
end

%now, in Linux console, use ImageMagick to create .gif from .png files:
%
% convert -delay 50 *.png kmeans.gif
