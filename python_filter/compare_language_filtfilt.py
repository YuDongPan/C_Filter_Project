# Designer:Ethan Pan
# Coder:God's hands
# Time:2024/11/12 11:43
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import butter, filtfilt

import matplotlib
matplotlib.use('TkAgg')  # Windows/Linux推荐

# 设计4阶Butterworth带通滤波器和带阻滤波器
fs = 250   # 采样率
nyq = 0.5 * fs  # 归一化频率
order = 4  # 阶数
low_bandpass, high_bandpass = 4, 40
Wn_bandpass = [low_bandpass / nyq, high_bandpass / nyq]
b_bandpass, a_bandpass = butter(order, Wn_bandpass, btype='bandpass', analog=False)

print("b_bandpass:", b_bandpass.tolist())
print("a_bandpass:", a_bandpass.tolist())

f0 = 50.0   # 工频干扰的频率（Hz）
bandwidth = 10  # 带阻滤波器的带宽（Hz）
low_bandstop = (f0 - bandwidth / 2)
high_bandstop = (f0 + bandwidth / 2)
Wn_bandstop = [low_bandstop / nyq, high_bandstop / nyq]
b_bandstop, a_bandstop = butter(order, Wn_bandstop, btype='bandstop', analog=False)
print("b_bandstop:", b_bandstop.tolist())
print("a_bandstop:", a_bandstop.tolist())

input_signal = pd.read_csv('data/data.csv', index_col=0)
input_signal = input_signal.values
input_signal = input_signal[216:715, 1].tolist()

print("input_signal:", input_signal)

python_output_signal = filtfilt(b_bandpass, a_bandpass, input_signal, padlen=3 * (max(len(b_bandpass), len(a_bandpass)) - 1))

# Python输出结果
print("Python滤波后输出结果：")
print([float(f'{output:.6f}') for output in python_output_signal])

# C输出结果
c_output_signal = []
with open("output_signal/c_output_signal.txt", "r") as file:
    line = file.readline()
    while line:
        c_output_signal.append(float(line))
        line = file.readline()

print("C滤波后输出结果：")
print([float(f'{output:.6f}') for output in c_output_signal])

# matlab输出结果
matlab_output_signal = np.loadtxt("output_signal/matlab_output_signal.txt")


print("matlab滤波后输出结果：")
print([float(f'{output:.6f}') for output in matlab_output_signal])


# ==============================
# 创建保存目录
# ==============================
save_dir = "../figures"
os.makedirs(save_dir, exist_ok=True)

# ==============================
# 三种语言滤波结果时域对比图
# ==============================

ws = 1.0
t = np.arange(0, ws, 1 / fs)

plt.figure(figsize=(12, 6), dpi=240)
plt.plot(t, python_output_signal[:int(ws * fs)], label="Python output")
plt.plot(t, c_output_signal[:int(ws * fs)], label="C output")
plt.plot(t, matlab_output_signal[:int(ws * fs)], label="MATLAB output")
plt.legend()
plt.title("Time Domain Comparison Between Filter Outputs of Different Programming Languages")
plt.tight_layout()
plt.savefig(os.path.join(save_dir, "01_time_domain_comparison.png"))
plt.close()


# ==============================
# 差值对比图
# ==============================

min_len = min(len(python_output_signal), len(c_output_signal), len(matlab_output_signal))

python_arr = np.array(python_output_signal[:min_len])
c_arr = np.array(c_output_signal[:min_len])
matlab_arr = np.array(matlab_output_signal[:min_len])

diff_py_c = python_arr - c_arr
diff_py_mat = python_arr - matlab_arr
diff_c_mat = c_arr - matlab_arr

plt.figure(figsize=(12, 6), dpi=240)
plt.plot(t[:int(ws * fs)], diff_py_c[:int(ws * fs)], label="Python - C")
plt.plot(t[:int(ws * fs)], diff_py_mat[:int(ws * fs)], label="Python - MATLAB")
plt.plot(t[:int(ws * fs)], diff_c_mat[:int(ws * fs)], label="C - MATLAB")
plt.title("Difference Between Filter Outputs of Different Programming Languages")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(os.path.join(save_dir, "02_difference_comparison.png"))
plt.close()


# ==============================
# 误差统计柱状图
# ==============================

def calc_metrics(x, y):
    mae = np.mean(np.abs(x - y))
    rmse = np.sqrt(np.mean((x - y) ** 2))
    return mae, rmse

mae_py_c, rmse_py_c = calc_metrics(python_arr, c_arr)
mae_py_mat, rmse_py_mat = calc_metrics(python_arr, matlab_arr)
mae_c_mat, rmse_c_mat = calc_metrics(c_arr, matlab_arr)

labels = ['Py-C', 'Py-MATLAB', 'C-MATLAB']
mae_values = [mae_py_c, mae_py_mat, mae_c_mat]
rmse_values = [rmse_py_c, rmse_py_mat, rmse_c_mat]

x = np.arange(len(labels))
width = 0.35

plt.figure(figsize=(10, 6), dpi=240)
plt.bar(x - width/2, mae_values, width, label='MAE')
plt.bar(x + width/2, rmse_values, width, label='RMSE')
plt.xticks(x, labels)
plt.title("Error Metrics Between Filter Outputs of Different Programming Languages")
plt.legend()
plt.tight_layout()
plt.savefig(os.path.join(save_dir, "03_error_metrics.png"))
plt.close()


# ==============================
# 频谱对比图
# ==============================

def plot_fft(signal, fs, label):
    N = len(signal)
    freqs = np.fft.rfftfreq(N, 1/fs)
    fft_values = np.abs(np.fft.rfft(signal))
    plt.plot(freqs, fft_values, label=label)

plt.figure(figsize=(12, 6), dpi=240)
plot_fft(python_arr, fs, "Python")
plot_fft(c_arr, fs, "C")
plot_fft(matlab_arr, fs, "MATLAB")

plt.xlim(0, 80)
plt.title("FFT Spectrum Comparison Between Filter Outputs of Different Programming Languages")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Amplitude")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(os.path.join(save_dir, "04_fft_comparison.png"))
plt.close()

print("所有图像已保存至 figures/ 文件夹")