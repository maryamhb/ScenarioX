 %% Scrambled Message Analysis
% Scenario X: Real-time Audio Descrambler
% Maryam H.

clear
[orig, FS] = audioread('original.wav');
[scr, FS] = audioread('scrambled.wav');
n = length(scr);
T = (1/44100)*n; 
t = linspace(0,T,n); %time domain
figure
plot(t,scr), grid, title('Signal in time domain'), xlabel('t')
hold on
plot(t,orig), legend('scrambled','original')


f = linspace(0,FS,n); %frequency domain
origf = abs(fft(orig));
scrf = abs(fft(scr));
figure
plot(f,origf), grid, title('Signal in freq domain'), xlabel('f')
hold on
plot(f,scrf), legend('original','scrambled')


% Butterworth band-stop filter
h  = fdesign.bandstop('N,F3dB1,F3dB2', 4, 7900, 8100, FS);
bandstop = design(h, 'butter');

% Butterworth low-pass 5kHz
h  = fdesign.lowpass('N,F3dB', 4, 5000, FS);
lowpass = design(h, 'butter');

scrBSd = filter(bandstop, scr); %remove 8Hz freq

sine = sin(2*pi*7000*t).'; %modulate onto sine wave at 7kHz
scrMod = scrBSd .* sine;

unscr = filter(lowpass, scrMod); %remove high freq. (will be done in analog filter)

% Unscrambled freq domain
unscrf = fft(unscr,n);
figure
plot(f,fftshift(abs(unscrf))), grid, title('unscrambled signal')

sound(unscr, FS) %unscrambled message


