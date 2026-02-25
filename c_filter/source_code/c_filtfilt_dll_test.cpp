#include<stdio.h>
#include<windows.h>

typedef int (*Add)(int, int);
typedef void (*Butterworth_Bandpass)(double*, double*, int, double*, double*, int);
typedef void (*Butterworth_Bandstop)(double*, double*, int, double*, double*, int);



int main()
{
	// 例子带通滤波器系数（4阶Butterworth带通滤波器，采样率250， 4-40Hz）
    double b_bandpass[] = {0.016255176549541173, 0.0, -0.06502070619816469, 0.0, 0.09753105929724704, 0.0, -0.06502070619816469, 0.0, 0.016255176549541173};
    double a_bandpass[] = {1.0, -5.360703285534569, 12.74230451194405, -17.736407401314917, 15.949378473690096, -9.51256976782561, 3.6615254094399727, -0.8286427351763588, 0.08515392332605591};
	
	// 例子带通滤波器系数（4阶Butterworth带阻滤波器，设计50Hz陷波，带宽10Hz）
    double b_bandstop[] = {0.7193595528106432, -1.7924889256448098, 4.552374202572755, -6.073063344717725, 7.7743590926364154, -6.073063344717723, 4.552374202572753, -1.7924889256448087, 0.7193595528106429};
    double a_bandstop[] = {1.0, -2.287479226634381, 5.310674820821043, -6.518348047026466, 7.6666445252031945, -5.530109835375761, 3.82302905759094, -1.3951674316884646, 0.5174781997880401};
	
	
	// 读取csv文件
	double *x = (double *)malloc(500 * sizeof(double));
	memset(x, 0, 500 * sizeof(double));
	
	FILE *csv_file = fopen("data.csv", "r");
    if (csv_file == NULL) {
        perror("Unable to open file");
        return 1;
    }

    int MAX_LINE_LENGTH = 1024000;
	int START_INDEX = 218;
	int END_INDEX = 717;
	
    char line[MAX_LINE_LENGTH];
    int current_index = 1;  // 用于跟踪当前读取到的元素序号
    int target_index = 0;

    while (fgets(line, sizeof(line), csv_file)) {
        char *token;
        int column_index = 0;  // 用于跟踪当前列的序号
        token = strtok(line, ",");

        while (token != NULL) {
            column_index++;

            if (column_index == 3) {  // 只处理第1列
                if (current_index >= START_INDEX && current_index <= END_INDEX) {
                    // printf("Element %d: %s\n", current_index, token);
                    x[target_index++] = strtod(token, NULL);
                }
                current_index++;
                break;
            }

            token = strtok(NULL, ",");
        }

        // 如果当前索引超过了END_INDEX，终止循环
        if (current_index > END_INDEX) {
            break;
        }
    }
	
    fclose(csv_file);
    
    printf("原始数据为:");
    for (int i = 0; i < 500; i++) {
        printf("%f, ", x[i]);
		
    }
    printf("\n");
    
	
    // 使用自定义的滤波器对csv文件数据进行滤波
    int x_length = 500;
    double y1[500];
    double y2[500];
    
    int coeff_length = sizeof(b_bandpass) / sizeof(b_bandpass[0]);
    
    printf("coeff_length:%d\n", coeff_length);

	
	// 打开文件进行写入
    FILE *output_file = fopen("c_output_signal.txt", "w");
    FILE *output_file_python = fopen("../../python_filter/output_signal/c_output_signal.txt", "w");
    
    if (output_file == NULL) {
        printf("Failed to open file for writing!\n");
        return 1;
    }
    
	HINSTANCE hMod = LoadLibrary("C_Filter.dll"); // C_Filter.dll
    if (hMod != NULL) {
        Butterworth_Bandpass bandpass_filter = (Butterworth_Bandpass)GetProcAddress(hMod, "bandpass_filter"); // bandpass_filter
        Butterworth_Bandstop bandstop_filter = (Butterworth_Bandstop)GetProcAddress(hMod, "bandstop_filter"); // bandstop_filter
        // Add add = (Add)GetProcAddress(hMod, "add");
        
//        if (add != NULL){
//        	printf("%d\n", add(5, 100));
//		}
        
        if (bandpass_filter != NULL) {
		    // 应用filtfilt进行带通滤波
		    bandpass_filter(x, y1, x_length, a_bandpass, b_bandpass, coeff_length);  

        }
        
        if (bandstop_filter != NULL) {
            // 应用filtfilt进行带阻滤波(50Hz陷波)
			bandstop_filter(y1, y2, x_length, a_bandstop, b_bandstop, coeff_length); 
        }
        
    } 
	else 
	{
        printf("Failed to load DLL!");
    }
	
    // 输出结果
    printf("C语言进行带通滤波后输出结果:\n");
    for (int i = 0; i < x_length; i++) {
        printf("%f, ", y1[i]);
        fprintf(output_file, "%f\n", y1[i]);
        fprintf(output_file_python, "%f\n", y1[i]);
		
    }
    
    
    printf("\n\nC语言进行带阻滤波后输出结果:\n");
    for (int i = 0; i < x_length; i++) {
        printf("%f, ", y2[i]);
        // fprintf(output_file, "%f\n", y2[i]);
    }
    
    
    return 0;
}