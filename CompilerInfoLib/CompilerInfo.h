//ȷ�����ͷ�ļ��ڱ��������ֻ������һ�Σ���ֹ���ֺ���������ظ������������
#pragma once

//COMPILERINFOLIB_EXPORTSͨ�����ڴ���DLL��Ŀʱ����ĺ궨�壨�൱���ַ�����������IDE��Ŀ-����-Ԥ��������һ���Զ��ж��壩��
// �����������˵�����ڱ���dll�����ͽ�COMPILERINFOLIB_API����������ֶ���������Ϊ��������__declspec(dllexport)
//���û�����˵��dll���ⲿ����ʹ�ã��ͽ�����Ϊ���뺯��
#ifdef COMPILERINFOLIB_EXPORTS
#define COMPILERINFOLIB_API __declspec(dllexport)
#else
#define COMPILERINFOLIB_API __declspec(dllimport)
#endif

//һ�����ȴ���pragma once�ͺ궨�壬��֤���ǵ����ȼ�������ӹ���string֮��ı�׼�⹦�ܰ�
#include <string>

// ��������̣�����һ���� CompilerInfo
class COMPILERINFOLIB_API CompilerInfo {
public:
    CompilerInfo();  // ���캯��
    ~CompilerInfo(); // ��������

    // ����ʶ���������Ϣ�ĺ���IdentifyCompiler
    std::string IdentifyCompiler(const std::string& exePath);
};

// ����һ��C++���ݵĺ������ⲿģ�����
/**extern "C" {
    COMPILERINFOLIB_API const char* GetCompilerInfo(const char* exePath);
}
**/
extern "C" {
    COMPILERINFOLIB_API const char* GetCompilerInfoJson(const char* jsonInput);
}