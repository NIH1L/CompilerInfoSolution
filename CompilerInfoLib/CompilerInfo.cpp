#include "pch.h"
#include "CompilerInfo.h"
#include <windows.h>
#include <json/json.h>

CompilerInfo::CompilerInfo() {}

CompilerInfo::~CompilerInfo() {}

//����CompilerInfo���е�IdentifyCompiler������ʹ��const����exePath��������������ʽ���룬ʹ��&����exePath��ֵΪ���ã���ֱ��ȡ��ַ
std::string CompilerInfo::IdentifyCompiler(const std::string& exePath) {
    //ʹ��windows.h�е�CreateFileA������ȡ�ļ������c_str()Ϊת��ΪC����ַ�������Ϊ�ú���ֻ����ô�á�
    //ֻ���������������̹�����ļ���������������ȫ����ΪĬ��NULL��ֻ�������ļ����ļ�����Ϊ��׼�ļ���ģ���ļ���Ϊ����Ҫ
    HANDLE hFile = CreateFileA(exePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    //CreateFileA����INVALID_HANDLE_VALUE����û��ȡ����������ļ������ڻ��޷���
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

    //��ʼ����û��ʶ��ƥ��İ汾�����"δ֪������"
    std::string compilerInfo = "δ֪������";
    /**
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
    **/
    if (ntHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ||
        ntHeaders->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) {

        int linkerVersion = ntHeaders->OptionalHeader.MajorLinkerVersion * 10 + ntHeaders->OptionalHeader.MinorLinkerVersion;

        //VS�汾�ж�
        if (linkerVersion >= 143) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2022) ����";
        }
        else if (linkerVersion >= 142) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2019) ����";
        }
        else if (linkerVersion >= 141) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2017) ����";
        }
        else if (linkerVersion >= 140) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2015) ����";
        }
        else if (linkerVersion >= 120) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2013) ����";
        }
        else if (linkerVersion >= 110) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2012) ����";
        }
        else if (linkerVersion >= 100) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2010) ����";
        }
        else if (linkerVersion >= 90) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2008) ����";
        }
        else if (linkerVersion >= 80) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 2005) ����";
        }
        else if (linkerVersion >= 71) {
            compilerInfo = "ʹ�� MSVC (Visual Studio .NET 2003) ����";
        }
        else if (linkerVersion >= 60) {
            compilerInfo = "ʹ�� MSVC (Visual Studio 6.0) ����";
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
/**extern "C" COMPILERINFOLIB_API const char* GetCompilerInfo(const char* exePath) {
    static CompilerInfo compiler;
    static std::string result = compiler.IdentifyCompiler(exePath);
    return result.c_str();
}
**/
extern "C" COMPILERINFOLIB_API const char* GetCompilerInfoJson(const char* jsonInput) {
    static std::string resultJson;  // ȷ�����ص��ַ������ᱻ�ͷ�

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