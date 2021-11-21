% % % data = audioread('../LeafsNewSong.mp3', 'native');
% % % min_data = min(data(:));
% % % max_data = max(data(:));
% % % step = (max_data-min_data) / 100;
% % % data_range = min_data : step : max_data;
% % % hist(data(:), data_range)
% % 
% % [y, Fs] = audioread('../LeafsNewSong.mp3');
% % histogram(y, 'FaceColor', 'blue');
% % grid on;
% 
% A = zeros(16);
% A(:)=a;
% mat = A; 
% imagesc(mat); 
% colormap(flipud(gray)); 

a = imread('newmain.png');
[counts,binLocations] = imhist(a);
imhist(a)

w = (counts~=0)*1;
% tmp = zeros(16);
% tmp(:) = w;
% tmp = tmp';
% w = tmp(:);


bs = '';
for i=1:256
    bs = [bs,num2str(w(i))];
end
bs

imagesc(w);
colormap(flipud(gray))