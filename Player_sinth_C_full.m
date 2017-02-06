clear, clc, clf
min_value=1;
max_value=255;
max_freq =3900;
stopband_freq=4000;
%%new_fs=8000;
new_fs=10000;
%%new_fs=8013;
file_in = 'sine.flac';
%%file_out_C = 'rom.h';
file_out_C = 'rom.h';

[x, Fs]=audioread(file_in);

% Aprox free mem: 32433 Bytes [pgm is 330 Bytes] (32256 if program size is 512 Bytes)
% @ Fs=10KHz, duration avaible is 3.243 s (3.225 s)
% @ Fs=8Khz, duration avaible is 4.05 s (4.03 s)
% @ Fs=8,01282 [20e6/(64*39); Fosc, prescaler and count], duration avaible
% is 4.047 s (4.0255 s)	
% 

% Resampling
y=resample(x, new_fs, Fs);
soundsc(y, new_fs);

% Padding, offseting and casting to integer
min_y = min(y);
y_offset= y - min_y;
max_y = max (y_offset);
ynorm = y_offset/max_y;
ys= round(ynorm*(max_value-min_value))+min_value;

% Plotting
figure(2), axis tight
plot(ys);

disp('Size (KB):')
disp((size(ys,1)+2)/1024);
disp('Size (bytes):');
disp(size(ys,1)+2);

% string formatting C
curvaC='const unsigned char rom[] PROGMEM = { ';
i=0;
for k=1:length(ys)-1
    if (rem(i, 100)==99)
        curvaC = [curvaC, num2str(ys(k)), ',\n\r'];    
    else
        curvaC = [curvaC, num2str(ys(k)), ', '];
    end
    i=1+1;
end
curvaC=[curvaC, num2str(ys(end)), ', 0, 0};'];

% Data storaging C
fileID = fopen(file_out_C,'w');
fprintf(fileID, curvaC);
fclose(fileID);