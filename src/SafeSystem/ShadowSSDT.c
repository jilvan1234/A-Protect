#include "ShadowSSDT.h"
#include "SDTShadowRestore.h"

NTSTATUS __stdcall NewNtUserFindWindowEx(
	IN HWND hwndParent, 
	IN HWND hwndChild, 
	IN PUNICODE_STRING pstrClassName OPTIONAL, 
	IN PUNICODE_STRING pstrWindowName OPTIONAL, 
	IN DWORD dwType)
{
	ULONG result;
	NTUSERFINDWINDOWEX OldNtUserFindWindowEx;
	NTUSERQUERYWINDOW OldNtUserQueryWindow;

	//KdPrint(("NtUserFindWindowEx Called\n"));

	OldNtUserFindWindowEx =(NTUSERFINDWINDOWEX) g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserFindWindowExIndex];
	result = OldNtUserFindWindowEx(hwndParent, hwndChild, pstrClassName, pstrWindowName, dwType);

	if (g_fnRPsGetCurrentProcess() != g_protectEProcess)
	{
		ULONG ProcessID;

		OldNtUserQueryWindow =(NTUSERQUERYWINDOW) g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserQueryWindowIndex];
		ProcessID = OldNtUserQueryWindow(result, 0);

		if (ProcessID==(ULONG)ProtectProcessId)
			return 0;
	}
	//如果A盾自己操作自己，则放过
	return result;
}

NTSTATUS  __stdcall NewNtUserBuildHwndList(IN HDESK hdesk, IN HWND hwndNext, IN ULONG fEnumChildren, IN DWORD idThread, IN UINT cHwndMax, OUT HWND *phwndFirst, OUT ULONG* pcHwndNeeded)
{
	NTSTATUS result;
	NTUSERQUERYWINDOW OldNtUserQueryWindow;
	NTUSERBUILDHWNDLIST OldNtUserBuildHwndList;

	//KdPrint(("NtUserBuildHwndList Called\n"));

	if (g_fnRPsGetCurrentProcess() != g_protectEProcess)
	{
		ULONG ProcessID;

		if (fEnumChildren==1)
		{
			OldNtUserQueryWindow = (NTUSERQUERYWINDOW)g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserQueryWindowIndex];
			ProcessID = OldNtUserQueryWindow((ULONG)hwndNext, 0);

			if (ProcessID==(ULONG)ProtectProcessId)
				return STATUS_UNSUCCESSFUL;
		}
		OldNtUserBuildHwndList =(NTUSERBUILDHWNDLIST) g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserBuildHwndListIndex];
		result = OldNtUserBuildHwndList(hdesk,hwndNext,fEnumChildren,idThread,cHwndMax,phwndFirst,pcHwndNeeded);

		if (result==STATUS_SUCCESS)
		{
			ULONG i=0;
			ULONG j;

			while (i<*pcHwndNeeded)
			{
				OldNtUserQueryWindow =(NTUSERQUERYWINDOW) g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserQueryWindowIndex];
				ProcessID=OldNtUserQueryWindow((ULONG)phwndFirst[i],0);
				if (ProcessID==(ULONG)ProtectProcessId)
				{
					for (j=i; j<(*pcHwndNeeded)-1; j++)					
						phwndFirst[j]=phwndFirst[j+1]; 

					phwndFirst[*pcHwndNeeded-1]=0; 

					(*pcHwndNeeded)--;
					continue; 
				}
				i++;				
			}

		}
		return result;
	}
	//如果A盾自己操作自己，则放过
	OldNtUserBuildHwndList =(NTUSERBUILDHWNDLIST) g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserBuildHwndListIndex];
	return OldNtUserBuildHwndList(hdesk,hwndNext,fEnumChildren,idThread,cHwndMax,phwndFirst,pcHwndNeeded);
}

ULONG  __stdcall NewNtUserGetForegroundWindow(VOID)
{
	ULONG result;
	NTUSERGETFOREGROUNDWINDOW OldNtUserGetForegroundWindow;
	NTUSERQUERYWINDOW OldNtUserQueryWindow;

	//KdPrint(("NtUserGetForegroundWindow Called\n"));

	OldNtUserGetForegroundWindow = (NTUSERGETFOREGROUNDWINDOW)g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserGetForegroundWindowIndex];
	result= OldNtUserGetForegroundWindow();	

	if (g_fnRPsGetCurrentProcess() != g_protectEProcess)
	{
		ULONG ProcessID;

		OldNtUserQueryWindow =(NTUSERQUERYWINDOW)g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserQueryWindowIndex];
		ProcessID=OldNtUserQueryWindow(result, 0);

		if (ProcessID == (ULONG)ProtectProcessId)
			result=LastForegroundWindow;
		else
			LastForegroundWindow=result;
	}	
	//如果A盾自己操作自己，则放过
	return result;
}

UINT_PTR  __stdcall NewNtUserQueryWindow(IN ULONG WindowHandle,IN ULONG TypeInformation)
{
	ULONG WindowHandleProcessID;
	NTUSERQUERYWINDOW OldNtUserQueryWindow;

	//KdPrint(("NtUserQueryWindow Called\n"));

	OldNtUserQueryWindow =(NTUSERQUERYWINDOW )g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserQueryWindowIndex];

	if (g_fnRPsGetCurrentProcess() != g_protectEProcess)
	{
		WindowHandleProcessID = OldNtUserQueryWindow(WindowHandle,0);
		if (WindowHandleProcessID==(ULONG)ProtectProcessId)
			return 0;
	}
	//如果A盾自己操作自己，则放过
	return OldNtUserQueryWindow(WindowHandle,TypeInformation);
}
BOOL __stdcall NewNtUserDestroyWindow(HWND hWnd)  
{   
	NTUSERDESTROYWINDOW OldNtUserDestroyWindow;
	NTUSERQUERYWINDOW OldNtUserQueryWindow;
	ULONG WindowHandleProcessID;

	//KdPrint(("NtUserDestroyWindow Called\n"));

	if (g_fnRPsGetCurrentProcess() != g_protectEProcess)
	{
		OldNtUserQueryWindow =(NTUSERQUERYWINDOW) g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserQueryWindowIndex];
		WindowHandleProcessID = OldNtUserQueryWindow((ULONG)hWnd,0);
		if (WindowHandleProcessID == (ULONG)ProtectProcessId)
			return 0;
	}
	//如果A盾自己操作自己，则放过
	OldNtUserDestroyWindow =(NTUSERDESTROYWINDOW) g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserDestroyWindowIndex];
	return OldNtUserDestroyWindow(hWnd);
}
NTSTATUS __stdcall NewNtUserPostMessage(
	IN HWND hWnd,
	IN ULONG pMsg,
	IN ULONG wParam,
	IN ULONG lParam
	)  
{   
	NTUSERQUERYWINDOW OldNtUserQueryWindow;
	NTUSERPOSTMESSAGE OldNtUserPostMessage;

	ULONG WindowHandleProcessID;

	//KdPrint(("NtUserPostMessage Called\n"));

	if (g_fnRPsGetCurrentProcess() != g_protectEProcess)
	{
		OldNtUserQueryWindow = (NTUSERQUERYWINDOW)g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserQueryWindowIndex];
		WindowHandleProcessID = OldNtUserQueryWindow((ULONG)hWnd,0);
		if (WindowHandleProcessID == (ULONG)ProtectProcessId)
			return 0;
	}
	//如果A盾自己操作自己，则放过
	OldNtUserPostMessage =(NTUSERPOSTMESSAGE) g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserPostMessageIndex];
	return OldNtUserPostMessage(hWnd,pMsg,wParam,lParam);
}
BOOL __stdcall NewNtUserPostThreadMessage(
	IN DWORD idThread,
	IN UINT Msg,
	IN ULONG wParam,
	IN ULONG lParam
	)
{
	PEPROCESS EProcess = NULL;
	PETHREAD  Ethread = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	NTUSERPOSTTHREADMESSAGE OldNtUserPostThreadMessage;

	//不清楚NtUserPostThreadMessage是运行在什么等级下，只能限定下PASSIVE_LEVEL了
	if (KeGetCurrentIrql() != PASSIVE_LEVEL){
		goto _FuncRet;
	}
	if (g_fnRPsGetCurrentProcess() != g_protectEProcess)
	{
		status=PsLookupThreadByThreadId((HANDLE)idThread,&Ethread);
		if (NT_SUCCESS(status))
		{
			//清除引数
			ObDereferenceObject(Ethread);

			EProcess=IoThreadToProcess(Ethread);
			if(EProcess == g_protectEProcess){
				//操作者不是A盾，但是确操作A盾的窗口，拒绝
				return FALSE;
			}
		}
	}
_FuncRet:
	//如果A盾自己操作自己，则放过
	OldNtUserPostThreadMessage = (NTUSERPOSTTHREADMESSAGE)g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserPostThreadMessageIndex];
	return OldNtUserPostThreadMessage(
		idThread,
		Msg,
		wParam,
		lParam
		);
}
//拦截全局钩子哦
HHOOK __stdcall NewNtUserSetWindowsHookEx(
	HINSTANCE Mod, 
	PUNICODE_STRING UnsafeModuleName, 
	DWORD ThreadId, 
	int HookId, 
	PVOID HookProc, 
	BOOL Ansi)
{
	PEPROCESS EProcess = NULL;
	PETHREAD  Ethread = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	NTUSERSETWINDOWSHOOKEX OldNtUserSetWindowsHookEx;

	//发现全局钩子了
	if (!ThreadId){

		//如果 bDisSetWindowsHook = FALSE，则阻止全局钩子
		if (!bDisSetWindowsHook){
			return FALSE;
		}
	}
	OldNtUserSetWindowsHookEx =(NTUSERSETWINDOWSHOOKEX) g_OriginalShadowServiceDescriptorTable->ServiceTable[NtUserSetWindowsHookExIndex];
	return OldNtUserSetWindowsHookEx(
		Mod, 
		UnsafeModuleName, 
		ThreadId, 
		HookId, 
		HookProc,
		Ansi 
		);
}
VOID InitShadowSSDTHook()
{
	UNICODE_STRING UnicdeFunction;

	//EnumWindows 枚举所有顶层窗口
	if (SystemCallEntryShadowSSDTTableHook(
		"NtUserBuildHwndList",
		&NtUserBuildHwndListIndex,
		(DWORD)NewNtUserBuildHwndList) == TRUE)
	{
		if (g_bDebugOn)
			KdPrint(("Init NtUserBuildHwndList Thread success\r\n"));
	}
	//GetForegroundWindow 得到当前顶层窗口
	if (SystemCallEntryShadowSSDTTableHook(
		"NtUserGetForegroundWindow",
		&NtUserGetForegroundWindowIndex,
		(DWORD)NewNtUserGetForegroundWindow) == TRUE)
	{
		if (g_bDebugOn)
			KdPrint(("Init NtUserGetForegroundWindow Thread success\r\n"));
	}
	//GetWindowThreadProcessId 获取句柄对应的进程PID
	if (SystemCallEntryShadowSSDTTableHook(
		"NtUserQueryWindow",
		&NtUserQueryWindowIndex,
		(DWORD)NewNtUserQueryWindow) == TRUE)
	{
		if (g_bDebugOn)
			KdPrint(("Init NtUserQueryWindow Thread success\r\n"));
	}
	//FindWindow 查找窗口获取句柄
	if (SystemCallEntryShadowSSDTTableHook(
		"NtUserFindWindowEx",
		&NtUserFindWindowExIndex,
		(DWORD)NewNtUserFindWindowEx) == TRUE)
	{
		if (g_bDebugOn)
			KdPrint(("Init NtUserFindWindowEx Thread success\r\n"));
	}
	//DestroyWindow 销毁窗口
	if (SystemCallEntryShadowSSDTTableHook(
		"NtUserDestroyWindow",
		&NtUserDestroyWindowIndex,
		(DWORD)NewNtUserDestroyWindow) == TRUE)
	{
		if (g_bDebugOn) 
			KdPrint(("Init NtUserDestroyWindow Thread success\r\n"));
	}
	//PostMessage 发送消息
	if (SystemCallEntryShadowSSDTTableHook(
		"NtUserPostMessage",
		&NtUserPostMessageIndex,
		(DWORD)NewNtUserPostMessage) == TRUE)
	{
		if (g_bDebugOn) 
			KdPrint(("Init NtUserPostMessage Thread success\r\n"));
	}
	//SendMessage 发送消息
	if (SystemCallEntryShadowSSDTTableHook(
		"NtUserPostThreadMessage",
		&NtUserPostThreadMessageIndex,
		(DWORD)NewNtUserPostThreadMessage) == TRUE)
	{
		if (g_bDebugOn) 
			KdPrint(("Init NtUserPostThreadMessage Thread success\r\n"));
	}
	//拦截全局钩子
	if (SystemCallEntryShadowSSDTTableHook(
		"NtUserSetWindowsHookEx",
		&NtUserSetWindowsHookExIndex,
		(DWORD)NewNtUserSetWindowsHookEx) == TRUE)
	{
		if (g_bDebugOn) 
			KdPrint(("Init NtUserSetWindowsHookEx Thread success\r\n"));
	}
}
VOID ShadowSSDTHookCheck(PSHADOWSSDTINFO ShadowSSDTInfo)
{
	ULONG ulReloadShadowSSDTAddress;
	ULONG ulOldShadowSSDTAddress;
	ULONG ulCodeSize,ulReloadCodeSize;
	ULONG ulHookFunctionAddress;
	PUCHAR ulTemp;
	PUCHAR ulReloadTemp;

	INSTRUCTION	Inst;
	INSTRUCTION	Instb;
	PUCHAR p;
	ULONG ulHookModuleBase;
	ULONG ulHookModuleSize;
	CHAR lpszHookModuleImage[256];
	CHAR *lpszFunction = NULL;
	int i = 0,count=0;
	WIN_VER_DETAIL WinVer;
	ULONG ulMemoryFunctionBase;
	BOOL bIsHooked = FALSE;
	ULONG ulSize;
	int JmpCount = 0;

	__try
	{
		for (i=0 ;i<(int)g_OriginalShadowServiceDescriptorTable->TableSize ;i++)
		{
			ulReloadShadowSSDTAddress = g_OriginalShadowServiceDescriptorTable->ServiceTable[i];
			ulOldShadowSSDTAddress = ulReloadShadowSSDTAddress - (ULONG)Win32kImageModuleBase + ulWin32kBase;

			if (MmIsAddressValidEx((PVOID)ulReloadShadowSSDTAddress) &&
				MmIsAddressValidEx((PVOID)ulOldShadowSSDTAddress))
			{
				ulReloadCodeSize = GetFunctionCodeSize((PVOID)ulReloadShadowSSDTAddress);

				if (IsFuncInInitSection(ulOldShadowSSDTAddress,ulReloadCodeSize)){
					continue;
				}
				ulCodeSize = GetFunctionCodeSize((PVOID)ulOldShadowSSDTAddress);
				if (ulCodeSize != ulReloadCodeSize){
					continue;
				}
				if (memcmp((PVOID)ulReloadShadowSSDTAddress,(PVOID)ulOldShadowSSDTAddress,ulCodeSize) != 0)
				{
					//KdPrint(("find inline hook[%d]%08x-%08x",i,ulOldShadowSSDTAddress,ulReloadShadowSSDTAddress));

					//开始扫描hook
					for (p=(PUCHAR)ulOldShadowSSDTAddress ;p< (PUCHAR)ulOldShadowSSDTAddress+ulCodeSize; p++)
					{
						//折半扫描，如果前面一半一样，则开始扫描下一半
						if (memcmp((PVOID)ulReloadShadowSSDTAddress,(PVOID)ulOldShadowSSDTAddress,ulCodeSize/2) == 0)
						{
							ulCodeSize = ulCodeSize + ulCodeSize/2;
							continue;
						}
						//是否结束？
						if (*p == 0xcc ||
							*p == 0xc2)
						{
							break;
						}
						ulTemp = NULL;
						get_instruction(&Inst,p,MODE_32);
						switch (Inst.type)
						{
						case INSTRUCTION_TYPE_JMP:
							if(Inst.opcode==0xFF&&Inst.modrm==0x25)
							{
								//DIRECT_JMP
								ulTemp = (PUCHAR)Inst.op1.displacement;
							}
							else if (Inst.opcode==0xEB)
							{
								ulTemp = (PUCHAR)(p+Inst.op1.immediate);
							}
							else if(Inst.opcode==0xE9)
							{
								//RELATIVE_JMP;
								ulTemp = (PUCHAR)(p+Inst.op1.immediate);
							}
							break;
						case INSTRUCTION_TYPE_CALL:
							if(Inst.opcode==0xFF&&Inst.modrm==0x15)
							{
								//DIRECT_CALL
								ulTemp = (PUCHAR)Inst.op1.displacement;
							}
							else if (Inst.opcode==0x9A)
							{
								ulTemp = (PUCHAR)(p+Inst.op1.immediate);
							}
							else if(Inst.opcode==0xE8)
							{
								//RELATIVE_CALL;
								ulTemp = (PUCHAR)(p+Inst.op1.immediate);
							}
							break;
						case INSTRUCTION_TYPE_PUSH:
							if(!MmIsAddressValidEx((PVOID)(p)))
							{
								break;
							}
							get_instruction(&Instb,(BYTE*)(p),MODE_32);
							if(Instb.type == INSTRUCTION_TYPE_RET)
							{
								//StartAddress+len-inst.length-instb.length;
								ulTemp = (PUCHAR)Instb.op1.displacement;
							}
							break;
						}
						if (MmIsAddressValidEx(ulTemp) &&
							MmIsAddressValidEx(p) && 
							ulTemp != p)   //hook的地址也要有效才可以哦
						{
							//得到长度
							ulSize =(ULONG)( p - ulOldShadowSSDTAddress);
							ulReloadTemp = (PUCHAR)(ulReloadShadowSSDTAddress + ulSize);
							if (MmIsAddressValidEx(ulReloadTemp))
							{
								if (*(ULONG *)p == *(ULONG *)ulReloadTemp){
									continue;
								}
							}else{
								continue;
							}
							if (g_bDebugOn)
								KdPrint(("ulTemp:%08x %08x %08x\n",ulTemp,ulReloadTemp,p));

// 							ulTemp = ulTemp+0x5;
// 							//简单处理一下二级跳
// 							if (*ulTemp == 0xe9 ||
// 								*ulTemp == 0xe8)
// 							{
// 								if (DebugOn)
// 									KdPrint(("ulTemp == 0xe9"));
// 
// 								ulTemp = *(PULONG)(ulTemp+1)+(ULONG)(ulTemp+5);
// 							}
							//简单处理一下多级跳(就默认10级跳转)
							for (JmpCount=0;JmpCount<10;JmpCount++)
							{
								if (MmIsAddressValidEx(ulTemp))
								{
									ulTemp = ulTemp+0x5;

									if (*ulTemp == 0xe9 ||
										*ulTemp == 0xe8)
									{
										if (g_bDebugOn)
											KdPrint(("ulTemp == 0xe9"));

										ulTemp = (PUCHAR)(*(PULONG)(ulTemp+1)+(ULONG)(ulTemp+5));

									}else
									{
										break;
									}
								}
							}
							//这里就是hook了
							memset(lpszHookModuleImage,0,sizeof(lpszHookModuleImage));
							if (!IsAddressInSystem(
								(ULONG)ulTemp,
								&ulHookModuleBase,
								&ulHookModuleSize,
								lpszHookModuleImage))
							{
								memset(lpszHookModuleImage,0,sizeof(lpszHookModuleImage));
								strcat(lpszHookModuleImage,"Unknown");
								ulHookModuleBase = 0;
								ulHookModuleSize = 0;
							}
							ShadowSSDTInfo->ulCount = count;
							ShadowSSDTInfo->SSDT[count].ulNumber = i;
							ShadowSSDTInfo->SSDT[count].ulMemoryFunctionBase =(ULONG) ulTemp;
							ShadowSSDTInfo->SSDT[count].ulRealFunctionBase = ulOldShadowSSDTAddress;

							memset(ShadowSSDTInfo->SSDT[count].lpszFunction,0,sizeof(ShadowSSDTInfo->SSDT[count].lpszFunction));
							WinVer = GetWindowsVersion();
							switch (WinVer)
							{
							case WINDOWS_VERSION_XP:
								strncpy(ShadowSSDTInfo->SSDT[count].lpszFunction,XPProcName[i],strlen(XPProcName[i]));
								break;
							case WINDOWS_VERSION_2K3_SP1_SP2:
								strncpy(ShadowSSDTInfo->SSDT[count].lpszFunction,Win2003ProcName[i],strlen(Win2003ProcName[i]));
								break;
							case WINDOWS_VERSION_7_7000:
							case WINDOWS_VERSION_7_7600_UP:
								strncpy(ShadowSSDTInfo->SSDT[count].lpszFunction,Win7ProcName[i],strlen(Win7ProcName[i]));
								break;
							}

							memset(ShadowSSDTInfo->SSDT[count].lpszHookModuleImage,0,sizeof(ShadowSSDTInfo->SSDT[count].lpszHookModuleImage));
							strncpy(ShadowSSDTInfo->SSDT[count].lpszHookModuleImage,lpszHookModuleImage,strlen(lpszHookModuleImage));
							ShadowSSDTInfo->SSDT[count].ulHookModuleBase = ulHookModuleBase;
							ShadowSSDTInfo->SSDT[count].ulHookModuleSize = ulHookModuleSize;
							ShadowSSDTInfo->SSDT[count].IntHookType = SSDTINLINEHOOK;

							if (g_bDebugOn)
								KdPrint(("[%d]Found ssdt inline hook!!:%s-%s",
								ShadowSSDTInfo->ulCount,
								ShadowSSDTInfo->SSDT[count].lpszHookModuleImage,
								ShadowSSDTInfo->SSDT[count].lpszFunction));

							count++;

							ulTemp = NULL;
						}
					}
				}
			}
			//shadowssdt hook
			bShadowHooked = TRUE;
			ulMemoryFunctionBase = ShadowSSDTTable[1].ServiceTable[i];
			//相等说明不是hook
			if (ulMemoryFunctionBase == ulOldShadowSSDTAddress)
			{
				bShadowHooked = FALSE;
			}
			//枚举被hook的 或者所有的shadow函数
			if (bShadowHooked ||
				bShadowSSDTAll == TRUE)
			{
				memset(lpszHookModuleImage,0,sizeof(lpszHookModuleImage));
				if (!IsAddressInSystem(
					ulMemoryFunctionBase,
					&ulHookModuleBase,
					&ulHookModuleSize,
					lpszHookModuleImage))
				{
					strcat(lpszHookModuleImage,"Unknown");
					ulHookModuleBase = 0;
					ulHookModuleSize = 0;
				}
				ShadowSSDTInfo->ulCount = count;
				ShadowSSDTInfo->SSDT[count].ulNumber = i;
				ShadowSSDTInfo->SSDT[count].ulMemoryFunctionBase = ulMemoryFunctionBase;
				ShadowSSDTInfo->SSDT[count].ulRealFunctionBase = ulOldShadowSSDTAddress;

				memset(ShadowSSDTInfo->SSDT[count].lpszFunction,0,sizeof(ShadowSSDTInfo->SSDT[count].lpszFunction));
				WinVer = GetWindowsVersion();
				switch (WinVer)
				{
				case WINDOWS_VERSION_XP:
					strncpy(ShadowSSDTInfo->SSDT[count].lpszFunction,XPProcName[i],strlen(XPProcName[i]));
					break;
				case WINDOWS_VERSION_2K3_SP1_SP2:
					strncpy(ShadowSSDTInfo->SSDT[count].lpszFunction,Win2003ProcName[i],strlen(Win2003ProcName[i]));
					break;
				case WINDOWS_VERSION_7_7000:
				case WINDOWS_VERSION_7_7600_UP:
					strncpy(ShadowSSDTInfo->SSDT[count].lpszFunction,Win7ProcName[i],strlen(Win7ProcName[i]));
					break;
				}

				memset(ShadowSSDTInfo->SSDT[count].lpszHookModuleImage,0,sizeof(ShadowSSDTInfo->SSDT[count].lpszHookModuleImage));
				strncpy(ShadowSSDTInfo->SSDT[count].lpszHookModuleImage,lpszHookModuleImage,strlen(lpszHookModuleImage));

				ShadowSSDTInfo->SSDT[count].ulHookModuleBase = ulHookModuleBase;
				ShadowSSDTInfo->SSDT[count].ulHookModuleSize = ulHookModuleSize;

				if (bShadowHooked == FALSE)
				{
					ShadowSSDTInfo->SSDT[count].IntHookType = NOHOOK;
				}else
					ShadowSSDTInfo->SSDT[count].IntHookType = SSDTHOOK;  //ssdt hook

				if (g_bDebugOn)
					KdPrint(("[%d]Found ssdt hook!!:%s-%s",
					ShadowSSDTInfo->ulCount,
					ShadowSSDTInfo->SSDT[count].lpszHookModuleImage,
					ShadowSSDTInfo->SSDT[count].lpszFunction));

				count++;
			}
		}

	}__except(EXCEPTION_EXECUTE_HANDLER){

	}
}
//8888为全部，其余为单个
BOOL RestoreAllShadowSSDTFunction(ULONG IntType)
{
	ULONG ulMemoryFunctionBase;
	ULONG ulRealMemoryFunctionBase;
	ULONG ulReloadFunctionBase;

	BOOL bHooked = FALSE;
	int i = 0;
	BOOL bReSetOne = FALSE;

	for (i=0 ;i<(int)ShadowSSDTTable[1].TableSize ;i++)
	{
		bHooked = TRUE;
		ulMemoryFunctionBase = ShadowSSDTTable[1].ServiceTable[i];

		//得到原来系统内存中shadowssdt的地址，不是reload的shadowssdt哦，亲
		ulReloadFunctionBase = g_OriginalShadowServiceDescriptorTable->ServiceTable[i];
		ulRealMemoryFunctionBase = ulReloadFunctionBase - (ULONG)Win32kImageModuleBase + ulWin32kBase;

		if (ulMemoryFunctionBase > ulWin32kBase &&
			ulMemoryFunctionBase < ulWin32kBase + ulWin32kSize)
		{
			bHooked = FALSE;
		}
		if (bHooked == TRUE)
		{
			if (g_bDebugOn)
				KdPrint(("[%d]%08x  %08x",i,ulReloadFunctionBase,ulRealMemoryFunctionBase));

			//开始恢复
			//恢复全部
			if (IntType == 8888)
			{
				__asm
				{
					cli
						push eax
						mov eax,cr0
						and eax,not 0x10000
						mov cr0,eax
						pop eax
				}
				ShadowSSDTTable[1].ServiceTable[i] = ulRealMemoryFunctionBase;

				if (g_bDebugOn)
					KdPrint(("[%d]%08x  %08x",i,ShadowSSDTTable[1].ServiceTable[i],ulRealMemoryFunctionBase));
				__asm
				{
					push eax
						mov eax,cr0
						or eax,0x10000
						mov cr0,eax
						pop eax
						sti
				}
			}   
			else  //恢复单个
			{
				if (IntType == i)
				{
					__asm
					{
						cli
							push eax
							mov eax,cr0
							and eax,not 0x10000
							mov cr0,eax
							pop eax
					}
					ShadowSSDTTable[1].ServiceTable[i] = ulRealMemoryFunctionBase;
					__asm
					{
						push eax
							mov eax,cr0
							or eax,0x10000
							mov cr0,eax
							pop eax
							sti
					}
				}
			}
		}
		//恢复inline hook
		if (IntType == 8888)
		{
			RestoreShadowInlineHook(i);
		}else
		{
			if (IntType == i)
			{
				RestoreShadowInlineHook(i);
				break;
			}
		}
	}
	return TRUE;
}
BOOL RestoreShadowInlineHook(ULONG ulNumber)
{
	ULONG ulFunction = 0;
	ULONG ulReloadFunction;
	BOOL bInit = FALSE;
	int i=0;

	for (i=0 ;i<(int)ShadowSSDTTable[1].TableSize ;i++)
	{
		if (ulNumber == i)
		{
			ulFunction = g_OriginalShadowServiceDescriptorTable->ServiceTable[i] - (ULONG)Win32kImageModuleBase + ulWin32kBase;
			ulReloadFunction = g_OriginalShadowServiceDescriptorTable->ServiceTable[i];

			if (MmIsAddressValidEx((PVOID)ulFunction) &&
				MmIsAddressValidEx((PVOID)ulReloadFunction))
			{
				if (GetFunctionCodeSize((PVOID)ulFunction) != GetFunctionCodeSize((PVOID)ulReloadFunction))
				{
					return FALSE;
				}
				__asm
				{
					cli
						push eax
						mov eax,cr0
						and eax,not 0x10000
						mov cr0,eax
						pop eax
				}
				memcpy((PVOID)ulFunction,(PVOID)ulReloadFunction,GetFunctionCodeSize((PVOID)ulFunction));
				__asm
				{
					push eax
						mov eax,cr0
						or eax,0x10000
						mov cr0,eax
						pop eax
						sti
				}
				bInit = TRUE;
				break;
			}
		}
	}
	return TRUE;
}