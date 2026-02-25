#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#define EPS 0.000001

// IIR型数字滤波器函数
void digitalFilter(const double* inputSignal, double* outputSignal, int inputLength, double* filterCoefficientsA, double* filterCoefficientsB, 
                   int filterLength, double* initialConditions) 
	{
	    double normalizationFactor;
	    int i, j;
	
	    // 判断是否需要归一化
	    if ((filterCoefficientsA[0] - 1.0 > EPS) || (filterCoefficientsA[0] - 1.0 < -EPS)) {
	        normalizationFactor = filterCoefficientsA[0];
	        for (i = 0; i < filterLength; i++) {
	            filterCoefficientsB[i] /= normalizationFactor;
	            filterCoefficientsA[i] /= normalizationFactor;
	        }
	    }
	
	    // 将输出信号初始化为零
	    for (i = 0; i < inputLength; i++) {
	        outputSignal[i] = 0.0;
	    }
	    
	    // 递归式差分方程
	    filterCoefficientsA[0] = 0.0;
    	for (i = 0; i < inputLength; i++) {
	        for (j = 0; i >= j && j < filterLength; j++) {
	            // 计算滤波器输出
	            outputSignal[i] += (filterCoefficientsB[j] * inputSignal[i - j] - filterCoefficientsA[j] * outputSignal[i - j]);
	        }
	        // 如果有初始条件且当前位置小于滤波器长度减一，加上初始条件
	        if (initialConditions && i < filterLength - 1) {
	            outputSignal[i] += initialConditions[i];
	        }
    }
    filterCoefficientsA[0] = 1.0;
}



// 矩阵乘法函数
void matrixMultiplication(double* matrixA, double* matrixB, double* resultMatrix, int numRowsA, int numColsA, int numColsB) {
    int rowIndex, colIndex, commonIndex;
    int elementIndex;
    for (rowIndex = 0; rowIndex < numRowsA; rowIndex++) {
        for (colIndex = 0; colIndex < numColsB; colIndex++) {
            elementIndex = rowIndex * numColsB + colIndex;
            resultMatrix[elementIndex] = 0.0;
            // 遍历两个矩阵共同的维度，即矩阵A的列与矩阵B的行
            for (commonIndex = 0; commonIndex < numColsA; commonIndex++) {
                // 计算矩阵乘法
                resultMatrix[elementIndex] += matrixA[rowIndex * numColsA + commonIndex] * matrixB[commonIndex * numColsB + colIndex];
            }
        }
    }
    return;
}


//求逆矩阵，当返回值为0时成功求解逆矩阵
int inverseMatrix(double* matrix, int matrixSize) {
    // 用于存储行和列的交换索引
    int* rowIndices, *columnIndices;
    int i, j, k, l, u, v;
    double d, p;

    // 分配内存
    rowIndices = (int*)malloc(matrixSize * sizeof(int));
    columnIndices = (int*)malloc(matrixSize * sizeof(int));

    for (k = 0; k < matrixSize; k++) {
        // 寻找最大元素
        d = 0.0;
        for (i = k; i < matrixSize; i++) {
            for (j = k; j < matrixSize; j++) {
                l = i * matrixSize + j;
                p = fabs(matrix[l]);
                if (p > d) {
                    d = p;
                    rowIndices[k] = i;
                    columnIndices[k] = j;
                }
            }
        }

        // 判断是否可逆
        if (d + 1.0 == 1.0) {
            free(rowIndices);
            free(columnIndices);
            printf("err**not invn");
            return 0;
        }

        // 交换行
        if (rowIndices[k]!= k) {
            for (j = 0; j < matrixSize; j++) {
                u = k * matrixSize + j;
                v = rowIndices[k] * matrixSize + j;
                p = matrix[u];
                matrix[u] = matrix[v];
                matrix[v] = p;
            }
        }

        // 交换列
        if (columnIndices[k]!= k) {
            for (i = 0; i < matrixSize; i++) {
                u = i * matrixSize + k;
                v = i * matrixSize + columnIndices[k];
                p = matrix[u];
                matrix[u] = matrix[v];
                matrix[v] = p;
            }
        }

        // 计算对角线元素的倒数
        l = k * matrixSize + k;
        matrix[l] = 1.0 / matrix[l];

        // 更新非对角线上的元素
        for (j = 0; j < matrixSize; j++) {
            if (j!= k) {
                u = k * matrixSize + j;
                matrix[u] = matrix[u] * matrix[l];
            }
        }

        for (i = 0; i < matrixSize; i++) {
            if (i!= k) {
                for (j = 0; j < matrixSize; j++) {
                    if (j!= k) {
                        u = i * matrixSize + j;
                        matrix[u] = matrix[u] - matrix[i * matrixSize + k] * matrix[k * matrixSize + j];
                    }
                }
            }
        }

        for (i = 0; i < matrixSize; i++) {
            if (i!= k) {
                u = i * matrixSize + k;
                matrix[u] = -matrix[u] * matrix[l];
            }
        }
    }

    // 恢复交换的列和行
    for (k = matrixSize - 1; k >= 0; k--) {
        if (columnIndices[k]!= k) {
            for (j = 0; j < matrixSize; j++) {
                u = k * matrixSize + j;
                v = columnIndices[k] * matrixSize + j;
                p = matrix[u];
                matrix[u] = matrix[v];
                matrix[v] = p;
            }
        }

        if (rowIndices[k]!= k) {
            for (i = 0; i < matrixSize; i++) {
                u = i * matrixSize + k;
                v = i * matrixSize + rowIndices[k];
                p = matrix[u];
                matrix[u] = matrix[v];
                matrix[v] = p;
            }
        }
    }

    // 释放内存
    free(rowIndices);
    free(columnIndices);
    return 0;
}


// filtfilt 函数：对输入信号进行零相位滤波
int filtfilt(double* input_signal, double* output_signal, int signal_length, double* filter_a, double* filter_b, int filter_order)
{
    int reflection_length;
    int extended_length;  // 扩展序列的长度，包括边缘反射部分
    int i;
    double *extended_signal, *filtered_signal, *p, *t, *end;
    double *workspace_matrix, *transient_vector, *initial_conditions;
    double first_sample, last_sample;

    // 根据滤波器阶数计算反射长度
    reflection_length = filter_order - 1;

    // 检查输入信号长度是否过短或滤波器参数是否无效
    if(signal_length <= 3 * reflection_length || filter_order < 2) return -1;

    // 定义扩展长度以容纳边缘反射，用于减小边缘效应
    extended_length = 6 * reflection_length + signal_length;
    extended_signal = (double *)malloc(extended_length * sizeof(double));
    filtered_signal = (double *)malloc(extended_length * sizeof(double));

    workspace_matrix = (double*)malloc(sizeof(double) * reflection_length * reflection_length);
    transient_vector = (double*)malloc(sizeof(double) * reflection_length);
    initial_conditions = (double*)malloc(sizeof(double) * reflection_length);

    // 检查内存分配是否成功
    if( !extended_signal || !filtered_signal || !workspace_matrix || !transient_vector || !initial_conditions ){
        free(extended_signal);
        free(filtered_signal);
        free(workspace_matrix);
        free(transient_vector);
        free(initial_conditions);
        return 1;
    }

    // 在信号开始处反射以减小边缘效应
    first_sample = input_signal[0];
    for(p = input_signal + 3 * reflection_length, t = extended_signal; p > input_signal; --p, ++t) 
        *t = 2.0 * first_sample - *p;

    // 复制信号的主体部分
    for(end = input_signal + signal_length; p < end; ++p, ++t) 
        *t = *p;

    // 在信号结束处反射以减小边缘效应
    last_sample = input_signal[signal_length - 1];
    for(end = extended_signal + extended_length, p -= 2; t < end; --p, ++t) 
        *t = 2.0 * last_sample - *p;

    // 初始化工作矩阵，用于求解初始条件
    memset(workspace_matrix, 0, sizeof(double) * reflection_length * reflection_length);

    workspace_matrix[0] = 1.0 + filter_a[1];
    for(i = 1; i < reflection_length; i++) {
        workspace_matrix[i * reflection_length] = filter_a[i + 1];
        workspace_matrix[i * reflection_length + i] = 1.0;
        workspace_matrix[(i - 1) * reflection_length + i] = -1.0;
    }

    // 计算用于初始条件的瞬态向量
    for(i = 0; i < reflection_length; i++) {
        transient_vector[i] = filter_b[i + 1] - filter_a[i + 1] * filter_b[0];
    }

    // 如果矩阵求逆成功，计算初始条件
    if(inverseMatrix(workspace_matrix, reflection_length)) {
        free(initial_conditions);
        initial_conditions = NULL;
    } else {
        matrixMultiplication(workspace_matrix, transient_vector, initial_conditions, reflection_length, reflection_length, 1);
    }

    free(workspace_matrix);
    free(transient_vector);

    // 对扩展信号执行正向滤波，将结果保存在 filtered_signal 中
    first_sample = extended_signal[0];
    if(initial_conditions) {
        for(p = initial_conditions, end = initial_conditions + reflection_length; p < end; ) 
            *(p++) *= first_sample;
    }
    digitalFilter(extended_signal, filtered_signal, extended_length, filter_a, filter_b, filter_order, initial_conditions);

    // 反转滤波后的信号以进行逆向滤波
    for(p = filtered_signal, end = filtered_signal + extended_length - 1; p < end; p++, end--) {
        double temp = *p;
        *p = *end;
        *end = temp;
    }

    // 对反转后的信号执行逆向滤波
    last_sample = (*filtered_signal) / first_sample;
    if(initial_conditions) {
        for(p = initial_conditions, end = initial_conditions + reflection_length; p < end; ) 
            *(p++) *= last_sample;
    }
    digitalFilter(filtered_signal, extended_signal, extended_length, filter_a, filter_b, filter_order, initial_conditions);

    // 将滤波后的信号反转回原顺序，并存储在 output_signal 中
    end = output_signal + signal_length;
    p = extended_signal + 3 * reflection_length + signal_length - 1;
    while(output_signal < end) {
        *output_signal++ = *p--;
    }

    // 清理分配的内存
    free(initial_conditions);
    free(extended_signal);
    free(filtered_signal);

    return 0;
}

void adjust_precision(double arr[], int size, int precision) {
    double factor = pow(10.0, precision);
    for (int i = 0; i < size; i++) {
        arr[i] = round(arr[i] * factor) / factor;
    }
}


int main()
{
	// 例子带通滤波器系数（4阶Butterworth带通滤波器，采样率250Hz， 4-40Hz）
    double b_bandpass[] = {0.016255176549541173, 0.0, -0.06502070619816469, 0.0, 
	                       0.09753105929724704, 0.0, -0.06502070619816469, 0.0, 0.016255176549541173};
    double a_bandpass[] = {1.0, -5.360703285534569, 12.74230451194405, -17.736407401314917, 
	                       15.949378473690096, -9.51256976782561, 3.6615254094399727, -0.8286427351763588, 0.08515392332605591};

	
	int precision = 8; // 设置有效小数位数

    adjust_precision(b_bandpass, sizeof(b_bandpass) / sizeof(b_bandpass[0]), precision);
    adjust_precision(a_bandpass, sizeof(a_bandpass) / sizeof(a_bandpass[0]), precision);
	                       
	
	// 定义测试的待滤波数据, 1s, 250个采样点
	double x[] = {7994.9079,  7978.61475, 7976.6703,  7992.0918,  8006.7534,  8000.07075,  7987.5771,  7984.0458,  7999.9143,  8014.4418,
                  8004.0714,  7983.04005, 7980.8721,  7994.70675, 8009.0778,  7999.93665,  7981.4979,  7978.3242,  7994.66205, 8007.84855, 
				  7997.2323,  7981.6767,  7984.2693,  7999.9143,  8012.7879,  8001.18825,  7982.8389,  7984.06815, 7998.19335, 8004.8313,
                  7994.7291,  7978.9053,  7976.24565, 7989.52155, 7999.75785, 7988.31465,  7978.12305, 7977.2514,  7993.49985, 8009.10015, 
				  7997.74635, 7978.57005, 7973.4519,  7987.24185, 8006.28405, 7999.13205,  7983.88935, 7980.2463,  7992.38235, 8005.8147,
				  7998.7074,  7981.43085, 7977.2514,  7991.8236,  8008.07205, 7997.12055,  7980.0675,  7974.10005, 7985.36445, 7998.37215, 
				  7990.0803,  7974.8376,  7971.55215, 7982.45895, 7999.8249,  7995.2655,   7977.81015, 7978.7712,  7995.3549,  8004.9207, 
				  7990.8849,  7975.21755, 7977.00555, 7994.03625, 8008.0497,  7997.1429,   7979.86635, 7978.25715, 7996.40535, 8008.69785, 
				  7998.23805, 7983.26355, 7982.2131,  7996.89705, 8008.74255, 7999.0203,   7981.20735, 7978.7265,  7996.13715, 8013.3243, 
				  8005.39005, 7987.51005, 7984.91745, 7999.2438,  8010.7764,  8000.2272,   7989.11925, 7989.2757,  7999.44495, 8006.46285, 
				  7996.36065, 7982.8836,  7979.37465, 7998.8415,  8017.01205, 8006.59695,  7990.5273,  7983.5541,  7999.71315, 8015.2017, 
				  8008.7202,  7992.96345, 7991.48835, 8006.9769,  8018.1966,  8004.89835,  7983.2859,  7979.77695, 8002.1046,  8017.86135, 
				  8005.6806,  7985.9232,  7985.40915, 8005.47945, 8017.0791,  8003.8926,   7986.34785, 7990.90725, 8005.5465,  8017.99545, 
				  8011.2681,  7991.1531,  7988.09115, 8005.3677,  8016.498,   8003.95965,  7989.0522,  7995.6231,  8011.7598,  8022.86775, 
				  8014.6653,  7997.3664,  7995.3996,  8011.02225, 8022.6666,  8008.69785,  7988.80635, 7987.9794,  7998.86385, 8011.3128, 
				  8001.5682,  7987.3983,  7986.07965, 8000.07075, 8011.4022,  8003.8479,   7993.05285, 7994.2821,  8009.54715, 8023.203, 
				  8014.1736,  7996.8747,  7992.69525, 8005.5912,  8019.3588,  8009.1225,   7998.0369,  7999.6014,  8014.06185, 8020.4316, 
				  8011.37985, 8000.3166,  8000.8083,  8017.68255, 8027.226,   8007.98265,  7995.33255, 7997.52285, 8016.34155, 8029.2375, 
				  8016.52035, 7997.2323,  7993.7457,  8009.81535, 8020.72215, 8006.73105,  7994.84085, 7995.64545, 8007.0216,  8012.1621, 
				  7999.6908,  7987.28655, 7987.19715, 8001.6576,  8012.45265, 7997.96985,  7986.83955, 7987.51005, 8001.4341,  8010.03885,
				  7993.2093,  7978.5924,  7979.2629,  7995.8019,  8003.3562,  7993.72335,  7980.00045, 7979.91105, 7998.68505, 8014.1289, 
				  8000.0931,  7983.57645, 7982.05665, 7996.36065, 8008.8543,  8000.51775,  7983.93405, 7982.50365, 7997.47815, 8011.0446, 
				  8000.1378,  7981.0956,  7980.5592,  7998.1263,  8010.97755, 8001.4341,   7989.3204,  7989.07455, 7998.7074,  8007.53565, 
				  7995.91365, 7984.7163,  7984.91745, 8004.9207,  8015.13465, 8003.51265,  7987.2642,  7985.2080,  8007.4239,  8019.3588, 
				  8005.83705, 7994.61735, 7995.46665, 8006.5299,  8014.50885, 8001.41175,  7984.51515, 7983.5541,  8001.6576,  8009.9718};
	
    
    // 使用自定义的滤波器对250个采样点数据进行数字滤波
    int x_length = 250;
    double y[250];
    
    int coeff_length = sizeof(b_bandpass) / sizeof(b_bandpass[0]);

    // 应用filtfilt进行带通滤波
    filtfilt(x, y, x_length, a_bandpass, b_bandpass, coeff_length);
    
    // 打开文件进行写入
    FILE *output_file = fopen("c_output_signal_blog.txt", "w");
    
    if (output_file == NULL) {
        printf("Failed to open file for writing!\n");
        return 1;
    }
    
    // 输出结果
    printf("Output result after bandpass filtering in C language:\n");
    for (int i = 0; i < x_length; i++) {
        printf("%f, ", y[i]);
        // 打开文件进行写入
        fprintf(output_file, "%f\n", y[i]);
    }
    
    return 0;
}
