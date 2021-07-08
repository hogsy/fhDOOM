/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "win_local.h"


/*
==============================================================

	Clock ticks

==============================================================
*/

/*
================
Sys_GetClockTicks
================
*/
double Sys_GetClockTicks( void ) {
#if !defined( _M_IX86 )

	LARGE_INTEGER li;

	QueryPerformanceCounter( &li );

	return static_cast<double>(li.QuadPart);
	//return (double ) li.LowPart + (double) 0xFFFFFFFF * li.HighPart;

#else

	unsigned long lo, hi;

	__asm {
		push ebx
		xor eax, eax
		cpuid
		rdtsc
		mov lo, eax
		mov hi, edx
		pop ebx
	}
	return (double ) lo + (double) 0xFFFFFFFF * hi;

#endif
}

/*
================
Sys_ClockTicksPerSecond
================
*/
double Sys_ClockTicksPerSecond( void ) {
	static double ticks = 0;
#if 0

	if ( !ticks ) {
		LARGE_INTEGER li;
		QueryPerformanceFrequency( &li );
		ticks = li.QuadPart;
	}

#else

	if ( !ticks ) {
		HKEY hKey;
		LPBYTE ProcSpeed;
		DWORD buflen, ret;

		if ( !RegOpenKeyExA( HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey ) ) {
			ProcSpeed = 0;
			buflen = sizeof( ProcSpeed );
			ret = RegQueryValueExA( hKey, "~MHz", NULL, NULL, (LPBYTE) &ProcSpeed, &buflen );
			// If we don't succeed, try some other spellings.
			if ( ret != ERROR_SUCCESS ) {
				ret = RegQueryValueExA( hKey, "~Mhz", NULL, NULL, (LPBYTE) &ProcSpeed, &buflen );
			}
			if ( ret != ERROR_SUCCESS ) {
				ret = RegQueryValueExA( hKey, "~mhz", NULL, NULL, (LPBYTE) &ProcSpeed, &buflen );
			}
			RegCloseKey( hKey );
			if ( ret == ERROR_SUCCESS ) {
				ticks = (double) ((unsigned long)ProcSpeed) * 1000000;
			}
		}
	}

#endif
	return ticks;
}


/*
==============================================================

	CPU

==============================================================
*/

#if defined( _MSC_VER )
#	include <intrin.h>
#endif

static inline void CPUid( int index, int *a, int *b, int *c, int *d )
{
#if defined( _MSC_VER )
	int info[ 4 ] = {};

	// VS2005 and up
	__cpuid( info, index );

	*a = info[ 0 ];
	*b = info[ 1 ];
	*c = info[ 2 ];
	*d = info[ 3 ];
#elif defined( __GNUC__ )
	__get_cpuid( index, a, b, c, d );
#else
#error "Platform does not support CPUID"
#endif
}

enum
{
	CPU_FLAG_SSE1  = ( 1 << 25 ),
	CPU_FLAG_SSE2  = ( 1 << 26 ),
	CPU_FLAG_SSE3  = ( 1 << 0 ),
	CPU_FLAG_SSE41 = ( 1 << 19 ),
	CPU_FLAG_SSE42 = ( 1 << 20 ),
};

#define d_FXSAVE ( 1 << 24 )

static inline bool HasDAZ()
{
	int a, b, c, d;

	CPUid( 0, &a, &b, &c, &d );
	if ( a < 1 )
		return false;

	CPUid( 1, &a, &b, &c, &d );

	return ( d & d_FXSAVE ) == d_FXSAVE;
}

void Sys_FPU_SetFTZ( bool enable )
{
	_MM_SET_FLUSH_ZERO_MODE( enable ? _MM_FLUSH_ZERO_ON : _MM_FLUSH_ZERO_OFF );
}

void Sys_FPU_SetDAZ( bool enable )
{
	_MM_SET_DENORMALS_ZERO_MODE( enable ? _MM_DENORMALS_ZERO_ON : _MM_DENORMALS_ZERO_OFF );
}

#define _REG_EAX		0
#define _REG_EBX		1
#define _REG_ECX		2
#define _REG_EDX		3

/*
================
CPUID
================
*/
static void CPUID( int func, int regs[4] ) {
	CPUid( func, &regs[ 0 ], &regs[ 1 ], &regs[ 2 ], &regs[ 3 ] );
}


/*
================
IsAMD
================
*/
static bool IsAMD( void ) {
	char pstring[16];
	char processorString[13];

	// get name of processor
	CPUID( 0, ( int * ) pstring );
	processorString[0] = pstring[4];
	processorString[1] = pstring[5];
	processorString[2] = pstring[6];
	processorString[3] = pstring[7];
	processorString[4] = pstring[12];
	processorString[5] = pstring[13];
	processorString[6] = pstring[14];
	processorString[7] = pstring[15];
	processorString[8] = pstring[8];
	processorString[9] = pstring[9];
	processorString[10] = pstring[10];
	processorString[11] = pstring[11];
	processorString[12] = 0;

	if ( strcmp( processorString, "AuthenticAMD" ) == 0 ) {
		return true;
	}
	return false;
}

/*
================
HasCMOV
================
*/
static bool HasCMOV( void ) {
	int regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 15 of EDX denotes CMOV existence
	if ( regs[_REG_EDX] & ( 1 << 15 ) ) {
		return true;
	}
	return false;
}

/*
================
Has3DNow
================
*/
static bool Has3DNow( void ) {
	int regs[4];

	// check AMD-specific functions
	CPUID( 0x80000000, regs );
	if ( regs[_REG_EAX] < 0x80000000 ) {
		return false;
	}

	// bit 31 of EDX denotes 3DNow! support
	CPUID( 0x80000001, regs );
	if ( regs[_REG_EDX] & ( 1 << 31 ) ) {
		return true;
	}

	return false;
}

/*
================
HasMMX
================
*/
static bool HasMMX( void ) {
	int regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 23 of EDX denotes MMX existence
	if ( regs[_REG_EDX] & ( 1 << 23 ) ) {
		return true;
	}
	return false;
}

static bool HasSSE( void ) {
	int regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 25 of EDX denotes SSE existence
	if ( regs[_REG_EDX] & CPU_FLAG_SSE1 ) {
		return true;
	}
	return false;
}

static bool HasSSE2( void ) {
	int regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 26 of EDX denotes SSE2 existence
	if ( regs[_REG_EDX] & ( 1 << 26 ) ) {
		return true;
	}
	return false;
}

static inline bool HasSSE3()
{
	int a, b, c, d;

	CPUid( 0, &a, &b, &c, &d );
	if ( a < 1 )
		return false;

	CPUid( 1, &a, &b, &c, &d );

	return ( c & CPU_FLAG_SSE3 ) == CPU_FLAG_SSE3;
}

/*
================
LogicalProcPerPhysicalProc
================
*/
#define NUM_LOGICAL_BITS   0x00FF0000     // EBX[23:16] Bit 16-23 in ebx contains the number of logical
                                          // processors per physical processor when execute cpuid with
                                          // eax set to 1
static unsigned char LogicalProcPerPhysicalProc( void ) {
#if defined( _M_IX86 )
	unsigned int regebx = 0;
	__asm {
		mov eax, 1
		cpuid
		mov regebx, ebx
	}
	return (unsigned char) ((regebx & NUM_LOGICAL_BITS) >> 16);
#else
	SYSTEM_INFO systemInfo;
	GetNativeSystemInfo( &systemInfo );
	return systemInfo.dwNumberOfProcessors;
#endif
}

/*
================
GetAPIC_ID
================
*/
#define INITIAL_APIC_ID_BITS  0xFF000000  // EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique
                                          // initial APIC ID for the processor this code is running on.
                                          // Default value = 0xff if HT is not supported
static unsigned char GetAPIC_ID( void ) {
#if defined( _M_IX86 )
	unsigned int regebx = 0;
	__asm {
		mov eax, 1
		cpuid
		mov regebx, ebx
	}
	return (unsigned char) ((regebx & INITIAL_APIC_ID_BITS) >> 24);
#else
	return INITIAL_APIC_ID_BITS;
#endif
}

/*
================
CPUCount

	logicalNum is the number of logical CPU per physical CPU
    physicalNum is the total number of physical processor
	returns one of the HT_* flags
================
*/
#define HT_NOT_CAPABLE				0
#define HT_ENABLED					1
#define HT_DISABLED					2
#define HT_SUPPORTED_NOT_ENABLED	3
#define HT_CANNOT_DETECT			4

int CPUCount( int &logicalNum, int &physicalNum ) {
	int statusFlag;
	SYSTEM_INFO info;

	physicalNum = 1;
	logicalNum = 1;
	statusFlag = HT_NOT_CAPABLE;

	info.dwNumberOfProcessors = 0;
	GetSystemInfo (&info);

	// Number of physical processors in a non-Intel system
	// or in a 32-bit Intel system with Hyper-Threading technology disabled
	physicalNum = info.dwNumberOfProcessors;

	unsigned char HT_Enabled = 0;

	logicalNum = LogicalProcPerPhysicalProc();

	if ( logicalNum >= 1 ) {	// > 1 doesn't mean HT is enabled in the BIOS
		HANDLE hCurrentProcessHandle;
		DWORD_PTR  dwProcessAffinity;
		DWORD_PTR  dwSystemAffinity;
		DWORD_PTR  dwAffinityMask;

		// Calculate the appropriate  shifts and mask based on the
		// number of logical processors.

		unsigned char i = 1, PHY_ID_MASK  = 0xFF, PHY_ID_SHIFT = 0;

		while( i < logicalNum ) {
			i *= 2;
 			PHY_ID_MASK  <<= 1;
			PHY_ID_SHIFT++;
		}

		hCurrentProcessHandle = GetCurrentProcess();
		GetProcessAffinityMask( hCurrentProcessHandle, &dwProcessAffinity, &dwSystemAffinity );

		// Check if available process affinity mask is equal to the
		// available system affinity mask
		if ( dwProcessAffinity != dwSystemAffinity ) {
			statusFlag = HT_CANNOT_DETECT;
			physicalNum = -1;
			return statusFlag;
		}

		dwAffinityMask = 1;
		while ( dwAffinityMask != 0 && dwAffinityMask <= dwProcessAffinity ) {
			// Check if this CPU is available
			if ( dwAffinityMask & dwProcessAffinity ) {
				if ( SetProcessAffinityMask( hCurrentProcessHandle, dwAffinityMask ) ) {
					unsigned char APIC_ID, LOG_ID, PHY_ID;

					Sleep( 0 ); // Give OS time to switch CPU

					APIC_ID = GetAPIC_ID();
					LOG_ID  = APIC_ID & ~PHY_ID_MASK;
					PHY_ID  = APIC_ID >> PHY_ID_SHIFT;

					if ( LOG_ID != 0 ) {
						HT_Enabled = 1;
					}
				}
			}
			dwAffinityMask = dwAffinityMask << 1;
		}

		// Reset the processor affinity
		SetProcessAffinityMask( hCurrentProcessHandle, dwProcessAffinity );

		if ( logicalNum == 1 ) {  // Normal P4 : HT is disabled in hardware
			statusFlag = HT_DISABLED;
		} else {
			if ( HT_Enabled ) {
				// Total physical processors in a Hyper-Threading enabled system.
				physicalNum /= logicalNum;
				statusFlag = HT_ENABLED;
			} else {
				statusFlag = HT_SUPPORTED_NOT_ENABLED;
			}
		}
	}
	return statusFlag;
}

/*
================
HasHTT
================
*/
static bool HasHTT( void ) {
	int regs[4];
	int logicalNum, physicalNum, HTStatusFlag;

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 28 of EDX denotes HTT existence
	if ( !( regs[_REG_EDX] & ( 1 << 28 ) ) ) {
		return false;
	}

	HTStatusFlag = CPUCount( logicalNum, physicalNum );
	if ( HTStatusFlag != HT_ENABLED ) {
		return false;
	}
	return true;
}

/*
================
Sys_GetCPUId
================
*/
cpuid_t Sys_GetCPUId( void ) {
	int flags;

	// check for an AMD
	if ( IsAMD() ) {
		flags = CPUID_AMD;
	} else {
		flags = CPUID_INTEL;
	}

	// check for Multi Media Extensions
	if ( HasMMX() ) {
		flags |= CPUID_MMX;
	}

	// check for 3DNow!
	if ( Has3DNow() ) {
		flags |= CPUID_3DNOW;
	}

	// check for Streaming SIMD Extensions
	if ( HasSSE() ) {
		flags |= CPUID_SSE | CPUID_FTZ;
	}

	// check for Streaming SIMD Extensions 2
	if ( HasSSE2() ) {
		flags |= CPUID_SSE2;
	}

	// check for Streaming SIMD Extensions 3 aka Prescott's New Instructions
	if ( HasSSE3() ) {
		flags |= CPUID_SSE3;
	}

	// check for Hyper-Threading Technology
	if ( HasHTT() ) {
		flags |= CPUID_HTT;
	}

	// check for Conditional Move (CMOV) and fast floating point comparison (FCOMI) instructions
	if ( HasCMOV() ) {
		flags |= CPUID_CMOV;
	}
	
	// check for Denormals-Are-Zero mode
	if ( HasDAZ() ) {
		flags |= CPUID_DAZ;
	}

	return (cpuid_t)flags;
}


/*
===============================================================================

	FPU

===============================================================================
*/

typedef struct bitFlag_s {
	char *		name;
	int			bit;
} bitFlag_t;

static byte fpuState[128], *statePtr = fpuState;
static char fpuString[2048];
static bitFlag_t controlWordFlags[] = {
	{ "Invalid operation", 0 },
	{ "Denormalized operand", 1 },
	{ "Divide-by-zero", 2 },
	{ "Numeric overflow", 3 },
	{ "Numeric underflow", 4 },
	{ "Inexact result (precision)", 5 },
	{ "Infinity control", 12 },
	{ "", 0 }
};
static char *precisionControlField[] = {
	"Single Precision (24-bits)",
	"Reserved",
	"Double Precision (53-bits)",
	"Double Extended Precision (64-bits)"
};
static char *roundingControlField[] = {
	"Round to nearest",
	"Round down",
	"Round up",
	"Round toward zero"
};
static bitFlag_t statusWordFlags[] = {
	{ "Invalid operation", 0 },
	{ "Denormalized operand", 1 },
	{ "Divide-by-zero", 2 },
	{ "Numeric overflow", 3 },
	{ "Numeric underflow", 4 },
	{ "Inexact result (precision)", 5 },
	{ "Stack fault", 6 },
	{ "Error summary status", 7 },
	{ "FPU busy", 15 },
	{ "", 0 }
};

/*
===============
Sys_FPU_PrintStateFlags
===============
*/
int Sys_FPU_PrintStateFlags( char *ptr, int ctrl, int stat, int tags, int inof, int inse, int opof, int opse ) {
	int i, length = 0;

	length += sprintf( ptr+length,	"CTRL = %08x\n"
									"STAT = %08x\n"
									"TAGS = %08x\n"
									"INOF = %08x\n"
									"INSE = %08x\n"
									"OPOF = %08x\n"
									"OPSE = %08x\n"
									"\n",
									ctrl, stat, tags, inof, inse, opof, opse );

	length += sprintf( ptr+length, "Control Word:\n" );
	for ( i = 0; controlWordFlags[i].name[0]; i++ ) {
		length += sprintf( ptr+length, "  %-30s = %s\n", controlWordFlags[i].name, ( ctrl & ( 1 << controlWordFlags[i].bit ) ) ? "true" : "false" );
	}
	length += sprintf( ptr+length, "  %-30s = %s\n", "Precision control", precisionControlField[(ctrl>>8)&3] );
	length += sprintf( ptr+length, "  %-30s = %s\n", "Rounding control", roundingControlField[(ctrl>>10)&3] );

	length += sprintf( ptr+length, "Status Word:\n" );
	for ( i = 0; statusWordFlags[i].name[0]; i++ ) {
		ptr += sprintf( ptr+length, "  %-30s = %s\n", statusWordFlags[i].name, ( stat & ( 1 << statusWordFlags[i].bit ) ) ? "true" : "false" );
	}
	length += sprintf( ptr+length, "  %-30s = %d%d%d%d\n", "Condition code", (stat>>8)&1, (stat>>9)&1, (stat>>10)&1, (stat>>14)&1 );
	length += sprintf( ptr+length, "  %-30s = %d\n", "Top of stack pointer", (stat>>11)&7 );

	return length;
}

/*
===============
Sys_FPU_StackIsEmpty
===============
*/
bool Sys_FPU_StackIsEmpty( void ) {
#if defined( _M_IX86 )
	__asm {
		mov			eax, statePtr
		fnstenv		[eax]
		mov			eax, [eax+8]
		xor			eax, 0xFFFFFFFF
		and			eax, 0x0000FFFF
		jz			empty
	}
	return false;
empty:
	return true;
#else
	return true;
#endif
}

/*
===============
Sys_FPU_GetState

  gets the FPU state without changing the state
===============
*/
const char *Sys_FPU_GetState( void ) {
#if defined( _M_IX86 )
	double fpuStack[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double *fpuStackPtr = fpuStack;
	int i, numValues;
	char *ptr;

	__asm {
		mov			esi, statePtr
		mov			edi, fpuStackPtr
		fnstenv		[esi]
		mov			esi, [esi+8]
		xor			esi, 0xFFFFFFFF
		mov			edx, (3<<14)
		xor			eax, eax
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fst			qword ptr [edi+0]
		inc			eax
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st(1)
		fst			qword ptr [edi+8]
		inc			eax
		fxch		st(1)
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st(2)
		fst			qword ptr [edi+16]
		inc			eax
		fxch		st(2)
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st(3)
		fst			qword ptr [edi+24]
		inc			eax
		fxch		st(3)
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st(4)
		fst			qword ptr [edi+32]
		inc			eax
		fxch		st(4)
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st(5)
		fst			qword ptr [edi+40]
		inc			eax
		fxch		st(5)
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st(6)
		fst			qword ptr [edi+48]
		inc			eax
		fxch		st(6)
		shr			edx, 2
		mov			ecx, esi
		and			ecx, edx
		jz			done
		fxch		st(7)
		fst			qword ptr [edi+56]
		inc			eax
		fxch		st(7)
	done:
		mov			numValues, eax
	}

	int ctrl = *(int *)&fpuState[0];
	int stat = *(int *)&fpuState[4];
	int tags = *(int *)&fpuState[8];
	int inof = *(int *)&fpuState[12];
	int inse = *(int *)&fpuState[16];
	int opof = *(int *)&fpuState[20];
	int opse = *(int *)&fpuState[24];

	ptr = fpuString;
	ptr += sprintf( ptr,"FPU State:\n"
						"num values on stack = %d\n", numValues );
	for ( i = 0; i < 8; i++ ) {
		ptr += sprintf( ptr, "ST%d = %1.10e\n", i, fpuStack[i] );
	}

	Sys_FPU_PrintStateFlags( ptr, ctrl, stat, tags, inof, inse, opof, opse );

	return fpuString;
#else
	return "xxxxx";
#endif
}

/*
===============
Sys_FPU_EnableExceptions
===============
*/
void Sys_FPU_EnableExceptions( int exceptions ) {
#if defined( _M_IX86 )
	__asm {
		mov			eax, statePtr
		mov			ecx, exceptions
		and			cx, 63
		not			cx
		fnstcw		word ptr [eax]
		mov			bx, word ptr [eax]
		or			bx, 63
		and			bx, cx
		mov			word ptr [eax], bx
		fldcw		word ptr [eax]
	}
#endif
}

/*
===============
Sys_FPU_SetPrecision
===============
*/
void Sys_FPU_SetPrecision( int precision ) {
#ifdef _MSC_VER
#if defined( _M_IX86 )
	short precisionBitTable[4] = { 0, 1, 3, 0 };
	short precisionBits = precisionBitTable[precision & 3] << 8;
	short precisionMask = ~( ( 1 << 9 ) | ( 1 << 8 ) );

	__asm {
		mov			eax, statePtr
		mov			cx, precisionBits
		fnstcw		word ptr [eax]
		mov			bx, word ptr [eax]
		and			bx, precisionMask
		or			bx, cx
		mov			word ptr [eax], bx
		fldcw		word ptr [eax]
	}
#endif
#endif
}
