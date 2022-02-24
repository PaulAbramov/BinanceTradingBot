// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "detours/detours.h"
#include "detours/detver.h"
#include <iostream>

typedef int(WINAPI* MBA)(HWND, LPCSTR, LPCSTR, UINT);

MBA originalMBA = NULL;

int __stdcall HookedMessageBoxA(HWND   hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT   uType)
{
    return 0; originalMBA(NULL, "HOOKED", "HOOKED", MB_OK);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        originalMBA = (MBA)DetourFindFunction("user32.dll", "MessageBoxA");//Pointer the the original MBA (MessageBoxA) function.
        DetourAttach(&(PVOID&)originalMBA, (PVOID)HookedMessageBoxA);

        DetourTransactionCommit();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

