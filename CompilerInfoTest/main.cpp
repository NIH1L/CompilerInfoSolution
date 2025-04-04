﻿#include <iostream>
#include "CompilerInfo.h"  // 引入 DLL 的头文件

int main() {
    //const char* exePath = "C:\\Users\\admin\\Downloads\\HPDesignjetZ6100PhotoPrinterSeriesPS3.exe";  // 要检测的可执行文件路径
    const char* jsonInput = R"({"exePath": "D:\\files\\codes\\C\\NIH1L\\CompilerInfoSolution\\testfiles\\cmake.exe","rulesPath": "D:\\files\\codes\\C\\NIH1L\\CompilerInfoSolution\\rules.json"})";// 要检测的可执行文件路径

    //const char* compilerInfo = GetCompilerInfo(exePath);
    const char* compilerInfo = GetCompilerInfoJson(jsonInput);
    std::cout << "编译器信息: " << compilerInfo << std::endl;

    return 0;
}
