#include "pch.h"
#include "CompilerInfo.h"
#include <windows.h>

CompilerInfo::CompilerInfo() {}

CompilerInfo::~CompilerInfo() {}

std::string CompilerInfo::IdentifyCompiler(const std::string& exePath) {
    HANDLE hFile = CreateFileA(exePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

    std::string compilerInfo = "未知编译器";
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
    }

    UnmapViewOfFile(lpBase);
    CloseHandle(hMapping);
    CloseHandle(hFile);
    return compilerInfo;
}

// 导出函数
extern "C" COMPILERINFOLIB_API const char* GetCompilerInfo(const char* exePath) {
    static CompilerInfo compiler;
    static std::string result = compiler.IdentifyCompiler(exePath);
    return result.c_str();
}
