%%%%%%%%%%
 % @file distortion.m
 % @author Parthiv Krishna
 % @brief Script to generate, test, and visualize distortion array
 % 
 % Creates a distortion array with a given function mapping and
 % outputs the result to distortion_array.h.
 % 
 %%%%%%%%%

filename = "sandman";

% Waveshaping distortion

distortion_arr_len = (2^9) + 1; % generate 513-long array
distortion_in = linspace(-1, 1, distortion_arr_len);

% Choose function for waveshape
distortion_out = sign(distortion_in) .* abs(distortion_in).^(1/10);
% distortion_out = tanh(pi * distortion_in);

% q15
distortion_out = round(distortion_out * (2^15));
distortion_out(distortion_out > (2^15 - 1)) = 2^15 - 1;
distortion_out(distortion_out < (-2^15)) = -2^15;

% Plot distortion curve
figure(1);
plot(distortion_in, distortion_out/(2^15));
xlabel("Input level");
ylabel("Output level");
xline(0);
yline(0);
title("Distortion Curve");

% Plot distorted sine
t_sin = (1:20000)/1000;
y_sin = sin(t_sin);

idx_sin = int32((y_sin + 1) * distortion_arr_len / 2) + 1;
idx_sin(idx_sin > distortion_arr_len) = distortion_arr_len;
y_sin_dist = distortion_out(idx_sin) / (2^15);

figure(2);
hold on;
plot(t_sin, y_sin);
plot(t_sin, y_sin_dist);
legend("Input", "Output");
title("Sample distorted sine wave");
hold off;

% Apply distortion to sample audio
[y, fs] = audioread(filename + ".wav");
y = y / max(abs(y));
t = (1:size(y)) / fs;

idx = int32((y + 1) * distortion_arr_len / 2) + 1;
idx(idx > distortion_arr_len) = distortion_arr_len;
y_dist = distortion_out(idx)/(2^15);

audiowrite(filename + "_dist.wav", y_dist, fs);

% Generate distortion_array.h
fileID = fopen("distortion_array.h", "w+");

fprintf(fileID, '// autogenerated by Matlab\n');
fprintf(fileID, '// see matlab/distortion/distortion.m\n');

fprintf(fileID, '#ifndef DISTORTION_ARRAY_H\n');
fprintf(fileID, '#define DISTORTION_ARRAY_H\n\n');

fprintf(fileID, '#define DISTORTION_ARR_LEN %6d\n', distortion_arr_len);
fprintf(fileID, 'int16_t DISTORTION_ARR[DISTORTION_ARR_LEN] = {\n');

repeat_str = '    %6d, %6d, %6d, %6d, %6d, %6d, %6d, %6d,\n';
num_repeats = int32(numel(distortion_out)/8);
fmt = [repmat(repeat_str, 1, num_repeats)];
fprintf(fileID, fmt, distortion_out);

repeat_str2 = '%6d, ';
num_remaining = numel(distortion_out) - num_repeats*8 - 1;
fmt2 = ['    ' repmat(repeat_str2, 1, num_remaining) '\n};\n'];
fprintf(fileID, fmt2, distortion_out((num_repeats * 8 + 1 + 1):end));
fprintf(fileID, '\n#endif // DISTORTION_ARRAY_H\n');

fclose(fileID);