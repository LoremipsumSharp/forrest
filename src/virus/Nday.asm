
;以下是本人习惯使用的注释方法
;==================================================
;说明
;--------------------------------------------------
;程序体
;; ┓; ┃; ┣ ; ┛
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
;说明（结束）
;==================================================

.386
.model flat,stdcall
option casemap:none

DlgProc			proto	:DWORD, :DWORD, :DWORD, :DWORD
MyMemcpy		proto	:DWORD, :DWORD, :DWORD
MyStrcpy		proto	:DWORD, :DWORD

AddASectionToFile	proto	:DWORD, :DWORD, :DWORD

include		\masm32\include\windows.inc
include		\masm32\include\user32.inc
include		\masm32\include\kernel32.inc
include		\masm32\include\comdlg32.inc

includelib	\masm32\lib\user32.lib
includelib	\masm32\lib\kernel32.lib
includelib	\masm32\lib\comdlg32.lib

.const
IDD_DIALOG1	equ	  101
IDM_ADD		equ	40004
IDC_BTN_ADD	equ	 1002

MAXSIZE		equ	  260


.data

ofn			OPENFILENAME	<>
FilterString		db		"All Files",0,"*.*",0
			db		"Text Files",0,"*.txt",0,0
buffer			db		MAXSIZE dup(0)
hMapFile		HANDLE		0

; 以下是新添加的节相关内容-------------------------------------------------------------------
szNameOfNewSection	BYTE		".datax", 0, 0		; 新的 Section 的名字
iCharacteristics	DWORD		0E0000020H		; 
; ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^以上是新添加的节相关内容

.data?
hInstance		HINSTANCE	?
hFileRead		HANDLE		?
hMenu			HANDLE		?
hMainDlg		DWORD		?	; 主对话框

.code
start:
	invoke GetModuleHandle,NULL
	invoke DialogBoxParam, eax, IDD_DIALOG1,NULL,addr DlgProc, 0
	invoke ExitProcess, 0	
	
DlgProc proc hDlg:DWORD, uMsg:DWORD, wParam:DWORD, lParam:DWORD		
	.if uMsg==WM_INITDIALOG

		mov	eax, hDlg
		mov	hMainDlg, eax
		invoke	GetMenu,hDlg
		mov	hMenu,eax
		mov	ofn.lStructSize,SIZEOF ofn
		push	hDlg
		pop	ofn.hWndOwner
		push	hInstance
		pop	ofn.hInstance
		mov	ofn.lpstrFilter, OFFSET FilterString
		mov	ofn.lpstrFile, OFFSET buffer
		mov	ofn.nMaxFile,MAXSIZE
	.elseif uMsg==WM_CLOSE
		invoke EndDialog,hDlg,0
	.elseif uMsg==WM_COMMAND
		.if lParam==0
			mov eax,wParam
			.if ax==IDM_ADD
				mov  ofn.Flags, OFN_FILEMUSTEXIST or \
                                OFN_PATHMUSTEXIST or OFN_LONGNAMES or\
                                OFN_EXPLORER or OFN_HIDEREADONLY
				invoke GetOpenFileName, ADDR ofn
				.if eax==TRUE
					lea	eax, iCodeInsideLength
					lea	ebx, LABEL_CODE_INSIDE
					invoke	AddASectionToFile, addr buffer, ebx, [eax]
				.endif
			.else	; IDM_EXIT
				invoke SendMessage,hDlg,WM_CLOSE,0,0
			.endif
		.else
			mov eax,wParam
			mov edx,wParam
			shr edx,16
			.IF dx==BN_CLICKED
				.IF ax==IDC_BTN_ADD
					mov  ofn.Flags, OFN_FILEMUSTEXIST or \
					OFN_PATHMUSTEXIST or OFN_LONGNAMES or\
					OFN_EXPLORER or OFN_HIDEREADONLY
					invoke GetOpenFileName, ADDR ofn
					.if eax==TRUE
						lea	eax, iCodeInsideLength
						lea	ebx, LABEL_CODE_INSIDE
						invoke	AddASectionToFile, addr buffer, ebx, [eax]
					.endif
				.ENDIF
			.ENDIF
		.endif
	.else
		mov eax,FALSE
		ret
	.endif
	mov eax,TRUE
	ret
DlgProc endp

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
	LOCAL	pBufToWrite[SIZE_OF_BUFFER_TO_WRITE]	: BYTE
	LOCAL	iNumOfBytesRead				: DWORD
	LOCAL	iNumOfBytesWritten			: DWORD
	LOCAL	iNumberOfSectionsOrigin			: WORD
	LOCAL	iSectionAlignment			: DWORD	; 节对齐粒度
	LOCAL	iFileAlignment				: DWORD	; 文件对齐粒度
	LOCAL	iNewPhysicalAddress			: DWORD	; ┓
	LOCAL	iNewVirtualAddress			: DWORD	; ┃ 最后一个节中诸数据
	LOCAL	iNewSizeOfRawData			: DWORD	; ┃
	LOCAL	iNewPointerToRawData			: DWORD	; ┛ 
	LOCAL	iNewSectionLength			: DWORD	; 新的 Section 节对齐后的长度
	LOCAL	pOptionalHeader				: DWORD ; 指向 Optional Header 的指针
	LOCAL	iOriginalAddressOfEntryPoint		: DWORD ; 原来的程序入口地址
	LOCAL	iOriginalImageBase			: DWORD ; 原来的基址

	cmp	iSectionLength, SIZE_OF_BUFFER_TO_WRITE
	ja	LABEL_ADD_SECTION_ERROR

	invoke CreateFile, pszFileName,\
		GENERIC_READ or GENERIC_WRITE,\
		FILE_SHARE_READ,\
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,\
		NULL
	mov hFileToBeModified,eax	; 以写方式打开文件

	invoke	SetFilePointer, hFileToBeModified, 0, NULL, FILE_BEGIN

	invoke	ReadFile, hFileToBeModified, addr pBuf, 1024, addr iNumOfBytesRead, NULL

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
	invoke	SetFilePointer, hFileToBeModified, eax, NULL, FILE_BEGIN	; 移动文件指针到 NumberOfSections 处
	mov	dx, iNumberOfSectionsOrigin
	inc	dx				;  dx <-- 新的节数
	lea	esi, pBufToWrite		; esi --> pBufToWrite 首地址
	mov	word ptr [esi], dx		; pBufToWrite <-- 新的节数
	add	esi, 2
	mov	byte ptr [esi], 0		; pBufToWrite[2] <-- 0  空零结束
	invoke	WriteFile, hFileToBeModified, addr pBufToWrite, 2, addr iNumOfBytesWritten, NULL	; 往文件中写

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
	invoke	SetFilePointer, hFileToBeModified, eax, NULL, FILE_BEGIN	; 移动文件指针到新的 Section header 处

	sub	edi, sizeof IMAGE_SECTION_HEADER	; edi -> 最后一个 Section header
	assume	edi : ptr IMAGE_SECTION_HEADER		; edi 强制类型转换成指向 IMAGE_SECTION_HEADER 的指针

	mov	eax, [edi].Misc.PhysicalAddress		; 最后一个 Section header 中内容有多长
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
	cmp	eax, 0					; 这个偏移有时候居然是 0 !如果真是 0 我们就不得不退回一个节.
	jnz	LABEL_POINTERTORAWDATA_NONZERO		; 不是 0 , edi 仍指向倒数第一个 Section
	sub	edi, sizeof IMAGE_SECTION_HEADER	; edi 指向倒数第二个 Section
LABEL_POINTERTORAWDATA_NONZERO:
	mov	eax, [edi].PointerToRawData		; 倒数第一或第二个 Section 在文件中的偏移
	add	eax, [edi].SizeOfRawData		; 即使是倒数第二个的话，也不再判断了, 直接加上这个节经过文件对齐以后的长度
	mov	iNewPointerToRawData, eax		; iNewPointerToRawData <- 新的 Section 的 PointerToRawData

	invoke	RtlZeroMemory, addr pBufToWrite, SIZE_OF_BUFFER_TO_WRITE	; 将缓冲区清零
	lea	esi, pBufToWrite			; esi --> pBufToWrite 首地址
	assume	esi : ptr IMAGE_SECTION_HEADER		; esi 强制类型转换成指向 IMAGE_SECTION_HEADER 的指针

	invoke	MyMemcpy, addr [esi].Name1, addr szNameOfNewSection, 8

	mov	eax, iSectionLength
	mov	[esi].Misc.PhysicalAddress, eax

	mov	eax, iNewVirtualAddress
	mov	[esi].VirtualAddress, eax

	mov	eax, iNewSizeOfRawData
	mov	[esi].SizeOfRawData, eax

	mov	eax, iNewPointerToRawData
	mov	[esi].PointerToRawData, eax

	mov	eax, iCharacteristics
	mov	[esi].Characteristics, eax

	invoke	WriteFile, hFileToBeModified, addr pBufToWrite, sizeof IMAGE_SECTION_HEADER, addr iNumOfBytesWritten, NULL	; 往文件中写

	invoke	SetFilePointer, hFileToBeModified, 0, NULL, FILE_END		; 移动文件指针到文件尾
	invoke	RtlZeroMemory, addr pBufToWrite, SIZE_OF_BUFFER_TO_WRITE	; 缓冲区清零
	invoke	MyMemcpy, addr pBufToWrite, pSectionContent, iSectionLength	; 将要添加的内容拷贝到 pBufToWrite 中
	lea	esi, pBufToWrite			; ┓
	add	esi, iCodeInsideLength			; ┃
	sub	esi, 5					; ┣  新的代码执行完了后，要返回程序原来的入口
	mov	eax, [iOriginalAddressOfEntryPoint]	; ┃
	add	eax, [iOriginalImageBase]		; ┃
	mov	dword ptr [esi], eax			; ┛
	invoke	WriteFile, hFileToBeModified, addr pBufToWrite, iNewSizeOfRawData, addr iNumOfBytesWritten, NULL	; 往文件中写

	mov	edi, pOptionalHeader			; edi 指向 Optional Header
	assume	edi : ptr IMAGE_OPTIONAL_HEADER32	; edi 强制类型转换成指向 IMAGE_OPTIONAL_HEADER32 的指针
	lea	eax, [edi].SizeOfImage			; ┓ 
	lea	edx, pBuf				; ┣ SizeOfImage 相对于文件头的偏移 <- SizeOfImage 的地址 - 文件头的地址
	sub	eax, edx				; ┛
	invoke	SetFilePointer, hFileToBeModified, eax, NULL, FILE_BEGIN	; 移动文件指针到 SizeOfImage 处
	invoke	RtlZeroMemory, addr pBufToWrite, SIZE_OF_BUFFER_TO_WRITE	; 将缓冲区清零
	lea	esi, pBufToWrite			; esi --> pBufToWrite 首地址
	mov	ecx, [edi].SizeOfImage			; 旧的 SizeOfImage
	add	ecx, iNewSectionLength			; 新的 SizeOfImage
	mov	dword ptr [esi], ecx
	invoke	WriteFile, hFileToBeModified, addr pBufToWrite, 4, addr iNumOfBytesWritten, NULL	; 往文件中写

	lea	eax, [edi].AddressOfEntryPoint		; ┓ 
	lea	edx, pBuf				; ┣ AddressOfEntryPoint 相对于文件头的偏移 <- AddressOfEntryPoint 的地址 - 文件头的地址
	sub	eax, edx				; ┛
	invoke	SetFilePointer, hFileToBeModified, eax, NULL, FILE_BEGIN	; 移动文件指针到 AddressOfEntryPoint 处
	invoke	RtlZeroMemory, addr pBufToWrite, SIZE_OF_BUFFER_TO_WRITE	; 将缓冲区清零
	lea	esi, pBufToWrite			; esi --> pBufToWrite 首地址
	add	ecx, iNewVirtualAddress			; 新的 AddressOfEntryPoint
	mov	dword ptr [esi], ecx
	invoke	WriteFile, hFileToBeModified, addr pBufToWrite, 4, addr iNumOfBytesWritten, NULL	; 往文件中写
	
	assume	edi : nothing

	mov	eax, 0			; 添加成功就返回 0
	jmp	LABEL_ADD_SECTION_END


LABEL_ADD_SECTION_ERROR:
	mov	eax, 0ffffffffh	; 添加失败就返回 ffffffffh

LABEL_ADD_SECTION_END:
	invoke	CloseHandle, hFileToBeModified
	ret
AddASectionToFile ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
; 增加一个 Section（结束）
;==================================================


;==================================================
; 内存拷贝，仿 Memcpy
;--------------------------------------------------
; 参数：
;	pDes : 目的内存首地址
;	pSrc : 源内存首地址
;	iSize: 要拷贝的长度
; 返回值：
;	没有
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
	ret
MyMemcpy ENDP
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
;内存拷贝，仿 Memcpy（结束）
;==================================================




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

	BASE_OF_KERNEL32	EQU	77E60000h

	pLoadLibraryA		DWORD	?	; 函数 LoadLibraryA 入口地址
	pGetProcAddress		DWORD	?	; 函数 GetProcAddress 入口地址

	szKernel32Name		BYTE	"KERNEL32.dll", 0
	szUser32Name		BYTE	"USER32.dll", 0

	szLoadLibraryA		BYTE	"LoadLibraryA", 0
	szGetProcAddress	BYTE	"GetProcAddress", 0
	szMessageBoxA		BYTE	"MessageBoxA",0

	szTitle			BYTE	"title", 0
	szMsg			BYTE	"msg", 0
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
	LOCAL	iLocalNumberOfExptFunctions	: DWORD
	LOCAL	iLocalBase				: DWORD
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
	mov	iLocalNumberOfExptFunctions, eax; ┣ 保存有名字的函数数目
	mov	eax, [edi].nBase		; ┃
	mov	iLocalBase, eax			; ┛

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
	add	edx, iLocalBase			; edx <- 函数序数(加了基数的)

	mov	eax, pApiName			; ┓
	push	eax				; ┃
	mov	eax, dword ptr [esi]		; ┣ MyStrcpy( iBaseAddr + [esi],
	add	eax, iBaseAddr			; ┃	       pApiName );
	push	eax				; ┃
	call	MyStrcmp			; ┛
	cmp	eax, 0				; 判断返回值, 如果为 0 表示字符串一样
	jz	LABEL_FUNCTION_FOUND		; 函数被找到

	add	esi, 4
	add	edi, 2
	dec	iLocalNumberOfExptFunctions
	cmp	iLocalNumberOfExptFunctions, 0
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
;	没有
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
	mov	[ebx + pLoadLibraryA], eax	; 存起来

	lea	eax, szGetProcAddress
	add	eax, ebx
	push	eax
	push	BASE_OF_KERNEL32
	lea	eax, szKernel32Name
	add	eax, ebx
	push	eax
	call	GetApiAddress			; 取得 GetProcAddress 的入口地址
	mov	[ebx + pGetProcAddress], eax	; 存起来

	lea	eax, szUser32Name
	add	eax, ebx
	push	eax
	call	dword ptr [ebx + pLoadLibraryA]	; 获得 User32.dll 的句柄

	lea	edx, szMessageBoxA
	add	edx, ebx
	push	edx
	push	eax
	mov	eax, dword ptr [ebx + pGetProcAddress]
	call	eax	; 取得 MessageBoxA 的入口地址
	
	push    40h+1000h	; style
	lea	edx, szTitle	; title
	add	edx, ebx
	push	edx
	lea	edx, szMsg	; 消息内容
	add	edx, ebx
	push	edx
	push    0
	call    eax				; 一个消息框产生了...嘿嘿
						; 有理由为此高兴吧,因为我们没有预先引入

	BYTE	068H, 000H, 010H, 040H, 000H	; = push 00401000
						; 在写入文件时，00401000 将被替换成实际上程序原来的入口地址

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

iCodeInsideLength	DWORD	$-LABEL_CODE_INSIDE	; 要写入宿主程序的部分的长度

end start
