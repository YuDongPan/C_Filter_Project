% 清空命令行
clc; 
clear; 
close all;

%% 使用filtfilt进行带通滤波
% 设定参数
Fs = 250;                      % 采样率 (Hz)
low_cutoff_bandpass = 4;       % 带通滤波器的低频截止 (Hz)
high_cutoff_bandpass = 40;     % 带通滤波器的高频截止 (Hz)
order = 4;                     % 滤波器阶数

% 归一化截止频率
low_normalized_bandpass = low_cutoff_bandpass / (Fs / 2);
high_normalized_bandpass = high_cutoff_bandpass / (Fs / 2);

% 设计4阶巴特沃斯带通滤波器
[b_bandpass, a_bandpass] = butter(order, [low_normalized_bandpass high_normalized_bandpass], 'bandpass');
disp('b_bandpass:');
disp(b_bandpass);
disp('a_bandpass:');
disp(a_bandpass);

% 读取data.csv文件中的第二列数据（去除索引列）
% 关键修改：使用readmatrix读取CSV文件，然后提取第二列
csv_data = readmatrix('data.csv');  % 读取整个CSV文件为数值矩阵
input_signal = csv_data(218:717, 3);      % 提取第二列所有行的数据

% 数据有效性检查（可选但推荐）
if isempty(input_signal)
    error('读取的数据为空，请检查data.csv文件路径和格式');
end

% 应用带通滤波器
filtered_signal_bandpass = filtfilt(b_bandpass, a_bandpass, input_signal);

%% 将滤波结果存入 .txt 文件
filename = 'matlab_output_signal.txt';
writematrix(filtered_signal_bandpass, filename, 'Delimiter', 'tab');  % 使用制表符分隔

filename_python = '../python_filter/output_signal/matlab_output_signal.txt';
writematrix(filtered_signal_bandpass, filename_python, 'Delimiter', 'tab');  % 使用制表符分隔

% 显示保存成功的消息
disp(['Filtered signal saved to ', filename]);
disp(['Filtered signal saved to ', filename_python]);