#ifdef __cplusplus
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

EXPORT void SetDllString(char dst,char* str);
EXPORT void GetDllString(char* str);
EXPORT void WaitGetDllString(char dst,char* str);