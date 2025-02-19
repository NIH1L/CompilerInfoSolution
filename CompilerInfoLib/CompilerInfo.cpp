#include "pch.h"
#include "CompilerInfo.h"
#include <windows.h>
#include <json/json.h>

CompilerInfo::CompilerInfo() {}

CompilerInfo::~CompilerInfo() {}

//定义CompilerInfo类中的IdentifyCompiler函数，使用const可让exePath能以字面量的形式输入，使用&传递exePath的值为引用，将直接取地址
std::string CompilerInfo::IdentifyCompiler(const std::string& exePath) {
    //使用windows.h中的CreateFileA函数读取文件句柄，c_str()为转换为C风格字符串，因为该函数只能这么用。
    //只读，允许其它进程共享该文件（不锁定），安全属性为默认NULL，只打开已有文件，文件属性为标准文件，模板文件设为不需要
    HANDLE hFile = CreateFileA(exePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    //CreateFileA返回INVALID_HANDLE_VALUE则是没获取到句柄，即文件不存在或无法打开
    if (hFile == INVALID_HANDLE_VALUE) {
        return "错误：无法打开文件。";
    }

    HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!hMapping) {
        CloseHandle(hFile);
        return "错误：无法创建文件映射。";
    }

    LPVOID lpBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (!lpBase) {
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return "错误：无法映射文件视图。";
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)lpBase;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        UnmapViewOfFile(lpBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return "错误：不是有效的可执行文件。";
    }

    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)lpBase + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        UnmapViewOfFile(lpBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return "错误：无效的PE头。";
    }

    //初始化，没有识别到匹配的版本则输出"未知编译器"
    std::string compilerInfo = "未知编译器";
    /**
    if (ntHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ||
        ntHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) {
        if (ntHeaders->OptionalHeader.MajorLinkerVersion >= 14) {
            compilerInfo = "使用 MSVC (Visual Studio 2015 或更高版本) 编译";
        }
        else if (ntHeaders->OptionalHeader.MajorLinkerVersion >= 9) {
            compilerInfo = "使用 MSVC (Visual Studio 2008 或更高版本) 编译";
        }
        else {
            compilerInfo = "使用较旧版本的 MSVC 编译";
        }
    **/
    if (ntHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ||
        ntHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) {

        int linkerVersion = ntHeaders->OptionalHeader.MajorLinkerVersion * 10 + ntHeaders->OptionalHeader.MinorLinkerVersion;

        //VS版本判断
        if (linkerVersion >= 143) {
            compilerInfo = "使用 MSVC (Visual Studio 2022) 编译";
        }
        else if (linkerVersion >= 142) {
            compilerInfo = "使用 MSVC (Visual Studio 2019) 编译";
        }
        else if (linkerVersion >= 141) {
            compilerInfo = "使用 MSVC (Visual Studio 2017) 编译";
        }
        else if (linkerVersion >= 140) {
            compilerInfo = "使用 MSVC (Visual Studio 2015) 编译";
        }
        else if (linkerVersion >= 120) {
            compilerInfo = "使用 MSVC (Visual Studio 2013) 编译";
        }
        else if (linkerVersion >= 110) {
            compilerInfo = "使用 MSVC (Visual Studio 2012) 编译";
        }
        else if (linkerVersion >= 100) {
            compilerInfo = "使用 MSVC (Visual Studio 2010) 编译";
        }
        else if (linkerVersion >= 90) {
            compilerInfo = "使用 MSVC (Visual Studio 2008) 编译";
        }
        else if (linkerVersion >= 80) {
            compilerInfo = "使用 MSVC (Visual Studio 2005) 编译";
        }
        else if (linkerVersion >= 71) {
            compilerInfo = "使用 MSVC (Visual Studio .NET 2003) 编译";
        }
        else if (linkerVersion >= 60) {
            compilerInfo = "使用 MSVC (Visual Studio 6.0) 编译";
        }
        else {
            compilerInfo = "使用较旧版本的 MSVC 编译";
        }


    }

    UnmapViewOfFile(lpBase);
    CloseHandle(hMapping);
    CloseHandle(hFile);
    return compilerInfo;
}

// 导出函数
/**extern "C" COMPILERINFOLIB_API const char* GetCompilerInfo(const char* exePath) {
    static CompilerInfo compiler;
    static std::string result = compiler.IdentifyCompiler(exePath);
    return result.c_str();
}
**/
extern "C" COMPILERINFOLIB_API const char* GetCompilerInfoJson(const char* jsonInput) {
    static std::string resultJson;  // 确保返回的字符串不会被释放

    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istringstream s(jsonInput);

    if (!Json::parseFromStream(reader, s, &root, &errors)) {
        resultJson = R"({"error": "Invalid JSON"})";
        return resultJson.c_str();
    }

    if (!root.isMember("exePath")) {
        resultJson = R"({"error": "Missing exePath"})";
        return resultJson.c_str();
    }

    std::string exePath = root["exePath"].asString();
    CompilerInfo compiler;
    std::string compilerResult = compiler.IdentifyCompiler(exePath);

    Json::Value output;
    output["exePath"] = exePath;
    output["compilerInfo"] = compilerResult;
    resultJson = output.toStyledString();

    return resultJson.c_str();
}