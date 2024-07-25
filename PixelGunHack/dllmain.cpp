#include <thread>
#include <cstdio>
#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include "minhook/minhook.h"
using namespace std;

inline float(__stdcall* originalFunc)(void* arg);

inline float __stdcall speed(void* arg)
{
    float original_speed = originalFunc(arg);
    printf("Original speed: %f\n", original_speed);
    return 2.0f;
}



void hook(uintptr_t address, LPVOID detour, void* original)
{
    MH_STATUS create_hook = MH_CreateHook((LPVOID*)address, detour, (LPVOID*)original);
    if (create_hook == MH_OK)
    {
        MH_STATUS enable_status = MH_EnableHook((LPVOID*)address);
        if (enable_status == MH_OK)
        {
            printf("Hook sus\n");
        }
        else
        {
            printf("Hook UnSus reason: %s\n", MH_StatusToString(enable_status));
        }
    }
    else
    {
        printf("Hook UnSus reason: %s\n", MH_StatusToString(create_hook));
    }
}


void omg(HMODULE module)
{

    MH_Initialize();
    uintptr_t GameBase = (uintptr_t)GetModuleHandleA(NULL);
    uintptr_t GameAssembly = (uintptr_t)GetModuleHandleA("GameAssembly.dll");
    uintptr_t UnityPlayer = (uintptr_t)GetModuleHandleA("UnityPlayer.dll");

    hook(GameAssembly + 0x114DBE0, &speed, &originalFunc);
}
bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    static FILE* p_file{ nullptr };


    if (reason == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen_s(&p_file, "CONOUT$", "w", stdout);

        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)omg, 0, 0, 0);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        fclose(p_file);
        FreeConsole();
    }

    return true;
}
