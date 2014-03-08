
K = 5; %number of clusters
N = 10000; %number of data points

%Read data
d = csvread('input.csv');

%Read assignments
r = csvread('assignments.csv');
iter = length(r)/N;
r = reshape(r, N, iter) + 1; %need to add 1 because MATLAB indexes from 1, C indexes from 0

%Read centroids
cc_0 = csvread('initialCentroids.csv');
cc = csvread('centroids.csv');
cc = reshape(cc, 5*iter, 2);

filename = 'evolution.gif';

%Colours for plots
col = ['b', 'g', 'm', 'c', 'k'];
% 
% evol = figure(1);
% hold on;
% title('Iteration 0', 'fontsize', 16);
% 
% %Plot unassigned data
% for n = 1:N
%     scatter(d(n,1), d(n,2), '.b')
% end
% 
% %Plot initial centroids
% for k = 1:K
%     scatter(cc_0(k,1), cc_0(k, 2), 'or', 'Linewidth', 5);
% end

%Make gif
% frame = getframe(evol);
% im = frame2im(frame);
% [imind, cm] = rgb2ind(im, 256);
% imwrite(imind, cm, filename, 'gif', 'Loopcount', Inf, 'DelayTime', 3);
% close all;

%Plot assignment each iteration
for i = 2
    evol = figure(i+1);
    hold on;
    for k = 1:K
        for n = 1:N
            if r(n,i) == k
               scatter(d(n,1), d(n,2), col(k), '.');
            end
        end

        scatter(cc(k + 5*(i-1), 1), cc(k + 5*(i-1), 2), col(k), 'o', 'Linewidth', 9);
    end
    titleStr = sprintf('Iteration %i', i);
    title(titleStr, 'fontsize', 16);
    
    %Append gif with this iteration's plot
%     frame = getframe(evol);
%     im = frame2im(frame);
%     [imind, cm] = rgb2ind(im, 256);
%     imwrite(imind, cm, filename, 'gif', 'WriteMode', 'append', 'DelayTime', 3);
%     close all;
end
