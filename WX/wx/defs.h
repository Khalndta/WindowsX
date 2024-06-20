#pragma once

#include <windows.h>
#include <uxtheme.h>
#include <strsafe.h>

#include <type_traits>
#include <cstdint>
#include <utility>

#pragma comment(lib,"gdi32.lib")
#pragma comment(linker, "\"" \
	"/manifestdependency:" \
	"type='win32' " \
	"name='Microsoft.Windows.Common-Controls' " \
	"version='6.0.0.0' " \
	"processorArchitecture='*' " \
	"publicKeyToken='6595b64144ccf1df' " \
	"language='*'" \
"\"")

#define _M_(...) __VA_ARGS__
#define __braceO (
#define Obrace__ )
#define _B_(...) { __VA_ARGS__ }

#define elif else if
#define fn(...) [&](__VA_ARGS__)

#define ___inline_class___ __inline_cls_
#define extends(...) : __VA_ARGS__
#define inline_constructor(...) ___inline_class___(__VA_ARGS__)
#define inline_destructor() ~___inline_class___()
#define inline_newex(ext, argslist, ...) \
	([&] { \
		struct ___inline_class___ ext __VA_ARGS__; \
		return new ___inline_class___ argslist; \
	})()
#define inline_new(ext, argslist, ...) inline_newex(extends(ext), argslist, __VA_ARGS__)
#define inline_classex(ext, argslist, ...) \
	([&] { \
		struct ___inline_class___ ext __VA_ARGS__; \
		return ___inline_class___ argslist; \
	})()
#define inline_class(argslist, ...) inline_classex(_M_(), argslist, __VA_ARGS__)

#define msvc_property(...) __declspec(property(__VA_ARGS__))

#define misuse_assert(cond, note) static_assert(cond, "Misused: " note)
#define misdef_assert(cond, note) static_assert(cond, "Misdefined: " note)
#define child_assert(base, sub) \
	misuse_assert((std::is_base_of_v<base, sub>), #sub " must base on " #base)

#define reflect_as(...) { return __VA_ARGS__; }
#define reflect_to(line, ...) { line; return __VA_ARGS__; }

#define reflect_to_self(...)  { __VA_ARGS__; retself; }
#define reflect_to_child(...) { __VA_ARGS__; retchild; }

#define _if(...) __VA_ARGS__ ? 
#define _else :
#define _elif(...) : __VA_ARGS__ ?
#define _return

namespace WX {

#ifdef UNICODE
constexpr bool IsUnicode = true;
#else
constexpr bool IsUnicode = false;
#endif

#define T(str) TEXT(str)

template<class OutType, class InType>
inline static OutType force_cast(InType in) reflect_as(*(OutType *)&in);

template<class OutType, class InType>
inline static OutType &reuse_as(InType &in) {
	misuse_assert(sizeof(OutType) == sizeof(InType), "Must fit same size");
	return *(OutType *)(&in);
}

using Null = decltype(nullptr);
constexpr Null O = nullptr;

template<class AnyType, size_t Len>
using arrayof = AnyType[Len];
template<class...Args>
constexpr size_t ArgsCount(Args...) reflect_as(sizeof...(Args));
template<class AnyType>
static constexpr size_t lengthof = 0;
template<class AnyType, size_t Len>
static constexpr size_t lengthof<const arrayof<AnyType, Len> &> = Len;
template<class AnyType, size_t Len>
constexpr auto &_CountOf(const arrayof<AnyType, Len> &a) reflect_as(a);
#define CountOf(...) lengthof<decltype(_CountOf(__VA_ARGS__))>

template<class AnyClass>
union RefAs {
private:
	AnyClass t;
public:
	template<class...AnyType>
	RefAs(AnyType &...arg) : t(arg...) {}
	template<class...AnyType>
	RefAs(AnyType &&...arg) : t(arg...) {}
	template<class...AnyType>
	RefAs(const AnyType &...arg) : t(arg...) {}
	~RefAs() {}
	inline AnyClass &operator*() reflect_as(t);
	inline const AnyClass &operator*() const reflect_as(t);
	inline AnyClass *operator&() reflect_as(std::addressof(t));
	inline const AnyClass *operator&() const reflect_as(std::addressof(t));
	inline AnyClass *operator->() reflect_as(std::addressof(t));
	inline const AnyClass *operator->() const reflect_as(std::addressof(t));
	inline operator AnyClass &() reflect_as(t);
	inline operator const AnyClass &() const reflect_as(t);
	template<class AnyType>
	inline operator AnyType() reflect_as(t);
	template<class AnyType>
	inline operator AnyType() const reflect_as(t);
};

struct Exception {
	LPCTSTR lpszLine;
	DWORD dwErrCode;
public:
	Exception(LPCTSTR lpszLine, DWORD dwErrCode) :
		lpszLine(lpszLine), dwErrCode(dwErrCode) {}
	Exception(LPCTSTR lpszLine) :
		lpszLine(lpszLine), dwErrCode(GetLastError()) {}
};

#define assert(line) \
{ if (!(line)) throw Exception(TEXT(#line)); }

#pragma region Z-Token
template<class AnyChar = TCHAR>
struct ZRegexX {

	using LP_CSTR = const AnyChar *;
	using HP_CSTR = const LP_CSTR;

#pragma region Offsets
	template<AnyChar st, AnyChar ed = st>
	static constexpr bool is(AnyChar ch) reflect_as(st <= ed ? (st <= ch && ch <= ed) : (st < ch || ch < ed));
	template<AnyChar...chs>
	static constexpr bool in(AnyChar ch) reflect_as(((ch == chs) || ...));

	static constexpr bool is_09(AnyChar ch) reflect_as(is<'0', '9'>(ch));
	static constexpr bool is_AZ(AnyChar ch) reflect_as(is<'A', 'Z'>(ch));
	static constexpr bool is_az(AnyChar ch) reflect_as(is<'a', 'z'>(ch));
	static constexpr bool is_Az(AnyChar ch) reflect_as(is_AZ(ch) || is_az(ch));
	static constexpr bool is_Word(AnyChar ch) reflect_as(is_Az(ch) || is_09(ch) || ch == '_');
	static constexpr bool is_Space(AnyChar ch) reflect_as(in<' ', '\t'>(ch));
	static constexpr bool is_Blank(AnyChar ch) reflect_as(is_Space(ch) || in<'\n', '\r'>(ch));

	static constexpr HP_CSTR OffsetProtoWord(LP_CSTR lpcl, HP_CSTR lpch) reflect_as(
		_if (is_Word(lpcl[0]))
			_return OffsetProtoWord(lpcl + 1, lpch)
		_else
			_return lpcl
	);
	static constexpr HP_CSTR OffsetWord(LP_CSTR lpcl, HP_CSTR lpch) reflect_as(
		_if (is_Word(lpcl[0]) && !is_09(lpcl[0]))
			_return OffsetProtoWord(lpcl + 1, lpch)
		_else
			_return lpcl
	);
	static constexpr HP_CSTR OffsetNumber(LP_CSTR lpcl, HP_CSTR lpch) reflect_as(
		_if (is_09(lpcl[0]))
			_return OffsetNumber(lpcl + 1, lpch)
		_else
			_return lpcl
	);
	static constexpr HP_CSTR OffsetBlank(LP_CSTR lpcl, HP_CSTR lpch) reflect_as(
		_if (is_Blank(lpcl[0]))
			_return OffsetBlank(lpcl + 1, lpch)
		_else
			_return lpcl
	);
	static constexpr HP_CSTR OffsetSpace(LP_CSTR lpcl, HP_CSTR lpch) reflect_as(
		_if (is_Space(lpcl[0]))
			_return OffsetSpace(lpcl + 1, lpch)
		_else 
			_return lpcl
	);
	static constexpr HP_CSTR OffsetToken(LP_CSTR lpcl, HP_CSTR lpch, LP_CSTR lpclDst, HP_CSTR lpchDst) reflect_as(
		_if (lpcl[0] == lpclDst[0])
			_return OffsetToken(lpcl + 1, lpch, lpclDst + 1, lpchDst)
		_else
			_return lpcl
	);
	//static constexpr HP_CSTR OffsetValue(LP_CSTR lpcl, HP_CSTR lpch, size_t Level = 0) reflect_as(
	//	_if (lpcl[0] == '\0')
	//		_return lpcl
	//	_elif (lpcl[0] == '(')
	//		_return OffsetValue(lpcl + 1, lpch, Level + 1)
	//	_elif (lpcl[0] == ',')
	//		_if (Level == 0)
	//			_return lpcl + 1
	//		_else
	//			_return OffsetValue(lpcl + 1, lpch, Level)
	//	_elif (lpcl[0] == ')')
	//		_if (Level == 0)
	//			_return lpcl
	//		_else
	//			_return OffsetValue(lpcl + 1, lpch, Level - 1)
	//	_else
	//		_return OffsetValue(lpcl + 1, lpch, Level)
	//);
#pragma endregion

	struct Token {
		LP_CSTR Low;
		HP_CSTR High;
		bool Matched = Low < High;
		bool Catched;
		constexpr Token(
			LP_CSTR Low, HP_CSTR High,
			bool Catched = false) :
			Low(Low), High(High),
			Catched(Catched) {}
		constexpr Token(
			LP_CSTR Low, HP_CSTR High,
			bool Catched, bool Matched) :
			Low(Low), High(High),
			Matched(Matched), Catched(Catched) {}
		constexpr auto Length() const reflect_as(HighOff - LowOff);
		constexpr operator bool() const reflect_as(Matched);
	};

	struct Blank { constexpr auto Parse(LP_CSTR Low, HP_CSTR High) reflect_as(Token(Low, is_Blank(Low[0]) ? Low + 1 : Low)); };
	struct Blanks { constexpr auto Parse(LP_CSTR Low, HP_CSTR High) reflect_as(Token(Low, OffsetBlank(Low, High))); };
	struct Space { constexpr auto Parse(LP_CSTR Low, HP_CSTR High) reflect_as(Token(Low, is_Space(Low[0]) ? Low + 1 : Low)); };
	struct Spaces { constexpr auto Parse(LP_CSTR Low, HP_CSTR High) reflect_as(Token(Low, OffsetSpace(Low, High))); };
	struct Word { constexpr auto Parse(LP_CSTR Low, HP_CSTR High) reflect_as(Token(Low, is_Word(Low[0]) ? Low + 1 : Low)); };
	struct Words { constexpr auto Parse(LP_CSTR Low, HP_CSTR High) reflect_as(Token(Low, OffsetWord(Low, High))); };
	struct Number { constexpr auto Parse(LP_CSTR Low, HP_CSTR High) reflect_as(Token(Low, is_09(Low[0]) ? Low + 1 : Low)); };
	struct Numbers { constexpr auto Parse(LP_CSTR Low, HP_CSTR High) reflect_as(Token(Low, OffsetNumber(Low, High)));) };

	struct Is {
		LP_CSTR st, ed;
		size_t Len = 1;
		constexpr auto Parse(LP_CSTR Low, HP_CSTR High) {}
	};
	//template<AnyChar...chs>
	//struct In : Token {
	//	constexpr In(LP_CSTR lpcStr, size_t LowOff = 0) :
	//		Token(LowOff, in<chs...>(str[LowOff]) ? LowOff + 1 : LowOff) {}
	//};

	//template<class AnyToken, size_t Time>
	//struct Rept;
	//template<class AnyToken>
	//struct Rept<AnyToken, 1> {
	//	AnyToken tok;
	//	size_t LowOff = tok.LowOff, HighOff = tok.HighOff;
	//	bool Matched = tok.Matched, Catched = tok.Catched;
	//	constexpr Rept(LP_CSTR lpcStr, size_t LowOff = 0) :
	//		tok(str, LowOff) {}
	//	constexpr auto Length() const reflect_as(tok.Length());
	//};
	//template<class AnyToken, size_t Times>
	//struct Rept {
	//	static_assert(Times);
	//	AnyToken tok;
	//	Rept<AnyToken, Times - 1> last;
	//	size_t LowOff = last.LowOff, HighOff = last.HighOff;
	//	bool Matched = tok.Matched && last.Matched, Catched = tok.Catched || last.Catched;
	//	constexpr Rept(LP_CSTR lpcStr, size_t LowOff = 0) : 
	//		tok(str, LowOff), last(str, tok.HighOff) {}
	//	constexpr auto Length() const reflect_as(tok.Length() + last.Length());
	//};

	//template<class AnyToken>
	//struct ReptMax {
	//	const size_t Times;
	//	AnyToken last;
	//	size_t LowOff = last.LowOff, HighOff = last.HighOff;
	//	bool Matched = last.Matched, Catched = last.Catched;
	//	constexpr ReptMax(LP_CSTR lpcStr, size_t LowOff = 0) :
	//		Times(GetTimes(str, LowOff)), last(GetToken(Times, str, LowOff)) {}
	//	constexpr auto Length() const reflect_as(HighOff - LowOff);
	//	static constexpr AnyToken GetToken(size_t ind, LP_CSTR lpcStr, size_t LowOff) reflect_as(
	//		_if (ind <= 1)
	//			_return GetToken(ind, str, AnyToken(str, LowOff).HighOff)
	//		_else
	//			_return AnyToken(str, LowOff)
	//	);
	//	static constexpr size_t GetTimes(LP_CSTR lpcStr, size_t LowOff) reflect_as(
	//		_if (AnyToken(str, LowOff).Matched)
	//			_return 1 + GetTimes(str, AnyToken(str, LowOff).HighOff)
	//		_else
	//			_return 0
	//	);
	//};

	//template<class...>
	//struct And;
	//template<class AnyToken>
	//struct And<AnyToken> {
	//	AnyToken tok;
	//	size_t LowOff = tok.LowOff, HighOff = tok.HighOff;
	//	bool Matched = tok.Matched, Catched = tok.Catched;
	//	constexpr And(LP_CSTR lpcStr, size_t LowOff = 0) :
	//		tok(str, LowOff) {}
	//	constexpr auto Length() const reflect_as(tok.Length());
	//	template<size_t ind>
	//	constexpr auto CatchOf() const {
	//		if constexpr (AnyToken::Catched && ind == 0)
	//			return tok;
	//		else
	//			return;
	//	}
	//};
	//template<class AnyToken, class...Args>
	//struct And<AnyToken, Args...> {
	//	AnyToken tok;
	//	And<Args...> last;
	//	size_t LowOff = last.LowOff, HighOff = last.HighOff;
	//	bool
	//		Matched = tok.Matched && last.Matched,
	//		Catched = tok.Catched || last.Catched;
	//	constexpr And(LP_CSTR lpcStr, size_t LowOff = 0) :
	//		tok(str, LowOff), last(str, tok.HighOff) {}
	//	constexpr auto Length() const reflect_as(HighOff - LowOff);
	//	template<size_t ind>
	//	constexpr auto CatchOf() const {
	//		if constexpr (AnyToken::Catched)
	//			if constexpr (ind == 0)
	//				return tok;
	//			else
	//				return last.template catchOf<ind - 1>();
	//		else
	//			return last.template catchOf<ind>();
	//	}
	//};

	//template<class...>
	//struct Or;
	//template<class AnyToken>
	//struct Or<AnyToken> {
	//	AnyToken tok;
	//	size_t LowOff = tok.LowOff, HighOff = tok.HighOff;
	//	bool Matched = true, Catched = tok.Catched;
	//	constexpr Or(LP_CSTR lpcStr, size_t LowOff = 0) :
	//		tok(str, LowOff) {}
	//	constexpr auto Length() const reflect_as(tok.Length());
	//	template<size_t ind>
	//	constexpr auto CatchOf() const {
	//		if constexpr (AnyToken::Catched && ind == 0)
	//			return tok;
	//		else
	//			return;
	//	}
	//};
	//template<class AnyToken, class...Args>
	//struct Or<AnyToken, Args...> {
	//	AnyToken tok;
	//	Or<Args...> last;
	//	size_t
	//		LowOff = tok.Matched ? tok.LowOff : last.LowOff,
	//		HighOff = tok.Matched ? tok.HighOff : last.HighOff;
	//	bool
	//		Matched = tok.Matched ? tok.Matched : last.Matched,
	//		Catched = tok.Catched || last.Catched;
	//	constexpr Or(LP_CSTR lpcStr, size_t LowOff = 0) :
	//		tok(str, LowOff), last(str, LowOff) {}
	//	constexpr auto Length() const reflect_as(HighOff - LowOff);
	//	template<size_t ind>
	//	constexpr auto CatchOf() const {
	//		if constexpr (AnyToken::Catched)
	//			if constexpr (ind == 0)
	//				return tok;
	//			else
	//				return last.template catchOf<ind - 1>();
	//		else
	//			return last.template catchOf<ind>();
	//	}
	//};

};
using ZRegex = ZRegexX<>;
#pragma endregion

#pragma region Type Traits

#pragma region KChain

template<class ParentClass, class ChildClass>
using KChain = std::conditional_t<std::is_void_v<ChildClass>, ParentClass, ChildClass>;

template<class ParentClass, class ChildClass>
struct ChainExtend {
	using Child = KChain<ParentClass, ChildClass>;
	using Self = ParentClass;
	constexpr ChainExtend() {
		if constexpr (!std::is_void_v<ChildClass>)
			child_assert(ParentClass, ChildClass);
	}
	Child &child() reflect_as(*static_cast<Child *>(this));
	const Child &child() const reflect_as(*static_cast<const Child *>(this));
	Self &self() reflect_as(*static_cast<Self *>(this));
	const Self &self() const reflect_as(*static_cast<const Self *>(this));
};

#define child  (this->child())
#define self   (*this)
#define retself return self
#define retchild return child

#define assert_reflect_as(line, ...) \
{ if (line) return __VA_ARGS__; throw Exception(TEXT(#line)); }
#define assert_reflect_as_self(line)  assert_reflect_as(line, self)
#define assert_reflect_as_child(line) assert_reflect_as(line, child)

#define assert_reflect_to(defs, line, ...) \
{ defs; if (line) return __VA_ARGS__; throw Exception(TEXT(#line)); }
#define assert_reflect_to_self(defs, line)  assert_reflect_to(defs, line, self)
#define assert_reflect_to_child(defs, line) assert_reflect_to(defs, line, child)

#define check_reflect_to(line, ...) \
{ line; if (auto _err = GetLastError()) throw Exception(TEXT(#line), _err); return __VA_ARGS__; }
#define check_reflect_to_self(line)  check_reflect_to(line, self)
#define check_reflect_to_child(line) check_reflect_to(line, child)

#pragma endregion

#pragma region Compatible
template <class AnyType, class Ret, class... Args>
static auto _static_compatible(...) -> std::false_type;
template <class AnyType, class Ret, class... Args>
static auto _static_compatible(int) -> std::is_convertible<
	decltype(std::declval<AnyType>()(std::declval<Args>()...)), Ret>;
template<class AnyType, class FuncType>
static constexpr bool static_compatible = false;
template<class AnyType, class Ret, class... Args>
static constexpr bool static_compatible<AnyType, Ret(Args...)> =
decltype(_static_compatible<AnyType, Ret, Args...>(0))::value;
#pragma endregion

#pragma region SFINAE Type Helper
#define def_memberof(name) \
template <class AnyClass> class member_##name##_of { \
	template<class _AnyClass> \
	static auto _retof_##name(int) -> decltype(&_AnyClass::name); \
	template<class _AnyClass> \
	static auto _retof_##name(...) -> void; \
	template<class _AnyClass, class Ret, class... Args> \
	static auto _compatible(Ret(*)(Args...)) -> std::is_convertible< \
		decltype(std::declval<_AnyClass>().name(std::declval<Args>()...)), Ret>; \
	template<class _AnyClass> \
	static auto _compatible(...) -> std::false_type; \
public: \
	using type = decltype(_retof_##name<AnyClass>(0)); \
	static constexpr bool existed = !std::is_void_v<type>; \
	template<class AnyType> \
	static constexpr bool compatible_to = \
		decltype(_compatible<AnyClass>(std::declval<AnyType *>()))::value; }
#define subtype_branch(name) \
template<class AnyType, class OtherType> \
inline static auto __subtype_branchof_##name(int) -> \
	typename AnyType::name; \
template<class AnyType, class OtherType> \
inline static auto __subtype_branchof_##name(...) -> \
	OtherType; \
template<class AnyType, class OtherType> \
using subtype_branchof_##name = \
	decltype(__subtype_branchof_##name<AnyType, OtherType>(0))
#pragma endregion

#pragma region Enumerate
#define enum_default
#define enum_alias
#define enum_complex
enum class EnumModifies : CHAR {
	no = 0,
	defualt = 'd',
	alias = 'a',
	complex = 'c'
};
class String;
/*
struct SkipBlanks : ZToken {
	constexpr SkipBlanks(const TCHAR *str, size_t Off) : ZToken(Off, OffsetSpace(str, Off)) {}
};
struct en_modify : ZToken {
	static constexpr TCHAR TOK_enum_default[] = T("enum_default");
	static constexpr TCHAR TOK_enum_complex[] = T("enum_complex");
	static constexpr TCHAR TOK_enum_alias[] = T("enum_alias");
	constexpr en_modify(const TCHAR *str, size_t Off) : Token(
		_return ztoken<EnumModifies>(EnumModifies::defualt, Off, OffsetToken(str, TOK_enum_default, Off),
				ztoken<EnumModifies>(EnumModifies::complex, Off, OffsetToken(str, TOK_enum_complex, Off),
				Token(EnumModifies::alias, Off, OffsetToken(str, TOK_enum_alias, Off),
				Token(EnumModifies::no))))) {}
}; 
struct en_name : Tokenizer<TCHAR> {
	using Token = Tokeni<bool>;
	static constexpr Token Parse(const TCHAR *str, size_t Off) reflect_as(Token(true, Off, OffsetWord(str, Off)));
};
struct en_eq : Tokenizer<TCHAR> {
	static constexpr Token Parse(const TCHAR *str, size_t Off) reflect_as(Token(Off, Off + (str[Off] == '=' ? 1 : 0)));
};
struct en_val : Tokenizer<TCHAR> {
	using Token = Tokeni<bool>;
	static constexpr Token Parse(const TCHAR *str, size_t Off) reflect_as(Token(true, Off, OffsetValue(str, Off)));
};*/
// using EnumTokenizer = Tokenizer<TCHAR>::Tokens<en_modify, skip_blanks, en_name, skip_blanks, en_eq, skip_blanks, en_val>;

template<class ProtoType>
struct EnumToken {
	const String *pszEnumName;
	const ProtoType *val;
	LPCTSTR lpLeft = O, lpRight = O;
	USHORT lnLeft = 0, lnRight = 0;
	EnumModifies mod = EnumModifies::no;
	EnumToken() {}
	EnumToken(const String &szEnumName, const ProtoType *val) :
		pszEnumName(std::addressof(szEnumName)), val(val) {}
	inline const String Left() const;
	inline const String Right() const;
	inline const ProtoType &Value() const reflect_as(*val);
	inline const String &EnumName() const reflect_as(*pszEnumName);
};
template<class AnyType>
struct EnumUnitBase {
	using ProtoType = AnyType;
	using ProtoUnit = EnumUnitBase;
	ProtoType val;
	constexpr EnumUnitBase(ProtoType val) : val(val) {}
};
def_memberof(EnumName);
template<class AnyType>
static constexpr bool IsEnumBased = member_EnumName_of<AnyType>::existed;
template<class AnyType>
using EnumUnit = std::conditional_t<
	IsEnumBased<AnyType>, AnyType,
	EnumUnitBase<AnyType>>;
template<class SubClass, class BaseType>
struct EnumBase {
	static constexpr bool IsProtoEnum = !IsEnumBased<BaseType>;
	static const String &&EnumName;
	using ProtoEnum = std::conditional_t<IsProtoEnum, SubClass, BaseType>;
	using ProtoUnit = typename EnumUnit<BaseType>::ProtoUnit;
	using ProtoType = typename ProtoUnit::ProtoType;
	using Token = EnumToken<ProtoType>;
	static constexpr size_t CountAll() reflect_as(IsProtoEnum ? SubClass::Count : SubClass::Count + ProtoEnum::CountAll());
private:
};
#define __enum_ex(name, base, oprt, estr, ...) \
class name : public EnumBase<name, base> { \
public: \
	using super = EnumBase<name, base>; \
	using ProtoType = typename super::ProtoType; \
	using ProtoUnit = typename super::ProtoUnit; \
public: \
	friend struct EnumBase<name, base>; \
	static constexpr TCHAR __Name[] = TEXT(#name); \
	static constexpr TCHAR __Entries[] = TEXT(estr); \
	inline static const ProtoType *__Index(size_t ind) { \
		static const ProtoType *const values[] = { __VA_ARGS__ }; \
		return values[ind]; \
	} \
	class Unit : protected ProtoUnit { \
	protected: friend class name; \
		constexpr Unit(const ProtoType &val) : ProtoUnit(val) { static_assert(sizeof(val) == sizeof(self), "alignment error"); } \
		const ProtoType *operator=(const ProtoType &) const reflect_as(std::addressof(val)); \
	public: constexpr Unit(const ProtoUnit &val) : ProtoUnit(val.val) {} \
		oprt(Unit); \
		inline ProtoType yield() const reflect_as(val); \
	} val; \
public: \
	inline static const Unit __VA_ARGS__; \
	oprt(Unit); \
	constexpr name(Unit val) : val(val.val) {} \
	static constexpr size_t Count = CountOf({ __VA_ARGS__ }); \
	inline ProtoType yield() const reflect_as(val.yield()); \
	inline operator Unit() const reflect_as(val); }
#define enum_flags_opr(name) \
	inline name operator~ (      ) const reflect_as(~val); \
	inline name operator& (name v) const reflect_as( val &  v.val); \
	inline name operator* (name v) const reflect_as( val &  v.val); \
	inline name operator| (name v) const reflect_as( val |  v.val); \
	inline name operator+ (name v) const reflect_as( val |  v.val); \
	inline name operator- (name v) const reflect_as( val & ~v.val); \
	inline name operator^ (name v) const reflect_as( val ^  v.val); \
	inline name operator/ (name v) const reflect_as( val ^  v.val); \
	inline name operator&=(name v) reflect_to_self( val &=  v.val); \
	inline name operator*=(name v) reflect_to_self( val &=  v.val); \
	inline name operator|=(name v) reflect_to_self( val |=  v.val); \
	inline name operator+=(name v) reflect_to_self( val |=  v.val); \
	inline name operator-=(name v) reflect_to_self( val &= ~v.val); \
	inline name operator^=(name v) reflect_to_self( val ^=  v.val); \
	inline name operator/=(name v) reflect_to_self( val ^=  v.val); \
	inline bool operator==(name v) const reflect_as( val == v.val); \
	inline bool operator!=(name v) const reflect_as( val != v.val); \
	inline bool operator<=(name v) const reflect_as((val &  v.val) ==   val); \
	inline bool operator>=(name v) const reflect_as((val &  v.val) == v.val)
#define enum_class_opr(name) \
	inline bool operator==(name v) const reflect_as(val == v.val); \
	inline bool operator!=(name v) const reflect_as(val != v.val)
#define enum_explicit inline operator const ProtoType &() const reflect_as(val)
#define enum_ex(name, base, oprt, ...) __enum_ex(name, base, oprt, #__VA_ARGS__, __VA_ARGS__)
#define enum_flags(name, base, ...) __enum_ex(name, base, enum_flags_opr, #__VA_ARGS__, __VA_ARGS__)
#define enum_class(name, base, ...) __enum_ex(name, base, enum_class_opr, #__VA_ARGS__, __VA_ARGS__)
#define enum_flags_explicit(name, base, ...) __enum_ex(name, base, enum_explicit enum_flags_opr, #__VA_ARGS__, __VA_ARGS__)
#define enum_class_explicit(name, base, ...) __enum_ex(name, base, enum_explicit enum_class_opr, #__VA_ARGS__, __VA_ARGS__)
#pragma endregion

#pragma region Type List
template<class...>
struct TypeList;
template<>
struct TypeList<> {
	static constexpr size_t Length() reflect_as(0);
	template<class AnyType>
	inline auto invoke(AnyType &f) reflect_as(f());
};
template<>
struct TypeList<void> {
	template<int ind>
	struct index;
	//struct index { misuse_assert(ind >= 0, "Index overflowed"); };
	//template<class ___>
	//struct index<0, ___> { using type = void; };
	//template<size_t ind>
	//using IndexOf = typename index<ind>::type;
	template<size_t ind>
	inline void indexof() { misuse_assert(ind >= 0, "index overflowed"); }
	template<class AnyType>
	inline auto invoke(AnyType &f) reflect_as(f());
};
template<class Type0>
struct TypeList<Type0> {
	Type0 type;
	template<class AnyType>
	TypeList(AnyType &type) : type(type) {}
	template<class AnyType>
	TypeList(AnyType &&type) : type(type) {}
	template<int ind, class = void>
	struct index { misuse_assert(ind >= 0, "Index overflowed"); };
	template<class ___>
	struct index<0, ___> { using type = Type0; };
	template<size_t ind>
	using IndexOf = typename index<ind>::type;
	template<size_t ind>
	inline auto &indexof() reflect_to(misuse_assert(ind >= 0, "index overflowed"), type);
public:
	template<class ClassType, class MethodType>
	inline auto invoke(ClassType &cls, MethodType method) reflect_as((cls.*method)(std::forward<Type0>(type)));
	template<class AnyType>
	inline auto invoke(AnyType &f) reflect_as(f(std::forward<Type0>(type)));
};
template<class Type0, class...Types>
struct TypeList<Type0, Types...> : TypeList<Types...> {
	Type0 type;
	template<class AnyType, class...Args>
	TypeList(AnyType &type, Args...args) : TypeList<Types...>(args...), type(type) {}
	template<class AnyType, class...Args>
	TypeList(AnyType &&type, Args...args) : TypeList<Types...>(args...), type(type) {}
	static constexpr size_t Length() reflect_as(1 + sizeof...(Types));
	template<size_t ind, class = void>
	struct index : TypeList<Types...>::template index<ind - 1> {};
	template<class ___>
	struct index<0, ___> { using type = Type0; };
	template<size_t ind>
	using IndexOf = typename index<ind>::type;
	template<size_t ind>
	inline auto &indexof() {
		if constexpr (ind == 0) return type;
		else return TypeList<Types...>::template indexof<ind - 1>();
	}
	using ind_seq = std::index_sequence_for<Type0, Types...>;
private:
	template<class ClassType, class MethodType, size_t... ind>
	inline auto invoke(ClassType &cls, MethodType method, std::index_sequence<ind...>) reflect_as((cls.*method)(indexof<ind>()...));
	template<class AnyType, size_t... ind>
	inline auto invoke(AnyType &f, std::index_sequence<ind...>) reflect_as(f(indexof<ind>()...));
public:
	template<class ClassType, class MethodType>
	inline auto invoke(ClassType &cls, MethodType method) reflect_as(call(cls, method, ind_seq{}));
	template<class AnyType>
	inline auto invoke(AnyType &f) reflect_as(call(f, ind_seq{}));
};
#pragma endregion

#pragma region Const Array
template<class AnyType, size_t Len>
struct ConstArray {
	AnyType array[Len];
	template<size_t xLen, size_t... ind>
	constexpr ConstArray(const AnyType(&arr)[xLen], std::index_sequence<ind...>, size_t Off = 0) : array{ arr[ind + Off]... } {}
	template<size_t xLen>
	constexpr ConstArray(const AnyType(&arr)[xLen], size_t Off = 0) : ConstArray(arr, std::make_index_sequence<Len>{}, Off) {}
	static constexpr size_t Length = Len;
};
template<class AnyType>
struct ConstArray<AnyType, 0> {
	static constexpr size_t Length = 0;
};
template<class AnyType, size_t Len>
constexpr ConstArray<AnyType, Len> ArrayOf(const AnyType(&arr)[Len], size_t Off = 0) reflect_as({ arr, Off });
#pragma endregion

#pragma region Proto
template<class AnyType>
struct to_proto_t {
	using type = AnyType;
};
template<class AnyType>
struct to_proto_t<AnyType *> : to_proto_t<AnyType> {};
template<class AnyType>
struct to_proto_t<AnyType &> : to_proto_t<AnyType> {};
template<class AnyType>
struct to_proto_t<AnyType &&> : to_proto_t<AnyType> {};
template<class AnyType>
struct to_proto_t<const AnyType> : to_proto_t<AnyType> {};
template<class AnyType>
struct to_proto_t<volatile AnyType> : to_proto_t<AnyType> {};
template<class AnyType>
using to_proto = typename to_proto_t<AnyType>::type;
#pragma endregion

#pragma region Function Of
template<class OtherType>
struct __functionof {
	static constexpr bool is_static = false;
	static constexpr bool is_method = false;
};
template<class Ret, class AnyClass, class...Args>
struct __functionof<Ret(AnyClass:: *)(Args...)> {
	using Belong = AnyClass;
	using Return = Ret;
	using ArgsList = TypeList<Args...>;
	using Pointer = Ret(AnyClass:: *)(Args...);
	static constexpr bool is_method = true;
	static constexpr bool is_static = false;
	static constexpr bool is_ellipsis = false;
};
template<class Ret, class AnyClass, class...Args>
struct __functionof<Ret(AnyClass:: *)(Args..., ...)> {
	using Belong = AnyClass;
	using Return = Ret;
	using ArgsList = TypeList<Args...>;
	using Pointer = Ret(AnyClass:: *)(Args...);
	static constexpr bool is_method = true;
	static constexpr bool is_static = false;
	static constexpr bool is_ellipsis = true;
};
template<class Ret, class... Args>
struct __functionof<Ret(Args...)> {
	using Return = Ret;
	using ArgsList = TypeList<Args...>;
	using Pointer = Ret(*)(Args...);
	static constexpr bool is_method = false;
	static constexpr bool is_static = true;
	static constexpr bool is_ellipsis = false;
};
template<class Ret, class... Args>
struct __functionof<Ret(Args..., ...)> {
	using Return = Ret;
	using ArgsList = TypeList<Args...>;
	using Pointer = Ret(*)(Args...);
	static constexpr bool is_method = false;
	static constexpr bool is_static = true;
	static constexpr bool is_ellipsis = true;
};
template<class OtherType>
using functionof = __functionof<to_proto<OtherType>>;
#pragma endregion

#pragma region Function
template<class RetType, class ArgsList>
struct FunctionBase {};
template<class RetType, class... Args>
struct FunctionBase<RetType, TypeList<Args...>> {
	virtual ~FunctionBase() = default;
	virtual RetType operator()(Args ...) = 0;
};
template<class PackType, class RetType, class ArgsList>
class FunctionPackage {};
template<class PackType, class RetType, class... Args>
class FunctionPackage<PackType, RetType, TypeList<Args...>> :
	public FunctionBase<RetType, TypeList<Args...>> {
	mutable PackType func;
public:
	FunctionPackage(PackType &f) : func(f) {}
	RetType operator()(Args ...args) override {
		static_assert(static_compatible<PackType, RetType(Args...)>, "Argument list uncompatible");
		if constexpr (std::is_pointer_v<PackType>) reflect_to(assert(func), func(args...))
		else reflect_as(func(args...));
	}
};
template<class...>
class Function;
template<class Ret, class... Args>
class Function<Ret(Args...)> {
public:
	using RetType = Ret;
	using ArgsList = TypeList<Args...>;
private:
	mutable FunctionBase<Ret, ArgsList> *func = O;
	mutable bool bAllocated = false;
public:
	Function() {}
	Function(Null) {}
	Function(const Function &f) : func(f.func) {}
public:
	template<class Type>
	Function(Type &f) : func(new FunctionPackage<Type, Ret, ArgsList>(f)), bAllocated(true) {}
	template<class Type>
	Function(Type f) : func(new FunctionPackage<Type, Ret, ArgsList>(f)), bAllocated(true) {}
	~Function() { if (func && bAllocated) delete func, func = O; }
public:
	inline operator bool() const reflect_as(!func);
	inline bool operator!=(Null) const reflect_as(func);
	inline bool operator==(Null) const reflect_as(!func);
	inline Function operator&() reflect_to_self();
	inline const Function operator&() const reflect_to_self();
	inline Ret operator()(Args...args) const reflect_to(assert(func), (*func)(args...));
	inline auto &operator=(const Function &f) const noexcept reflect_to_self(func = f.func, bAllocated = false);
	inline auto &operator=(const Function &f) noexcept reflect_to_self(func = f.func, bAllocated = false);
	inline auto &operator=(Function &&f) noexcept reflect_to_self(func = f.func, f.func = O, bAllocated = true);
};
template<class...FuncTypes>
using fn = Function<FuncTypes...>;
#pragma endregion

#pragma endregion

#pragma region Basic Types
struct LSize;
struct LPoint : public POINT {
	LPoint() : POINT{ 0 } {}
	LPoint(const LPoint &p) : POINT{ p.x, p.y } {}
	LPoint(LONG a) : POINT{ a, a } {}
	LPoint(LONG x, LONG y) : POINT{ x, y } {}
	inline LPoint  operator+ ()                const reflect_to_self();
	inline LPoint  operator~ ()                const reflect_as({ y,  x });
	inline LPoint  operator* (double l)        const reflect_as({ LONG((double)x * l), LONG((double)y * l) });
	inline LPoint  operator/ (double l)        const reflect_as({ LONG((double)x / l), LONG((double)y / l) });
	inline LPoint  operator* (int l)           const reflect_as({ x * l, y * l });
	inline LPoint  operator/ (int l)           const reflect_as({ x / l, y / l });
	inline LPoint  operator- ()                const reflect_as({ -x, -y });
	inline LPoint  operator+ (const LPoint &s) const reflect_as({ x + s.x, y + s.y });
	inline LPoint  operator- (const LPoint &s) const reflect_as({ x - s.x, y - s.y });
	inline LPoint &operator*=(double p)              reflect_to_self(x = LONG((double)x * p), y = LONG((double)y * p));
	inline LPoint &operator/=(double p)              reflect_to_self(x = LONG((double)x / p), y = LONG((double)y / p));
	inline LPoint &operator+=(const LPoint &p)       reflect_to_self(x += p.x, y += p.y);
	inline LPoint &operator-=(const LPoint &p)       reflect_to_self(x -= p.x, y -= p.y);
	inline LPoint &operator =(const LPoint &p)       reflect_to_self(x = p.x, y = p.y);
	inline operator LPARAM() const reflect_as((LPARAM)this);
	operator LSize() const;
};
struct LSize : public SIZE {
	LSize() : SIZE{ 0 } {}
	LSize(const SIZE &LSize) : SIZE(LSize) {}
	LSize(const LSize &LSize) : SIZE(LSize) {}
	LSize(LONG c) : SIZE{ c, c } {}
	LSize(LONG cx, LONG cy) : SIZE{ cx, cy } {}
	inline LSize  operator+ ()               const reflect_to_self();
	inline LSize  operator- ()               const reflect_as({ -cx, -cy });
	inline LSize  operator* (double l)       const reflect_as({ LONG((double)cx * l), LONG((double)cy * l) });
	inline LSize  operator/ (double l)       const reflect_as({ LONG((double)cx / l), LONG((double)cy / l) });
	inline LSize  operator* (int l)          const reflect_as({ cx * l, cy * l });
	inline LSize  operator/ (int l)          const reflect_as({ cx / l, cy / l });
	inline LSize  operator+ (const LSize &s) const reflect_as({ cx + s.cx, cy + s.cy });
	inline LSize  operator- (const LSize &s) const reflect_as({ cx - s.cx, cy - s.cy });
	inline LSize &operator*=(double p)             reflect_to_self(cx = LONG((double)cx * p), cy = LONG((double)cy * p));
	inline LSize &operator/=(double p)             reflect_to_self(cx = LONG((double)cx / p), cy = LONG((double)cy / p));
	inline LSize &operator+=(const LSize &p)       reflect_to_self(cx += p.cx, cy += p.cy);
	inline LSize &operator-=(const LSize &p)       reflect_to_self(cx -= p.cx, cy -= p.cy);
	inline LSize &operator =(const LSize &p)       reflect_to_self(cx = p.cx, cy = p.cy);
	inline operator LPARAM() const reflect_as((LPARAM)this);
	inline operator LPoint() const reflect_as({ cx, cy });
};
inline LPoint::operator LSize() const reflect_as({ x, y });
struct LRect : public RECT {
	LRect() : RECT{ 0 } {}
	LRect(const MARGINS &m) : RECT({ m.cxLeftWidth, m.cyTopHeight, m.cxRightWidth, m.cyBottomHeight }) {}
	LRect(const RECT &rc) : RECT(rc) {}
	LRect(const LRect &r) : RECT(r) {}
	LRect(const LPoint &pt, const LSize &sz = 0) : RECT{ pt.x, pt.y, pt.x + sz.cx, pt.y + sz.cy } {}
	LRect(const LSize &sz, const LPoint &pt = 0) : RECT{ pt.x, pt.y, pt.x + sz.cx, pt.y + sz.cy } {}
	LRect(LONG a) : RECT{ a, a, a, a } {}
	LRect(LONG left, LONG top, LONG right, LONG bottom) : RECT{ left, top, right, bottom } {}
	inline LSize  size()         const reflect_as({ right - left, bottom - top });
	inline LPoint top_left()     const reflect_as({ left,    top });
	inline LPoint bottom_left()  const reflect_as({ left, bottom });
	inline LPoint top_right()    const reflect_as({ right,    top });
	inline LPoint bottom_right() const reflect_as({ right, bottom });
	inline LRect  operator+ ()                const reflect_to_self();
	inline LRect  operator- ()                const reflect_as({ -left,   -top, -right, -bottom });
	inline LRect  operator~ ()                const reflect_as({ right, bottom,   left,     top });
	inline LRect  operator+ (const LRect &r)  const reflect_as({ left + r.left, top + r.top, right + r.right, bottom + r.bottom });
	inline LRect  operator- (const LRect &r)  const reflect_as({ left - r.left, top - r.top, right - r.right, bottom - r.bottom });
	inline LRect  operator* (double l)        const reflect_as({ LONG((double)left * l), LONG((double)top * l), LONG((double)right * l), LONG((double)bottom * l) });
	inline LRect  operator/ (double l)        const reflect_as({ LONG((double)left / l), LONG((double)top / l), LONG((double)right / l), LONG((double)bottom / l) });
	inline LRect  operator* (int l)           const reflect_as({ left * l, top * l, right * l, bottom * l });
	inline LRect  operator/ (int l)           const reflect_as({ left / l, top / l, right / l, bottom / l });
	inline LRect &operator*=(double l)              reflect_to_self(left = LONG((double)left * l), top = LONG((double)top * l), right = LONG((double)right * l), bottom = LONG((double)bottom * l));
	inline LRect &operator/=(double l)              reflect_to_self(left = LONG((double)left / l), top = LONG((double)top / l), right = LONG((double)right / l), bottom = LONG((double)bottom / l));
	inline LRect &operator/=(int l)                 reflect_to_self(left /= l, top /= l, right /= l, bottom /= l);
	inline LRect &operator*=(int l)                 reflect_to_self(left *= l; top *= l, right *= l, bottom *= l);
	inline LRect &operator+=(const LRect &r)        reflect_to_self(left += r.left, top += r.top, right += r.right, bottom += r.bottom);
	inline LRect &operator-=(const LRect &r)        reflect_to_self(left -= r.left, top -= r.top, right -= r.right, bottom -= r.bottom);
	inline LRect  operator+ (const LPoint &p) const reflect_as({ left + p.x, top + p.y, right + p.x, bottom + p.y });
	inline LRect  operator- (const LPoint &p) const reflect_as({ left - p.x, top - p.y, right - p.x, bottom - p.y });
	inline LRect &operator+=(const LPoint &p)       reflect_to_self(left += p.x, top += p.y, right += p.x, bottom += p.y);
	inline LRect &operator-=(const LPoint &p)       reflect_to_self(left -= p.x, top -= p.y, right -= p.x, bottom -= p.y);
	inline operator MARGINS() const reflect_as({ left, right, top, bottom });
	inline operator LSize()   const reflect_as({ right - left, bottom - top });
	inline operator LPoint()  const reflect_as({ left, top });
	inline operator LPRECT()        reflect_as(this);
	inline operator LPCRECT() const reflect_as(this);
	inline operator LPARAM()  const reflect_as((LPARAM)this);
	inline static LRect &Attach(RECT &rc) reflect_as(reuse_as<LRect>(rc));
};
inline LRect operator+(const LPoint &p, const LRect &r) reflect_as(r + p);
inline LRect operator-(const LPoint &p, const LRect &r) reflect_as(-(r - p));
inline LRect operator&(const LPoint &p, const LSize &s) reflect_as({ p, s });
inline LRect operator&(const LSize &s, const LPoint &p) reflect_as({ p, s });

class ColorRGB {
protected:
	COLORREF cr;
public:
	ColorRGB(COLORREF color) : cr(color) {}
	ColorRGB(BYTE red, BYTE green, BYTE blue) :
		cr(RGB(red, green, blue)) {}

	inline BYTE Red()   const reflect_as(GetRValue(self));
	inline BYTE Green() const reflect_as(GetGValue(self));
	inline BYTE Blue()  const reflect_as(GetBValue(self));

	template<size_t len>
	inline static arrayof<ColorRGB, len> &Attach(arrayof<COLORREF, len> &ary) reflect_as(reuse_as<arrayof<ColorRGB, len>>(ary));
	inline operator COLORREF() const { return cr; }
	inline static ColorRGB &Attach(COLORREF &clr) reflect_as(*(ColorRGB *)&clr);
};

struct SysTime : public SYSTEMTIME {
	SysTime() : SYSTEMTIME{ 0 } {}
	SysTime(const FILETIME &ft) assert(FileTimeToSystemTime(&ft, this));
	inline static SysTime LocalTime()  reflect_to(SysTime st; GetLocalTime(&st), st);
	inline static SysTime SystemTime() reflect_to(SysTime st; GetSystemTime(&st), st);
};
#pragma endregion

}
