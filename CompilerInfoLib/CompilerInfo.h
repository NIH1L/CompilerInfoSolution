//确保这个头文件在编译过程中只被包含一次，防止出现函数或变量重复定义产生错误。
#pragma once

//COMPILERINFOLIB_EXPORTS通常是在创建DLL项目时定义的宏定义（相当于字符串变量，在IDE项目-属性-预处理器中一般自动有定义），
// 如果定义了它说明正在编译dll本身，就将COMPILERINFOLIB_API这个变量（手动命名）设为导出函数__declspec(dllexport)
//如果没定义就说明dll被外部程序使用，就将他设为导入函数
#ifdef COMPILERINFOLIB_EXPORTS
#define COMPILERINFOLIB_API __declspec(dllexport)
#else
#define COMPILERINFOLIB_API __declspec(dllimport)
#endif

//一般优先处理pragma once和宏定义，保证它们的优先级，再添加功能string之类的标准库功能包
#include <string>

// 面向对象编程：定义一个类 CompilerInfo
class COMPILERINFOLIB_API CompilerInfo {
public:
    CompilerInfo();  // 构造函数
    ~CompilerInfo(); // 析构函数

    // 声明识别编译器信息的函数IdentifyCompiler
    std::string IdentifyCompiler(const std::string& exePath);
};

// 导出一个C++兼容的函数给外部模块调用
/**extern "C" {
    COMPILERINFOLIB_API const char* GetCompilerInfo(const char* exePath);
}
**/
extern "C" {
    COMPILERINFOLIB_API const char* GetCompilerInfoJson(const char* jsonInput);
}