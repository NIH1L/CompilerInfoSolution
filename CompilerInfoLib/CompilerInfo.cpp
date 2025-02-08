#include "pch.h"
#include "CompilerInfo.h"
#include <windows.h>

CompilerInfo::CompilerInfo() {}

CompilerInfo::~CompilerInfo() {}

std::string CompilerInfo::IdentifyCompiler(const std::string& exePath) {
    HANDLE hFile = CreateFileA(exePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return "�����޷����ļ���";
    }

    HANDLE hMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!hMapping) {
        CloseHandle(hFile);
        return "�����޷������ļ�ӳ�䡣";
    }

    LPVOID lpBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (!lpBase) {
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return "�����޷�ӳ���ļ���ͼ��";
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)lpBase;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        UnmapViewOfFile(lpBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return "���󣺲�����Ч�Ŀ�ִ���ļ���";
    }

    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)lpBase + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        UnmapViewOfFile(lpBase);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return "������Ч��PEͷ��";
    }

    std::string compilerInfo = "δ֪������";
    if (ntHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ||
        ntHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) {
        if (ntHeaders->OptionalHeader.MajorLinkerVersion >= 14) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2015 ����߰汾) ����";
        }
        else if (ntHeaders->OptionalHeader.MajorLinkerVersion >= 9) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2008 ����߰汾) ����";
        }
        else {
            compilerInfo = "ʹ�ýϾɰ汾�� MSVC ����";
        }
    }

    UnmapViewOfFile(lpBase);
    CloseHandle(hMapping);
    CloseHandle(hFile);
    return compilerInfo;
}

// ��������
extern "C" COMPILERINFOLIB_API const char* GetCompilerInfo(const char* exePath) {
    static CompilerInfo compiler;
    static std::string result = compiler.IdentifyCompiler(exePath);
    return result.c_str();
}
