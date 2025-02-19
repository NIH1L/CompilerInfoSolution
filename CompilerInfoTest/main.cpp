#include <iostream>
#include "CompilerInfo.h"  // 引入 DLL 的头文件

int main() {
    const char* exePath = "C:\\Users\\admin\\Downloads\\HPDesignjetZ6100PhotoPrinterSeriesPS3.exe";  // 你要检测的可执行文件路径（确保 a.exe 在同一目录下）

    const char* compilerInfo = GetCompilerInfo(exePath);
    std::cout << "编译器信息: " << compilerInfo << std::endl;

    return 0;
}
