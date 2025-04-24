// dear py, v1.85
// (internal structures/api)

// You may use this file to debug, understand or extend py features but we don't provide any guarantee of forward compatibility!
// Set:
//   #define py_DEFINE_MATH_OPERATORS
// To implement maths operators for ImVec2 (disabled by default to not collide with using IM_VEC2_CLASS_EXTRA along with your own math types+operators)

/*

Index of this file:

// [SECTION] Header mess
// [SECTION] Forward declarations
// [SECTION] Context pointer
// [SECTION] STB libraries includes
// [SECTION] Macros
// [SECTION] Generic helpers
// [SECTION] ImDrawList support
// [SECTION] Widgets support: flags, enums, data structures
// [SECTION] Navigation support
// [SECTION] Columns support
// [SECTION] Multi-select support
// [SECTION] Docking support
// [SECTION] Viewport support
// [SECTION] Settings support
// [SECTION] Metrics, Debug tools
// [SECTION] Generic context hooks
// [SECTION] pyContext (main py context)
// [SECTION] pyWindowTempData, pyWindow
// [SECTION] Tab bar, Tab item support
// [SECTION] Table support
// [SECTION] py internal API
// [SECTION] ImFontAtlas internal API
// [SECTION] Test Engine specific hooks (py_test_engine)

*/

#pragma once
#ifndef py_DISABLE

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

#ifndef py_VERSION
#include "py.h"
#endif

#include <stdio.h>      // FILE*, sscanf
#include <stdlib.h>     // NULL, malloc, free, qsort, atoi, atof
#include <math.h>       // sqrtf, fabsf, fmodf, powf, floorf, ceilf, cosf, sinf
#include <limits.h>     // INT_MIN, INT_MAX

// Enable SSE intrinsics if available
#if (defined __SSE__ || defined __x86_64__ || defined _M_X64) && !defined(py_DISABLE_SSE)
#define py_ENABLE_SSE
#include <immintrin.h>
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4251)     // class 'xxx' needs to have dll-interface to be used by clients of struct 'xxx' // when py_API is set to__declspec(dllexport)
#pragma warning (disable: 26812)    // The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum' (Enum.3). [MSVC Static Analyzer)
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
#if defined(_MSC_VER) && _MSC_VER >= 1922 // MSVC 2019 16.2 or later
#pragma warning (disable: 5054)     // operator '|': deprecated between enumerations of different types
#endif
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe // storing and comparing against same constants ok, for ImFloorSigned()
#pragma clang diagnostic ignored "-Wunused-function"                // for stb_textedit.h
#pragma clang diagnostic ignored "-Wmissing-prototypes"             // for stb_textedit.h
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' may lose precision
#pragma clang diagnostic ignored "-Wmissing-noreturn"               // warning: function 'xxx' could be declared with attribute 'noreturn'
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"              // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wclass-memaccess"      // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

// Legacy defines
#ifdef py_DISABLE_FORMAT_STRING_FUNCTIONS            // Renamed in 1.74
#error Use py_DISABLE_DEFAULT_FORMAT_FUNCTIONS
#endif
#ifdef py_DISABLE_MATH_FUNCTIONS                     // Renamed in 1.74
#error Use py_DISABLE_DEFAULT_MATH_FUNCTIONS
#endif

// Enable stb_truetype by default unless FreeType is enabled.
// You can compile with both by defining both py_ENABLE_FREETYPE and py_ENABLE_STB_TRUETYPE together.
#ifndef py_ENABLE_FREETYPE
#define py_ENABLE_STB_TRUETYPE
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

struct ImBitVector;                 // Store 1-bit per value
struct ImRect;                      // An axis-aligned rectangle (2 points)
struct ImDrawDataBuilder;           // Helper to build a ImDrawData instance
struct ImDrawListSharedData;        // Data shared between all ImDrawList instances
struct pyColorMod;               // Stacked color modifier, backup of modified data so we can restore it
struct pyContext;                // Main Dear py context
struct pyContextHook;            // Hook for extensions like pyTestEngine
struct pyDataTypeInfo;           // Type information associated to a pyDataType enum
struct pyGroupData;              // Stacked storage data for BeginGroup()/EndGroup()
struct pyInputTextState;         // Internal state of the currently focused/edited text input box
struct pyLastItemData;           // Status storage for last submitted items
struct pyMenuColumns;            // Simple column measurement, currently used for MenuItem() only
struct pyNavItemData;            // Result of a gamepad/keyboard directional navigation move query result
struct pyMetricsConfig;          // Storage for ShowMetricsWindow() and DebugNodeXXX() functions
struct pyNextWindowData;         // Storage for SetNextWindow** functions
struct pyNextItemData;           // Storage for SetNextItem** functions
struct pyOldColumnData;          // Storage data for a single column for legacy Columns() api
struct pyOldColumns;             // Storage data for a columns set for legacy Columns() api
struct pyPopupData;              // Storage for current popup stack
struct pySettingsHandler;        // Storage for one type registered in the .ini file
struct pyStackSizes;             // Storage of stack sizes for debugging/asserting
struct pyStyleMod;               // Stacked style modifier, backup of modified data so we can restore it
struct pyTabBar;                 // Storage for a tab bar
struct pyTabItem;                // Storage for a tab item (within a tab bar)
struct pyTable;                  // Storage for a table
struct pyTableColumn;            // Storage for one column of a table
struct pyTableTempData;          // Temporary storage for one table (one per table in the stack), shared between tables.
struct pyTableSettings;          // Storage for a table .ini settings
struct pyTableColumnsSettings;   // Storage for a column .ini settings
struct pyWindow;                 // Storage for one window
struct pyWindowTempData;         // Temporary storage for one window (that's the data which in theory we could ditch at the end of the frame, in practice we currently keep it for each window)
struct pyWindowSettings;         // Storage for a window .ini settings (we keep one of those even if the actual window wasn't instanced during this session)

// Use your programming IDE "Go to definition" facility on the names of the center columns to find the actual flags/enum lists.
typedef int pyLayoutType;            // -> enum pyLayoutType_         // Enum: Horizontal or vertical
typedef int pyActivateFlags;         // -> enum pyActivateFlags_      // Flags: for navigation/focus function (will be for ActivateItem() later)
typedef int pyItemFlags;             // -> enum pyItemFlags_          // Flags: for PushItemFlag()
typedef int pyItemStatusFlags;       // -> enum pyItemStatusFlags_    // Flags: for DC.LastItemStatusFlags
typedef int pyOldColumnFlags;        // -> enum pyOldColumnFlags_     // Flags: for BeginColumns()
typedef int pyNavHighlightFlags;     // -> enum pyNavHighlightFlags_  // Flags: for RenderNavHighlight()
typedef int pyNavDirSourceFlags;     // -> enum pyNavDirSourceFlags_  // Flags: for GetNavInputAmount2d()
typedef int pyNavMoveFlags;          // -> enum pyNavMoveFlags_       // Flags: for navigation requests
typedef int pyNextItemDataFlags;     // -> enum pyNextItemDataFlags_  // Flags: for SetNextItemXXX() functions
typedef int pyNextWindowDataFlags;   // -> enum pyNextWindowDataFlags_// Flags: for SetNextWindowXXX() functions
typedef int pyScrollFlags;           // -> enum pyScrollFlags_        // Flags: for ScrollToItem() and navigation requests
typedef int pySeparatorFlags;        // -> enum pySeparatorFlags_     // Flags: for SeparatorEx()
typedef int pyTextFlags;             // -> enum pyTextFlags_          // Flags: for TextEx()
typedef int pyTooltipFlags;          // -> enum pyTooltipFlags_       // Flags: for BeginTooltipEx()

typedef void (*pyErrorLogCallback)(void* user_data, const char* fmt, ...);

//-----------------------------------------------------------------------------
// [SECTION] Context pointer
// See implementation of this variable in py.cpp for comments and details.
//-----------------------------------------------------------------------------

#ifndef Gpy
extern py_API pyContext* Gpy;  // Current implicit context pointer
#endif

//-------------------------------------------------------------------------
// [SECTION] STB libraries includes
//-------------------------------------------------------------------------

namespace ImStb
{

#undef STB_TEXTEDIT_STRING
#undef STB_TEXTEDIT_CHARTYPE
#define STB_TEXTEDIT_STRING             pyInputTextState
#define STB_TEXTEDIT_CHARTYPE           ImWchar
#define STB_TEXTEDIT_GETWIDTH_NEWLINE   (-1.0f)
#define STB_TEXTEDIT_UNDOSTATECOUNT     99
#define STB_TEXTEDIT_UNDOCHARCOUNT      999
#include "imstb_textedit.h"

} // namespace ImStb

//-----------------------------------------------------------------------------
// [SECTION] Macros
//-----------------------------------------------------------------------------

// Debug Logging
#ifndef py_DEBUG_LOG
#define py_DEBUG_LOG(_FMT,...)       printf("[%05d] " _FMT, Gpy->FrameCount, __VA_ARGS__)
#endif

// Debug Logging for selected systems. Remove the '((void)0) //' to enable.
//#define py_DEBUG_LOG_POPUP         py_DEBUG_LOG // Enable log
//#define py_DEBUG_LOG_NAV           py_DEBUG_LOG // Enable log
#define py_DEBUG_LOG_POPUP(...)      ((void)0)       // Disable log
#define py_DEBUG_LOG_NAV(...)        ((void)0)       // Disable log

// Static Asserts
#if (__cplusplus >= 201100) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201100)
#define IM_STATIC_ASSERT(_COND)         static_assert(_COND, "")
#else
#define IM_STATIC_ASSERT(_COND)         typedef char static_assertion_##__line__[(_COND)?1:-1]
#endif

// "Paranoid" Debug Asserts are meant to only be enabled during specific debugging/work, otherwise would slow down the code too much.
// We currently don't have many of those so the effect is currently negligible, but onward intent to add more aggressive ones in the code.
//#define py_DEBUG_PARANOID
#ifdef py_DEBUG_PARANOID
#define IM_ASSERT_PARANOID(_EXPR)       IM_ASSERT(_EXPR)
#else
#define IM_ASSERT_PARANOID(_EXPR)
#endif

// Error handling
// Down the line in some frameworks/languages we would like to have a way to redirect those to the programmer and recover from more faults.
#ifndef IM_ASSERT_USER_ERROR
#define IM_ASSERT_USER_ERROR(_EXP,_MSG) IM_ASSERT((_EXP) && _MSG)   // Recoverable User Error
#endif

// Misc Macros
#define IM_PI                           3.14159265358979323846f
#ifdef _WIN32
#define IM_NEWLINE                      "\r\n"   // Play it nice with Windows users (Update: since 2018-05, Notepad finally appears to support Unix-style carriage returns!)
#else
#define IM_NEWLINE                      "\n"
#endif
#define IM_TABSIZE                      (4)
#define IM_MEMALIGN(_OFF,_ALIGN)        (((_OFF) + (_ALIGN - 1)) & ~(_ALIGN - 1))               // Memory align e.g. IM_ALIGN(0,4)=0, IM_ALIGN(1,4)=4, IM_ALIGN(4,4)=4, IM_ALIGN(5,4)=8
#define IM_F32_TO_INT8_UNBOUND(_VAL)    ((int)((_VAL) * 255.0f + ((_VAL)>=0 ? 0.5f : -0.5f)))   // Unsaturated, for display purpose
#define IM_F32_TO_INT8_SAT(_VAL)        ((int)(ImSaturate(_VAL) * 255.0f + 0.5f))               // Saturated, always output 0..255
#define IM_FLOOR(_VAL)                  ((float)(int)(_VAL))                                    // ImFloor() is not inlined in MSVC debug builds
#define IM_ROUND(_VAL)                  ((float)(int)((_VAL) + 0.5f))                           //

// Enforce cdecl calling convention for functions called by the standard library, in case compilation settings changed the default to e.g. __vectorcall
#ifdef _MSC_VER
#define py_CDECL __cdecl
#else
#define py_CDECL
#endif

// Warnings
#if defined(_MSC_VER) && !defined(__clang__)
#define IM_MSVC_WARNING_SUPPRESS(XXXX)  __pragma(warning(suppress: XXXX))
#else
#define IM_MSVC_WARNING_SUPPRESS(XXXX)
#endif

// Debug Tools
// Use 'Metrics->Tools->Item Picker' to break into the call-stack of a specific item.
#ifndef IM_DEBUG_BREAK
#if defined(__clang__)
#define IM_DEBUG_BREAK()    __builtin_debugtrap()
#elif defined (_MSC_VER)
#define IM_DEBUG_BREAK()    __debugbreak()
#else
#define IM_DEBUG_BREAK()    IM_ASSERT(0)    // It is expected that you define IM_DEBUG_BREAK() into something that will break nicely in a debugger!
#endif
#endif // #ifndef IM_DEBUG_BREAK

//-----------------------------------------------------------------------------
// [SECTION] Generic helpers
// Note that the ImXXX helpers functions are lower-level than py functions.
// py functions or the py context are never called/used from other ImXXX functions.
//-----------------------------------------------------------------------------
// - Helpers: Hashing
// - Helpers: Sorting
// - Helpers: Bit manipulation
// - Helpers: String, Formatting
// - Helpers: UTF-8 <> wchar conversions
// - Helpers: ImVec2/ImVec4 operators
// - Helpers: Maths
// - Helpers: Geometry
// - Helper: ImVec1
// - Helper: ImVec2ih
// - Helper: ImRect
// - Helper: ImBitArray
// - Helper: ImBitVector
// - Helper: ImSpan<>, ImSpanAllocator<>
// - Helper: ImPool<>
// - Helper: ImChunkStream<>
//-----------------------------------------------------------------------------

// Helpers: Hashing
py_API pyID       ImHashData(const void* data, size_t data_size, ImU32 seed = 0);
py_API pyID       ImHashStr(const char* data, size_t data_size = 0, ImU32 seed = 0);
#ifndef py_DISABLE_OBSOLETE_FUNCTIONS
static inline pyID   ImHash(const void* data, int size, ImU32 seed = 0) { return size ? ImHashData(data, (size_t)size, seed) : ImHashStr((const char*)data, 0, seed); } // [moved to ImHashStr/ImHashData in 1.68]
#endif

// Helpers: Sorting
#define ImQsort         qsort

// Helpers: Color Blending
py_API ImU32         ImAlphaBlendColors(ImU32 col_a, ImU32 col_b);

// Helpers: Bit manipulation
static inline bool      ImIsPowerOfTwo(int v)           { return v != 0 && (v & (v - 1)) == 0; }
static inline bool      ImIsPowerOfTwo(ImU64 v)         { return v != 0 && (v & (v - 1)) == 0; }
static inline int       ImUpperPowerOfTwo(int v)        { v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16; v++; return v; }

// Helpers: String, Formatting
py_API int           ImStricmp(const char* str1, const char* str2);
py_API int           ImStrnicmp(const char* str1, const char* str2, size_t count);
py_API void          ImStrncpy(char* dst, const char* src, size_t count);
py_API char*         ImStrdup(const char* str);
py_API char*         ImStrdupcpy(char* dst, size_t* p_dst_size, const char* str);
py_API const char*   ImStrchrRange(const char* str_begin, const char* str_end, char c);
py_API int           ImStrlenW(const ImWchar* str);
py_API const char*   ImStreolRange(const char* str, const char* str_end);                // End end-of-line
py_API const ImWchar*ImStrbolW(const ImWchar* buf_mid_line, const ImWchar* buf_begin);   // Find beginning-of-line
py_API const char*   ImStristr(const char* haystack, const char* haystack_end, const char* needle, const char* needle_end);
py_API void          ImStrTrimBlanks(char* str);
py_API const char*   ImStrSkipBlank(const char* str);
py_API int           ImFormatString(char* buf, size_t buf_size, const char* fmt, ...) IM_FMTARGS(3);
py_API int           ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args) IM_FMTLIST(3);
py_API const char*   ImParseFormatFindStart(const char* format);
py_API const char*   ImParseFormatFindEnd(const char* format);
py_API const char*   ImParseFormatTrimDecorations(const char* format, char* buf, size_t buf_size);
py_API int           ImParseFormatPrecision(const char* format, int default_value);
static inline bool      ImCharIsBlankA(char c)          { return c == ' ' || c == '\t'; }
static inline bool      ImCharIsBlankW(unsigned int c)  { return c == ' ' || c == '\t' || c == 0x3000; }

// Helpers: UTF-8 <> wchar conversions
py_API const char*   ImTextCharToUtf8(char out_buf[5], unsigned int c);                                                      // return out_buf
py_API int           ImTextStrToUtf8(char* out_buf, int out_buf_size, const ImWchar* in_text, const ImWchar* in_text_end);   // return output UTF-8 bytes count
py_API int           ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end);               // read one character. return input UTF-8 bytes count
py_API int           ImTextStrFromUtf8(ImWchar* out_buf, int out_buf_size, const char* in_text, const char* in_text_end, const char** in_remaining = NULL);   // return input UTF-8 bytes count
py_API int           ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end);                                 // return number of UTF-8 code-points (NOT bytes count)
py_API int           ImTextCountUtf8BytesFromChar(const char* in_text, const char* in_text_end);                             // return number of bytes to express one char in UTF-8
py_API int           ImTextCountUtf8BytesFromStr(const ImWchar* in_text, const ImWchar* in_text_end);                        // return number of bytes to express string in UTF-8

// Helpers: ImVec2/ImVec4 operators
// We are keeping those disabled by default so they don't leak in user space, to allow user enabling implicit cast operators between ImVec2 and their own types (using IM_VEC2_CLASS_EXTRA etc.)
// We unfortunately don't have a unary- operator for ImVec2 because this would needs to be defined inside the class itself.
#ifdef py_DEFINE_MATH_OPERATORS
IM_MSVC_RUNTIME_CHECKS_OFF
static inline ImVec2 operator*(const ImVec2& lhs, const float rhs)              { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs)              { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs)                  { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs)                  { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
IM_MSVC_RUNTIME_CHECKS_RESTORE
#endif

// Helpers: File System
#ifdef py_DISABLE_FILE_FUNCTIONS
#define py_DISABLE_DEFAULT_FILE_FUNCTIONS
typedef void* ImFileHandle;
static inline ImFileHandle  ImFileOpen(const char*, const char*)                    { return NULL; }
static inline bool          ImFileClose(ImFileHandle)                               { return false; }
static inline ImU64         ImFileGetSize(ImFileHandle)                             { return (ImU64)-1; }
static inline ImU64         ImFileRead(void*, ImU64, ImU64, ImFileHandle)           { return 0; }
static inline ImU64         ImFileWrite(const void*, ImU64, ImU64, ImFileHandle)    { return 0; }
#endif
#ifndef py_DISABLE_DEFAULT_FILE_FUNCTIONS
typedef FILE* ImFileHandle;
py_API ImFileHandle      ImFileOpen(const char* filename, const char* mode);
py_API bool              ImFileClose(ImFileHandle file);
py_API ImU64             ImFileGetSize(ImFileHandle file);
py_API ImU64             ImFileRead(void* data, ImU64 size, ImU64 count, ImFileHandle file);
py_API ImU64             ImFileWrite(const void* data, ImU64 size, ImU64 count, ImFileHandle file);
#else
#define py_DISABLE_TTY_FUNCTIONS // Can't use stdout, fflush if we are not using default file functions
#endif
py_API void*             ImFileLoadToMemory(const char* filename, const char* mode, size_t* out_file_size = NULL, int padding_bytes = 0);

// Helpers: Maths
IM_MSVC_RUNTIME_CHECKS_OFF
// - Wrapper for standard libs functions. (Note that py_demo.cpp does _not_ use them to keep the code easy to copy)
#ifndef py_DISABLE_DEFAULT_MATH_FUNCTIONS
#define ImFabs(X)           fabsf(X)
#define ImSqrt(X)           sqrtf(X)
#define ImFmod(X, Y)        fmodf((X), (Y))
#define ImCos(X)            cosf(X)
#define ImSin(X)            sinf(X)
#define ImAcos(X)           acosf(X)
#define ImAtan2(Y, X)       atan2f((Y), (X))
#define ImAtof(STR)         atof(STR)
//#define ImFloorStd(X)     floorf(X)           // We use our own, see ImFloor() and ImFloorSigned()
#define ImCeil(X)           ceilf(X)
static inline float  ImPow(float x, float y)    { return powf(x, y); }          // DragBehaviorT/SliderBehaviorT uses ImPow with either float/double and need the precision
static inline double ImPow(double x, double y)  { return pow(x, y); }
static inline float  ImLog(float x)             { return logf(x); }             // DragBehaviorT/SliderBehaviorT uses ImLog with either float/double and need the precision
static inline double ImLog(double x)            { return log(x); }
static inline int    ImAbs(int x)               { return x < 0 ? -x : x; }
static inline float  ImAbs(float x)             { return fabsf(x); }
static inline double ImAbs(double x)            { return fabs(x); }
static inline float  ImSign(float x)            { return (x < 0.0f) ? -1.0f : ((x > 0.0f) ? 1.0f : 0.0f); } // Sign operator - returns -1, 0 or 1 based on sign of argument
static inline double ImSign(double x)           { return (x < 0.0) ? -1.0 : ((x > 0.0) ? 1.0 : 0.0); }
#ifdef py_ENABLE_SSE
static inline float  ImRsqrt(float x)           { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))); }
#else
static inline float  ImRsqrt(float x)           { return 1.0f / sqrtf(x); }
#endif
static inline double ImRsqrt(double x)          { return 1.0 / sqrt(x); }
#endif
// - ImMin/ImMax/ImClamp/ImLerp/ImSwap are used by widgets which support variety of types: signed/unsigned int/long long float/double
// (Exceptionally using templates here but we could also redefine them for those types)
template<typename T> static inline T ImMin(T lhs, T rhs)                        { return lhs < rhs ? lhs : rhs; }
template<typename T> static inline T ImMax(T lhs, T rhs)                        { return lhs >= rhs ? lhs : rhs; }
template<typename T> static inline T ImClamp(T v, T mn, T mx)                   { return (v < mn) ? mn : (v > mx) ? mx : v; }
template<typename T> static inline T ImLerp(T a, T b, float t)                  { return (T)(a + (b - a) * t); }
template<typename T> static inline void ImSwap(T& a, T& b)                      { T tmp = a; a = b; b = tmp; }
template<typename T> static inline T ImAddClampOverflow(T a, T b, T mn, T mx)   { if (b < 0 && (a < mn - b)) return mn; if (b > 0 && (a > mx - b)) return mx; return a + b; }
template<typename T> static inline T ImSubClampOverflow(T a, T b, T mn, T mx)   { if (b > 0 && (a < mn + b)) return mn; if (b < 0 && (a > mx + b)) return mx; return a - b; }
// - Misc maths helpers
static inline ImVec2 ImMin(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImMax(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImClamp(const ImVec2& v, const ImVec2& mn, ImVec2 mx)      { return ImVec2((v.x < mn.x) ? mn.x : (v.x > mx.x) ? mx.x : v.x, (v.y < mn.y) ? mn.y : (v.y > mx.y) ? mx.y : v.y); }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, float t)          { return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t); }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, const ImVec2& t)  { return ImVec2(a.x + (b.x - a.x) * t.x, a.y + (b.y - a.y) * t.y); }
static inline ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t)          { return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t); }
static inline float  ImSaturate(float f)                                        { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
static inline float  ImLengthSqr(const ImVec2& lhs)                             { return (lhs.x * lhs.x) + (lhs.y * lhs.y); }
static inline float  ImLengthSqr(const ImVec4& lhs)                             { return (lhs.x * lhs.x) + (lhs.y * lhs.y) + (lhs.z * lhs.z) + (lhs.w * lhs.w); }
static inline float  ImInvLength(const ImVec2& lhs, float fail_value)           { float d = (lhs.x * lhs.x) + (lhs.y * lhs.y); if (d > 0.0f) return ImRsqrt(d); return fail_value; }
static inline float  ImFloor(float f)                                           { return (float)(int)(f); }
static inline float  ImFloorSigned(float f)                                     { return (float)((f >= 0 || (int)f == f) ? (int)f : (int)f - 1); } // Decent replacement for floorf()
static inline ImVec2 ImFloor(const ImVec2& v)                                   { return ImVec2((float)(int)(v.x), (float)(int)(v.y)); }
static inline int    ImModPositive(int a, int b)                                { return (a + b) % b; }
static inline float  ImDot(const ImVec2& a, const ImVec2& b)                    { return a.x * b.x + a.y * b.y; }
static inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)        { return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a); }
static inline float  ImLinearSweep(float current, float target, float speed)    { if (current < target) return ImMin(current + speed, target); if (current > target) return ImMax(current - speed, target); return current; }
static inline ImVec2 ImMul(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
IM_MSVC_RUNTIME_CHECKS_RESTORE

// Helpers: Geometry
py_API ImVec2     ImBezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t);
py_API ImVec2     ImBezierCubicClosestPoint(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p, int num_segments);       // For curves with explicit number of segments
py_API ImVec2     ImBezierCubicClosestPointCasteljau(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p, float tess_tol);// For auto-tessellated curves you can use tess_tol = style.CurveTessellationTol
py_API ImVec2     ImBezierQuadraticCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float t);
py_API ImVec2     ImLineClosestPoint(const ImVec2& a, const ImVec2& b, const ImVec2& p);
py_API bool       ImTriangleContainsPoint(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p);
py_API ImVec2     ImTriangleClosestPoint(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p);
py_API void       ImTriangleBarycentricCoords(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p, float& out_u, float& out_v, float& out_w);
inline float         ImTriangleArea(const ImVec2& a, const ImVec2& b, const ImVec2& c) { return ImFabs((a.x * (b.y - c.y)) + (b.x * (c.y - a.y)) + (c.x * (a.y - b.y))) * 0.5f; }
py_API pyDir   ImGetDirQuadrantFromDelta(float dx, float dy);

// Helper: ImVec1 (1D vector)
// (this odd construct is used to facilitate the transition between 1D and 2D, and the maintenance of some branches/patches)
IM_MSVC_RUNTIME_CHECKS_OFF
struct ImVec1
{
    float   x;
    ImVec1()         { x = 0.0f; }
    ImVec1(float _x) { x = _x; }
};

// Helper: ImVec2ih (2D vector, half-size integer, for long-term packed storage)
struct ImVec2ih
{
    short   x, y;
    ImVec2ih()                           { x = y = 0; }
    ImVec2ih(short _x, short _y)         { x = _x; y = _y; }
    explicit ImVec2ih(const ImVec2& rhs) { x = (short)rhs.x; y = (short)rhs.y; }
};

// Helper: ImRect (2D axis aligned bounding-box)
// NB: we can't rely on ImVec2 math operators being available here!
struct py_API ImRect
{
    ImVec2      Min;    // Upper-left
    ImVec2      Max;    // Lower-right

    ImRect()                                        : Min(0.0f, 0.0f), Max(0.0f, 0.0f)  {}
    ImRect(const ImVec2& min, const ImVec2& max)    : Min(min), Max(max)                {}
    ImRect(const ImVec4& v)                         : Min(v.x, v.y), Max(v.z, v.w)      {}
    ImRect(float x1, float y1, float x2, float y2)  : Min(x1, y1), Max(x2, y2)          {}

    ImVec2      GetCenter() const                   { return ImVec2((Min.x + Max.x) * 0.5f, (Min.y + Max.y) * 0.5f); }
    ImVec2      GetSize() const                     { return ImVec2(Max.x - Min.x, Max.y - Min.y); }
    float       GetWidth() const                    { return Max.x - Min.x; }
    float       GetHeight() const                   { return Max.y - Min.y; }
    float       GetArea() const                     { return (Max.x - Min.x) * (Max.y - Min.y); }
    ImVec2      GetTL() const                       { return Min; }                   // Top-left
    ImVec2      GetTR() const                       { return ImVec2(Max.x, Min.y); }  // Top-right
    ImVec2      GetBL() const                       { return ImVec2(Min.x, Max.y); }  // Bottom-left
    ImVec2      GetBR() const                       { return Max; }                   // Bottom-right
    bool        Contains(const ImVec2& p) const     { return p.x     >= Min.x && p.y     >= Min.y && p.x     <  Max.x && p.y     <  Max.y; }
    bool        Contains(const ImRect& r) const     { return r.Min.x >= Min.x && r.Min.y >= Min.y && r.Max.x <= Max.x && r.Max.y <= Max.y; }
    bool        Overlaps(const ImRect& r) const     { return r.Min.y <  Max.y && r.Max.y >  Min.y && r.Min.x <  Max.x && r.Max.x >  Min.x; }
    void        Add(const ImVec2& p)                { if (Min.x > p.x)     Min.x = p.x;     if (Min.y > p.y)     Min.y = p.y;     if (Max.x < p.x)     Max.x = p.x;     if (Max.y < p.y)     Max.y = p.y; }
    void        Add(const ImRect& r)                { if (Min.x > r.Min.x) Min.x = r.Min.x; if (Min.y > r.Min.y) Min.y = r.Min.y; if (Max.x < r.Max.x) Max.x = r.Max.x; if (Max.y < r.Max.y) Max.y = r.Max.y; }
    void        Expand(const float amount)          { Min.x -= amount;   Min.y -= amount;   Max.x += amount;   Max.y += amount; }
    void        Expand(const ImVec2& amount)        { Min.x -= amount.x; Min.y -= amount.y; Max.x += amount.x; Max.y += amount.y; }
    void        Translate(const ImVec2& d)          { Min.x += d.x; Min.y += d.y; Max.x += d.x; Max.y += d.y; }
    void        TranslateX(float dx)                { Min.x += dx; Max.x += dx; }
    void        TranslateY(float dy)                { Min.y += dy; Max.y += dy; }
    void        ClipWith(const ImRect& r)           { Min = ImMax(Min, r.Min); Max = ImMin(Max, r.Max); }                   // Simple version, may lead to an inverted rectangle, which is fine for Contains/Overlaps test but not for display.
    void        ClipWithFull(const ImRect& r)       { Min = ImClamp(Min, r.Min, r.Max); Max = ImClamp(Max, r.Min, r.Max); } // Full version, ensure both points are fully clipped.
    void        Floor()                             { Min.x = IM_FLOOR(Min.x); Min.y = IM_FLOOR(Min.y); Max.x = IM_FLOOR(Max.x); Max.y = IM_FLOOR(Max.y); }
    bool        IsInverted() const                  { return Min.x > Max.x || Min.y > Max.y; }
    ImVec4      ToVec4() const                      { return ImVec4(Min.x, Min.y, Max.x, Max.y); }
};
IM_MSVC_RUNTIME_CHECKS_RESTORE

// Helper: ImBitArray
inline bool     ImBitArrayTestBit(const ImU32* arr, int n)      { ImU32 mask = (ImU32)1 << (n & 31); return (arr[n >> 5] & mask) != 0; }
inline void     ImBitArrayClearBit(ImU32* arr, int n)           { ImU32 mask = (ImU32)1 << (n & 31); arr[n >> 5] &= ~mask; }
inline void     ImBitArraySetBit(ImU32* arr, int n)             { ImU32 mask = (ImU32)1 << (n & 31); arr[n >> 5] |= mask; }
inline void     ImBitArraySetBitRange(ImU32* arr, int n, int n2) // Works on range [n..n2)
{
    n2--;
    while (n <= n2)
    {
        int a_mod = (n & 31);
        int b_mod = (n2 > (n | 31) ? 31 : (n2 & 31)) + 1;
        ImU32 mask = (ImU32)(((ImU64)1 << b_mod) - 1) & ~(ImU32)(((ImU64)1 << a_mod) - 1);
        arr[n >> 5] |= mask;
        n = (n + 32) & ~31;
    }
}

// Helper: ImBitArray class (wrapper over ImBitArray functions)
// Store 1-bit per value.
template<int BITCOUNT>
struct py_API ImBitArray
{
    ImU32           Storage[(BITCOUNT + 31) >> 5];
    ImBitArray()                                { ClearAllBits(); }
    void            ClearAllBits()              { memset(Storage, 0, sizeof(Storage)); }
    void            SetAllBits()                { memset(Storage, 255, sizeof(Storage)); }
    bool            TestBit(int n) const        { IM_ASSERT(n < BITCOUNT); return ImBitArrayTestBit(Storage, n); }
    void            SetBit(int n)               { IM_ASSERT(n < BITCOUNT); ImBitArraySetBit(Storage, n); }
    void            ClearBit(int n)             { IM_ASSERT(n < BITCOUNT); ImBitArrayClearBit(Storage, n); }
    void            SetBitRange(int n, int n2)  { ImBitArraySetBitRange(Storage, n, n2); } // Works on range [n..n2)
};

// Helper: ImBitVector
// Store 1-bit per value.
struct py_API ImBitVector
{
    ImVector<ImU32> Storage;
    void            Create(int sz)              { Storage.resize((sz + 31) >> 5); memset(Storage.Data, 0, (size_t)Storage.Size * sizeof(Storage.Data[0])); }
    void            Clear()                     { Storage.clear(); }
    bool            TestBit(int n) const        { IM_ASSERT(n < (Storage.Size << 5)); return ImBitArrayTestBit(Storage.Data, n); }
    void            SetBit(int n)               { IM_ASSERT(n < (Storage.Size << 5)); ImBitArraySetBit(Storage.Data, n); }
    void            ClearBit(int n)             { IM_ASSERT(n < (Storage.Size << 5)); ImBitArrayClearBit(Storage.Data, n); }
};

// Helper: ImSpan<>
// Pointing to a span of data we don't own.
template<typename T>
struct ImSpan
{
    T*                  Data;
    T*                  DataEnd;

    // Constructors, destructor
    inline ImSpan()                                 { Data = DataEnd = NULL; }
    inline ImSpan(T* data, int size)                { Data = data; DataEnd = data + size; }
    inline ImSpan(T* data, T* data_end)             { Data = data; DataEnd = data_end; }

    inline void         set(T* data, int size)      { Data = data; DataEnd = data + size; }
    inline void         set(T* data, T* data_end)   { Data = data; DataEnd = data_end; }
    inline int          size() const                { return (int)(ptrdiff_t)(DataEnd - Data); }
    inline int          size_in_bytes() const       { return (int)(ptrdiff_t)(DataEnd - Data) * (int)sizeof(T); }
    inline T&           operator[](int i)           { T* p = Data + i; IM_ASSERT(p >= Data && p < DataEnd); return *p; }
    inline const T&     operator[](int i) const     { const T* p = Data + i; IM_ASSERT(p >= Data && p < DataEnd); return *p; }

    inline T*           begin()                     { return Data; }
    inline const T*     begin() const               { return Data; }
    inline T*           end()                       { return DataEnd; }
    inline const T*     end() const                 { return DataEnd; }

    // Utilities
    inline int  index_from_ptr(const T* it) const   { IM_ASSERT(it >= Data && it < DataEnd); const ptrdiff_t off = it - Data; return (int)off; }
};

// Helper: ImSpanAllocator<>
// Facilitate storing multiple chunks into a single large block (the "arena")
// - Usage: call Reserve() N times, allocate GetArenaSizeInBytes() worth, pass it to SetArenaBasePtr(), call GetSpan() N times to retrieve the aligned ranges.
template<int CHUNKS>
struct ImSpanAllocator
{
    char*   BasePtr;
    int     CurrOff;
    int     CurrIdx;
    int     Offsets[CHUNKS];
    int     Sizes[CHUNKS];

    ImSpanAllocator()                               { memset(this, 0, sizeof(*this)); }
    inline void  Reserve(int n, size_t sz, int a=4) { IM_ASSERT(n == CurrIdx && n < CHUNKS); CurrOff = IM_MEMALIGN(CurrOff, a); Offsets[n] = CurrOff; Sizes[n] = (int)sz; CurrIdx++; CurrOff += (int)sz; }
    inline int   GetArenaSizeInBytes()              { return CurrOff; }
    inline void  SetArenaBasePtr(void* base_ptr)    { BasePtr = (char*)base_ptr; }
    inline void* GetSpanPtrBegin(int n)             { IM_ASSERT(n >= 0 && n < CHUNKS && CurrIdx == CHUNKS); return (void*)(BasePtr + Offsets[n]); }
    inline void* GetSpanPtrEnd(int n)               { IM_ASSERT(n >= 0 && n < CHUNKS && CurrIdx == CHUNKS); return (void*)(BasePtr + Offsets[n] + Sizes[n]); }
    template<typename T>
    inline void  GetSpan(int n, ImSpan<T>* span)    { span->set((T*)GetSpanPtrBegin(n), (T*)GetSpanPtrEnd(n)); }
};

// Helper: ImPool<>
// Basic keyed storage for contiguous instances, slow/amortized insertion, O(1) indexable, O(Log N) queries by ID over a dense/hot buffer,
// Honor constructor/destructor. Add/remove invalidate all pointers. Indexes have the same lifetime as the associated object.
typedef int ImPoolIdx;
template<typename T>
struct py_API ImPool
{
    ImVector<T>     Buf;        // Contiguous data
    pyStorage    Map;        // ID->Index
    ImPoolIdx       FreeIdx;    // Next free idx to use
    ImPoolIdx       AliveCount; // Number of active/alive items (for display purpose)

    ImPool()    { FreeIdx = AliveCount = 0; }
    ~ImPool()   { Clear(); }
    T*          GetByKey(pyID key)               { int idx = Map.GetInt(key, -1); return (idx != -1) ? &Buf[idx] : NULL; }
    T*          GetByIndex(ImPoolIdx n)             { return &Buf[n]; }
    ImPoolIdx   GetIndex(const T* p) const          { IM_ASSERT(p >= Buf.Data && p < Buf.Data + Buf.Size); return (ImPoolIdx)(p - Buf.Data); }
    T*          GetOrAddByKey(pyID key)          { int* p_idx = Map.GetIntRef(key, -1); if (*p_idx != -1) return &Buf[*p_idx]; *p_idx = FreeIdx; return Add(); }
    bool        Contains(const T* p) const          { return (p >= Buf.Data && p < Buf.Data + Buf.Size); }
    void        Clear()                             { for (int n = 0; n < Map.Data.Size; n++) { int idx = Map.Data[n].val_i; if (idx != -1) Buf[idx].~T(); } Map.Clear(); Buf.clear(); FreeIdx = AliveCount = 0; }
    T*          Add()                               { int idx = FreeIdx; if (idx == Buf.Size) { Buf.resize(Buf.Size + 1); FreeIdx++; } else { FreeIdx = *(int*)&Buf[idx]; } IM_PLACEMENT_NEW(&Buf[idx]) T(); AliveCount++; return &Buf[idx]; }
    void        Remove(pyID key, const T* p)     { Remove(key, GetIndex(p)); }
    void        Remove(pyID key, ImPoolIdx idx)  { Buf[idx].~T(); *(int*)&Buf[idx] = FreeIdx; FreeIdx = idx; Map.SetInt(key, -1); AliveCount--; }
    void        Reserve(int capacity)               { Buf.reserve(capacity); Map.Data.reserve(capacity); }

    // To iterate a ImPool: for (int n = 0; n < pool.GetMapSize(); n++) if (T* t = pool.TryGetMapData(n)) { ... }
    // Can be avoided if you know .Remove() has never been called on the pool, or AliveCount == GetMapSize()
    int         GetAliveCount() const               { return AliveCount; }      // Number of active/alive items in the pool (for display purpose)
    int         GetBufSize() const                  { return Buf.Size; }
    int         GetMapSize() const                  { return Map.Data.Size; }   // It is the map we need iterate to find valid items, since we don't have "alive" storage anywhere
    T*          TryGetMapData(ImPoolIdx n)          { int idx = Map.Data[n].val_i; if (idx == -1) return NULL; return GetByIndex(idx); }
#ifndef py_DISABLE_OBSOLETE_FUNCTIONS
    int         GetSize()                           { return GetMapSize(); } // For ImPlot: should use GetMapSize() from (py_VERSION_NUM >= 18304)
#endif
};

// Helper: ImChunkStream<>
// Build and iterate a contiguous stream of variable-sized structures.
// This is used by Settings to store persistent data while reducing allocation count.
// We store the chunk size first, and align the final size on 4 bytes boundaries.
// The tedious/zealous amount of casting is to avoid -Wcast-align warnings.
template<typename T>
struct py_API ImChunkStream
{
    ImVector<char>  Buf;

    void    clear()                     { Buf.clear(); }
    bool    empty() const               { return Buf.Size == 0; }
    int     size() const                { return Buf.Size; }
    T*      alloc_chunk(size_t sz)      { size_t HDR_SZ = 4; sz = IM_MEMALIGN(HDR_SZ + sz, 4u); int off = Buf.Size; Buf.resize(off + (int)sz); ((int*)(void*)(Buf.Data + off))[0] = (int)sz; return (T*)(void*)(Buf.Data + off + (int)HDR_SZ); }
    T*      begin()                     { size_t HDR_SZ = 4; if (!Buf.Data) return NULL; return (T*)(void*)(Buf.Data + HDR_SZ); }
    T*      next_chunk(T* p)            { size_t HDR_SZ = 4; IM_ASSERT(p >= begin() && p < end()); p = (T*)(void*)((char*)(void*)p + chunk_size(p)); if (p == (T*)(void*)((char*)end() + HDR_SZ)) return (T*)0; IM_ASSERT(p < end()); return p; }
    int     chunk_size(const T* p)      { return ((const int*)p)[-1]; }
    T*      end()                       { return (T*)(void*)(Buf.Data + Buf.Size); }
    int     offset_from_ptr(const T* p) { IM_ASSERT(p >= begin() && p < end()); const ptrdiff_t off = (const char*)p - Buf.Data; return (int)off; }
    T*      ptr_from_offset(int off)    { IM_ASSERT(off >= 4 && off < Buf.Size); return (T*)(void*)(Buf.Data + off); }
    void    swap(ImChunkStream<T>& rhs) { rhs.Buf.swap(Buf); }

};

//-----------------------------------------------------------------------------
// [SECTION] ImDrawList support
//-----------------------------------------------------------------------------

// ImDrawList: Helper function to calculate a circle's segment count given its radius and a "maximum error" value.
// Estimation of number of circle segment based on error is derived using method described in https://stackoverflow.com/a/2244088/15194693
// Number of segments (N) is calculated using equation:
//   N = ceil ( pi / acos(1 - error / r) )     where r > 0, error <= r
// Our equation is significantly simpler that one in the post thanks for choosing segment that is
// perpendicular to X axis. Follow steps in the article from this starting condition and you will
// will get this result.
//
// Rendering circles with an odd number of segments, while mathematically correct will produce
// asymmetrical results on the raster grid. Therefore we're rounding N to next even number (7->8, 8->8, 9->10 etc.)
//
#define IM_ROUNDUP_TO_EVEN(_V)                                  ((((_V) + 1) / 2) * 2)
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MIN                     4
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX                     512
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(_RAD,_MAXERROR)    ImClamp(IM_ROUNDUP_TO_EVEN((int)ImCeil(IM_PI / ImAcos(1 - ImMin((_MAXERROR), (_RAD)) / (_RAD)))), IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MIN, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX)

// Raw equation from IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC rewritten for 'r' and 'error'.
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_R(_N,_MAXERROR)    ((_MAXERROR) / (1 - ImCos(IM_PI / ImMax((float)(_N), IM_PI))))
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_ERROR(_N,_RAD)     ((1 - ImCos(IM_PI / ImMax((float)(_N), IM_PI))) / (_RAD))

// ImDrawList: Lookup table size for adaptive arc drawing, cover full circle.
#ifndef IM_DRAWLIST_ARCFAST_TABLE_SIZE
#define IM_DRAWLIST_ARCFAST_TABLE_SIZE                          48 // Number of samples in lookup table.
#endif
#define IM_DRAWLIST_ARCFAST_SAMPLE_MAX                          IM_DRAWLIST_ARCFAST_TABLE_SIZE // Sample index _PathArcToFastEx() for 360 angle.

// Data shared between all ImDrawList instances
// You may want to create your own instance of this if you want to use ImDrawList completely without py. In that case, watch out for future changes to this structure.
struct py_API ImDrawListSharedData
{
    ImVec2          TexUvWhitePixel;            // UV of white pixel in the atlas
    ImFont*         Font;                       // Current/default font (optional, for simplified AddText overload)
    float           FontSize;                   // Current/default font size (optional, for simplified AddText overload)
    float           CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo()
    float           CircleSegmentMaxError;      // Number of circle segments to use per pixel of radius for AddCircle() etc
    ImVec4          ClipRectFullscreen;         // Value for PushClipRectFullscreen()
    ImDrawListFlags InitialFlags;               // Initial flags at the beginning of the frame (it is possible to alter flags on a per-drawlist basis afterwards)

    // [Internal] Lookup tables
    ImVec2          ArcFastVtx[IM_DRAWLIST_ARCFAST_TABLE_SIZE]; // Sample points on the quarter of the circle.
    float           ArcFastRadiusCutoff;                        // Cutoff radius after which arc drawing will fallback to slower PathArcTo()
    ImU8            CircleSegmentCounts[64];    // Precomputed segment count for given radius before we calculate it dynamically (to avoid calculation overhead)
    const ImVec4*   TexUvLines;                 // UV of anti-aliased lines in the atlas

    ImDrawListSharedData();
    void SetCircleTessellationMaxError(float max_error);
};

struct ImDrawDataBuilder
{
    ImVector<ImDrawList*>   Layers[2];           // Global layers for: regular, tooltip

    void Clear()                    { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].resize(0); }
    void ClearFreeMemory()          { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].clear(); }
    int  GetDrawListCount() const   { int count = 0; for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) count += Layers[n].Size; return count; }
    py_API void FlattenIntoSingleLayer();
};

//-----------------------------------------------------------------------------
// [SECTION] Widgets support: flags, enums, data structures
//-----------------------------------------------------------------------------

// Transient per-window flags, reset at the beginning of the frame. For child window, inherited from parent on first Begin().
// This is going to be exposed in py.h when stabilized enough.
enum pyItemFlags_
{
    pyItemFlags_None                     = 0,
    pyItemFlags_NoTabStop                = 1 << 0,  // false     // Disable keyboard tabbing (FIXME: should merge with _NoNav)
    pyItemFlags_ButtonRepeat             = 1 << 1,  // false     // Button() will return true multiple times based on io.KeyRepeatDelay and io.KeyRepeatRate settings.
    pyItemFlags_Disabled                 = 1 << 2,  // false     // Disable interactions but doesn't affect visuals. See BeginDisabled()/EndDisabled(). See github.com/ocornut/py/issues/211
    pyItemFlags_NoNav                    = 1 << 3,  // false     // Disable keyboard/gamepad directional navigation (FIXME: should merge with _NoTabStop)
    pyItemFlags_NoNavDefaultFocus        = 1 << 4,  // false     // Disable item being a candidate for default focus (e.g. used by title bar items)
    pyItemFlags_SelectableDontClosePopup = 1 << 5,  // false     // Disable MenuItem/Selectable() automatically closing their popup window
    pyItemFlags_MixedValue               = 1 << 6,  // false     // [BETA] Represent a mixed/indeterminate value, generally multi-selection where values differ. Currently only supported by Checkbox() (later should support all sorts of widgets)
    pyItemFlags_ReadOnly                 = 1 << 7,  // false     // [ALPHA] Allow hovering interactions but underlying value is not changed.
    pyItemFlags_Inputable                = 1 << 8   // false     // [WIP] Auto-activate input mode when tab focused. Currently only used and supported by a few items before it becomes a generic feature.
};

// Storage for LastItem data
enum pyItemStatusFlags_
{
    pyItemStatusFlags_None               = 0,
    pyItemStatusFlags_HoveredRect        = 1 << 0,   // Mouse position is within item rectangle (does NOT mean that the window is in correct z-order and can be hovered!, this is only one part of the most-common IsItemHovered test)
    pyItemStatusFlags_HasDisplayRect     = 1 << 1,   // g.LastItemData.DisplayRect is valid
    pyItemStatusFlags_Edited             = 1 << 2,   // Value exposed by item was edited in the current frame (should match the bool return value of most widgets)
    pyItemStatusFlags_ToggledSelection   = 1 << 3,   // Set when Selectable(), TreeNode() reports toggling a selection. We can't report "Selected", only state changes, in order to easily handle clipping with less issues.
    pyItemStatusFlags_ToggledOpen        = 1 << 4,   // Set when TreeNode() reports toggling their open state.
    pyItemStatusFlags_HasDeactivated     = 1 << 5,   // Set if the widget/group is able to provide data for the pyItemStatusFlags_Deactivated flag.
    pyItemStatusFlags_Deactivated        = 1 << 6,   // Only valid if pyItemStatusFlags_HasDeactivated is set.
    pyItemStatusFlags_HoveredWindow      = 1 << 7,   // Override the HoveredWindow test to allow cross-window hover testing.
    pyItemStatusFlags_FocusedByTabbing   = 1 << 8    // Set when the Focusable item just got focused by Tabbing (FIXME: to be removed soon)

#ifdef py_ENABLE_TEST_ENGINE
    , // [py_tests only]
    pyItemStatusFlags_Openable           = 1 << 20,  //
    pyItemStatusFlags_Opened             = 1 << 21,  //
    pyItemStatusFlags_Checkable          = 1 << 22,  //
    pyItemStatusFlags_Checked            = 1 << 23   //
#endif
};

// Extend pyInputTextFlags_
enum pyInputTextFlagsPrivate_
{
    // [Internal]
    pyInputTextFlags_Multiline           = 1 << 26,  // For internal use by InputTextMultiline()
    pyInputTextFlags_NoMarkEdited        = 1 << 27,  // For internal use by functions using InputText() before reformatting data
    pyInputTextFlags_MergedItem          = 1 << 28   // For internal use by TempInputText(), will skip calling ItemAdd(). Require bounding-box to strictly match.
};

// Extend pyButtonFlags_
enum pyButtonFlagsPrivate_
{
    pyButtonFlags_PressedOnClick         = 1 << 4,   // return true on click (mouse down event)
    pyButtonFlags_PressedOnClickRelease  = 1 << 5,   // [Default] return true on click + release on same item <-- this is what the majority of Button are using
    pyButtonFlags_PressedOnClickReleaseAnywhere = 1 << 6, // return true on click + release even if the release event is not done while hovering the item
    pyButtonFlags_PressedOnRelease       = 1 << 7,   // return true on release (default requires click+release)
    pyButtonFlags_PressedOnDoubleClick   = 1 << 8,   // return true on double-click (default requires click+release)
    pyButtonFlags_PressedOnDragDropHold  = 1 << 9,   // return true when held into while we are drag and dropping another item (used by e.g. tree nodes, collapsing headers)
    pyButtonFlags_Repeat                 = 1 << 10,  // hold to repeat
    pyButtonFlags_FlattenChildren        = 1 << 11,  // allow interactions even if a child window is overlapping
    pyButtonFlags_AllowItemOverlap       = 1 << 12,  // require previous frame HoveredId to either match id or be null before being usable, use along with SetItemAllowOverlap()
    pyButtonFlags_DontClosePopups        = 1 << 13,  // disable automatically closing parent popup on press // [UNUSED]
    //pyButtonFlags_Disabled             = 1 << 14,  // disable interactions -> use BeginDisabled() or pyItemFlags_Disabled
    pyButtonFlags_AlignTextBaseLine      = 1 << 15,  // vertically align button to match text baseline - ButtonEx() only // FIXME: Should be removed and handled by SmallButton(), not possible currently because of DC.CursorPosPrevLine
    pyButtonFlags_NoKeyModifiers         = 1 << 16,  // disable mouse interaction if a key modifier is held
    pyButtonFlags_NoHoldingActiveId      = 1 << 17,  // don't set ActiveId while holding the mouse (pyButtonFlags_PressedOnClick only)
    pyButtonFlags_NoNavFocus             = 1 << 18,  // don't override navigation focus when activated
    pyButtonFlags_NoHoveredOnFocus       = 1 << 19,  // don't report as hovered when nav focus is on this item
    pyButtonFlags_PressedOnMask_         = pyButtonFlags_PressedOnClick | pyButtonFlags_PressedOnClickRelease | pyButtonFlags_PressedOnClickReleaseAnywhere | pyButtonFlags_PressedOnRelease | pyButtonFlags_PressedOnDoubleClick | pyButtonFlags_PressedOnDragDropHold,
    pyButtonFlags_PressedOnDefault_      = pyButtonFlags_PressedOnClickRelease
};

// Extend pyComboFlags_
enum pyComboFlagsPrivate_
{
    pyComboFlags_CustomPreview           = 1 << 20   // enable BeginComboPreview()
};

// Extend pySliderFlags_
enum pySliderFlagsPrivate_
{
    pySliderFlags_Vertical               = 1 << 20,  // Should this slider be orientated vertically?
    pySliderFlags_ReadOnly               = 1 << 21
};

// Extend pySelectableFlags_
enum pySelectableFlagsPrivate_
{
    // NB: need to be in sync with last value of pySelectableFlags_
    pySelectableFlags_NoHoldingActiveID      = 1 << 20,
    pySelectableFlags_SelectOnNav            = 1 << 21,  // (WIP) Auto-select when moved into. This is not exposed in public API as to handle multi-select and modifiers we will need user to explicitly control focus scope. May be replaced with a BeginSelection() API.
    pySelectableFlags_SelectOnClick          = 1 << 22,  // Override button behavior to react on Click (default is Click+Release)
    pySelectableFlags_SelectOnRelease        = 1 << 23,  // Override button behavior to react on Release (default is Click+Release)
    pySelectableFlags_SpanAvailWidth         = 1 << 24,  // Span all avail width even if we declared less for layout purpose. FIXME: We may be able to remove this (added in 6251d379, 2bcafc86 for menus)
    pySelectableFlags_DrawHoveredWhenHeld    = 1 << 25,  // Always show active when held, even is not hovered. This concept could probably be renamed/formalized somehow.
    pySelectableFlags_SetNavIdOnHover        = 1 << 26,  // Set Nav/Focus ID on mouse hover (used by MenuItem)
    pySelectableFlags_NoPadWithHalfSpacing   = 1 << 27   // Disable padding each side with ItemSpacing * 0.5f
};

// Extend pyTreeNodeFlags_
enum pyTreeNodeFlagsPrivate_
{
    pyTreeNodeFlags_ClipLabelForTrailingButton = 1 << 20
};

enum pySeparatorFlags_
{
    pySeparatorFlags_None                = 0,
    pySeparatorFlags_Horizontal          = 1 << 0,   // Axis default to current layout type, so generally Horizontal unless e.g. in a menu bar
    pySeparatorFlags_Vertical            = 1 << 1,
    pySeparatorFlags_SpanAllColumns      = 1 << 2
};

enum pyTextFlags_
{
    pyTextFlags_None = 0,
    pyTextFlags_NoWidthForLargeClippedText = 1 << 0
};

enum pyTooltipFlags_
{
    pyTooltipFlags_None = 0,
    pyTooltipFlags_OverridePreviousTooltip = 1 << 0      // Override will clear/ignore previously submitted tooltip (defaults to append)
};

// FIXME: this is in development, not exposed/functional as a generic feature yet.
// Horizontal/Vertical enums are fixed to 0/1 so they may be used to index ImVec2
enum pyLayoutType_
{
    pyLayoutType_Horizontal = 0,
    pyLayoutType_Vertical = 1
};

enum pyLogType
{
    pyLogType_None = 0,
    pyLogType_TTY,
    pyLogType_File,
    pyLogType_Buffer,
    pyLogType_Clipboard
};

// X/Y enums are fixed to 0/1 so they may be used to index ImVec2
enum pyAxis
{
    pyAxis_None = -1,
    pyAxis_X = 0,
    pyAxis_Y = 1
};

enum pyPlotType
{
    pyPlotType_Lines,
    pyPlotType_Histogram
};

enum pyInputSource
{
    pyInputSource_None = 0,
    pyInputSource_Mouse,
    pyInputSource_Keyboard,
    pyInputSource_Gamepad,
    pyInputSource_Nav,               // Stored in g.ActiveIdSource only
    pyInputSource_Clipboard,         // Currently only used by InputText()
    pyInputSource_COUNT
};

// FIXME-NAV: Clarify/expose various repeat delay/rate
enum pyInputReadMode
{
    pyInputReadMode_Down,
    pyInputReadMode_Pressed,
    pyInputReadMode_Released,
    pyInputReadMode_Repeat,
    pyInputReadMode_RepeatSlow,
    pyInputReadMode_RepeatFast
};

enum pyPopupPositionPolicy
{
    pyPopupPositionPolicy_Default,
    pyPopupPositionPolicy_ComboBox,
    pyPopupPositionPolicy_Tooltip
};

struct pyDataTypeTempStorage
{
    ImU8        Data[8];        // Can fit any data up to pyDataType_COUNT
};

// Type information associated to one pyDataType. Retrieve with DataTypeGetInfo().
struct pyDataTypeInfo
{
    size_t      Size;           // Size in bytes
    const char* Name;           // Short descriptive name for the type, for debugging
    const char* PrintFmt;       // Default printf format for the type
    const char* ScanFmt;        // Default scanf format for the type
};

// Extend pyDataType_
enum pyDataTypePrivate_
{
    pyDataType_String = pyDataType_COUNT + 1,
    pyDataType_Pointer,
    pyDataType_ID
};

// Stacked color modifier, backup of modified data so we can restore it
struct pyColorMod
{
    pyCol    Col;
    ImVec4      BackupValue;
};

// Stacked style modifier, backup of modified data so we can restore it. Data type inferred from the variable.
struct pyStyleMod
{
    pyStyleVar   VarIdx;
    union           { int BackupInt[2]; float BackupFloat[2]; };
    pyStyleMod(pyStyleVar idx, int v)     { VarIdx = idx; BackupInt[0] = v; }
    pyStyleMod(pyStyleVar idx, float v)   { VarIdx = idx; BackupFloat[0] = v; }
    pyStyleMod(pyStyleVar idx, ImVec2 v)  { VarIdx = idx; BackupFloat[0] = v.x; BackupFloat[1] = v.y; }
};

// Storage data for BeginComboPreview()/EndComboPreview()
struct py_API pyComboPreviewData
{
    ImRect          PreviewRect;
    ImVec2          BackupCursorPos;
    ImVec2          BackupCursorMaxPos;
    ImVec2          BackupCursorPosPrevLine;
    float           BackupPrevLineTextBaseOffset;
    pyLayoutType BackupLayout;

    pyComboPreviewData() { memset(this, 0, sizeof(*this)); }
};

// Stacked storage data for BeginGroup()/EndGroup()
struct py_API pyGroupData
{
    pyID     WindowID;
    ImVec2      BackupCursorPos;
    ImVec2      BackupCursorMaxPos;
    ImVec1      BackupIndent;
    ImVec1      BackupGroupOffset;
    ImVec2      BackupCurrLineSize;
    float       BackupCurrLineTextBaseOffset;
    pyID     BackupActiveIdIsAlive;
    bool        BackupActiveIdPreviousFrameIsAlive;
    bool        BackupHoveredIdIsAlive;
    bool        EmitItem;
};

// Simple column measurement, currently used for MenuItem() only.. This is very short-sighted/throw-away code and NOT a generic helper.
struct py_API pyMenuColumns
{
    ImU32       TotalWidth;
    ImU32       NextTotalWidth;
    ImU16       Spacing;
    ImU16       OffsetIcon;         // Always zero for now
    ImU16       OffsetLabel;        // Offsets are locked in Update()
    ImU16       OffsetShortcut;
    ImU16       OffsetMark;
    ImU16       Widths[4];          // Width of:   Icon, Label, Shortcut, Mark  (accumulators for current frame)

    pyMenuColumns() { memset(this, 0, sizeof(*this)); }
    void        Update(float spacing, bool window_reappearing);
    float       DeclColumns(float w_icon, float w_label, float w_shortcut, float w_mark);
    void        CalcNextTotalWidth(bool update_offsets);
};

// Internal state of the currently focused/edited text input box
// For a given item ID, access with py::GetInputTextState()
struct py_API pyInputTextState
{
    pyID                 ID;                     // widget id owning the text state
    int                     CurLenW, CurLenA;       // we need to maintain our buffer length in both UTF-8 and wchar format. UTF-8 length is valid even if TextA is not.
    ImVector<ImWchar>       TextW;                  // edit buffer, we need to persist but can't guarantee the persistence of the user-provided buffer. so we copy into own buffer.
    ImVector<char>          TextA;                  // temporary UTF8 buffer for callbacks and other operations. this is not updated in every code-path! size=capacity.
    ImVector<char>          InitialTextA;           // backup of end-user buffer at the time of focus (in UTF-8, unaltered)
    bool                    TextAIsValid;           // temporary UTF8 buffer is not initially valid before we make the widget active (until then we pull the data from user argument)
    int                     BufCapacityA;           // end-user buffer capacity
    float                   ScrollX;                // horizontal scrolling/offset
    ImStb::STB_TexteditState Stb;                   // state for stb_textedit.h
    float                   CursorAnim;             // timer for cursor blink, reset on every user action so the cursor reappears immediately
    bool                    CursorFollow;           // set when we want scrolling to follow the current cursor position (not always!)
    bool                    SelectedAllMouseLock;   // after a double-click to select all, we ignore further mouse drags to update selection
    bool                    Edited;                 // edited this frame
    pyInputTextFlags     Flags;                  // copy of InputText() flags
    pyInputTextCallback  UserCallback;           // "
    void*                   UserCallbackData;       // "

    pyInputTextState()                   { memset(this, 0, sizeof(*this)); }
    void        ClearText()                 { CurLenW = CurLenA = 0; TextW[0] = 0; TextA[0] = 0; CursorClamp(); }
    void        ClearFreeMemory()           { TextW.clear(); TextA.clear(); InitialTextA.clear(); }
    int         GetUndoAvailCount() const   { return Stb.undostate.undo_point; }
    int         GetRedoAvailCount() const   { return STB_TEXTEDIT_UNDOSTATECOUNT - Stb.undostate.redo_point; }
    void        OnKeyPressed(int key);      // Cannot be inline because we call in code in stb_textedit.h implementation

    // Cursor & Selection
    void        CursorAnimReset()           { CursorAnim = -0.30f; }                                   // After a user-input the cursor stays on for a while without blinking
    void        CursorClamp()               { Stb.cursor = ImMin(Stb.cursor, CurLenW); Stb.select_start = ImMin(Stb.select_start, CurLenW); Stb.select_end = ImMin(Stb.select_end, CurLenW); }
    bool        HasSelection() const        { return Stb.select_start != Stb.select_end; }
    void        ClearSelection()            { Stb.select_start = Stb.select_end = Stb.cursor; }
    int         GetCursorPos() const        { return Stb.cursor; }
    int         GetSelectionStart() const   { return Stb.select_start; }
    int         GetSelectionEnd() const     { return Stb.select_end; }
    void        SelectAll()                 { Stb.select_start = 0; Stb.cursor = Stb.select_end = CurLenW; Stb.has_preferred_x = 0; }
};

// Storage for current popup stack
struct pyPopupData
{
    pyID             PopupId;        // Set on OpenPopup()
    pyWindow*        Window;         // Resolved on BeginPopup() - may stay unresolved if user never calls OpenPopup()
    pyWindow*        SourceWindow;   // Set on OpenPopup() copy of NavWindow at the time of opening the popup
    int                 OpenFrameCount; // Set on OpenPopup()
    pyID             OpenParentId;   // Set on OpenPopup(), we need this to differentiate multiple menu sets from each others (e.g. inside menu bar vs loose menu items)
    ImVec2              OpenPopupPos;   // Set on OpenPopup(), preferred popup position (typically == OpenMousePos when using mouse)
    ImVec2              OpenMousePos;   // Set on OpenPopup(), copy of mouse position at the time of opening popup

    pyPopupData()    { memset(this, 0, sizeof(*this)); OpenFrameCount = -1; }
};

enum pyNextWindowDataFlags_
{
    pyNextWindowDataFlags_None               = 0,
    pyNextWindowDataFlags_HasPos             = 1 << 0,
    pyNextWindowDataFlags_HasSize            = 1 << 1,
    pyNextWindowDataFlags_HasContentSize     = 1 << 2,
    pyNextWindowDataFlags_HasCollapsed       = 1 << 3,
    pyNextWindowDataFlags_HasSizeConstraint  = 1 << 4,
    pyNextWindowDataFlags_HasFocus           = 1 << 5,
    pyNextWindowDataFlags_HasBgAlpha         = 1 << 6,
    pyNextWindowDataFlags_HasScroll          = 1 << 7
};

// Storage for SetNexWindow** functions
struct pyNextWindowData
{
    pyNextWindowDataFlags    Flags;
    pyCond                   PosCond;
    pyCond                   SizeCond;
    pyCond                   CollapsedCond;
    ImVec2                      PosVal;
    ImVec2                      PosPivotVal;
    ImVec2                      SizeVal;
    ImVec2                      ContentSizeVal;
    ImVec2                      ScrollVal;
    bool                        CollapsedVal;
    ImRect                      SizeConstraintRect;
    pySizeCallback           SizeCallback;
    void*                       SizeCallbackUserData;
    float                       BgAlphaVal;             // Override background alpha
    ImVec2                      MenuBarOffsetMinVal;    // (Always on) This is not exposed publicly, so we don't clear it and it doesn't have a corresponding flag (could we? for consistency?)

    pyNextWindowData()       { memset(this, 0, sizeof(*this)); }
    inline void ClearFlags()    { Flags = pyNextWindowDataFlags_None; }
};

enum pyNextItemDataFlags_
{
    pyNextItemDataFlags_None     = 0,
    pyNextItemDataFlags_HasWidth = 1 << 0,
    pyNextItemDataFlags_HasOpen  = 1 << 1
};

struct pyNextItemData
{
    pyNextItemDataFlags      Flags;
    float                       Width;          // Set by SetNextItemWidth()
    pyID                     FocusScopeId;   // Set by SetNextItemMultiSelectData() (!= 0 signify value has been set, so it's an alternate version of HasSelectionData, we don't use Flags for this because they are cleared too early. This is mostly used for debugging)
    pyCond                   OpenCond;
    bool                        OpenVal;        // Set by SetNextItemOpen()

    pyNextItemData()         { memset(this, 0, sizeof(*this)); }
    inline void ClearFlags()    { Flags = pyNextItemDataFlags_None; } // Also cleared manually by ItemAdd()!
};

// Status storage for the last submitted item
struct pyLastItemData
{
    pyID                 ID;
    pyItemFlags          InFlags;            // See pyItemFlags_
    pyItemStatusFlags    StatusFlags;        // See pyItemStatusFlags_
    ImRect                  Rect;               // Full rectangle
    ImRect                  NavRect;            // Navigation scoring rectangle (not displayed)
    ImRect                  DisplayRect;        // Display rectangle (only if pyItemStatusFlags_HasDisplayRect is set)

    pyLastItemData()     { memset(this, 0, sizeof(*this)); }
};

struct py_API pyStackSizes
{
    short   SizeOfIDStack;
    short   SizeOfColorStack;
    short   SizeOfStyleVarStack;
    short   SizeOfFontStack;
    short   SizeOfFocusScopeStack;
    short   SizeOfGroupStack;
    short   SizeOfItemFlagsStack;
    short   SizeOfBeginPopupStack;
    short   SizeOfDisabledStack;

    pyStackSizes() { memset(this, 0, sizeof(*this)); }
    void SetToCurrentState();
    void CompareWithCurrentState();
};

// Data saved for each window pushed into the stack
struct pyWindowStackData
{
    pyWindow*            Window;
    pyLastItemData       ParentLastItemDataBackup;
    pyStackSizes         StackSizesOnBegin;      // Store size of various stacks for asserting
};

struct pyShrinkWidthItem
{
    int         Index;
    float       Width;
};

struct pyPtrOrIndex
{
    void*       Ptr;            // Either field can be set, not both. e.g. Dock node tab bars are loose while BeginTabBar() ones are in a pool.
    int         Index;          // Usually index in a main pool.

    pyPtrOrIndex(void* ptr)  { Ptr = ptr; Index = -1; }
    pyPtrOrIndex(int index)  { Ptr = NULL; Index = index; }
};

//-----------------------------------------------------------------------------
// [SECTION] Navigation support
//-----------------------------------------------------------------------------

enum pyActivateFlags_
{
    pyActivateFlags_None                 = 0,
    pyActivateFlags_PreferInput          = 1 << 0,       // Favor activation that requires keyboard text input (e.g. for Slider/Drag). Default if keyboard is available.
    pyActivateFlags_PreferTweak          = 1 << 1,       // Favor activation for tweaking with arrows or gamepad (e.g. for Slider/Drag). Default if keyboard is not available.
    pyActivateFlags_TryToPreserveState   = 1 << 2        // Request widget to preserve state if it can (e.g. InputText will try to preserve cursor/selection)
};

// Early work-in-progress API for ScrollToItem()
enum pyScrollFlags_
{
    pyScrollFlags_None                   = 0,
    pyScrollFlags_KeepVisibleEdgeX       = 1 << 0,       // If item is not visible: scroll as little as possible on X axis to bring item back into view [default for X axis]
    pyScrollFlags_KeepVisibleEdgeY       = 1 << 1,       // If item is not visible: scroll as little as possible on Y axis to bring item back into view [default for Y axis for windows that are already visible]
    pyScrollFlags_KeepVisibleCenterX     = 1 << 2,       // If item is not visible: scroll to make the item centered on X axis [rarely used]
    pyScrollFlags_KeepVisibleCenterY     = 1 << 3,       // If item is not visible: scroll to make the item centered on Y axis
    pyScrollFlags_AlwaysCenterX          = 1 << 4,       // Always center the result item on X axis [rarely used]
    pyScrollFlags_AlwaysCenterY          = 1 << 5,       // Always center the result item on Y axis [default for Y axis for appearing window)
    pyScrollFlags_NoScrollParent         = 1 << 6,       // Disable forwarding scrolling to parent window if required to keep item/rect visible (only scroll window the function was applied to).
    pyScrollFlags_MaskX_                 = pyScrollFlags_KeepVisibleEdgeX | pyScrollFlags_KeepVisibleCenterX | pyScrollFlags_AlwaysCenterX,
    pyScrollFlags_MaskY_                 = pyScrollFlags_KeepVisibleEdgeY | pyScrollFlags_KeepVisibleCenterY | pyScrollFlags_AlwaysCenterY
};

enum pyNavHighlightFlags_
{
    pyNavHighlightFlags_None             = 0,
    pyNavHighlightFlags_TypeDefault      = 1 << 0,
    pyNavHighlightFlags_TypeThin         = 1 << 1,
    pyNavHighlightFlags_AlwaysDraw       = 1 << 2,       // Draw rectangular highlight if (g.NavId == id) _even_ when using the mouse.
    pyNavHighlightFlags_NoRounding       = 1 << 3
};

enum pyNavDirSourceFlags_
{
    pyNavDirSourceFlags_None             = 0,
    pyNavDirSourceFlags_Keyboard         = 1 << 0,
    pyNavDirSourceFlags_PadDPad          = 1 << 1,
    pyNavDirSourceFlags_PadLStick        = 1 << 2
};

enum pyNavMoveFlags_
{
    pyNavMoveFlags_None                  = 0,
    pyNavMoveFlags_LoopX                 = 1 << 0,   // On failed request, restart from opposite side
    pyNavMoveFlags_LoopY                 = 1 << 1,
    pyNavMoveFlags_WrapX                 = 1 << 2,   // On failed request, request from opposite side one line down (when NavDir==right) or one line up (when NavDir==left)
    pyNavMoveFlags_WrapY                 = 1 << 3,   // This is not super useful but provided for completeness
    pyNavMoveFlags_AllowCurrentNavId     = 1 << 4,   // Allow scoring and considering the current NavId as a move target candidate. This is used when the move source is offset (e.g. pressing PageDown actually needs to send a Up move request, if we are pressing PageDown from the bottom-most item we need to stay in place)
    pyNavMoveFlags_AlsoScoreVisibleSet   = 1 << 5,   // Store alternate result in NavMoveResultLocalVisible that only comprise elements that are already fully visible (used by PageUp/PageDown)
    pyNavMoveFlags_ScrollToEdgeY         = 1 << 6,   // Force scrolling to min/max (used by Home/End) // FIXME-NAV: Aim to remove or reword, probably unnecessary
    pyNavMoveFlags_Forwarded             = 1 << 7,
    pyNavMoveFlags_DebugNoResult         = 1 << 8,   // Dummy scoring for debug purpose, don't apply result
    pyNavMoveFlags_Tabbing               = 1 << 9,   // == Focus + Activate if item is Inputable + DontChangeNavHighlight
    pyNavMoveFlags_Activate              = 1 << 10,
    pyNavMoveFlags_DontSetNavHighlight   = 1 << 11   // Do not alter the visible state of keyboard vs mouse nav highlight
};

enum pyNavLayer
{
    pyNavLayer_Main  = 0,    // Main scrolling layer
    pyNavLayer_Menu  = 1,    // Menu layer (access with Alt/pyNavInput_Menu)
    pyNavLayer_COUNT
};

struct pyNavItemData
{
    pyWindow*        Window;         // Init,Move    // Best candidate window (result->ItemWindow->RootWindowForNav == request->Window)
    pyID             ID;             // Init,Move    // Best candidate item ID
    pyID             FocusScopeId;   // Init,Move    // Best candidate focus scope ID
    ImRect              RectRel;        // Init,Move    // Best candidate bounding box in window relative space
    pyItemFlags      InFlags;        // ????,Move    // Best candidate item flags
    float               DistBox;        //      Move    // Best candidate box distance to current NavId
    float               DistCenter;     //      Move    // Best candidate center distance to current NavId
    float               DistAxial;      //      Move    // Best candidate axial distance to current NavId

    pyNavItemData()  { Clear(); }
    void Clear()        { Window = NULL; ID = FocusScopeId = 0; InFlags = 0; DistBox = DistCenter = DistAxial = FLT_MAX; }
};

//-----------------------------------------------------------------------------
// [SECTION] Columns support
//-----------------------------------------------------------------------------

// Flags for internal's BeginColumns(). Prefix using BeginTable() nowadays!
enum pyOldColumnFlags_
{
    pyOldColumnFlags_None                    = 0,
    pyOldColumnFlags_NoBorder                = 1 << 0,   // Disable column dividers
    pyOldColumnFlags_NoResize                = 1 << 1,   // Disable resizing columns when clicking on the dividers
    pyOldColumnFlags_NoPreserveWidths        = 1 << 2,   // Disable column width preservation when adjusting columns
    pyOldColumnFlags_NoForceWithinWindow     = 1 << 3,   // Disable forcing columns to fit within window
    pyOldColumnFlags_GrowParentContentsSize  = 1 << 4    // (WIP) Restore pre-1.51 behavior of extending the parent window contents size but _without affecting the columns width at all_. Will eventually remove.

    // Obsolete names (will be removed)
#ifndef py_DISABLE_OBSOLETE_FUNCTIONS
    , pyColumnsFlags_None                    = pyOldColumnFlags_None,
    pyColumnsFlags_NoBorder                  = pyOldColumnFlags_NoBorder,
    pyColumnsFlags_NoResize                  = pyOldColumnFlags_NoResize,
    pyColumnsFlags_NoPreserveWidths          = pyOldColumnFlags_NoPreserveWidths,
    pyColumnsFlags_NoForceWithinWindow       = pyOldColumnFlags_NoForceWithinWindow,
    pyColumnsFlags_GrowParentContentsSize    = pyOldColumnFlags_GrowParentContentsSize
#endif
};

struct pyOldColumnData
{
    float               OffsetNorm;         // Column start offset, normalized 0.0 (far left) -> 1.0 (far right)
    float               OffsetNormBeforeResize;
    pyOldColumnFlags Flags;              // Not exposed
    ImRect              ClipRect;

    pyOldColumnData() { memset(this, 0, sizeof(*this)); }
};

struct pyOldColumns
{
    pyID             ID;
    pyOldColumnFlags Flags;
    bool                IsFirstFrame;
    bool                IsBeingResized;
    int                 Current;
    int                 Count;
    float               OffMinX, OffMaxX;       // Offsets from HostWorkRect.Min.x
    float               LineMinY, LineMaxY;
    float               HostCursorPosY;         // Backup of CursorPos at the time of BeginColumns()
    float               HostCursorMaxPosX;      // Backup of CursorMaxPos at the time of BeginColumns()
    ImRect              HostInitialClipRect;    // Backup of ClipRect at the time of BeginColumns()
    ImRect              HostBackupClipRect;     // Backup of ClipRect during PushColumnsBackground()/PopColumnsBackground()
    ImRect              HostBackupParentWorkRect;//Backup of WorkRect at the time of BeginColumns()
    ImVector<pyOldColumnData> Columns;
    ImDrawListSplitter  Splitter;

    pyOldColumns()   { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Multi-select support
//-----------------------------------------------------------------------------

#ifdef py_HAS_MULTI_SELECT
// <this is filled in 'range_select' branch>
#endif // #ifdef py_HAS_MULTI_SELECT

//-----------------------------------------------------------------------------
// [SECTION] Docking support
//-----------------------------------------------------------------------------

#ifdef py_HAS_DOCK
// <this is filled in 'docking' branch>
#endif // #ifdef py_HAS_DOCK

//-----------------------------------------------------------------------------
// [SECTION] Viewport support
//-----------------------------------------------------------------------------

// pyViewport Private/Internals fields (cardinal sin: we are using inheritance!)
// Every instance of pyViewport is in fact a pyViewportP.
struct pyViewportP : public pyViewport
{
    int                 DrawListsLastFrame[2];  // Last frame number the background (0) and foreground (1) draw lists were used
    ImDrawList*         DrawLists[2];           // Convenience background (0) and foreground (1) draw lists. We use them to draw software mouser cursor when io.MouseDrawCursor is set and to draw most debug overlays.
    ImDrawData          DrawDataP;
    ImDrawDataBuilder   DrawDataBuilder;

    ImVec2              WorkOffsetMin;          // Work Area: Offset from Pos to top-left corner of Work Area. Generally (0,0) or (0,+main_menu_bar_height). Work Area is Full Area but without menu-bars/status-bars (so WorkArea always fit inside Pos/Size!)
    ImVec2              WorkOffsetMax;          // Work Area: Offset from Pos+Size to bottom-right corner of Work Area. Generally (0,0) or (0,-status_bar_height).
    ImVec2              BuildWorkOffsetMin;     // Work Area: Offset being built during current frame. Generally >= 0.0f.
    ImVec2              BuildWorkOffsetMax;     // Work Area: Offset being built during current frame. Generally <= 0.0f.

    pyViewportP()    { DrawListsLastFrame[0] = DrawListsLastFrame[1] = -1; DrawLists[0] = DrawLists[1] = NULL; }
    ~pyViewportP()   { if (DrawLists[0]) IM_DELETE(DrawLists[0]); if (DrawLists[1]) IM_DELETE(DrawLists[1]); }

    // Calculate work rect pos/size given a set of offset (we have 1 pair of offset for rect locked from last frame data, and 1 pair for currently building rect)
    ImVec2  CalcWorkRectPos(const ImVec2& off_min) const                            { return ImVec2(Pos.x + off_min.x, Pos.y + off_min.y); }
    ImVec2  CalcWorkRectSize(const ImVec2& off_min, const ImVec2& off_max) const    { return ImVec2(ImMax(0.0f, Size.x - off_min.x + off_max.x), ImMax(0.0f, Size.y - off_min.y + off_max.y)); }
    void    UpdateWorkRect()            { WorkPos = CalcWorkRectPos(WorkOffsetMin); WorkSize = CalcWorkRectSize(WorkOffsetMin, WorkOffsetMax); } // Update public fields

    // Helpers to retrieve ImRect (we don't need to store BuildWorkRect as every access tend to change it, hence the code asymmetry)
    ImRect  GetMainRect() const         { return ImRect(Pos.x, Pos.y, Pos.x + Size.x, Pos.y + Size.y); }
    ImRect  GetWorkRect() const         { return ImRect(WorkPos.x, WorkPos.y, WorkPos.x + WorkSize.x, WorkPos.y + WorkSize.y); }
    ImRect  GetBuildWorkRect() const    { ImVec2 pos = CalcWorkRectPos(BuildWorkOffsetMin); ImVec2 size = CalcWorkRectSize(BuildWorkOffsetMin, BuildWorkOffsetMax); return ImRect(pos.x, pos.y, pos.x + size.x, pos.y + size.y); }
};

//-----------------------------------------------------------------------------
// [SECTION] Settings support
//-----------------------------------------------------------------------------

// Windows data saved in py.ini file
// Because we never destroy or rename pyWindowSettings, we can store the names in a separate buffer easily.
// (this is designed to be stored in a ImChunkStream buffer, with the variable-length Name following our structure)
struct pyWindowSettings
{
    pyID     ID;
    ImVec2ih    Pos;
    ImVec2ih    Size;
    bool        Collapsed;
    bool        WantApply;      // Set when loaded from .ini data (to enable merging/loading .ini data into an already running context)

    pyWindowSettings()       { memset(this, 0, sizeof(*this)); }
    char* GetName()             { return (char*)(this + 1); }
};

struct pySettingsHandler
{
    const char* TypeName;       // Short description stored in .ini file. Disallowed characters: '[' ']'
    pyID     TypeHash;       // == ImHashStr(TypeName)
    void        (*ClearAllFn)(pyContext* ctx, pySettingsHandler* handler);                                // Clear all settings data
    void        (*ReadInitFn)(pyContext* ctx, pySettingsHandler* handler);                                // Read: Called before reading (in registration order)
    void*       (*ReadOpenFn)(pyContext* ctx, pySettingsHandler* handler, const char* name);              // Read: Called when entering into a new ini entry e.g. "[Window][Name]"
    void        (*ReadLineFn)(pyContext* ctx, pySettingsHandler* handler, void* entry, const char* line); // Read: Called for every line of text within an ini entry
    void        (*ApplyAllFn)(pyContext* ctx, pySettingsHandler* handler);                                // Read: Called after reading (in registration order)
    void        (*WriteAllFn)(pyContext* ctx, pySettingsHandler* handler, pyTextBuffer* out_buf);      // Write: Output every entries into 'out_buf'
    void*       UserData;

    pySettingsHandler() { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Metrics, Debug Tools
//-----------------------------------------------------------------------------

struct pyMetricsConfig
{
    bool        ShowStackTool;
    bool        ShowWindowsRects;
    bool        ShowWindowsBeginOrder;
    bool        ShowTablesRects;
    bool        ShowDrawCmdMesh;
    bool        ShowDrawCmdBoundingBoxes;
    int         ShowWindowsRectsType;
    int         ShowTablesRectsType;

    pyMetricsConfig()
    {
        ShowStackTool = false;
        ShowWindowsRects = false;
        ShowWindowsBeginOrder = false;
        ShowTablesRects = false;
        ShowDrawCmdMesh = true;
        ShowDrawCmdBoundingBoxes = true;
        ShowWindowsRectsType = -1;
        ShowTablesRectsType = -1;
    }
};

struct pyStackLevelInfo
{
    pyID                 ID;
    ImS8                    QueryFrameCount;            // >= 1: Query in progress
    bool                    QuerySuccess;               // Obtained result from DebugHookIdInfo()
    char                    Desc[58];                   // Arbitrarily sized buffer to hold a result (FIXME: could replace Results[] with a chunk stream?)

    pyStackLevelInfo()   { memset(this, 0, sizeof(*this)); }
};

// State for Stack tool queries
struct pyStackTool
{
    int                     LastActiveFrame;
    int                     StackLevel;                 // -1: query stack and resize Results, >= 0: individual stack level
    pyID                 QueryId;                    // ID to query details for
    ImVector<pyStackLevelInfo> Results;

    pyStackTool()        { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Generic context hooks
//-----------------------------------------------------------------------------

typedef void (*pyContextHookCallback)(pyContext* ctx, pyContextHook* hook);
enum pyContextHookType { pyContextHookType_NewFramePre, pyContextHookType_NewFramePost, pyContextHookType_EndFramePre, pyContextHookType_EndFramePost, pyContextHookType_RenderPre, pyContextHookType_RenderPost, pyContextHookType_Shutdown, pyContextHookType_PendingRemoval_ };

struct pyContextHook
{
    pyID                     HookId;     // A unique ID assigned by AddContextHook()
    pyContextHookType        Type;
    pyID                     Owner;
    pyContextHookCallback    Callback;
    void*                       UserData;

    pyContextHook()          { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] pyContext (main py context)
//-----------------------------------------------------------------------------

struct pyContext
{
    bool                    Initialized;
    bool                    FontAtlasOwnedByContext;            // IO.Fonts-> is owned by the pyContext and will be destructed along with it.
    pyIO                 IO;
    pyStyle              Style;
    ImFont*                 Font;                               // (Shortcut) == FontStack.empty() ? IO.Font : FontStack.back()
    float                   FontSize;                           // (Shortcut) == FontBaseSize * g.CurrentWindow->FontWindowScale == window->FontSize(). Text height for current window.
    float                   FontBaseSize;                       // (Shortcut) == IO.FontGlobalScale * Font->Scale * Font->FontSize. Base text height.
    ImDrawListSharedData    DrawListSharedData;
    double                  Time;
    int                     FrameCount;
    int                     FrameCountEnded;
    int                     FrameCountRendered;
    bool                    WithinFrameScope;                   // Set by NewFrame(), cleared by EndFrame()
    bool                    WithinFrameScopeWithImplicitWindow; // Set by NewFrame(), cleared by EndFrame() when the implicit debug window has been pushed
    bool                    WithinEndChild;                     // Set within EndChild()
    bool                    GcCompactAll;                       // Request full GC
    bool                    TestEngineHookItems;                // Will call test engine hooks: pyTestEngineHook_ItemAdd(), pyTestEngineHook_ItemInfo(), pyTestEngineHook_Log()
    void*                   TestEngine;                         // Test engine user data

    // Windows state
    ImVector<pyWindow*>  Windows;                            // Windows, sorted in display order, back to front
    ImVector<pyWindow*>  WindowsFocusOrder;                  // Root windows, sorted in focus order, back to front.
    ImVector<pyWindow*>  WindowsTempSortBuffer;              // Temporary buffer used in EndFrame() to reorder windows so parents are kept before their child
    ImVector<pyWindowStackData> CurrentWindowStack;
    pyStorage            WindowsById;                        // Map window's pyID to pyWindow*
    int                     WindowsActiveCount;                 // Number of unique windows submitted by frame
    ImVec2                  WindowsHoverPadding;                // Padding around resizable windows for which hovering on counts as hovering the window == ImMax(style.TouchExtraPadding, WINDOWS_HOVER_PADDING)
    pyWindow*            CurrentWindow;                      // Window being drawn into
    pyWindow*            HoveredWindow;                      // Window the mouse is hovering. Will typically catch mouse inputs.
    pyWindow*            HoveredWindowUnderMovingWindow;     // Hovered window ignoring MovingWindow. Only set if MovingWindow is set.
    pyWindow*            MovingWindow;                       // Track the window we clicked on (in order to preserve focus). The actual window that is moved is generally MovingWindow->RootWindow.
    pyWindow*            WheelingWindow;                     // Track the window we started mouse-wheeling on. Until a timer elapse or mouse has moved, generally keep scrolling the same window even if during the course of scrolling the mouse ends up hovering a child window.
    ImVec2                  WheelingWindowRefMousePos;
    float                   WheelingWindowTimer;

    // Item/widgets state and tracking information
    pyID                 DebugHookIdInfo;                    // Will call core hooks: DebugHookIdInfo() from GetID functions, used by Stack Tool [next HoveredId/ActiveId to not pull in an extra cache-line]
    pyID                 HoveredId;                          // Hovered widget, filled during the frame
    pyID                 HoveredIdPreviousFrame;
    bool                    HoveredIdAllowOverlap;
    bool                    HoveredIdUsingMouseWheel;           // Hovered widget will use mouse wheel. Blocks scrolling the underlying window.
    bool                    HoveredIdPreviousFrameUsingMouseWheel;
    bool                    HoveredIdDisabled;                  // At least one widget passed the rect test, but has been discarded by disabled flag or popup inhibit. May be true even if HoveredId == 0.
    float                   HoveredIdTimer;                     // Measure contiguous hovering time
    float                   HoveredIdNotActiveTimer;            // Measure contiguous hovering time where the item has not been active
    pyID                 ActiveId;                           // Active widget
    pyID                 ActiveIdIsAlive;                    // Active widget has been seen this frame (we can't use a bool as the ActiveId may change within the frame)
    float                   ActiveIdTimer;
    bool                    ActiveIdIsJustActivated;            // Set at the time of activation for one frame
    bool                    ActiveIdAllowOverlap;               // Active widget allows another widget to steal active id (generally for overlapping widgets, but not always)
    bool                    ActiveIdNoClearOnFocusLoss;         // Disable losing active id if the active id window gets unfocused.
    bool                    ActiveIdHasBeenPressedBefore;       // Track whether the active id led to a press (this is to allow changing between PressOnClick and PressOnRelease without pressing twice). Used by range_select branch.
    bool                    ActiveIdHasBeenEditedBefore;        // Was the value associated to the widget Edited over the course of the Active state.
    bool                    ActiveIdHasBeenEditedThisFrame;
    bool                    ActiveIdUsingMouseWheel;            // Active widget will want to read mouse wheel. Blocks scrolling the underlying window.
    ImU32                   ActiveIdUsingNavDirMask;            // Active widget will want to read those nav move requests (e.g. can activate a button and move away from it)
    ImU32                   ActiveIdUsingNavInputMask;          // Active widget will want to read those nav inputs.
    ImU64                   ActiveIdUsingKeyInputMask;          // Active widget will want to read those key inputs. When we grow the pyKey enum we'll need to either to order the enum to make useful keys come first, either redesign this into e.g. a small array.
    ImVec2                  ActiveIdClickOffset;                // Clicked offset from upper-left corner, if applicable (currently only set by ButtonBehavior)
    pyWindow*            ActiveIdWindow;
    pyInputSource        ActiveIdSource;                     // Activating with mouse or nav (gamepad/keyboard)
    int                     ActiveIdMouseButton;
    pyID                 ActiveIdPreviousFrame;
    bool                    ActiveIdPreviousFrameIsAlive;
    bool                    ActiveIdPreviousFrameHasBeenEditedBefore;
    pyWindow*            ActiveIdPreviousFrameWindow;
    pyID                 LastActiveId;                       // Store the last non-zero ActiveId, useful for animation.
    float                   LastActiveIdTimer;                  // Store the last non-zero ActiveId timer since the beginning of activation, useful for animation.

    // Next window/item data
    pyItemFlags          CurrentItemFlags;                      // == g.ItemFlagsStack.back()
    pyNextItemData       NextItemData;                       // Storage for SetNextItem** functions
    pyLastItemData       LastItemData;                       // Storage for last submitted item (setup by ItemAdd)
    pyNextWindowData     NextWindowData;                     // Storage for SetNextWindow** functions

    // Shared stacks
    ImVector<pyColorMod> ColorStack;                         // Stack for PushStyleColor()/PopStyleColor() - inherited by Begin()
    ImVector<pyStyleMod> StyleVarStack;                      // Stack for PushStyleVar()/PopStyleVar() - inherited by Begin()
    ImVector<ImFont*>       FontStack;                          // Stack for PushFont()/PopFont() - inherited by Begin()
    ImVector<pyID>       FocusScopeStack;                    // Stack for PushFocusScope()/PopFocusScope() - not inherited by Begin(), unless child window
    ImVector<pyItemFlags>ItemFlagsStack;                     // Stack for PushItemFlag()/PopItemFlag() - inherited by Begin()
    ImVector<pyGroupData>GroupStack;                         // Stack for BeginGroup()/EndGroup() - not inherited by Begin()
    ImVector<pyPopupData>OpenPopupStack;                     // Which popups are open (persistent)
    ImVector<pyPopupData>BeginPopupStack;                    // Which level of BeginPopup() we are in (reset every frame)

    // Viewports
    ImVector<pyViewportP*> Viewports;                        // Active viewports (Size==1 in 'master' branch). Each viewports hold their copy of ImDrawData.

    // Gamepad/keyboard Navigation
    pyWindow*            NavWindow;                          // Focused window for navigation. Could be called 'FocusWindow'
    pyID                 NavId;                              // Focused item for navigation
    pyID                 NavFocusScopeId;                    // Identify a selection scope (selection code often wants to "clear other items" when landing on an item of the selection set)
    pyID                 NavActivateId;                      // ~~ (g.ActiveId == 0) && IsNavInputPressed(pyNavInput_Activate) ? NavId : 0, also set when calling ActivateItem()
    pyID                 NavActivateDownId;                  // ~~ IsNavInputDown(pyNavInput_Activate) ? NavId : 0
    pyID                 NavActivatePressedId;               // ~~ IsNavInputPressed(pyNavInput_Activate) ? NavId : 0
    pyID                 NavActivateInputId;                 // ~~ IsNavInputPressed(pyNavInput_Input) ? NavId : 0; pyActivateFlags_PreferInput will be set and NavActivateId will be 0.
    pyActivateFlags      NavActivateFlags;
    pyID                 NavJustTabbedId;                    // Just tabbed to this id.
    pyID                 NavJustMovedToId;                   // Just navigated to this id (result of a successfully MoveRequest).
    pyID                 NavJustMovedToFocusScopeId;         // Just navigated to this focus scope id (result of a successfully MoveRequest).
    pyKeyModFlags        NavJustMovedToKeyMods;
    pyID                 NavNextActivateId;                  // Set by ActivateItem(), queued until next frame.
    pyActivateFlags      NavNextActivateFlags;
    pyInputSource        NavInputSource;                     // Keyboard or Gamepad mode? THIS WILL ONLY BE None or NavGamepad or NavKeyboard.
    pyNavLayer           NavLayer;                           // Layer we are navigating on. For now the system is hard-coded for 0=main contents and 1=menu/title bar, may expose layers later.
    int                     NavIdTabCounter;                    // == NavWindow->DC.FocusIdxTabCounter at time of NavId processing
    bool                    NavIdIsAlive;                       // Nav widget has been seen this frame ~~ NavRectRel is valid
    bool                    NavMousePosDirty;                   // When set we will update mouse position if (io.ConfigFlags & pyConfigFlags_NavEnableSetMousePos) if set (NB: this not enabled by default)
    bool                    NavDisableHighlight;                // When user starts using mouse, we hide gamepad/keyboard highlight (NB: but they are still available, which is why NavDisableHighlight isn't always != NavDisableMouseHover)
    bool                    NavDisableMouseHover;               // When user starts using gamepad/keyboard, we hide mouse hovering highlight until mouse is touched again.

    // Navigation: Init & Move Requests
    bool                    NavAnyRequest;                      // ~~ NavMoveRequest || NavInitRequest this is to perform early out in ItemAdd()
    bool                    NavInitRequest;                     // Init request for appearing window to select first item
    bool                    NavInitRequestFromMove;
    pyID                 NavInitResultId;                    // Init request result (first item of the window, or one for which SetItemDefaultFocus() was called)
    ImRect                  NavInitResultRectRel;               // Init request result rectangle (relative to parent window)
    bool                    NavMoveSubmitted;                   // Move request submitted, will process result on next NewFrame()
    bool                    NavMoveScoringItems;                // Move request submitted, still scoring incoming items
    bool                    NavMoveForwardToNextFrame;
    pyNavMoveFlags       NavMoveFlags;
    pyScrollFlags        NavMoveScrollFlags;
    pyKeyModFlags        NavMoveKeyMods;
    pyDir                NavMoveDir;                         // Direction of the move request (left/right/up/down)
    pyDir                NavMoveDirForDebug;
    pyDir                NavMoveClipDir;                     // FIXME-NAV: Describe the purpose of this better. Might want to rename?
    ImRect                  NavScoringRect;                     // Rectangle used for scoring, in screen space. Based of window->NavRectRel[], modified for directional navigation scoring.
    int                     NavScoringDebugCount;               // Metrics for debugging
    int                     NavTabbingInputableRemaining;       // >0 when counting items for tabbing
    pyNavItemData        NavMoveResultLocal;                 // Best move request candidate within NavWindow
    pyNavItemData        NavMoveResultLocalVisible;          // Best move request candidate within NavWindow that are mostly visible (when using pyNavMoveFlags_AlsoScoreVisibleSet flag)
    pyNavItemData        NavMoveResultOther;                 // Best move request candidate within NavWindow's flattened hierarchy (when using pyWindowFlags_NavFlattened flag)

    // Navigation: Windowing (CTRL+TAB for list, or Menu button + keys or directional pads to move/resize)
    pyWindow*            NavWindowingTarget;                 // Target window when doing CTRL+Tab (or Pad Menu + FocusPrev/Next), this window is temporarily displayed top-most!
    pyWindow*            NavWindowingTargetAnim;             // Record of last valid NavWindowingTarget until DimBgRatio and NavWindowingHighlightAlpha becomes 0.0f, so the fade-out can stay on it.
    pyWindow*            NavWindowingListWindow;             // Internal window actually listing the CTRL+Tab contents
    float                   NavWindowingTimer;
    float                   NavWindowingHighlightAlpha;
    bool                    NavWindowingToggleLayer;

    // Legacy Focus/Tabbing system (older than Nav, active even if Nav is disabled, misnamed. FIXME-NAV: This needs a redesign!)
    pyWindow*            TabFocusRequestCurrWindow;          //
    pyWindow*            TabFocusRequestNextWindow;          //
    int                     TabFocusRequestCurrCounterTabStop;  // Tab item being requested for focus, stored as an index
    int                     TabFocusRequestNextCounterTabStop;  // "
    bool                    TabFocusPressed;                    // Set in NewFrame() when user pressed Tab

    // Render
    float                   DimBgRatio;                         // 0.0..1.0 animation when fading in a dimming background (for modal window and CTRL+TAB list)
    pyMouseCursor        MouseCursor;

    // Drag and Drop
    bool                    DragDropActive;
    bool                    DragDropWithinSource;               // Set when within a BeginDragDropXXX/EndDragDropXXX block for a drag source.
    bool                    DragDropWithinTarget;               // Set when within a BeginDragDropXXX/EndDragDropXXX block for a drag target.
    pyDragDropFlags      DragDropSourceFlags;
    int                     DragDropSourceFrameCount;
    int                     DragDropMouseButton;
    pyPayload            DragDropPayload;
    ImRect                  DragDropTargetRect;                 // Store rectangle of current target candidate (we favor small targets when overlapping)
    pyID                 DragDropTargetId;
    pyDragDropFlags      DragDropAcceptFlags;
    float                   DragDropAcceptIdCurrRectSurface;    // Target item surface (we resolve overlapping targets by prioritizing the smaller surface)
    pyID                 DragDropAcceptIdCurr;               // Target item id (set at the time of accepting the payload)
    pyID                 DragDropAcceptIdPrev;               // Target item id from previous frame (we need to store this to allow for overlapping drag and drop targets)
    int                     DragDropAcceptFrameCount;           // Last time a target expressed a desire to accept the source
    pyID                 DragDropHoldJustPressedId;          // Set when holding a payload just made ButtonBehavior() return a press.
    ImVector<unsigned char> DragDropPayloadBufHeap;             // We don't expose the ImVector<> directly, pyPayload only holds pointer+size
    unsigned char           DragDropPayloadBufLocal[16];        // Local buffer for small payloads

    // Table
    pyTable*                     CurrentTable;
    int                             CurrentTableStackIdx;
    ImPool<pyTable>              Tables;
    ImVector<pyTableTempData>    TablesTempDataStack;
    ImVector<float>                 TablesLastTimeActive;       // Last used timestamp of each tables (SOA, for efficient GC)
    ImVector<ImDrawChannel>         DrawChannelsTempMergeBuffer;

    // Tab bars
    pyTabBar*                    CurrentTabBar;
    ImPool<pyTabBar>             TabBars;
    ImVector<pyPtrOrIndex>       CurrentTabBarStack;
    ImVector<pyShrinkWidthItem>  ShrinkWidthBuffer;

    // Widget state
    ImVec2                  MouseLastValidPos;
    pyInputTextState     InputTextState;
    ImFont                  InputTextPasswordFont;
    pyID                 TempInputId;                        // Temporary text input when CTRL+clicking on a slider, etc.
    pyColorEditFlags     ColorEditOptions;                   // Store user options for color edit widgets
    float                   ColorEditLastHue;                   // Backup of last Hue associated to LastColor, so we can restore Hue in lossy RGB<>HSV round trips
    float                   ColorEditLastSat;                   // Backup of last Saturation associated to LastColor, so we can restore Saturation in lossy RGB<>HSV round trips
    ImU32                   ColorEditLastColor;                 // RGB value with alpha set to 0.
    ImVec4                  ColorPickerRef;                     // Initial/reference color at the time of opening the color picker.
    pyComboPreviewData   ComboPreviewData;
    float                   SliderCurrentAccum;                 // Accumulated slider delta when using navigation controls.
    bool                    SliderCurrentAccumDirty;            // Has the accumulated slider delta changed since last time we tried to apply it?
    bool                    DragCurrentAccumDirty;
    float                   DragCurrentAccum;                   // Accumulator for dragging modification. Always high-precision, not rounded by end-user precision settings
    float                   DragSpeedDefaultRatio;              // If speed == 0.0f, uses (max-min) * DragSpeedDefaultRatio
    float                   ScrollbarClickDeltaToGrabCenter;    // Distance between mouse and center of grab box, normalized in parent space. Use storage?
    float                   DisabledAlphaBackup;                // Backup for style.Alpha for BeginDisabled()
    short                   DisabledStackSize;
    short                   TooltipOverrideCount;
    float                   TooltipSlowDelay;                   // Time before slow tooltips appears (FIXME: This is temporary until we merge in tooltip timer+priority work)
    ImVector<char>          ClipboardHandlerData;               // If no custom clipboard handler is defined
    ImVector<pyID>       MenusIdSubmittedThisFrame;          // A list of menu IDs that were rendered at least once

    // Platform support
    ImVec2                  PlatformImePos;                     // Cursor position request & last passed to the OS Input Method Editor
    ImVec2                  PlatformImeLastPos;
    char                    PlatformLocaleDecimalPoint;         // '.' or *localeconv()->decimal_point

    // Settings
    bool                    SettingsLoaded;
    float                   SettingsDirtyTimer;                 // Save .ini Settings to memory when time reaches zero
    pyTextBuffer         SettingsIniData;                    // In memory .ini settings
    ImVector<pySettingsHandler>      SettingsHandlers;       // List of .ini settings handlers
    ImChunkStream<pyWindowSettings>  SettingsWindows;        // pyWindow .ini settings entries
    ImChunkStream<pyTableSettings>   SettingsTables;         // pyTable .ini settings entries
    ImVector<pyContextHook>          Hooks;                  // Hooks for extensions (e.g. test engine)
    pyID                             HookIdNext;             // Next available HookId

    // Capture/Logging
    bool                    LogEnabled;                         // Currently capturing
    pyLogType            LogType;                            // Capture target
    ImFileHandle            LogFile;                            // If != NULL log to stdout/ file
    pyTextBuffer         LogBuffer;                          // Accumulation buffer when log to clipboard. This is pointer so our Gpy static constructor doesn't call heap allocators.
    const char*             LogNextPrefix;
    const char*             LogNextSuffix;
    float                   LogLinePosY;
    bool                    LogLineFirstItem;
    int                     LogDepthRef;
    int                     LogDepthToExpand;
    int                     LogDepthToExpandDefault;            // Default/stored value for LogDepthMaxExpand if not specified in the LogXXX function call.

    // Debug Tools
    bool                    DebugItemPickerActive;              // Item picker is active (started with DebugStartItemPicker())
    pyID                 DebugItemPickerBreakId;             // Will call IM_DEBUG_BREAK() when encountering this ID
    pyMetricsConfig      DebugMetricsConfig;
    pyStackTool          DebugStackTool;

    // Misc
    float                   FramerateSecPerFrame[120];          // Calculate estimate of framerate for user over the last 2 seconds.
    int                     FramerateSecPerFrameIdx;
    int                     FramerateSecPerFrameCount;
    float                   FramerateSecPerFrameAccum;
    int                     WantCaptureMouseNextFrame;          // Explicit capture via CaptureKeyboardFromApp()/CaptureMouseFromApp() sets those flags
    int                     WantCaptureKeyboardNextFrame;
    int                     WantTextInputNextFrame;
    char                    TempBuffer[1024 * 3 + 1];           // Temporary text buffer

    pyContext(ImFontAtlas* shared_font_atlas)
    {
        Initialized = false;
        FontAtlasOwnedByContext = shared_font_atlas ? false : true;
        Font = NULL;
        FontSize = FontBaseSize = 0.0f;
        IO.Fonts = shared_font_atlas ? shared_font_atlas : IM_NEW(ImFontAtlas)();
        Time = 0.0f;
        FrameCount = 0;
        FrameCountEnded = FrameCountRendered = -1;
        WithinFrameScope = WithinFrameScopeWithImplicitWindow = WithinEndChild = false;
        GcCompactAll = false;
        TestEngineHookItems = false;
        TestEngine = NULL;

        WindowsActiveCount = 0;
        CurrentWindow = NULL;
        HoveredWindow = NULL;
        HoveredWindowUnderMovingWindow = NULL;
        MovingWindow = NULL;
        WheelingWindow = NULL;
        WheelingWindowTimer = 0.0f;

        DebugHookIdInfo = 0;
        HoveredId = HoveredIdPreviousFrame = 0;
        HoveredIdAllowOverlap = false;
        HoveredIdUsingMouseWheel = HoveredIdPreviousFrameUsingMouseWheel = false;
        HoveredIdDisabled = false;
        HoveredIdTimer = HoveredIdNotActiveTimer = 0.0f;
        ActiveId = 0;
        ActiveIdIsAlive = 0;
        ActiveIdTimer = 0.0f;
        ActiveIdIsJustActivated = false;
        ActiveIdAllowOverlap = false;
        ActiveIdNoClearOnFocusLoss = false;
        ActiveIdHasBeenPressedBefore = false;
        ActiveIdHasBeenEditedBefore = false;
        ActiveIdHasBeenEditedThisFrame = false;
        ActiveIdUsingMouseWheel = false;
        ActiveIdUsingNavDirMask = 0x00;
        ActiveIdUsingNavInputMask = 0x00;
        ActiveIdUsingKeyInputMask = 0x00;
        ActiveIdClickOffset = ImVec2(-1, -1);
        ActiveIdWindow = NULL;
        ActiveIdSource = pyInputSource_None;
        ActiveIdMouseButton = -1;
        ActiveIdPreviousFrame = 0;
        ActiveIdPreviousFrameIsAlive = false;
        ActiveIdPreviousFrameHasBeenEditedBefore = false;
        ActiveIdPreviousFrameWindow = NULL;
        LastActiveId = 0;
        LastActiveIdTimer = 0.0f;

        CurrentItemFlags = pyItemFlags_None;

        NavWindow = NULL;
        NavId = NavFocusScopeId = NavActivateId = NavActivateDownId = NavActivatePressedId = NavActivateInputId = 0;
        NavJustTabbedId = NavJustMovedToId = NavJustMovedToFocusScopeId = NavNextActivateId = 0;
        NavActivateFlags = NavNextActivateFlags = pyActivateFlags_None;
        NavJustMovedToKeyMods = pyKeyModFlags_None;
        NavInputSource = pyInputSource_None;
        NavLayer = pyNavLayer_Main;
        NavIdTabCounter = INT_MAX;
        NavIdIsAlive = false;
        NavMousePosDirty = false;
        NavDisableHighlight = true;
        NavDisableMouseHover = false;
        NavAnyRequest = false;
        NavInitRequest = false;
        NavInitRequestFromMove = false;
        NavInitResultId = 0;
        NavMoveSubmitted = false;
        NavMoveScoringItems = false;
        NavMoveForwardToNextFrame = false;
        NavMoveFlags = pyNavMoveFlags_None;
        NavMoveScrollFlags = pyScrollFlags_None;
        NavMoveKeyMods = pyKeyModFlags_None;
        NavMoveDir = NavMoveDirForDebug = NavMoveClipDir = pyDir_None;
        NavScoringDebugCount = 0;
        NavTabbingInputableRemaining = 0;

        NavWindowingTarget = NavWindowingTargetAnim = NavWindowingListWindow = NULL;
        NavWindowingTimer = NavWindowingHighlightAlpha = 0.0f;
        NavWindowingToggleLayer = false;

        TabFocusRequestCurrWindow = TabFocusRequestNextWindow = NULL;
        TabFocusRequestCurrCounterTabStop = INT_MAX;
        TabFocusRequestNextCounterTabStop = INT_MAX;
        TabFocusPressed = false;

        DimBgRatio = 0.0f;
        MouseCursor = pyMouseCursor_Arrow;

        DragDropActive = DragDropWithinSource = DragDropWithinTarget = false;
        DragDropSourceFlags = pyDragDropFlags_None;
        DragDropSourceFrameCount = -1;
        DragDropMouseButton = -1;
        DragDropTargetId = 0;
        DragDropAcceptFlags = pyDragDropFlags_None;
        DragDropAcceptIdCurrRectSurface = 0.0f;
        DragDropAcceptIdPrev = DragDropAcceptIdCurr = 0;
        DragDropAcceptFrameCount = -1;
        DragDropHoldJustPressedId = 0;
        memset(DragDropPayloadBufLocal, 0, sizeof(DragDropPayloadBufLocal));

        CurrentTable = NULL;
        CurrentTableStackIdx = -1;
        CurrentTabBar = NULL;

        TempInputId = 0;
        ColorEditOptions = pyColorEditFlags_DefaultOptions_;
        ColorEditLastHue = ColorEditLastSat = 0.0f;
        ColorEditLastColor = 0;
        SliderCurrentAccum = 0.0f;
        SliderCurrentAccumDirty = false;
        DragCurrentAccumDirty = false;
        DragCurrentAccum = 0.0f;
        DragSpeedDefaultRatio = 1.0f / 100.0f;
        DisabledAlphaBackup = 0.0f;
        DisabledStackSize = 0;
        ScrollbarClickDeltaToGrabCenter = 0.0f;
        TooltipOverrideCount = 0;
        TooltipSlowDelay = 0.50f;

        PlatformImePos = PlatformImeLastPos = ImVec2(FLT_MAX, FLT_MAX);
        PlatformLocaleDecimalPoint = '.';

        SettingsLoaded = false;
        SettingsDirtyTimer = 0.0f;
        HookIdNext = 0;

        LogEnabled = false;
        LogType = pyLogType_None;
        LogNextPrefix = LogNextSuffix = NULL;
        LogFile = NULL;
        LogLinePosY = FLT_MAX;
        LogLineFirstItem = false;
        LogDepthRef = 0;
        LogDepthToExpand = LogDepthToExpandDefault = 2;

        DebugItemPickerActive = false;
        DebugItemPickerBreakId = 0;

        memset(FramerateSecPerFrame, 0, sizeof(FramerateSecPerFrame));
        FramerateSecPerFrameIdx = FramerateSecPerFrameCount = 0;
        FramerateSecPerFrameAccum = 0.0f;
        WantCaptureMouseNextFrame = WantCaptureKeyboardNextFrame = WantTextInputNextFrame = -1;
        memset(TempBuffer, 0, sizeof(TempBuffer));
    }
};

//-----------------------------------------------------------------------------
// [SECTION] pyWindowTempData, pyWindow
//-----------------------------------------------------------------------------

// Transient per-window data, reset at the beginning of the frame. This used to be called pyDrawContext, hence the DC variable name in pyWindow.
// (That's theory, in practice the delimitation between pyWindow and pyWindowTempData is quite tenuous and could be reconsidered..)
// (This doesn't need a constructor because we zero-clear it as part of pyWindow and all frame-temporary data are setup on Begin)
struct py_API pyWindowTempData
{
    // Layout
    ImVec2                  CursorPos;              // Current emitting position, in absolute coordinates.
    ImVec2                  CursorPosPrevLine;
    ImVec2                  CursorStartPos;         // Initial position after Begin(), generally ~ window position + WindowPadding.
    ImVec2                  CursorMaxPos;           // Used to implicitly calculate ContentSize at the beginning of next frame, for scrolling range and auto-resize. Always growing during the frame.
    ImVec2                  IdealMaxPos;            // Used to implicitly calculate ContentSizeIdeal at the beginning of next frame, for auto-resize only. Always growing during the frame.
    ImVec2                  CurrLineSize;
    ImVec2                  PrevLineSize;
    float                   CurrLineTextBaseOffset; // Baseline offset (0.0f by default on a new line, generally == style.FramePadding.y when a framed item has been added).
    float                   PrevLineTextBaseOffset;
    ImVec1                  Indent;                 // Indentation / start position from left of window (increased by TreePush/TreePop, etc.)
    ImVec1                  ColumnsOffset;          // Offset to the current column (if ColumnsCurrent > 0). FIXME: This and the above should be a stack to allow use cases like Tree->Column->Tree. Need revamp columns API.
    ImVec1                  GroupOffset;

    // Keyboard/Gamepad navigation
    pyNavLayer           NavLayerCurrent;        // Current layer, 0..31 (we currently only use 0..1)
    short                   NavLayersActiveMask;    // Which layers have been written to (result from previous frame)
    short                   NavLayersActiveMaskNext;// Which layers have been written to (accumulator for current frame)
    pyID                 NavFocusScopeIdCurrent; // Current focus scope ID while appending
    bool                    NavHideHighlightOneFrame;
    bool                    NavHasScroll;           // Set when scrolling can be used (ScrollMax > 0.0f)

    // Miscellaneous
    bool                    MenuBarAppending;       // FIXME: Remove this
    ImVec2                  MenuBarOffset;          // MenuBarOffset.x is sort of equivalent of a per-layer CursorPos.x, saved/restored as we switch to the menu bar. The only situation when MenuBarOffset.y is > 0 if when (SafeAreaPadding.y > FramePadding.y), often used on TVs.
    pyMenuColumns        MenuColumns;            // Simplified columns storage for menu items measurement
    int                     TreeDepth;              // Current tree depth.
    ImU32                   TreeJumpToParentOnPopMask; // Store a copy of !g.NavIdIsAlive for TreeDepth 0..31.. Could be turned into a ImU64 if necessary.
    ImVector<pyWindow*>  ChildWindows;
    pyStorage*           StateStorage;           // Current persistent per-window storage (store e.g. tree node open/close state)
    pyOldColumns*        CurrentColumns;         // Current columns set
    int                     CurrentTableIdx;        // Current table index (into g.Tables)
    pyLayoutType         LayoutType;
    pyLayoutType         ParentLayoutType;       // Layout type of parent window at the time of Begin()
    int                     FocusCounterTabStop;    // (Legacy Focus/Tabbing system) Same, but only count widgets which you can Tab through.

    // Local parameters stacks
    // We store the current settings outside of the vectors to increase memory locality (reduce cache misses). The vectors are rarely modified. Also it allows us to not heap allocate for short-lived windows which are not using those settings.
    float                   ItemWidth;              // Current item width (>0.0: width in pixels, <0.0: align xx pixels to the right of window).
    float                   TextWrapPos;            // Current text wrap pos.
    ImVector<float>         ItemWidthStack;         // Store item widths to restore (attention: .back() is not == ItemWidth)
    ImVector<float>         TextWrapPosStack;       // Store text wrap pos to restore (attention: .back() is not == TextWrapPos)
};

// Storage for one window
struct py_API pyWindow
{
    char*                   Name;                               // Window name, owned by the window.
    pyID                 ID;                                 // == ImHashStr(Name)
    pyWindowFlags        Flags;                              // See enum pyWindowFlags_
    ImVec2                  Pos;                                // Position (always rounded-up to nearest pixel)
    ImVec2                  Size;                               // Current size (==SizeFull or collapsed title bar size)
    ImVec2                  SizeFull;                           // Size when non collapsed
    ImVec2                  ContentSize;                        // Size of contents/scrollable client area (calculated from the extents reach of the cursor) from previous frame. Does not include window decoration or window padding.
    ImVec2                  ContentSizeIdeal;
    ImVec2                  ContentSizeExplicit;                // Size of contents/scrollable client area explicitly request by the user via SetNextWindowContentSize().
    ImVec2                  WindowPadding;                      // Window padding at the time of Begin().
    float                   WindowRounding;                     // Window rounding at the time of Begin(). May be clamped lower to avoid rendering artifacts with title bar, menu bar etc.
    float                   WindowBorderSize;                   // Window border size at the time of Begin().
    int                     NameBufLen;                         // Size of buffer storing Name. May be larger than strlen(Name)!
    pyID                 MoveId;                             // == window->GetID("#MOVE")
    pyID                 ChildId;                            // ID of corresponding item in parent window (for navigation to return from child window to parent window)
    ImVec2                  Scroll;
    ImVec2                  ScrollMax;
    ImVec2                  ScrollTarget;                       // target scroll position. stored as cursor position with scrolling canceled out, so the highest point is always 0.0f. (FLT_MAX for no change)
    ImVec2                  ScrollTargetCenterRatio;            // 0.0f = scroll so that target position is at top, 0.5f = scroll so that target position is centered
    ImVec2                  ScrollTargetEdgeSnapDist;           // 0.0f = no snapping, >0.0f snapping threshold
    ImVec2                  ScrollbarSizes;                     // Size taken by each scrollbars on their smaller axis. Pay attention! ScrollbarSizes.x == width of the vertical scrollbar, ScrollbarSizes.y = height of the horizontal scrollbar.
    bool                    ScrollbarX, ScrollbarY;             // Are scrollbars visible?
    bool                    Active;                             // Set to true on Begin(), unless Collapsed
    bool                    WasActive;
    bool                    WriteAccessed;                      // Set to true when any widget access the current window
    bool                    Collapsed;                          // Set when collapsing window to become only title-bar
    bool                    WantCollapseToggle;
    bool                    SkipItems;                          // Set when items can safely be all clipped (e.g. window not visible or collapsed)
    bool                    Appearing;                          // Set during the frame where the window is appearing (or re-appearing)
    bool                    Hidden;                             // Do not display (== HiddenFrames*** > 0)
    bool                    IsFallbackWindow;                   // Set on the "Debug##Default" window.
    bool                    HasCloseButton;                     // Set when the window has a close button (p_open != NULL)
    signed char             ResizeBorderHeld;                   // Current border being held for resize (-1: none, otherwise 0-3)
    short                   BeginCount;                         // Number of Begin() during the current frame (generally 0 or 1, 1+ if appending via multiple Begin/End pairs)
    short                   BeginOrderWithinParent;             // Begin() order within immediate parent window, if we are a child window. Otherwise 0.
    short                   BeginOrderWithinContext;            // Begin() order within entire py context. This is mostly used for debugging submission order related issues.
    short                   FocusOrder;                         // Order within WindowsFocusOrder[], altered when windows are focused.
    pyID                 PopupId;                            // ID in the popup stack when this window is used as a popup/menu (because we use generic Name/ID for recycling)
    ImS8                    AutoFitFramesX, AutoFitFramesY;
    ImS8                    AutoFitChildAxises;
    bool                    AutoFitOnlyGrows;
    pyDir                AutoPosLastDirection;
    ImS8                    HiddenFramesCanSkipItems;           // Hide the window for N frames
    ImS8                    HiddenFramesCannotSkipItems;        // Hide the window for N frames while allowing items to be submitted so we can measure their size
    ImS8                    HiddenFramesForRenderOnly;          // Hide the window until frame N at Render() time only
    ImS8                    DisableInputsFrames;                // Disable window interactions for N frames
    pyCond               SetWindowPosAllowFlags : 8;         // store acceptable condition flags for SetNextWindowPos() use.
    pyCond               SetWindowSizeAllowFlags : 8;        // store acceptable condition flags for SetNextWindowSize() use.
    pyCond               SetWindowCollapsedAllowFlags : 8;   // store acceptable condition flags for SetNextWindowCollapsed() use.
    ImVec2                  SetWindowPosVal;                    // store window position when using a non-zero Pivot (position set needs to be processed when we know the window size)
    ImVec2                  SetWindowPosPivot;                  // store window pivot for positioning. ImVec2(0, 0) when positioning from top-left corner; ImVec2(0.5f, 0.5f) for centering; ImVec2(1, 1) for bottom right.

    ImVector<pyID>       IDStack;                            // ID stack. ID are hashes seeded with the value at the top of the stack. (In theory this should be in the TempData structure)
    pyWindowTempData     DC;                                 // Temporary per-window data, reset at the beginning of the frame. This used to be called pyDrawContext, hence the "DC" variable name.

    // The best way to understand what those rectangles are is to use the 'Metrics->Tools->Show Windows Rectangles' viewer.
    // The main 'OuterRect', omitted as a field, is window->Rect().
    ImRect                  OuterRectClipped;                   // == Window->Rect() just after setup in Begin(). == window->Rect() for root window.
    ImRect                  InnerRect;                          // Inner rectangle (omit title bar, menu bar, scroll bar)
    ImRect                  InnerClipRect;                      // == InnerRect shrunk by WindowPadding*0.5f on each side, clipped within viewport or parent clip rect.
    ImRect                  WorkRect;                           // Initially covers the whole scrolling region. Reduced by containers e.g columns/tables when active. Shrunk by WindowPadding*1.0f on each side. This is meant to replace ContentRegionRect over time (from 1.71+ onward).
    ImRect                  ParentWorkRect;                     // Backup of WorkRect before entering a container such as columns/tables. Used by e.g. SpanAllColumns functions to easily access. Stacked containers are responsible for maintaining this. // FIXME-WORKRECT: Could be a stack?
    ImRect                  ClipRect;                           // Current clipping/scissoring rectangle, evolve as we are using PushClipRect(), etc. == DrawList->clip_rect_stack.back().
    ImRect                  ContentRegionRect;                  // FIXME: This is currently confusing/misleading. It is essentially WorkRect but not handling of scrolling. We currently rely on it as right/bottom aligned sizing operation need some size to rely on.
    ImVec2ih                HitTestHoleSize;                    // Define an optional rectangular hole where mouse will pass-through the window.
    ImVec2ih                HitTestHoleOffset;

    int                     LastFrameActive;                    // Last frame number the window was Active.
    float                   LastTimeActive;                     // Last timestamp the window was Active (using float as we don't need high precision there)
    float                   ItemWidthDefault;
    pyStorage            StateStorage;
    ImVector<pyOldColumns> ColumnsStorage;
    float                   FontWindowScale;                    // User scale multiplier per-window, via SetWindowFontScale()
    int                     SettingsOffset;                     // Offset into SettingsWindows[] (offsets are always valid as we only grow the array from the back)

    ImDrawList*             DrawList;                           // == &DrawListInst (for backward compatibility reason with code using py_internal.h we keep this a pointer)
    ImDrawList              DrawListInst;
    pyWindow*            ParentWindow;                       // If we are a child _or_ popup _or_ docked window, this is pointing to our parent. Otherwise NULL.
    pyWindow*            RootWindow;                         // Point to ourself or first ancestor that is not a child window. Doesn't cross through popups/dock nodes.
    pyWindow*            RootWindowPopupTree;                // Point to ourself or first ancestor that is not a child window. Cross through popups parent<>child.
    pyWindow*            RootWindowForTitleBarHighlight;     // Point to ourself or first ancestor which will display TitleBgActive color when this window is active.
    pyWindow*            RootWindowForNav;                   // Point to ourself or first ancestor which doesn't have the NavFlattened flag.

    pyWindow*            NavLastChildNavWindow;              // When going to the menu bar, we remember the child window we came from. (This could probably be made implicit if we kept g.Windows sorted by last focused including child window.)
    pyID                 NavLastIds[pyNavLayer_COUNT];    // Last known NavId for this window, per layer (0/1)
    ImRect                  NavRectRel[pyNavLayer_COUNT];    // Reference rectangle, in window relative space

    int                     MemoryDrawListIdxCapacity;          // Backup of last idx/vtx count, so when waking up the window we can preallocate and avoid iterative alloc/copy
    int                     MemoryDrawListVtxCapacity;
    bool                    MemoryCompacted;                    // Set when window extraneous data have been garbage collected

public:
    pyWindow(pyContext* context, const char* name);
    ~pyWindow();

    pyID     GetID(const char* str, const char* str_end = NULL);
    pyID     GetID(const void* ptr);
    pyID     GetID(int n);
    pyID     GetIDNoKeepAlive(const char* str, const char* str_end = NULL);
    pyID     GetIDNoKeepAlive(const void* ptr);
    pyID     GetIDNoKeepAlive(int n);
    pyID     GetIDFromRectangle(const ImRect& r_abs);

    // We don't use g.FontSize because the window may be != g.CurrentWidow.
    ImRect      Rect() const            { return ImRect(Pos.x, Pos.y, Pos.x + Size.x, Pos.y + Size.y); }
    float       CalcFontSize() const    { pyContext& g = *Gpy; float scale = g.FontBaseSize * FontWindowScale; if (ParentWindow) scale *= ParentWindow->FontWindowScale; return scale; }
    float       TitleBarHeight() const  { pyContext& g = *Gpy; return (Flags & pyWindowFlags_NoTitleBar) ? 0.0f : CalcFontSize() + g.Style.FramePadding.y * 2.0f; }
    ImRect      TitleBarRect() const    { return ImRect(Pos, ImVec2(Pos.x + SizeFull.x, Pos.y + TitleBarHeight())); }
    float       MenuBarHeight() const   { pyContext& g = *Gpy; return (Flags & pyWindowFlags_MenuBar) ? DC.MenuBarOffset.y + CalcFontSize() + g.Style.FramePadding.y * 2.0f : 0.0f; }
    ImRect      MenuBarRect() const     { float y1 = Pos.y + TitleBarHeight(); return ImRect(Pos.x, y1, Pos.x + SizeFull.x, y1 + MenuBarHeight()); }
};

//-----------------------------------------------------------------------------
// [SECTION] Tab bar, Tab item support
//-----------------------------------------------------------------------------

// Extend pyTabBarFlags_
enum pyTabBarFlagsPrivate_
{
    pyTabBarFlags_DockNode                   = 1 << 20,  // Part of a dock node [we don't use this in the master branch but it facilitate branch syncing to keep this around]
    pyTabBarFlags_IsFocused                  = 1 << 21,
    pyTabBarFlags_SaveSettings               = 1 << 22   // FIXME: Settings are handled by the docking system, this only request the tab bar to mark settings dirty when reordering tabs
};

// Extend pyTabItemFlags_
enum pyTabItemFlagsPrivate_
{
    pyTabItemFlags_SectionMask_              = pyTabItemFlags_Leading | pyTabItemFlags_Trailing,
    pyTabItemFlags_NoCloseButton             = 1 << 20,  // Track whether p_open was set or not (we'll need this info on the next frame to recompute ContentWidth during layout)
    pyTabItemFlags_Button                    = 1 << 21   // Used by TabItemButton, change the tab item behavior to mimic a button
};

// Storage for one active tab item (sizeof() 40 bytes)
struct pyTabItem
{
    pyID             ID;
    pyTabItemFlags   Flags;
    int                 LastFrameVisible;
    int                 LastFrameSelected;      // This allows us to infer an ordered list of the last activated tabs with little maintenance
    float               Offset;                 // Position relative to beginning of tab
    float               Width;                  // Width currently displayed
    float               ContentWidth;           // Width of label, stored during BeginTabItem() call
    ImS32               NameOffset;             // When Window==NULL, offset to name within parent pyTabBar::TabsNames
    ImS16               BeginOrder;             // BeginTabItem() order, used to re-order tabs after toggling pyTabBarFlags_Reorderable
    ImS16               IndexDuringLayout;      // Index only used during TabBarLayout()
    bool                WantClose;              // Marked as closed by SetTabItemClosed()

    pyTabItem()      { memset(this, 0, sizeof(*this)); LastFrameVisible = LastFrameSelected = -1; NameOffset = -1; BeginOrder = IndexDuringLayout = -1; }
};

// Storage for a tab bar (sizeof() 152 bytes)
struct pyTabBar
{
    ImVector<pyTabItem> Tabs;
    pyTabBarFlags    Flags;
    pyID             ID;                     // Zero for tab-bars used by docking
    pyID             SelectedTabId;          // Selected tab/window
    pyID             NextSelectedTabId;      // Next selected tab/window. Will also trigger a scrolling animation
    pyID             VisibleTabId;           // Can occasionally be != SelectedTabId (e.g. when previewing contents for CTRL+TAB preview)
    int                 CurrFrameVisible;
    int                 PrevFrameVisible;
    ImRect              BarRect;
    float               CurrTabsContentsHeight;
    float               PrevTabsContentsHeight; // Record the height of contents submitted below the tab bar
    float               WidthAllTabs;           // Actual width of all tabs (locked during layout)
    float               WidthAllTabsIdeal;      // Ideal width if all tabs were visible and not clipped
    float               ScrollingAnim;
    float               ScrollingTarget;
    float               ScrollingTargetDistToVisibility;
    float               ScrollingSpeed;
    float               ScrollingRectMinX;
    float               ScrollingRectMaxX;
    pyID             ReorderRequestTabId;
    ImS16               ReorderRequestOffset;
    ImS8                BeginCount;
    bool                WantLayout;
    bool                VisibleTabWasSubmitted;
    bool                TabsAddedNew;           // Set to true when a new tab item or button has been added to the tab bar during last frame
    ImS16               TabsActiveCount;        // Number of tabs submitted this frame.
    ImS16               LastTabItemIdx;         // Index of last BeginTabItem() tab for use by EndTabItem()
    float               ItemSpacingY;
    ImVec2              FramePadding;           // style.FramePadding locked at the time of BeginTabBar()
    ImVec2              BackupCursorPos;
    pyTextBuffer     TabsNames;              // For non-docking tab bar we re-append names in a contiguous buffer.

    pyTabBar();
    int                 GetTabOrder(const pyTabItem* tab) const  { return Tabs.index_from_ptr(tab); }
    const char*         GetTabName(const pyTabItem* tab) const
    {
        IM_ASSERT(tab->NameOffset != -1 && tab->NameOffset < TabsNames.Buf.Size);
        return TabsNames.Buf.Data + tab->NameOffset;
    }
};

//-----------------------------------------------------------------------------
// [SECTION] Table support
//-----------------------------------------------------------------------------

#define IM_COL32_DISABLE                IM_COL32(0,0,0,1)   // Special sentinel code which cannot be used as a regular color.
#define py_TABLE_MAX_COLUMNS         64                  // sizeof(ImU64) * 8. This is solely because we frequently encode columns set in a ImU64.
#define py_TABLE_MAX_DRAW_CHANNELS   (4 + 64 * 2)        // See TableSetupDrawChannels()

// Our current column maximum is 64 but we may raise that in the future.
typedef ImS8 pyTableColumnIdx;
typedef ImU8 pyTableDrawChannelIdx;

// [Internal] sizeof() ~ 104
// We use the terminology "Enabled" to refer to a column that is not Hidden by user/api.
// We use the terminology "Clipped" to refer to a column that is out of sight because of scrolling/clipping.
// This is in contrast with some user-facing api such as IsItemVisible() / IsRectVisible() which use "Visible" to mean "not clipped".
struct pyTableColumn
{
    pyTableColumnFlags   Flags;                          // Flags after some patching (not directly same as provided by user). See pyTableColumnFlags_
    float                   WidthGiven;                     // Final/actual width visible == (MaxX - MinX), locked in TableUpdateLayout(). May be > WidthRequest to honor minimum width, may be < WidthRequest to honor shrinking columns down in tight space.
    float                   MinX;                           // Absolute positions
    float                   MaxX;
    float                   WidthRequest;                   // Master width absolute value when !(Flags & _WidthStretch). When Stretch this is derived every frame from StretchWeight in TableUpdateLayout()
    float                   WidthAuto;                      // Automatic width
    float                   StretchWeight;                  // Master width weight when (Flags & _WidthStretch). Often around ~1.0f initially.
    float                   InitStretchWeightOrWidth;       // Value passed to TableSetupColumn(). For Width it is a content width (_without padding_).
    ImRect                  ClipRect;                       // Clipping rectangle for the column
    pyID                 UserID;                         // Optional, value passed to TableSetupColumn()
    float                   WorkMinX;                       // Contents region min ~(MinX + CellPaddingX + CellSpacingX1) == cursor start position when entering column
    float                   WorkMaxX;                       // Contents region max ~(MaxX - CellPaddingX - CellSpacingX2)
    float                   ItemWidth;                      // Current item width for the column, preserved across rows
    float                   ContentMaxXFrozen;              // Contents maximum position for frozen rows (apart from headers), from which we can infer content width.
    float                   ContentMaxXUnfrozen;
    float                   ContentMaxXHeadersUsed;         // Contents maximum position for headers rows (regardless of freezing). TableHeader() automatically softclip itself + report ideal desired size, to avoid creating extraneous draw calls
    float                   ContentMaxXHeadersIdeal;
    ImS16                   NameOffset;                     // Offset into parent ColumnsNames[]
    pyTableColumnIdx     DisplayOrder;                   // Index within Table's IndexToDisplayOrder[] (column may be reordered by users)
    pyTableColumnIdx     IndexWithinEnabledSet;          // Index within enabled/visible set (<= IndexToDisplayOrder)
    pyTableColumnIdx     PrevEnabledColumn;              // Index of prev enabled/visible column within Columns[], -1 if first enabled/visible column
    pyTableColumnIdx     NextEnabledColumn;              // Index of next enabled/visible column within Columns[], -1 if last enabled/visible column
    pyTableColumnIdx     SortOrder;                      // Index of this column within sort specs, -1 if not sorting on this column, 0 for single-sort, may be >0 on multi-sort
    pyTableDrawChannelIdx DrawChannelCurrent;            // Index within DrawSplitter.Channels[]
    pyTableDrawChannelIdx DrawChannelFrozen;             // Draw channels for frozen rows (often headers)
    pyTableDrawChannelIdx DrawChannelUnfrozen;           // Draw channels for unfrozen rows
    bool                    IsEnabled;                      // IsUserEnabled && (Flags & pyTableColumnFlags_Disabled) == 0
    bool                    IsUserEnabled;                  // Is the column not marked Hidden by the user? (unrelated to being off view, e.g. clipped by scrolling).
    bool                    IsUserEnabledNextFrame;
    bool                    IsVisibleX;                     // Is actually in view (e.g. overlapping the host window clipping rectangle, not scrolled).
    bool                    IsVisibleY;
    bool                    IsRequestOutput;                // Return value for TableSetColumnIndex() / TableNextColumn(): whether we request user to output contents or not.
    bool                    IsSkipItems;                    // Do we want item submissions to this column to be completely ignored (no layout will happen).
    bool                    IsPreserveWidthAuto;
    ImS8                    NavLayerCurrent;                // pyNavLayer in 1 byte
    ImU8                    AutoFitQueue;                   // Queue of 8 values for the next 8 frames to request auto-fit
    ImU8                    CannotSkipItemsQueue;           // Queue of 8 values for the next 8 frames to disable Clipped/SkipItem
    ImU8                    SortDirection : 2;              // pySortDirection_Ascending or pySortDirection_Descending
    ImU8                    SortDirectionsAvailCount : 2;   // Number of available sort directions (0 to 3)
    ImU8                    SortDirectionsAvailMask : 4;    // Mask of available sort directions (1-bit each)
    ImU8                    SortDirectionsAvailList;        // Ordered of available sort directions (2-bits each)

    pyTableColumn()
    {
        memset(this, 0, sizeof(*this));
        StretchWeight = WidthRequest = -1.0f;
        NameOffset = -1;
        DisplayOrder = IndexWithinEnabledSet = -1;
        PrevEnabledColumn = NextEnabledColumn = -1;
        SortOrder = -1;
        SortDirection = pySortDirection_None;
        DrawChannelCurrent = DrawChannelFrozen = DrawChannelUnfrozen = (ImU8)-1;
    }
};

// Transient cell data stored per row.
// sizeof() ~ 6
struct pyTableCellData
{
    ImU32                       BgColor;    // Actual color
    pyTableColumnIdx         Column;     // Column number
};

// FIXME-TABLE: more transient data could be stored in a per-stacked table structure: DrawSplitter, SortSpecs, incoming RowData
struct pyTable
{
    pyID                     ID;
    pyTableFlags             Flags;
    void*                       RawData;                    // Single allocation to hold Columns[], DisplayOrderToIndex[] and RowCellData[]
    pyTableTempData*         TempData;                   // Transient data while table is active. Point within g.CurrentTableStack[]
    ImSpan<pyTableColumn>    Columns;                    // Point within RawData[]
    ImSpan<pyTableColumnIdx> DisplayOrderToIndex;        // Point within RawData[]. Store display order of columns (when not reordered, the values are 0...Count-1)
    ImSpan<pyTableCellData>  RowCellData;                // Point within RawData[]. Store cells background requests for current row.
    ImU64                       EnabledMaskByDisplayOrder;  // Column DisplayOrder -> IsEnabled map
    ImU64                       EnabledMaskByIndex;         // Column Index -> IsEnabled map (== not hidden by user/api) in a format adequate for iterating column without touching cold data
    ImU64                       VisibleMaskByIndex;         // Column Index -> IsVisibleX|IsVisibleY map (== not hidden by user/api && not hidden by scrolling/cliprect)
    ImU64                       RequestOutputMaskByIndex;   // Column Index -> IsVisible || AutoFit (== expect user to submit items)
    pyTableFlags             SettingsLoadedFlags;        // Which data were loaded from the .ini file (e.g. when order is not altered we won't save order)
    int                         SettingsOffset;             // Offset in g.SettingsTables
    int                         LastFrameActive;
    int                         ColumnsCount;               // Number of columns declared in BeginTable()
    int                         CurrentRow;
    int                         CurrentColumn;
    ImS16                       InstanceCurrent;            // Count of BeginTable() calls with same ID in the same frame (generally 0). This is a little bit similar to BeginCount for a window, but multiple table with same ID look are multiple tables, they are just synched.
    ImS16                       InstanceInteracted;         // Mark which instance (generally 0) of the same ID is being interacted with
    float                       RowPosY1;
    float                       RowPosY2;
    float                       RowMinHeight;               // Height submitted to TableNextRow()
    float                       RowTextBaseline;
    float                       RowIndentOffsetX;
    pyTableRowFlags          RowFlags : 16;              // Current row flags, see pyTableRowFlags_
    pyTableRowFlags          LastRowFlags : 16;
    int                         RowBgColorCounter;          // Counter for alternating background colors (can be fast-forwarded by e.g clipper), not same as CurrentRow because header rows typically don't increase this.
    ImU32                       RowBgColor[2];              // Background color override for current row.
    ImU32                       BorderColorStrong;
    ImU32                       BorderColorLight;
    float                       BorderX1;
    float                       BorderX2;
    float                       HostIndentX;
    float                       MinColumnWidth;
    float                       OuterPaddingX;
    float                       CellPaddingX;               // Padding from each borders
    float                       CellPaddingY;
    float                       CellSpacingX1;              // Spacing between non-bordered cells
    float                       CellSpacingX2;
    float                       LastOuterHeight;            // Outer height from last frame
    float                       LastFirstRowHeight;         // Height of first row from last frame
    float                       InnerWidth;                 // User value passed to BeginTable(), see comments at the top of BeginTable() for details.
    float                       ColumnsGivenWidth;          // Sum of current column width
    float                       ColumnsAutoFitWidth;        // Sum of ideal column width in order nothing to be clipped, used for auto-fitting and content width submission in outer window
    float                       ResizedColumnNextWidth;
    float                       ResizeLockMinContentsX2;    // Lock minimum contents width while resizing down in order to not create feedback loops. But we allow growing the table.
    float                       RefScale;                   // Reference scale to be able to rescale columns on font/dpi changes.
    ImRect                      OuterRect;                  // Note: for non-scrolling table, OuterRect.Max.y is often FLT_MAX until EndTable(), unless a height has been specified in BeginTable().
    ImRect                      InnerRect;                  // InnerRect but without decoration. As with OuterRect, for non-scrolling tables, InnerRect.Max.y is
    ImRect                      WorkRect;
    ImRect                      InnerClipRect;
    ImRect                      BgClipRect;                 // We use this to cpu-clip cell background color fill
    ImRect                      Bg0ClipRectForDrawCmd;      // Actual ImDrawCmd clip rect for BG0/1 channel. This tends to be == OuterWindow->ClipRect at BeginTable() because output in BG0/BG1 is cpu-clipped
    ImRect                      Bg2ClipRectForDrawCmd;      // Actual ImDrawCmd clip rect for BG2 channel. This tends to be a correct, tight-fit, because output to BG2 are done by widgets relying on regular ClipRect.
    ImRect                      HostClipRect;               // This is used to check if we can eventually merge our columns draw calls into the current draw call of the current window.
    ImRect                      HostBackupInnerClipRect;    // Backup of InnerWindow->ClipRect during PushTableBackground()/PopTableBackground()
    pyWindow*                OuterWindow;                // Parent window for the table
    pyWindow*                InnerWindow;                // Window holding the table data (== OuterWindow or a child window)
    pyTextBuffer             ColumnsNames;               // Contiguous buffer holding columns names
    ImDrawListSplitter*         DrawSplitter;               // Shortcut to TempData->DrawSplitter while in table. Isolate draw commands per columns to avoid switching clip rect constantly
    pyTableColumnSortSpecs   SortSpecsSingle;
    ImVector<pyTableColumnSortSpecs> SortSpecsMulti;     // FIXME-OPT: Using a small-vector pattern would be good.
    pyTableSortSpecs         SortSpecs;                  // Public facing sorts specs, this is what we return in TableGetSortSpecs()
    pyTableColumnIdx         SortSpecsCount;
    pyTableColumnIdx         ColumnsEnabledCount;        // Number of enabled columns (<= ColumnsCount)
    pyTableColumnIdx         ColumnsEnabledFixedCount;   // Number of enabled columns (<= ColumnsCount)
    pyTableColumnIdx         DeclColumnsCount;           // Count calls to TableSetupColumn()
    pyTableColumnIdx         HoveredColumnBody;          // Index of column whose visible region is being hovered. Important: == ColumnsCount when hovering empty region after the right-most column!
    pyTableColumnIdx         HoveredColumnBorder;        // Index of column whose right-border is being hovered (for resizing).
    pyTableColumnIdx         AutoFitSingleColumn;        // Index of single column requesting auto-fit.
    pyTableColumnIdx         ResizedColumn;              // Index of column being resized. Reset when InstanceCurrent==0.
    pyTableColumnIdx         LastResizedColumn;          // Index of column being resized from previous frame.
    pyTableColumnIdx         HeldHeaderColumn;           // Index of column header being held.
    pyTableColumnIdx         ReorderColumn;              // Index of column being reordered. (not cleared)
    pyTableColumnIdx         ReorderColumnDir;           // -1 or +1
    pyTableColumnIdx         LeftMostEnabledColumn;      // Index of left-most non-hidden column.
    pyTableColumnIdx         RightMostEnabledColumn;     // Index of right-most non-hidden column.
    pyTableColumnIdx         LeftMostStretchedColumn;    // Index of left-most stretched column.
    pyTableColumnIdx         RightMostStretchedColumn;   // Index of right-most stretched column.
    pyTableColumnIdx         ContextPopupColumn;         // Column right-clicked on, of -1 if opening context menu from a neutral/empty spot
    pyTableColumnIdx         FreezeRowsRequest;          // Requested frozen rows count
    pyTableColumnIdx         FreezeRowsCount;            // Actual frozen row count (== FreezeRowsRequest, or == 0 when no scrolling offset)
    pyTableColumnIdx         FreezeColumnsRequest;       // Requested frozen columns count
    pyTableColumnIdx         FreezeColumnsCount;         // Actual frozen columns count (== FreezeColumnsRequest, or == 0 when no scrolling offset)
    pyTableColumnIdx         RowCellDataCurrent;         // Index of current RowCellData[] entry in current row
    pyTableDrawChannelIdx    DummyDrawChannel;           // Redirect non-visible columns here.
    pyTableDrawChannelIdx    Bg2DrawChannelCurrent;      // For Selectable() and other widgets drawing across columns after the freezing line. Index within DrawSplitter.Channels[]
    pyTableDrawChannelIdx    Bg2DrawChannelUnfrozen;
    bool                        IsLayoutLocked;             // Set by TableUpdateLayout() which is called when beginning the first row.
    bool                        IsInsideRow;                // Set when inside TableBeginRow()/TableEndRow().
    bool                        IsInitializing;
    bool                        IsSortSpecsDirty;
    bool                        IsUsingHeaders;             // Set when the first row had the pyTableRowFlags_Headers flag.
    bool                        IsContextPopupOpen;         // Set when default context menu is open (also see: ContextPopupColumn, InstanceInteracted).
    bool                        IsSettingsRequestLoad;
    bool                        IsSettingsDirty;            // Set when table settings have changed and needs to be reported into pyTableSetttings data.
    bool                        IsDefaultDisplayOrder;      // Set when display order is unchanged from default (DisplayOrder contains 0...Count-1)
    bool                        IsResetAllRequest;
    bool                        IsResetDisplayOrderRequest;
    bool                        IsUnfrozenRows;             // Set when we got past the frozen row.
    bool                        IsDefaultSizingPolicy;      // Set if user didn't explicitly set a sizing policy in BeginTable()
    bool                        MemoryCompacted;
    bool                        HostSkipItems;              // Backup of InnerWindow->SkipItem at the end of BeginTable(), because we will overwrite InnerWindow->SkipItem on a per-column basis

    py_API pyTable()      { memset(this, 0, sizeof(*this)); LastFrameActive = -1; }
    py_API ~pyTable()     { IM_FREE(RawData); }
};

// Transient data that are only needed between BeginTable() and EndTable(), those buffers are shared (1 per level of stacked table).
// - Accessing those requires chasing an extra pointer so for very frequently used data we leave them in the main table structure.
// - We also leave out of this structure data that tend to be particularly useful for debugging/metrics.
struct pyTableTempData
{
    int                         TableIndex;                 // Index in g.Tables.Buf[] pool
    float                       LastTimeActive;             // Last timestamp this structure was used

    ImVec2                      UserOuterSize;              // outer_size.x passed to BeginTable()
    ImDrawListSplitter          DrawSplitter;

    ImRect                      HostBackupWorkRect;         // Backup of InnerWindow->WorkRect at the end of BeginTable()
    ImRect                      HostBackupParentWorkRect;   // Backup of InnerWindow->ParentWorkRect at the end of BeginTable()
    ImVec2                      HostBackupPrevLineSize;     // Backup of InnerWindow->DC.PrevLineSize at the end of BeginTable()
    ImVec2                      HostBackupCurrLineSize;     // Backup of InnerWindow->DC.CurrLineSize at the end of BeginTable()
    ImVec2                      HostBackupCursorMaxPos;     // Backup of InnerWindow->DC.CursorMaxPos at the end of BeginTable()
    ImVec1                      HostBackupColumnsOffset;    // Backup of OuterWindow->DC.ColumnsOffset at the end of BeginTable()
    float                       HostBackupItemWidth;        // Backup of OuterWindow->DC.ItemWidth at the end of BeginTable()
    int                         HostBackupItemWidthStackSize;//Backup of OuterWindow->DC.ItemWidthStack.Size at the end of BeginTable()

    py_API pyTableTempData() { memset(this, 0, sizeof(*this)); LastTimeActive = -1.0f; }
};

// sizeof() ~ 12
struct pyTableColumnSettings
{
    float                   WidthOrWeight;
    pyID                 UserID;
    pyTableColumnIdx     Index;
    pyTableColumnIdx     DisplayOrder;
    pyTableColumnIdx     SortOrder;
    ImU8                    SortDirection : 2;
    ImU8                    IsEnabled : 1; // "Visible" in ini file
    ImU8                    IsStretch : 1;

    pyTableColumnSettings()
    {
        WidthOrWeight = 0.0f;
        UserID = 0;
        Index = -1;
        DisplayOrder = SortOrder = -1;
        SortDirection = pySortDirection_None;
        IsEnabled = 1;
        IsStretch = 0;
    }
};

// This is designed to be stored in a single ImChunkStream (1 header followed by N pyTableColumnSettings, etc.)
struct pyTableSettings
{
    pyID                     ID;                     // Set to 0 to invalidate/delete the setting
    pyTableFlags             SaveFlags;              // Indicate data we want to save using the Resizable/Reorderable/Sortable/Hideable flags (could be using its own flags..)
    float                       RefScale;               // Reference scale to be able to rescale columns on font/dpi changes.
    pyTableColumnIdx         ColumnsCount;
    pyTableColumnIdx         ColumnsCountMax;        // Maximum number of columns this settings instance can store, we can recycle a settings instance with lower number of columns but not higher
    bool                        WantApply;              // Set when loaded from .ini data (to enable merging/loading .ini data into an already running context)

    pyTableSettings()        { memset(this, 0, sizeof(*this)); }
    pyTableColumnSettings*   GetColumnSettings()     { return (pyTableColumnSettings*)(this + 1); }
};

//-----------------------------------------------------------------------------
// [SECTION] py internal API
// No guarantee of forward compatibility here!
//-----------------------------------------------------------------------------

namespace py
{
    // Windows
    // We should always have a CurrentWindow in the stack (there is an implicit "Debug" window)
    // If this ever crash because g.CurrentWindow is NULL it means that either
    // - py::NewFrame() has never been called, which is illegal.
    // - You are calling py functions after py::EndFrame()/py::Render() and before the next py::NewFrame(), which is also illegal.
    inline    pyWindow*  GetCurrentWindowRead()      { pyContext& g = *Gpy; return g.CurrentWindow; }
    inline    pyWindow*  GetCurrentWindow()          { pyContext& g = *Gpy; g.CurrentWindow->WriteAccessed = true; return g.CurrentWindow; }
    py_API pyWindow*  FindWindowByID(pyID id);
    py_API pyWindow*  FindWindowByName(const char* name);
    py_API void          UpdateWindowParentAndRootLinks(pyWindow* window, pyWindowFlags flags, pyWindow* parent_window);
    py_API ImVec2        CalcWindowNextAutoFitSize(pyWindow* window);
    py_API bool          IsWindowChildOf(pyWindow* window, pyWindow* potential_parent, bool popup_hierarchy);
    py_API bool          IsWindowAbove(pyWindow* potential_above, pyWindow* potential_below);
    py_API bool          IsWindowNavFocusable(pyWindow* window);
    py_API void          SetWindowPos(pyWindow* window, const ImVec2& pos, pyCond cond = 0);
    py_API void          SetWindowSize(pyWindow* window, const ImVec2& size, pyCond cond = 0);
    py_API void          SetWindowCollapsed(pyWindow* window, bool collapsed, pyCond cond = 0);
    py_API void          SetWindowHitTestHole(pyWindow* window, const ImVec2& pos, const ImVec2& size);

    // Windows: Display Order and Focus Order
    py_API void          FocusWindow(pyWindow* window);
    py_API void          FocusTopMostWindowUnderOne(pyWindow* under_this_window, pyWindow* ignore_window);
    py_API void          BringWindowToFocusFront(pyWindow* window);
    py_API void          BringWindowToDisplayFront(pyWindow* window);
    py_API void          BringWindowToDisplayBack(pyWindow* window);

    // Fonts, drawing
    py_API void          SetCurrentFont(ImFont* font);
    inline ImFont*          GetDefaultFont() { pyContext& g = *Gpy; return g.IO.FontDefault ? g.IO.FontDefault : g.IO.Fonts->Fonts[0]; }
    inline ImDrawList*      GetForegroundDrawList(pyWindow* window) { IM_UNUSED(window); return GetForegroundDrawList(); } // This seemingly unnecessary wrapper simplifies compatibility between the 'master' and 'docking' branches.
    py_API ImDrawList*   GetBackgroundDrawList(pyViewport* viewport);                     // get background draw list for the given viewport. this draw list will be the first rendering one. Useful to quickly draw shapes/text behind dear py contents.
    py_API ImDrawList*   GetForegroundDrawList(pyViewport* viewport);                     // get foreground draw list for the given viewport. this draw list will be the last rendered one. Useful to quickly draw shapes/text over dear py contents.

    // Init
    py_API void          Initialize(pyContext* context);
    py_API void          Shutdown(pyContext* context);    // Since 1.60 this is a _private_ function. You can call DestroyContext() to destroy the context created by CreateContext().

    // NewFrame
    py_API void          UpdateHoveredWindowAndCaptureFlags();
    py_API void          StartMouseMovingWindow(pyWindow* window);
    py_API void          UpdateMouseMovingWindowNewFrame();
    py_API void          UpdateMouseMovingWindowEndFrame();

    // Generic context hooks
    py_API pyID       AddContextHook(pyContext* context, const pyContextHook* hook);
    py_API void          RemoveContextHook(pyContext* context, pyID hook_to_remove);
    py_API void          CallContextHooks(pyContext* context, pyContextHookType type);

    // Settings
    py_API void                  MarkIniSettingsDirty();
    py_API void                  MarkIniSettingsDirty(pyWindow* window);
    py_API void                  ClearIniSettings();
    py_API pyWindowSettings*  CreateNewWindowSettings(const char* name);
    py_API pyWindowSettings*  FindWindowSettings(pyID id);
    py_API pyWindowSettings*  FindOrCreateWindowSettings(const char* name);
    py_API pySettingsHandler* FindSettingsHandler(const char* type_name);

    // Scrolling
    py_API void          SetNextWindowScroll(const ImVec2& scroll); // Use -1.0f on one axis to leave as-is
    py_API void          SetScrollX(pyWindow* window, float scroll_x);
    py_API void          SetScrollY(pyWindow* window, float scroll_y);
    py_API void          SetScrollFromPosX(pyWindow* window, float local_x, float center_x_ratio);
    py_API void          SetScrollFromPosY(pyWindow* window, float local_y, float center_y_ratio);

    // Early work-in-progress API (ScrollToItem() will become public)
    py_API void          ScrollToItem(pyScrollFlags flags = 0);
    py_API void          ScrollToRect(pyWindow* window, const ImRect& rect, pyScrollFlags flags = 0);
    py_API ImVec2        ScrollToRectEx(pyWindow* window, const ImRect& rect, pyScrollFlags flags = 0);
//#ifndef py_DISABLE_OBSOLETE_FUNCTIONS
    inline void             ScrollToBringRectIntoView(pyWindow* window, const ImRect& rect) { ScrollToRect(window, rect, pyScrollFlags_KeepVisibleEdgeY); }
//#endif

    // Basic Accessors
    inline pyID          GetItemID()     { pyContext& g = *Gpy; return g.LastItemData.ID; }   // Get ID of last item (~~ often same py::GetID(label) beforehand)
    inline pyItemStatusFlags GetItemStatusFlags(){ pyContext& g = *Gpy; return g.LastItemData.StatusFlags; }
    inline pyItemFlags   GetItemFlags()  { pyContext& g = *Gpy; return g.LastItemData.InFlags; }
    inline pyID          GetActiveID()   { pyContext& g = *Gpy; return g.ActiveId; }
    inline pyID          GetFocusID()    { pyContext& g = *Gpy; return g.NavId; }
    py_API void          SetActiveID(pyID id, pyWindow* window);
    py_API void          SetFocusID(pyID id, pyWindow* window);
    py_API void          ClearActiveID();
    py_API pyID       GetHoveredID();
    py_API void          SetHoveredID(pyID id);
    py_API void          KeepAliveID(pyID id);
    py_API void          MarkItemEdited(pyID id);     // Mark data associated to given item as "edited", used by IsItemDeactivatedAfterEdit() function.
    py_API void          PushOverrideID(pyID id);     // Push given value as-is at the top of the ID stack (whereas PushID combines old and new hashes)
    py_API pyID       GetIDWithSeed(const char* str_id_begin, const char* str_id_end, pyID seed);

    // Basic Helpers for widget code
    py_API void          ItemSize(const ImVec2& size, float text_baseline_y = -1.0f);
    py_API void          ItemSize(const ImRect& bb, float text_baseline_y = -1.0f);
    py_API bool          ItemAdd(const ImRect& bb, pyID id, const ImRect* nav_bb = NULL, pyItemFlags extra_flags = 0);
    py_API bool          ItemHoverable(const ImRect& bb, pyID id);
    py_API void          ItemInputable(pyWindow* window, pyID id);
    py_API bool          IsClippedEx(const ImRect& bb, pyID id);
    py_API ImVec2        CalcItemSize(ImVec2 size, float default_w, float default_h);
    py_API float         CalcWrapWidthForPos(const ImVec2& pos, float wrap_pos_x);
    py_API void          PushMultiItemsWidths(int components, float width_full);
    py_API bool          IsItemToggledSelection();                                   // Was the last item selection toggled? (after Selectable(), TreeNode() etc. We only returns toggle _event_ in order to handle clipping correctly)
    py_API ImVec2        GetContentRegionMaxAbs();
    py_API void          ShrinkWidths(pyShrinkWidthItem* items, int count, float width_excess);
    bool IsHovered(const ImRect& bb, pyID id, bool flatten_childs);
    // Parameter stacks
    py_API void          PushItemFlag(pyItemFlags option, bool enabled);
    py_API void          PopItemFlag();

#ifndef py_DISABLE_OBSOLETE_FUNCTIONS
    // Currently refactoring focus/nav/tabbing system
    // If you have old/custom copy-and-pasted widgets that used FocusableItemRegister():
    //  (Old) py_VERSION_NUM  < 18209: using 'ItemAdd(....)'                              and 'bool tab_focused = FocusableItemRegister(...)'
    //  (Old) py_VERSION_NUM >= 18209: using 'ItemAdd(..., pyItemAddFlags_Focusable)'  and 'bool tab_focused = (GetItemStatusFlags() & pyItemStatusFlags_Focused) != 0'
    //  (New) py_VERSION_NUM >= 18413: using 'ItemAdd(..., pyItemFlags_Inputable)'     and 'bool tab_focused = (GetItemStatusFlags() & pyItemStatusFlags_FocusedTabbing) != 0 || g.NavActivateInputId == id' (WIP)
    // Widget code are simplified as there's no need to call FocusableItemUnregister() while managing the transition from regular widget to TempInputText()
    inline bool FocusableItemRegister(pyWindow* window, pyID id)  { IM_ASSERT(0); IM_UNUSED(window); IM_UNUSED(id); return false; } // -> pass pyItemAddFlags_Inputable flag to ItemAdd()
    inline void FocusableItemUnregister(pyWindow* window)            { IM_ASSERT(0); IM_UNUSED(window); }                              // -> unnecessary: TempInputText() uses pyInputTextFlags_MergedItem
#endif

    // Logging/Capture
    py_API void          LogBegin(pyLogType type, int auto_open_depth);           // -> BeginCapture() when we design v2 api, for now stay under the radar by using the old name.
    py_API void          LogToBuffer(int auto_open_depth = -1);                      // Start logging/capturing to internal buffer
    py_API void          LogRenderedText(const ImVec2* ref_pos, const char* text, const char* text_end = NULL);
    py_API void          LogSetNextTextDecoration(const char* prefix, const char* suffix);

    // Popups, Modals, Tooltips
    py_API bool          BeginChildEx(const char* name, pyID id, const ImVec2& size_arg, bool border, pyWindowFlags flags);
    py_API void          OpenPopupEx(pyID id, pyPopupFlags popup_flags = pyPopupFlags_None);
    py_API void          ClosePopupToLevel(int remaining, bool restore_focus_to_window_under_popup);
    py_API void          ClosePopupsOverWindow(pyWindow* ref_window, bool restore_focus_to_window_under_popup);
    py_API void          ClosePopupsExceptModals();
    py_API bool          IsPopupOpen(pyID id, pyPopupFlags popup_flags);
    py_API bool          BeginPopupEx(pyID id, pyWindowFlags extra_flags);
    py_API void          BeginTooltipEx(pyWindowFlags extra_flags, pyTooltipFlags tooltip_flags);
    py_API ImRect        GetPopupAllowedExtentRect(pyWindow* window);
    py_API pyWindow*  GetTopMostPopupModal();
    py_API ImVec2        FindBestWindowPosForPopup(pyWindow* window);
    py_API ImVec2        FindBestWindowPosForPopupEx(const ImVec2& ref_pos, const ImVec2& size, pyDir* last_dir, const ImRect& r_outer, const ImRect& r_avoid, pyPopupPositionPolicy policy);

    // Menus
    py_API bool          BeginViewportSideBar(const char* name, pyViewport* viewport, pyDir dir, float size, pyWindowFlags window_flags);
    py_API bool          BeginMenuEx(const char* label, const char* icon, bool enabled = true);
    py_API bool          MenuItemEx(const char* label, const char* icon, const char* shortcut = NULL, bool selected = false, bool enabled = true);

    // Combos
    py_API bool          BeginComboPopup(pyID popup_id, const ImRect& bb, pyComboFlags flags);
    py_API bool          BeginComboPreview();
    py_API void          EndComboPreview();

    // Gamepad/Keyboard Navigation
    py_API void          NavInitWindow(pyWindow* window, bool force_reinit);
    py_API void          NavInitRequestApplyResult();
    py_API bool          NavMoveRequestButNoResultYet();
    py_API void          NavMoveRequestSubmit(pyDir move_dir, pyDir clip_dir, pyNavMoveFlags move_flags, pyScrollFlags scroll_flags);
    py_API void          NavMoveRequestForward(pyDir move_dir, pyDir clip_dir, pyNavMoveFlags move_flags, pyScrollFlags scroll_flags);
    py_API void          NavMoveRequestResolveWithLastItem();
    py_API void          NavMoveRequestCancel();
    py_API void          NavMoveRequestApplyResult();
    py_API void          NavMoveRequestTryWrapping(pyWindow* window, pyNavMoveFlags move_flags);
    py_API float         GetNavInputAmount(pyNavInput n, pyInputReadMode mode);
    py_API ImVec2        GetNavInputAmount2d(pyNavDirSourceFlags dir_sources, pyInputReadMode mode, float slow_factor = 0.0f, float fast_factor = 0.0f);
    py_API int           CalcTypematicRepeatAmount(float t0, float t1, float repeat_delay, float repeat_rate);
    py_API void          ActivateItem(pyID id);   // Remotely activate a button, checkbox, tree node etc. given its unique ID. activation is queued and processed on the next frame when the item is encountered again.
    py_API void          SetNavID(pyID id, pyNavLayer nav_layer, pyID focus_scope_id, const ImRect& rect_rel);

    // Focus Scope (WIP)
    // This is generally used to identify a selection set (multiple of which may be in the same window), as selection
    // patterns generally need to react (e.g. clear selection) when landing on an item of the set.
    py_API void          PushFocusScope(pyID id);
    py_API void          PopFocusScope();
    inline pyID          GetFocusedFocusScope()          { pyContext& g = *Gpy; return g.NavFocusScopeId; }                            // Focus scope which is actually active
    inline pyID          GetFocusScope()                 { pyContext& g = *Gpy; return g.CurrentWindow->DC.NavFocusScopeIdCurrent; }   // Focus scope we are outputting into, set by PushFocusScope()

    // Inputs
    // FIXME: Eventually we should aim to move e.g. IsActiveIdUsingKey() into IsKeyXXX functions.
    py_API void          SetItemUsingMouseWheel();
    py_API void          SetActiveIdUsingNavAndKeys();
    inline bool             IsActiveIdUsingNavDir(pyDir dir)                         { pyContext& g = *Gpy; return (g.ActiveIdUsingNavDirMask & (1 << dir)) != 0; }
    inline bool             IsActiveIdUsingNavInput(pyNavInput input)                { pyContext& g = *Gpy; return (g.ActiveIdUsingNavInputMask & (1 << input)) != 0; }
    inline bool             IsActiveIdUsingKey(pyKey key)                            { pyContext& g = *Gpy; IM_ASSERT(key < 64); return (g.ActiveIdUsingKeyInputMask & ((ImU64)1 << key)) != 0; }
    py_API bool          IsMouseDragPastThreshold(pyMouseButton button, float lock_threshold = -1.0f);
    inline bool             IsKeyPressedMap(pyKey key, bool repeat = true)           { pyContext& g = *Gpy; const int key_index = g.IO.KeyMap[key]; return (key_index >= 0) ? IsKeyPressed(key_index, repeat) : false; }
    inline bool             IsNavInputDown(pyNavInput n)                             { pyContext& g = *Gpy; return g.IO.NavInputs[n] > 0.0f; }
    inline bool             IsNavInputTest(pyNavInput n, pyInputReadMode rm)      { return (GetNavInputAmount(n, rm) > 0.0f); }
    py_API pyKeyModFlags GetMergedKeyModFlags();

    // Drag and Drop
    py_API bool          BeginDragDropTargetCustom(const ImRect& bb, pyID id);
    py_API void          ClearDragDrop();
    py_API bool          IsDragDropPayloadBeingAccepted();

    // Internal Columns API (this is not exposed because we will encourage transitioning to the Tables API)
    py_API void          SetWindowClipRectBeforeSetChannel(pyWindow* window, const ImRect& clip_rect);
    py_API void          BeginColumns(const char* str_id, int count, pyOldColumnFlags flags = 0); // setup number of columns. use an identifier to distinguish multiple column sets. close with EndColumns().
    py_API void          EndColumns();                                                               // close columns
    py_API void          PushColumnClipRect(int column_index);
    py_API void          PushColumnsBackground();
    py_API void          PopColumnsBackground();
    py_API pyID       GetColumnsID(const char* str_id, int count);
    py_API pyOldColumns* FindOrCreateColumns(pyWindow* window, pyID id);
    py_API float         GetColumnOffsetFromNorm(const pyOldColumns* columns, float offset_norm);
    py_API float         GetColumnNormFromOffset(const pyOldColumns* columns, float offset);

    // Tables: Candidates for public API
    py_API void          TableOpenContextMenu(int column_n = -1);
    py_API void          TableSetColumnWidth(int column_n, float width);
    py_API void          TableSetColumnSortDirection(int column_n, pySortDirection sort_direction, bool append_to_sort_specs);
    py_API int           TableGetHoveredColumn(); // May use (TableGetColumnFlags() & pyTableColumnFlags_IsHovered) instead. Return hovered column. return -1 when table is not hovered. return columns_count if the unused space at the right of visible columns is hovered.
    py_API float         TableGetHeaderRowHeight();
    py_API void          TablePushBackgroundChannel();
    py_API void          TablePopBackgroundChannel();

    // Tables: Internals
    inline    pyTable*   GetCurrentTable() { pyContext& g = *Gpy; return g.CurrentTable; }
    py_API pyTable*   TableFindByID(pyID id);
    py_API bool          BeginTableEx(const char* name, pyID id, int columns_count, pyTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0, 0), float inner_width = 0.0f);
    py_API void          TableBeginInitMemory(pyTable* table, int columns_count);
    py_API void          TableBeginApplyRequests(pyTable* table);
    py_API void          TableSetupDrawChannels(pyTable* table);
    py_API void          TableUpdateLayout(pyTable* table);
    py_API void          TableUpdateBorders(pyTable* table);
    py_API void          TableUpdateColumnsWeightFromWidth(pyTable* table);
    py_API void          TableDrawBorders(pyTable* table);
    py_API void          TableDrawContextMenu(pyTable* table);
    py_API void          TableMergeDrawChannels(pyTable* table);
    py_API void          TableSortSpecsSanitize(pyTable* table);
    py_API void          TableSortSpecsBuild(pyTable* table);
    py_API pySortDirection TableGetColumnNextSortDirection(pyTableColumn* column);
    py_API void          TableFixColumnSortDirection(pyTable* table, pyTableColumn* column);
    py_API float         TableGetColumnWidthAuto(pyTable* table, pyTableColumn* column);
    py_API void          TableBeginRow(pyTable* table);
    py_API void          TableEndRow(pyTable* table);
    py_API void          TableBeginCell(pyTable* table, int column_n);
    py_API void          TableEndCell(pyTable* table);
    py_API ImRect        TableGetCellBgRect(const pyTable* table, int column_n);
    py_API const char*   TableGetColumnName(const pyTable* table, int column_n);
    py_API pyID       TableGetColumnResizeID(const pyTable* table, int column_n, int instance_no = 0);
    py_API float         TableGetMaxColumnWidth(const pyTable* table, int column_n);
    py_API void          TableSetColumnWidthAutoSingle(pyTable* table, int column_n);
    py_API void          TableSetColumnWidthAutoAll(pyTable* table);
    py_API void          TableRemove(pyTable* table);
    py_API void          TableGcCompactTransientBuffers(pyTable* table);
    py_API void          TableGcCompactTransientBuffers(pyTableTempData* table);
    py_API void          TableGcCompactSettings();

    // Tables: Settings
    py_API void                  TableLoadSettings(pyTable* table);
    py_API void                  TableSaveSettings(pyTable* table);
    py_API void                  TableResetSettings(pyTable* table);
    py_API pyTableSettings*   TableGetBoundSettings(pyTable* table);
    py_API void                  TableSettingsInstallHandler(pyContext* context);
    py_API pyTableSettings*   TableSettingsCreate(pyID id, int columns_count);
    py_API pyTableSettings*   TableSettingsFindByID(pyID id);

    // Tab Bars
    py_API bool          BeginTabBarEx(pyTabBar* tab_bar, const ImRect& bb, pyTabBarFlags flags);
    py_API pyTabItem* TabBarFindTabByID(pyTabBar* tab_bar, pyID tab_id);
    py_API void          TabBarRemoveTab(pyTabBar* tab_bar, pyID tab_id);
    py_API void          TabBarCloseTab(pyTabBar* tab_bar, pyTabItem* tab);
    py_API void          TabBarQueueReorder(pyTabBar* tab_bar, const pyTabItem* tab, int offset);
    py_API void          TabBarQueueReorderFromMousePos(pyTabBar* tab_bar, const pyTabItem* tab, ImVec2 mouse_pos);
    py_API bool          TabBarProcessReorder(pyTabBar* tab_bar);
    py_API bool          TabItemEx(pyTabBar* tab_bar, const char* label, bool* p_open, pyTabItemFlags flags);
    py_API ImVec2        TabItemCalcSize(const char* label, bool has_close_button);
    py_API void          TabItemBackground(ImDrawList* draw_list, const ImRect& bb, pyTabItemFlags flags, ImU32 col);
    py_API void          TabItemLabelAndCloseButton(ImDrawList* draw_list, const ImRect& bb, pyTabItemFlags flags, ImVec2 frame_padding, const char* label, pyID tab_id, pyID close_button_id, bool is_contents_visible, bool* out_just_closed, bool* out_text_clipped);

    // Render helpers
    // AVOID USING OUTSIDE OF py.CPP! NOT FOR PUBLIC CONSUMPTION. THOSE FUNCTIONS ARE A MESS. THEIR SIGNATURE AND BEHAVIOR WILL CHANGE, THEY NEED TO BE REFACTORED INTO SOMETHING DECENT.
    // NB: All position are in absolute pixels coordinates (we are never using window coordinates internally)
    py_API void          RenderText(ImVec2 pos, const char* text, const char* text_end = NULL, bool hide_text_after_hash = true);
    py_API void          RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width);
    py_API void          RenderTextClipped(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align = ImVec2(0, 0), const ImRect* clip_rect = NULL);
    py_API void          RenderTextClippedEx(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align = ImVec2(0, 0), const ImRect* clip_rect = NULL);
    py_API void          RenderTextEllipsis(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, float clip_max_x, float ellipsis_max_x, const char* text, const char* text_end, const ImVec2* text_size_if_known);
    py_API void          RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border = true, float rounding = 0.0f);
    py_API void          RenderFrameBorder(ImVec2 p_min, ImVec2 p_max, float rounding = 0.0f);
    py_API void          RenderColorRectWithAlphaCheckerboard(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, float grid_step, ImVec2 grid_off, float rounding = 0.0f, ImDrawFlags flags = 0);
    py_API void          RenderNavHighlight(const ImRect& bb, pyID id, pyNavHighlightFlags flags = pyNavHighlightFlags_TypeDefault); // Navigation highlight
    py_API const char*   FindRenderedTextEnd(const char* text, const char* text_end = NULL); // Find the optional ## from which we stop displaying text.

    // Render helpers (those functions don't access any py state!)
    py_API void          RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, pyDir dir, float scale = 1.0f);
    py_API void          RenderBullet(ImDrawList* draw_list, ImVec2 pos, ImU32 col);
    py_API void          RenderCheckMark(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz);
    py_API void          RenderMouseCursor(ImDrawList* draw_list, ImVec2 pos, float scale, pyMouseCursor mouse_cursor, ImU32 col_fill, ImU32 col_border, ImU32 col_shadow);
    py_API void          RenderArrowPointingAt(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, pyDir direction, ImU32 col);
    py_API void          RenderRectFilledRangeH(ImDrawList* draw_list, const ImRect& rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding);
    py_API void          RenderRectFilledWithHole(ImDrawList* draw_list, ImRect outer, ImRect inner, ImU32 col, float rounding);

#ifndef py_DISABLE_OBSOLETE_FUNCTIONS
    // [1.71: 2019/06/07: Updating prototypes of some of the internal functions. Leaving those for reference for a short while]
    inline void RenderArrow(ImVec2 pos, pyDir dir, float scale=1.0f) { pyWindow* window = GetCurrentWindow(); RenderArrow(window->DrawList, pos, GetColorU32(pyCol_Text), dir, scale); }
    inline void RenderBullet(ImVec2 pos)                                { pyWindow* window = GetCurrentWindow(); RenderBullet(window->DrawList, pos, GetColorU32(pyCol_Text)); }
#endif

    // Widgets
    py_API void          TextEx(const char* text, const char* text_end = NULL, pyTextFlags flags = 0);
    py_API bool          ButtonEx(const char* label, const ImVec2& size_arg = ImVec2(0, 0), pyButtonFlags flags = 0);
    py_API bool          CloseButton(pyID id, const ImVec2& pos);
    py_API bool          CollapseButton(pyID id, const ImVec2& pos);
    py_API bool          ArrowButtonEx(const char* str_id, pyDir dir, ImVec2 size_arg, pyButtonFlags flags = 0);
    py_API void          Scrollbar(pyAxis axis);
    py_API bool          ScrollbarEx(const ImRect& bb, pyID id, pyAxis axis, float* p_scroll_v, float avail_v, float contents_v, ImDrawFlags flags);
    py_API bool          ImageButtonEx(pyID id, ImTextureID texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec2& padding, const ImVec4& bg_col, const ImVec4& tint_col);
    py_API ImRect        GetWindowScrollbarRect(pyWindow* window, pyAxis axis);
    py_API pyID       GetWindowScrollbarID(pyWindow* window, pyAxis axis);
    py_API pyID       GetWindowResizeCornerID(pyWindow* window, int n); // 0..3: corners
    py_API pyID       GetWindowResizeBorderID(pyWindow* window, pyDir dir);
    py_API void          SeparatorEx(pySeparatorFlags flags);
    py_API bool          CheckboxFlags(const char* label, ImS64* flags, ImS64 flags_value);
    py_API bool          CheckboxFlags(const char* label, ImU64* flags, ImU64 flags_value);

    // Widgets low-level behaviors
    py_API bool          ButtonBehavior(const ImRect& bb, pyID id, bool* out_hovered, bool* out_held, pyButtonFlags flags = 0);
    py_API bool          DragBehavior(pyID id, pyDataType data_type, void* p_v, float v_speed, const void* p_min, const void* p_max, const char* format, pySliderFlags flags);
    py_API bool          SliderBehavior(const ImRect& bb, pyID id, pyDataType data_type, void* p_v, const void* p_min, const void* p_max, const char* format, pySliderFlags flags, ImRect* out_grab_bb);
    py_API bool          SplitterBehavior(const ImRect& bb, pyID id, pyAxis axis, float* size1, float* size2, float min_size1, float min_size2, float hover_extend = 0.0f, float hover_visibility_delay = 0.0f);
    py_API bool          TreeNodeBehavior(pyID id, pyTreeNodeFlags flags, const char* label, const char* label_end = NULL);
    py_API bool          TreeNodeBehaviorIsOpen(pyID id, pyTreeNodeFlags flags = 0);                     // Consume previous SetNextItemOpen() data, if any. May return true when logging
    py_API void          TreePushOverrideID(pyID id);

    // Template functions are instantiated in py_widgets.cpp for a finite number of types.
    // To use them externally (for custom widget) you may need an "extern template" statement in your code in order to link to existing instances and silence Clang warnings (see #2036).
    // e.g. " extern template py_API float RoundScalarWithFormatT<float, float>(const char* format, pyDataType data_type, float v); "
    template<typename T, typename SIGNED_T, typename FLOAT_T>   py_API float ScaleRatioFromValueT(pyDataType data_type, T v, T v_min, T v_max, bool is_logarithmic, float logarithmic_zero_epsilon, float zero_deadzone_size);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   py_API T     ScaleValueFromRatioT(pyDataType data_type, float t, T v_min, T v_max, bool is_logarithmic, float logarithmic_zero_epsilon, float zero_deadzone_size);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   py_API bool  DragBehaviorT(pyDataType data_type, T* v, float v_speed, T v_min, T v_max, const char* format, pySliderFlags flags);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   py_API bool  SliderBehaviorT(const ImRect& bb, pyID id, pyDataType data_type, T* v, T v_min, T v_max, const char* format, pySliderFlags flags, ImRect* out_grab_bb);
    template<typename T, typename SIGNED_T>                     py_API T     RoundScalarWithFormatT(const char* format, pyDataType data_type, T v);
    template<typename T>                                        py_API bool  CheckboxFlagsT(const char* label, T* flags, T flags_value);

    // Data type helpers
    py_API const pyDataTypeInfo*  DataTypeGetInfo(pyDataType data_type);
    py_API int           DataTypeFormatString(char* buf, int buf_size, pyDataType data_type, const void* p_data, const char* format);
    py_API void          DataTypeApplyOp(pyDataType data_type, int op, void* output, const void* arg_1, const void* arg_2);
    py_API bool          DataTypeApplyOpFromText(const char* buf, const char* initial_value_buf, pyDataType data_type, void* p_data, const char* format);
    py_API int           DataTypeCompare(pyDataType data_type, const void* arg_1, const void* arg_2);
    py_API bool          DataTypeClamp(pyDataType data_type, void* p_data, const void* p_min, const void* p_max);

    // InputText
    py_API bool          InputTextEx(const char* label, const char* hint, char* buf, int buf_size, const ImVec2& size_arg, pyInputTextFlags flags, pyInputTextCallback callback = NULL, void* user_data = NULL);
    py_API bool          TempInputText(const ImRect& bb, pyID id, const char* label, char* buf, int buf_size, pyInputTextFlags flags);
    py_API bool          TempInputScalar(const ImRect& bb, pyID id, const char* label, pyDataType data_type, void* p_data, const char* format, const void* p_clamp_min = NULL, const void* p_clamp_max = NULL);
    inline bool             TempInputIsActive(pyID id)       { pyContext& g = *Gpy; return (g.ActiveId == id && g.TempInputId == id); }
    inline pyInputTextState* GetInputTextState(pyID id)   { pyContext& g = *Gpy; return (g.InputTextState.ID == id) ? &g.InputTextState : NULL; } // Get input text state if active

    // Color
    py_API void          ColorTooltip(const char* text, const float* col, pyColorEditFlags flags);
    py_API void          ColorEditOptionsPopup(const float* col, pyColorEditFlags flags);
    py_API void          ColorPickerOptionsPopup(const float* ref_col, pyColorEditFlags flags);

    // Plot
    py_API int           PlotEx(pyPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size);

    // Shade functions (write over already created vertices)
    py_API void          ShadeVertsLinearColorGradientKeepAlpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);
    py_API void          ShadeVertsLinearUV(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, bool clamp);

    // Garbage collection
    py_API void          GcCompactTransientMiscBuffers();
    py_API void          GcCompactTransientWindowBuffers(pyWindow* window);
    py_API void          GcAwakeTransientWindowBuffers(pyWindow* window);

    // Debug Tools
    py_API void          ErrorCheckEndFrameRecover(pyErrorLogCallback log_callback, void* user_data = NULL);
    py_API void          ErrorCheckEndWindowRecover(pyErrorLogCallback log_callback, void* user_data = NULL);
    inline void             DebugDrawItemRect(ImU32 col = IM_COL32(255,0,0,255))    { pyContext& g = *Gpy; pyWindow* window = g.CurrentWindow; GetForegroundDrawList(window)->AddRect(g.LastItemData.Rect.Min, g.LastItemData.Rect.Max, col); }
    inline void             DebugStartItemPicker()                                  { pyContext& g = *Gpy; g.DebugItemPickerActive = true; }

    py_API void          ShowFontAtlas(ImFontAtlas* atlas);
    py_API void          DebugHookIdInfo(pyID id, pyDataType data_type, const void* data_id, const void* data_id_end);
    py_API void          DebugNodeColumns(pyOldColumns* columns);
    py_API void          DebugNodeDrawList(pyWindow* window, const ImDrawList* draw_list, const char* label);
    py_API void          DebugNodeDrawCmdShowMeshAndBoundingBox(ImDrawList* out_draw_list, const ImDrawList* draw_list, const ImDrawCmd* draw_cmd, bool show_mesh, bool show_aabb);
    py_API void          DebugNodeFont(ImFont* font);
    py_API void          DebugNodeStorage(pyStorage* storage, const char* label);
    py_API void          DebugNodeTabBar(pyTabBar* tab_bar, const char* label);
    py_API void          DebugNodeTable(pyTable* table);
    py_API void          DebugNodeTableSettings(pyTableSettings* settings);
    py_API void          DebugNodeWindow(pyWindow* window, const char* label);
    py_API void          DebugNodeWindowSettings(pyWindowSettings* settings);
    py_API void          DebugNodeWindowsList(ImVector<pyWindow*>* windows, const char* label);
    py_API void          DebugNodeViewport(pyViewportP* viewport);
    py_API void          DebugRenderViewportThumbnail(ImDrawList* draw_list, pyViewportP* viewport, const ImRect& bb);

} // namespace py


//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas internal API
//-----------------------------------------------------------------------------

// This structure is likely to evolve as we add support for incremental atlas updates
struct ImFontBuilderIO
{
    bool    (*FontBuilder_Build)(ImFontAtlas* atlas);
};

// Helper for font builder
py_API const ImFontBuilderIO* ImFontAtlasGetBuilderForStbTruetype();
py_API void      ImFontAtlasBuildInit(ImFontAtlas* atlas);
py_API void      ImFontAtlasBuildSetupFont(ImFontAtlas* atlas, ImFont* font, ImFontConfig* font_config, float ascent, float descent);
py_API void      ImFontAtlasBuildPackCustomRects(ImFontAtlas* atlas, void* stbrp_context_opaque);
py_API void      ImFontAtlasBuildFinish(ImFontAtlas* atlas);
py_API void      ImFontAtlasBuildRender8bppRectFromString(ImFontAtlas* atlas, int x, int y, int w, int h, const char* in_str, char in_marker_char, unsigned char in_marker_pixel_value);
py_API void      ImFontAtlasBuildRender32bppRectFromString(ImFontAtlas* atlas, int x, int y, int w, int h, const char* in_str, char in_marker_char, unsigned int in_marker_pixel_value);
py_API void      ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_multiply_factor);
py_API void      ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride);

//-----------------------------------------------------------------------------
// [SECTION] Test Engine specific hooks (py_test_engine)
//-----------------------------------------------------------------------------

#ifdef py_ENABLE_TEST_ENGINE
extern void         pyTestEngineHook_ItemAdd(pyContext* ctx, const ImRect& bb, pyID id);
extern void         pyTestEngineHook_ItemInfo(pyContext* ctx, pyID id, const char* label, pyItemStatusFlags flags);
extern void         pyTestEngineHook_Log(pyContext* ctx, const char* fmt, ...);
extern const char*  pyTestEngine_FindItemDebugLabel(pyContext* ctx, pyID id);

#define py_TEST_ENGINE_ITEM_ADD(_BB,_ID)                 if (g.TestEngineHookItems) pyTestEngineHook_ItemAdd(&g, _BB, _ID)               // Register item bounding box
#define py_TEST_ENGINE_ITEM_INFO(_ID,_LABEL,_FLAGS)      if (g.TestEngineHookItems) pyTestEngineHook_ItemInfo(&g, _ID, _LABEL, _FLAGS)   // Register item label and status flags (optional)
#define py_TEST_ENGINE_LOG(_FMT,...)                     if (g.TestEngineHookItems) pyTestEngineHook_Log(&g, _FMT, __VA_ARGS__)          // Custom log entry from user land into test log
#else
#define py_TEST_ENGINE_ITEM_INFO(_ID,_LABEL,_FLAGS)      ((void)0)
#endif

//-----------------------------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // #ifndef py_DISABLE
