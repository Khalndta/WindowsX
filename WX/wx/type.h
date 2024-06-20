#pragma once

#include "defs.h"

namespace WX {

#pragma region HandleBase
enum_flags(HandleAccess, DWORD,
	Delete             = DELETE,
	ReadCtl            = READ_CONTROL,
	WriteDAC           = WRITE_DAC,
	WriteOwner         = WRITE_OWNER,
	Sync               = SYNCHRONIZE,
	GenericRead        = GENERIC_READ,
	GenericWrite       = GENERIC_WRITE,
	GenericExecute     = GENERIC_EXECUTE,
	GenericAll         = GENERIC_ALL);
template<class AnyChild>
class HandleBase;
using Handle = HandleBase<void>;
template<class AnyChild>
class HandleBase : public ChainExtend<HandleBase<AnyChild>, AnyChild> {
protected:
	HANDLE hObject;
public:
	using Child = KChain<HandleBase, AnyChild>;
	using Access = HandleAccess;

	HandleBase(HANDLE hObject = O) : hObject(hObject) {}
	~HandleBase() { Close(); }

	inline bool Close() {
		if (hObject)
			if (!CloseHandle(hObject))
				return false;
		hObject = O;
		return true;
	}

	inline bool Compare(HANDLE h) const reflect_as(CompareObjectHandles(self, h));
	inline bool WaitForSignal(DWORD dwMilliSec = INFINITE) const assert_reflect_to(DWORD ret, (ret = WaitForSingleObject(self, dwMilliSec)) != WAIT_FAILED, ret == WAIT_OBJECT_0);
	inline bool WaitForAbandon(DWORD dwMilliSec = INFINITE) const assert_reflect_to(DWORD ret, (ret = WaitForSingleObject(self, dwMilliSec)) != WAIT_FAILED, ret == WAIT_ABANDONED);
//	inline bool GetInformation(LPDWORD lpdwFlags) const reflect_as(GetHandleInformation(self, lpdwFlags));
//	inline bool SetInformation(DWORD dwMask, DWORD dwFlags) reflect_as(SetHandleInformation(self, dwMask, dwFlags));
public: // Property - Inherit
	//	/* W */ inline bool Inherit()
	//	/* R */ inline bool Inherit() const assert_rlect
public: // Property - Inherit
public:
	inline operator bool() const reflect_as(hObject &&hObject != INVALID_HANDLE_VALUE);
	inline operator Handle &() reflect_as(reuse_as<Handle>(self));
	inline operator const Handle() const reflect_as(reuse_as<Handle>(self));
	inline operator HANDLE &() reflect_as(hObject);
	inline operator HANDLE() const reflect_as(hObject);
	inline static auto &Attach(HANDLE &hObj) reflect_as(reuse_as<Child>(hObj));
};
#define Handle_Based(name) name : public HandleBase<name>
#pragma endregion

#pragma region Memory

#pragma region Local
enum_flags(AllocFlag, UINT,
		   enum_default Fixed = LMEM_FIXED,
		   Modify   = LMEM_MODIFY,
		   Moveable = LMEM_MOVEABLE,
		   ZeroInit = LMEM_ZEROINIT);
using LAF = AllocFlag;
class Local {
public:
	inline static void *Alloc(size_t size, LAF flags = LAF::Fixed) reflect_to(auto ptr = LocalAlloc(flags.yield(), size), ptr);
	template<class AnyType>
	inline static AnyType *Alloc(LAF flags = LAF::Fixed) reflect_to(auto ptr = (AnyType *)LocalAlloc(flags.yield(), sizeof(AnyType)), ptr);
	inline static void *Realloc(void *ptr, size_t size, LAF flags = LAF::Fixed) reflect_as(LocalReAlloc(ptr, size, flags.yield()));
	inline static void  Free(void *ptr) {
		if (ptr)
			assert(!LocalFree(ptr));
	}

	inline static void *Lock(void *ptr) reflect_as(LocalLock(ptr));
	inline static void Unlock(void *ptr) assert_reflect_as(LocalUnlock(ptr));
};
#pragma endregion

#pragma region Heap
class HeapSummary : protected HEAP_SUMMARY {
public:
	HeapSummary() : HEAP_SUMMARY{ 0 } reflect_to(this->cb = sizeof(*this));
	HeapSummary(Null) : HeapSummary() {}
	HeapSummary(const HEAP_SUMMARY &hs) : HEAP_SUMMARY(hs) {}
public: // Property - Allocated
	/* W */ inline auto &Allocated(size_t cbAllocated) reflect_to_self(this->cbAllocated = cbAllocated);
	/* R */ inline size_t Allocated() const reflect_as(this->cbAllocated);
public: // Property - Committed
	/* W */ inline auto &Committed(size_t cbCommitted) reflect_to_self(this->cbCommitted = cbCommitted);
	/* R */ inline size_t Committed() const reflect_as(this->cbCommitted);
public: // Property - Reserved
	/* W */ inline auto &Reserved(size_t cbReserved) reflect_to_self(this->cbReserved = cbReserved);
	/* R */ inline size_t Reserved() const reflect_as(this->cbReserved);
public: // Property - MaxReserve
	/* W */ inline auto &MaxReserve(size_t cbMaxReserve) reflect_to_self(this->cbMaxReserve = cbMaxReserve);
	/* R */ inline size_t MaxReserve() const reflect_as(this->cbMaxReserve);
public:
	inline LPHEAP_SUMMARY operator&() reflect_as(this);
};
enum_flags(HeapAllocFlag, UINT,
		   enum_default Fixed = 0,
		   GenerateExceptions = HEAP_GENERATE_EXCEPTIONS,
		   NoSerialize = HEAP_NO_SERIALIZE,
		   ZeroInit = HEAP_ZERO_MEMORY);
using HAF = HeapAllocFlag;
class Heap;
using CHeap = RefAs<Heap>;
class Handle_Based(Heap) {
public:
	using super = HandleBase<Heap>;
	Heap(HANDLE hHeap) : super(hHeap) {}
public:
	using Summary = HeapSummary;
	using AllocFlags = HeapAllocFlag;

	Heap() : super(GetProcessHeap()) {}
	Heap(Null) {}
	Heap(Heap & h) : super(h) {}
	Heap(Heap && h) : super(h) {}
	Heap(const Heap &) = delete;

	~Heap() reflect_to(Destroy());

	inline static Heap Create(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) reflect_as(HeapCreate(flOptions, dwInitialSize, dwMaximumSize));
	inline static CHeap Now() reflect_as(GetProcessHeap());
	//	DWORD GetProcessHeaps(_In_ DWORD NumberOfHeaps, _Out_writes_to_(NumberOfHeaps, return) PHANDLE ProcessHeaps);

	inline bool Destroy() {
		if (super::hObject)
			if (!HeapDestroy(super::hObject))
				return false;
		super::hObject = O;
		return true;
	}

	inline void *Alloc(size_t size, AllocFlag flags = AllocFlag::Fixed) assert_reflect_as(auto h = HeapAlloc(self, flags.yield(), size), h);
	inline void *Realloc(void *ptr, size_t nSize, DWORD flags = 0) assert_reflect_as((ptr = HeapReAlloc(self, flags, ptr, nSize)), ptr);
	inline bool Free(void *ptr, DWORD flags = 0) reflect_as(HeapFree(self, flags, ptr));

	inline size_t Sizeof(const void *ptr, DWORD flags) const assert_reflect_as(auto size = HeapSize(self, flags, ptr), size);
	inline bool IsValid(const void *ptr, DWORD flags) const reflect_as(HeapValidate(self, flags, ptr));

	inline auto &Lock() assert_reflect_as_self(HeapLock(self));
	inline auto &Unlock() assert_reflect_as_self(HeapUnlock(self));

	//	HeapWalk();
	//	SIZE_T HeapCompact(HANDLE hHeap, DWORD dwFlags);

public: // Property - Summaries
	inline Summary Summaries(DWORD dwFlags = 0) const assert_reflect_to(Summary s, ::HeapSummary(self, dwFlags, &s), s);

	//BOOL HeapSetInformation(
	//	_In_opt_ HANDLE HeapHandle,
	//	_In_ HEAP_INFORMATION_CLASS HeapInformationClass,
	//	_In_reads_bytes_opt_(HeapInformationLength) PVOID HeapInformation,
	//	_In_ SIZE_T HeapInformationLength);
	//BOOL HeapQueryInformation(
	//	_In_opt_ HANDLE HeapHandle,
	//	_In_ HEAP_INFORMATION_CLASS HeapInformationClass,
	//	_Out_writes_bytes_to_opt_(HeapInformationLength, *ReturnLength) PVOID HeapInformation,
	//	_In_ SIZE_T HeapInformationLength,
	//	_Out_opt_ PSIZE_T ReturnLength);
};
#pragma endregion

template<class AnyType, class HeapType>
class AutoPointer {
	AnyType *p;
public:
	AutoPointer(AnyType *p = O) : p(p) {}
	~AutoPointer() {
		if (p) HeapType::Free(p);
		p = O;
	}
	inline AutoPointer &operator=(AnyType *p) reflect_to_self(this->p = p);
	inline operator AnyType *() reflect_as(p);
	inline operator const AnyType *() const reflect_as(p);
	inline AnyType **operator&() reflect_as(&p);
	inline const AnyType **operator&() const reflect_as(&p);
	inline AnyType &operator*() reflect_as(*p);
	inline const AnyType &operator*() const reflect_as(*p);
};

#pragma endregion

// constexpr UINT MaxLenDefault = 512;
constexpr UINT MaxLenPath = MAX_PATH;
constexpr UINT MaxLenTitle = MaxLenPath;
constexpr UINT MaxLenClass = MaxLenTitle * 2;
constexpr UINT MaxLenNotice = 32767;

#pragma region String
enum_class(CodePages, UINT, 
	Active       = CP_ACP,
	OEM          = CP_OEMCP,
	Macintosh    = CP_MACCP,
	ThreadActive = CP_THREAD_ACP,
	Symbol       = CP_SYMBOL,
	UTF7         = CP_UTF7,
	UTF8         = CP_UTF8);
using ACHAR = std::conditional_t<IsUnicode, CHAR, WCHAR>;
using LPASTR = std::conditional_t<IsUnicode, LPSTR, LPWSTR>;
using LPCASTR = std::conditional_t<IsUnicode, LPCSTR, LPCWSTR>;
enum StrFlags : size_t {
	STR_DEF = 0,
	STR_READONLY = 1,
	STR_MOVABLE = 2,
	STR_RELEASE = 4
};
class String {
	LPTSTR mutable lpszStr = O;
	mutable size_t Len : sizeof(void*) * 8 - 3;
	mutable size_t Flags : 3;
private:
	friend const String CString(size_t uLen, LPCTSTR lpString);
	friend const String CString(LPCTSTR lpString, size_t maxLen);
	friend const String operator ""_str(LPCTSTR lpString, size_t uLen);
	String(size_t len, UINT flags, LPTSTR lpBuffer) :
		lpszStr(lpBuffer), Len((UINT)len), Flags(flags) {
		if (len <= 0 || !lpBuffer) {
			Len = 0;
			lpszStr = O;
		}
	}
	String(size_t len, LPCTSTR str) : String(len, STR_READONLY, const_cast<LPTSTR>(str)) {}
public:
	String() {}
	String(Null) {}
	String(const String &&) = delete;
	String(const String &) = delete;
	String(String &str) : String(str.Len, str.Flags, str.lpszStr) { str.Len = 0, str.Flags = STR_DEF, str.lpszStr = 0; }
	String(String &&str) : String(str.Len, str.Flags, str.lpszStr) { str.Len = 0, str.Flags = STR_DEF, str.lpszStr = 0; }

	explicit String(size_t len) : lpszStr(Alloc(len)), Len(len), Flags(STR_MOVABLE | STR_RELEASE) {}
	String(size_t len, LPTSTR str) : String(len, STR_MOVABLE | STR_RELEASE, str) {}

	String(ACHAR ch) : lpszStr(Alloc(1)), Len(1), Flags(STR_MOVABLE | STR_RELEASE) reflect_to(lpszStr[0] = (TCHAR)ch);
	String(TCHAR ch) : lpszStr(Alloc(1)), Len(1), Flags(STR_MOVABLE | STR_RELEASE) reflect_to(lpszStr[0] = ch);
	template<size_t len> String(TCHAR (&str)[len]) : String(len - 1, STR_DEF, str) {}
	template<size_t len> String(const TCHAR(&str)[len]) : lpszStr(const_cast<LPTSTR>(str)), Len(len - 1), Flags(STR_READONLY) {}
	template<size_t len> String(const ConstArray<TCHAR, len> &str) : String(str.array) {}
	template<size_t len> String(const ACHAR(&str)[len]) : lpszStr(const_cast<LPTSTR>(str)), Len(len - 1), Flags(STR_READONLY) {}
	template<size_t len> String(const ConstArray<ACHAR, len> &str, CodePages cp = CodePages::Active) : String(str.array, cp) {}
	template<size_t len> String(const ACHAR(&str)[len], CodePages cp = CodePages::Active) {
		int tLength;
#ifdef UNICODE
		assert((tLength = MultiByteToWideChar(cp.yield(), 0, str, (int)len, O, 0)) > 0);
#else
		assert((tLength = WideCharToMultiByte(cp.yield(), 0, str, (int)len, O, 0, O, O)) > 0);
#endif
		// if (uLength != tLength) warnning glyphs missing 
		Len = (UINT)tLength;
		lpszStr = String::Alloc(Len);
		Flags = STR_MOVABLE | STR_RELEASE;
#ifdef UNICODE
		assert(tLength == MultiByteToWideChar(cp.yield(), 0, str, (int)len, lpszStr, tLength));
#else
		assert(tLength == WideCharToMultiByte(cp.yield(), 0, str, (int)len, lpszStr, tLength, O, O));
#endif
		lpszStr[Len] = 0;
	}

	String(LPCASTR lpszString, size_t maxLength, CodePages cp = CodePages::Active) {
		if (!lpszString || !maxLength) return;
		size_t uLength = 0;
#ifdef UNICODE
		assert(SUCCEEDED(StringCchLengthA(lpszString, maxLength, &uLength)));
#else
		assert(SUCCEEDED(StringCchLengthW(lpszString, maxLength, &uLength)));
#endif
		if (!uLength) return;
		int tLength;
#ifdef UNICODE
		assert((tLength = MultiByteToWideChar(cp.yield(), 0, lpszString, (int)uLength, O, 0)) > 0);
#else
		assert((tLength = WideCharToMultiByte(cp.yield(), 0, lpszString, (int)uLength, O, 0, O, O)) > 0);
#endif
		// if (uLength != tLength) warnning glyphs missing 
		Len = (UINT)tLength;
		lpszStr = String::Alloc(Len);
		Flags = STR_MOVABLE | STR_RELEASE;
#ifdef UNICODE
		assert(tLength == MultiByteToWideChar(cp.yield(), 0, lpszString, (int)uLength, lpszStr, tLength));
#else
		assert(tLength == WideCharToMultiByte(cp.yield(), 0, lpszString, (int)uLength, lpszStr, tLength, O, O));
#endif
		lpszStr[Len] = 0;
	}
	~String() { operator~(); }
public:
	inline auto &Trunc() {
		assert(Flags & STR_MOVABLE);
		size_t uLength = 0;
		assert(SUCCEEDED(StringCchLength(lpszStr, Len, &uLength)));
		if (!uLength) retself;
		Len = (UINT)uLength;
		lpszStr = Realloc(uLength, lpszStr);
		retself;
	}
	inline auto &Resize(size_t uLength) {
		assert(Flags & STR_MOVABLE);
		if (uLength <= 0) retself;
		size_t urLength = 0;
		assert(SUCCEEDED(StringCchLength(lpszStr, Len, &urLength)));
		Len = (UINT)uLength;
		lpszStr = Realloc(uLength, lpszStr);
		if ((size_t)uLength < urLength)
			lpszStr[urLength] = 0;
		retself;
	}
public:
	inline const String str_safe() const { ////////////////////////////////
		if (!Len || !lpszStr) return TEXT("");
		if (!lpszStr[Len]) return &*this;
		return +*this;
	}
	inline size_t Length() const reflect_as(lpszStr ? Len : 0);
	inline size_t Size() const reflect_as(lpszStr ? (Len + 1) * sizeof(TCHAR) : 0);
protected:
	inline static LPTSTR Realloc(size_t len, LPTSTR lpsz) {
		if (!lpsz && len <= 0) return O;
		if (len <= 0) {
			Free(lpsz);
			return O;
		}
		return (LPTSTR)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, lpsz, (len + 1) * sizeof(TCHAR));
	}
	inline static LPTSTR Alloc(size_t len) {
		if (len == 0) return O;
		return (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (len + 1) * sizeof(TCHAR));
	}
	inline static void Free(void *lpsz) assert_reflect_as(HeapFree(GetProcessHeap(), 0, lpsz));
public:
	inline operator bool() const reflect_as(lpszStr && Len);
	inline operator LPTSTR() {
		assert(!(Flags & STR_READONLY));
		reflect_as(Len ? lpszStr : O);
	}
	inline operator LPCTSTR() const reflect_as(Len ? lpszStr : O);
	inline LPTSTR operator*() const {
		if (!lpszStr || !Len) return O;
		LPTSTR lpsz = String::Alloc(Len);
		assert(SUCCEEDED(StringCchCopyN(lpsz, Len + 1, lpszStr, Len)));
		lpsz[Len] = '\0';
		return lpsz;
	}
	inline String operator&() reflect_as({ Len, 0, lpszStr });
	inline const String operator&() const reflect_as({ Len, lpszStr });
	inline void operator~() const {
		if (lpszStr && (Flags & STR_RELEASE))
			Free(lpszStr);
		lpszStr = O;
		Len = 0;
	}
	inline String operator+() const {
		if (!lpszStr || !Len) return O;
		LPTSTR lpsz = String::Alloc(Len);
		assert(SUCCEEDED(StringCchCopyN(lpsz, Len + 1, lpszStr, Len)));
		lpsz[Len] = '\0';
		return { Len, lpsz };
	}
	inline String operator-() const {
		size_t uLength = 0;
		assert(SUCCEEDED(StringCchLength(lpszStr, Len, &uLength)));
		if (!uLength) return O;
		LPTSTR lpsz = String::Alloc(uLength);
		assert(SUCCEEDED(StringCchCopyN(lpsz, Len + 1, lpszStr, Len)));
		lpsz[Len] = '\0';
		return{ uLength, lpsz };
	}
public:
	auto operator=(String &) const = delete;
	auto operator=(const String &str) = delete;
	inline auto &operator=(String &&str) noexcept {
		uintptr_t c;
		c = Len, Len = str.Len, str.Len = c;
		c = Flags, Flags = str.Flags, str.Flags = Flags;
		std::swap(lpszStr, str.lpszStr);
		retself;
	}
	inline auto &operator=(String &str) noexcept {
		uintptr_t c;
		c = Len, Len = str.Len, str.Len = c;
		c = Flags, Flags = str.Flags, str.Flags = Flags;
		std::swap(lpszStr, str.lpszStr);
		retself;
	}
	inline auto &operator=(const String &str) const noexcept {
		uintptr_t c;
		c = Len, Len = str.Len, str.Len = c;
		c = Flags, Flags = str.Flags, str.Flags = Flags;
		std::swap(lpszStr, str.lpszStr);
		retself;
	}
};
inline const String operator ""_str(LPCTSTR lpString, size_t uLen) {
	if (uLen == 0 || !*lpString) return O;
	return { uLen, lpString };
}
inline const String CString(size_t uLen, LPCTSTR lpString) {
	if (uLen == 0 || !*lpString) return O;
	return { uLen, lpString };
}
inline const String CString(LPCTSTR lpString, size_t maxLen) {
	if (!lpString) return O;
	size_t uLen = 0;
	assert(SUCCEEDED(StringCchLength(lpString, maxLen, &uLen)));
	return { uLen, lpString };
}
#define S(str) TEXT(str##_str)

inline size_t LengthOf(const String &str) reflect_as(str.Length());
template<class... Args>
inline size_t LengthOf(const Args&... args) reflect_as((LengthOf((const String &)args) + ...));

inline LPTSTR Copies(LPTSTR lpBuffer) reflect_as((LPTSTR)lpBuffer);
template<class... Args>
inline LPTSTR Copies(LPTSTR lpBuffer, const String &str, const Args &... args) {
	auto uLen = str.Length();
	if (uLen > 0) assert(StringCchCopy(lpBuffer, uLen + 1, str) == 0);
	return Copies(lpBuffer + uLen, args...);
}

inline String Cats() reflect_as(O);
template<class... Args>
inline String Cats(const String &str, const Args &... args) {
	if (auto uLength = LengthOf<const String &, const Args &...>(str, args...)) {
		String str(uLength);
		*Copies(str, str, args...) = 0;
		return str;
	}
	return O;
}

inline void *Copy(void *lpDst) reflect_as(lpDst);
template<class... Args>
inline void *Copy(void *lpDst, const void *lpSrc, size_t uSize, Args... args) {
	if (!uSize) return Copy(lpDst, args...);
	assert(memcpy_s(lpDst, uSize, lpSrc, uSize) == 0);
	return Copy(((uint8_t *)lpDst) + uSize, args...);
}
#pragma endregion

template<class ProtoType>
inline const String EnumToken<ProtoType>::Left() const reflect_as(CString(lnLeft, lpLeft));
template<class ProtoType>
inline const String EnumToken<ProtoType>::Right() const reflect_as(CString(lnRight, lpRight));

template<class SubClass, class BaseType>
const String &&EnumBase<SubClass, BaseType>::EnumName = CString(SubClass::__Name, CountOf(SubClass::__Name));

#pragma region Numeral Format 
enum class Symbol : uint8_t { Def = 0, Neg, Pos, Space };
enum class Cap : uint8_t { Small = 0, Big };
enum class Align : uint8_t { Space = 0, Zero };
enum class Rad : uint8_t { Dec = 0, Qua, Oct, Hex };
struct fmt_word {
	bool int_align : 1;
	Align int_blank : 1;
	bool int_trunc : 1;
	uint8_t int_calign : 5;

	bool float_align : 1;
	Align float_blank : 1;
	bool float_force : 1;
	uint8_t float_calign : 5;

	Symbol symbol : 2;
	Cap alfa_type : 1;
	bool right_dir : 1;
	Rad radix_type : 2;
};
template<class CharType>
static const CharType *fmt_push(fmt_word &fmt, const CharType *format) {
	auto ch = *format;
	if (ch == CharType(' ')) {
		fmt.right_dir = true;
		fmt.symbol = Symbol::Space;
		ch = *++format;
	}
	elif(ch == CharType('*')) {
		fmt.symbol = Symbol::Space;
		ch = *++format;
	}
	switch (ch) {
		case CharType('+'):
			fmt.symbol = Symbol::Pos;
			ch = *++format;
			break;
	}
	switch (ch) {
		case CharType('0'):
			fmt.int_blank = Align::Zero;
			fmt.int_align = true;
			ch = *++format;
			break;
		case CharType(' '):
			fmt.int_blank = Align::Space;
			fmt.int_align = true;
			ch = *++format;
			break;
	}
	if (ch == CharType('~')) {
		fmt.int_trunc = true;
		ch = *++format;
	}
	auto align = 0;
	while (CharType('0') <= ch && ch <= CharType('9')) {
		align *= 10;
		align += ch - CharType('0');
		if (align > 32)
			return nullptr; // overflow
		ch = *++format;
	}
	fmt.int_calign = align;
	if (ch == CharType('.')) {
		fmt.float_force = true;
		ch = *++format;
	}
	elif(ch == CharType('0')) {
		fmt.float_blank = Align::Zero;
		fmt.float_align = true;
		ch = *++format;
	}
	align = 0;
	while (CharType('0') <= ch && ch <= CharType('9')) {
		align *= 10;
		align += ch - CharType('0');
		if (align > 32)
			return nullptr; // overflow
		ch = *++format;
	}
	fmt.float_calign = align;
	if (ch == CharType(' ')) {
		fmt.float_blank = Align::Space;
		fmt.float_align = true;
		ch = *++format;
	}
	switch (ch) {
		case CharType('q'):
			fmt.radix_type = Rad::Qua;
			ch = *++format;
			break;
		case CharType('o'):
			fmt.radix_type = Rad::Oct;
			ch = *++format;
			break;
		case CharType('d'):
			fmt.radix_type = Rad::Dec;
			ch = *++format;
			break;
		case CharType('X'):
			fmt.alfa_type = Cap::Big;
			[[fallthrough]];
		case CharType('x'):
			fmt.radix_type = Rad::Hex;
			ch = *++format;
			break;
		default:
			return nullptr;
	}
	return format;
}
template<class CharType>
static bool fmt_single(fmt_word &fmt, const CharType *format) {
	if (!format) return false;
	if (auto hpformat = fmt_push(fmt, format))
		return hpformat[0] == TEXT('\0');
	return true;
}
struct format_numeral : public fmt_word {
public:
	format_numeral() : fmt_word{ 0 } {}
	format_numeral(fmt_word fmt) : fmt_word(fmt) {}
	format_numeral(const char *lpFormat) : fmt_word{ 0 } assert(fmt_single(self, lpFormat));
	format_numeral(const wchar_t *lpFormat) : fmt_word{ 0 } assert(fmt_single(self, lpFormat));
private:
	inline static auto __push(
		uintptr_t ui, LPTSTR hpString,
		TCHAR alfa, uint8_t radix,
		uint8_t int_trunc) {
		uint8_t len = 0;
		do {
			auto d_sm = ui / radix;
			auto rbit = (TCHAR)(ui - d_sm * radix);
			ui = d_sm;
			*--hpString = rbit + (rbit < 10 ? TEXT('0') : alfa);
		} while (ui && ++len < int_trunc);
		return hpString;
	}
	inline static auto __push(
		double pure_float, LPTSTR lpString,
		TCHAR alfa, uint8_t radix,
		uint8_t float_trunc) {
		uint8_t len = 0;
		while (pure_float > 0.f && len++ < float_trunc) {
			pure_float *= radix;
			auto rbit = (TCHAR)pure_float;
			pure_float -= rbit;
			*lpString++ = rbit + (rbit < 10 ? TEXT('0') : alfa);
		}
		return lpString;
	}
	inline static auto __moveH2L(LPTSTR lpString, LPTSTR hpString,
								 int uLength) {
		while (uLength--) *lpString++ = *hpString++;
		return lpString;
	}
private:
	inline uint8_t _radix() const reflect_as(radix_type == Rad::Dec ? 10 : (2 << (uint8_t)radix_type));
	inline TCHAR _alfa_char() const reflect_as((alfa_type == Cap::Big ? TEXT('A') : TEXT('a')) - 10);
	inline auto _push(uintptr_t uint_part, LPTSTR hpBuffer, 
					  uint8_t radix, TCHAR alfa,
					  bool neg) const {
		auto lpBuffer = __push(uint_part, hpBuffer, alfa, radix, int_trunc ? int_calign : 32);
		auto int_len = hpBuffer - lpBuffer;
		if (int_len < int_calign && int_align) {
			auto fill = int_blank == Align::Space ?
				TEXT(' ') : TEXT('0');
			while (int_len++ < int_calign)
				*--lpBuffer = fill;
		}
		if (neg)
			*--lpBuffer = TEXT('-');
		elif (symbol == Symbol::Pos) 
			*--lpBuffer = TEXT('+');
		elif (symbol == Symbol::Space)
			*--lpBuffer = TEXT(' ');
		if (right_dir)
			while (int_len++ < int_calign)
				*--lpBuffer = TEXT(' ');
		return lpBuffer;
	}
	inline auto _push(double float_part, LPTSTR lpBuffer,
					  uint8_t radix, TCHAR alfa) const {
		auto hpBuffer = __push(
			float_part, lpBuffer + 1,
			alfa, radix, float_calign);
		auto float_len = hpBuffer - lpBuffer - 1;
		if (float_align) {
			auto fill = float_blank == Align::Space ?
				TEXT(' ') : TEXT('0');
			while (float_len++ < int_calign)
				*hpBuffer++ = fill;
		}
		if (float_len > 0)
			*lpBuffer = TEXT('.');
		elif(float_force)
			*lpBuffer = TEXT('.');
		elif(float_align)
			*lpBuffer = TEXT(' ');
		else --hpBuffer;
		return hpBuffer;
	}
public:
	inline auto push(uintptr_t uint_part, LPTSTR lpBuffer, LPTSTR &hpBuffer, size_t maxLength) const {
		hpBuffer = lpBuffer + maxLength - 2;
		lpBuffer = _push(uint_part, hpBuffer, _radix(), _alfa_char(), false);
		return lpBuffer;
	}
	inline auto push(intptr_t int_part, LPTSTR lpBuffer, LPTSTR &hpBuffer, size_t maxLength) const {
		bool neg = int_part < 0;
		if (neg) int_part = -int_part;
		hpBuffer = lpBuffer + maxLength - 2;
		lpBuffer = _push((uintptr_t)int_part, hpBuffer, _radix(), _alfa_char(), neg);
		return lpBuffer;
	}
	inline auto push(double float_part, LPTSTR lpBuffer, LPTSTR &hpBuffer, size_t maxLength) const {
		auto rad = _radix();
		auto alfa = _alfa_char();
		bool neg = float_part < 0.;
		if (neg) float_part = -float_part;
		auto uint_part = (uintptr_t)float_part;
		hpBuffer = lpBuffer + maxLength - 2;
		lpBuffer = _push(uint_part, hpBuffer, rad, alfa, neg);
		auto lpBufferNew = lpBuffer - float_calign;
		hpBuffer = __moveH2L(lpBufferNew, lpBuffer, (int)(hpBuffer - lpBuffer));
		lpBuffer = lpBufferNew;
		hpBuffer = _push(float_part - uint_part, hpBuffer, rad, alfa);
		return lpBuffer;
	}
public:
	static constexpr size_t maxLength = 64;
	template<class AnyType>
	inline String operator()(AnyType i) const {
		TCHAR lpBuffer[maxLength] = { 0 }, *hpString, *lpString;
		if constexpr (std::is_unsigned_v<AnyType>)
			lpString = push((uintptr_t)i, lpBuffer, hpString, maxLength);
		elif constexpr (std::is_integral_v<AnyType>)
			lpString = push((intptr_t)i, lpBuffer, hpString, maxLength);
		elif constexpr (std::is_floating_point_v<AnyType>)
			lpString = push(i, lpBuffer, hpString, maxLength);
		return +CString(hpString - lpString, lpString);
	}
};
inline static format_numeral operator ""_nx(const char *format, size_t n) reflect_as(format);
inline static format_numeral operator ""_nx(const wchar_t *format, size_t n) reflect_as(format);

#define nX(form, ...) (format_numeral(form)(__VA_ARGS__))

#ifdef _WIN64
#	define oPTR oH016
#else
#	define oPTR oH08
#endif

#pragma endregion

}
