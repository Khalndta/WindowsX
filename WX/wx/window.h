#pragma once

#include <unordered_map>
#include <typeinfo>

#include "./resource.h"

namespace WX {

#pragma region MessageBox
enum_flags(MB, int,
	Ok                   =  MB_OK,
	OkCancel             =  MB_OKCANCEL,
	AbortRetryIgnore     =  MB_ABORTRETRYIGNORE,
	YesNoCancel          =  MB_YESNOCANCEL,
	YesNo                =  MB_YESNO,
	RetryCancel          =  MB_RETRYCANCEL,
	CancelTryContinue    =  MB_CANCELTRYCONTINUE,
	IconHand             =  MB_ICONHAND,
	IconQuestion         =  MB_ICONQUESTION,
	IconExclamation      =  MB_ICONEXCLAMATION,
	IconAsterisk         =  MB_ICONASTERISK,
	UserIcon             =  MB_USERICON,
	IconWarning          =  MB_ICONWARNING,
	IconError            =  MB_ICONERROR,
	IconInformation      =  MB_ICONINFORMATION,
	IconStop             =  MB_ICONSTOP,
	DefButton1           =  MB_DEFBUTTON1,
	DefButton2           =  MB_DEFBUTTON2,
	DefButton3           =  MB_DEFBUTTON3,
	DefButton4           =  MB_DEFBUTTON4,
	ApplModal            =  MB_APPLMODAL,
	SystemModal          =  MB_SYSTEMMODAL,
	TaskModal            =  MB_TASKMODAL,
	Help                 =  MB_HELP,
	NoFocus              =  MB_NOFOCUS,
	SetForeground        =  MB_SETFOREGROUND,
	DefaultDesktopOnly   =  MB_DEFAULT_DESKTOP_ONLY,
	Topmost              =  MB_TOPMOST,
	Right                =  MB_RIGHT,
	RtlReading           =  MB_RTLREADING,
	ServiceNotification  =  MB_SERVICE_NOTIFICATION);
inline MB MsgBox(LPCTSTR lpCaption = S(""), LPCTSTR lpText = S(""), MB uType = MB::Ok, HWND hParent = O) reflect_as(force_cast<MB>(MessageBox(hParent, lpText, lpCaption, uType.yield())));
#pragma endregion

class WMailSlot {
	using MsgProc = Function<LRESULT(WPARAM, LPARAM)>;
	std::unordered_map<UINT, MsgProc> msglst;
public:
	inline MsgProc Attach(UINT msgid, MsgProc mf) {
		std::swap(msglst[msgid], mf); return mf;
	}
	inline bool operator()(LRESULT &res, UINT msgid, WPARAM wParam, LPARAM lParam) {
		auto &&itaFunc = msglst.find(msgid);
		if (itaFunc == msglst.end()) return false;
		res = itaFunc->second(wParam, lParam);
		return true;
	}
};

#pragma region WindowBase

#pragma region Misc
struct TrackMouseEventBox : protected TRACKMOUSEEVENT {
	TrackMouseEventBox(HWND hWnd) : TRACKMOUSEEVENT{ 0 } {
		this->cbSize = sizeof(TRACKMOUSEEVENT);
		this->hwndTrack = hWnd;
	}
public: // Property - HoverTime
	/* W */ inline auto &HoverTime(DWORD dwHoverTime) reflect_to_self(this->dwHoverTime = dwHoverTime);
	/* R */ inline DWORD HoverTime() const reflect_as(this->dwHoverTime);
public: // Property - Flags
	enum_flags(Flag, DWORD,
			  Hover = TME_HOVER,
			  Leave = TME_LEAVE,
			  NoClient = TME_NONCLIENT,
			  Query = TME_QUERY,
			  Cancel = TME_CANCEL);
	/* W */ inline auto &Flags(Flag flag) reflect_to_self(this->dwFlags = flag.yield());
	/* R */ inline Flag  Flags() const reflect_as(reuse_as<Flag>(this->dwFlags));
public:
	inline operator bool() reflect_as(TrackMouseEvent(this));
	inline LPTRACKMOUSEEVENT operator&() reflect_as(this);
	inline const TRACKMOUSEEVENT *operator&() const reflect_as(this);
};
using TME = TrackMouseEventBox;

class PaintStruct : protected PAINTSTRUCT {
	HWND hwnd;
	HDC hddc;
public:
	PaintStruct(HWND hwnd) : hwnd(hwnd) assert(hddc = BeginPaint(hwnd, this));
	~PaintStruct() {
		EndPaint(hwnd, this);
	}
	inline CDC DDC() reflect_as(this->hddc);
	inline CDC DC() reflect_as(this->hdc);
	inline LPPAINTSTRUCT operator&() reflect_as(this);
	inline const PAINTSTRUCT *operator&() const reflect_as(this);
};

class WindowProcedure {
	WNDPROC pfnWndProc = O;
	HWND hWnd = O;
public:
	WindowProcedure() {}
	WindowProcedure(HWND hWnd, WNDPROC pfnWndProc = O) : pfnWndProc(pfnWndProc), hWnd(hWnd) {}
	WindowProcedure(WNDPROC pfnWndProc, HWND hWnd = O) : pfnWndProc(pfnWndProc), hWnd(hWnd) {}
	inline LRESULT Call(UINT Msg, WPARAM wParam, LPARAM lParam) const reflect_as(CallWindowProc(pfnWndProc, hWnd, Msg, wParam, lParam));
	inline LRESULT Call(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) const reflect_as(CallWindowProc(pfnWndProc, hWnd, Msg, wParam, lParam));
	inline auto &operator*=(HWND hWnd) reflect_to_self(this->hWnd = hWnd);
	inline LRESULT operator()(UINT Msg, WPARAM wParam, LPARAM lParam) const reflect_as(Call(Msg, wParam, lParam));
	inline LRESULT operator()(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) const reflect_as(Call(hWnd, Msg, wParam, lParam));
	inline bool operator<=(HWND hWnd) {
		if (auto lpfnWndProc = GetWindowLongPtr(hWnd, GWLP_WNDPROC)) {
			this->hWnd = hWnd;
			this->pfnWndProc = force_cast<WNDPROC>(lpfnWndProc);
			return true;
		}
		return false;
	}
	inline operator bool() const reflect_as(pfnWndProc);
	inline operator WNDPROC() const reflect_as(pfnWndProc);
	inline operator HWND() const reflect_as(hWnd);
public:
#define _SEND_(msgid, wparam, lparam) Call(msgid, wparam, lparam) 
#define MSG_TRANS(msgid, ret, name, argslist, args, send, call) \
	inline ret name argslist reflect_as(send);
#include "msg.inl"
};
using WndProc = WindowProcedure;

enum_class(ShowWindows, int,
	Hide             = SW_HIDE,
	ShowNormal       = SW_SHOWNORMAL,
	Normal           = SW_NORMAL,
	ShowMinimized    = SW_SHOWMINIMIZED,
	ShowMaximized    = SW_SHOWMAXIMIZED,
	Maximize         = SW_MAXIMIZE,
	ShowNoActivate   = SW_SHOWNOACTIVATE,
	Show             = SW_SHOW,
	Minimize         = SW_MINIMIZE,
	ShowMinNoActive  = SW_SHOWMINNOACTIVE,
	ShowNA           = SW_SHOWNA,
	Restore          = SW_RESTORE,
	ShowDefault      = SW_SHOWDEFAULT,
	ForceMinimize    = SW_FORCEMINIMIZE,
	Max              = SW_MAX);
using SW = ShowWindows;
#pragma endregion

#pragma region Class
enum_flags(ClassStyle, UINT,
	VRedraw            = CS_VREDRAW,
	HRedraw            = CS_HREDRAW,
	Redraw             = CS_VREDRAW | CS_HREDRAW,
	DoubleClick        = CS_DBLCLKS,
	OwnDC              = CS_OWNDC,
	ClassDC            = CS_CLASSDC,
	ParentDC           = CS_PARENTDC,
	NoClose            = CS_NOCLOSE,
	SaveBits           = CS_SAVEBITS,
	ByteAlignClient    = CS_BYTEALIGNCLIENT,
	ByteAlignWindow    = CS_BYTEALIGNWINDOW,
	GlobalClass        = CS_GLOBALCLASS,
	IME                = CS_IME,
	DropShadow         = CS_DROPSHADOW);
using CStyle = ClassStyle;
template<class = void>
struct ClassBase;
using Class = ClassBase<>;
template<class AnyChild>
struct ClassBase : public ChainExtend<ClassBase<AnyChild>, AnyChild>,
	protected WNDCLASS {
	using Child = KChain<ClassBase, AnyChild>;
	ClassBase() : WNDCLASS{ 0 } { this->cbClsExtra = sizeof(Child) - sizeof(WNDCLASS); }
	using Style = CStyle;
public: // Property - Styles
	/* W */ inline auto  &Styles(Style style) reflect_to_child(this->style = style.yield());
	/* S */ inline Style &Styles() reflect_as(reuse_as<Style>(this->style));
	/* R */ inline Style  Styles() const reflect_as(reuse_as<Style>(this->style));
public: // Property - WndProc
	/* W */ inline auto       &WndProc(WNDPROC lpfnWndProc) reflect_to_child(this->lpfnWndProc = lpfnWndProc);
	/* R */ inline WX::WndProc WndProc() const reflect_as(this->lpfnWndProc);
public: // Property - WndExtra
	/* W */ inline auto &WndExtra(int cbWndExtra) reflect_to_child(this->cbWndExtra = cbWndExtra);
	/* S */ inline int  &WndExtra() reflect_as(this->cbWndExtra);
	/* R */ inline int   WndExtra() const reflect_as(this->cbWndExtra);
public: // Property - Module
	/* W */ inline auto       &Module(HINSTANCE hModule) reflect_to_child(this->hInstance = hModule);
	/* S */ inline WX::Module &Module() reflect_as(WX::Module::Attach(this->hInstance));
	/* R */ inline CModule     Module() const reflect_as(this->hInstance);
public: // Property - Icon
	/* W */ inline auto     &Icon(LPCTSTR lpszName) reflect_to_child(this->hIcon = Module().Icon(lpszName));
	/* W */ inline auto     &Icon(CIcon icon) reflect_to_child(this->hIcon = icon);
	/* W */ inline auto     &Icon(WORD wID) reflect_to_child(this->hIcon = Module().Icon(wID));
	/* S */ inline WX::Icon &Icon() reflect_as(WX::Icon::Attach(this->hIcon));
	/* R */ inline CIcon     Icon() const reflect_as(this->hIcon);
public: // Property - Cursor
	/* W */ inline auto       &Cursor(LPCTSTR lpszName) reflect_to_child(this->hCursor = Module().Cursor(lpszName));
	/* W */ inline auto       &Cursor(HCURSOR hCursor) reflect_to_child(this->hCursor = hCursor);
	/* W */ inline auto       &Cursor(WORD wID) reflect_to_child(this->hCursor = Module().Cursor(wID));
	/* S */ inline WX::Cursor &Cursor() reflect_as(WX::Cursor::Attach(this->hCursor));
	/* R */ inline CCursor     Cursor() const reflect_as(this->hCursor);
public: // Property - Background
	/* W */ inline auto  &Background(HBRUSH hbrBackground) reflect_to_child(this->hbrBackground = hbrBackground);
	/* W */ inline auto  &Background(SysColor sc) reflect_to_child(this->hbrBackground = force_cast<HBRUSH>(sc.yield()));
	/* S */ inline Brush &Background() reflect_as(Brush::Attach(this->hbrBackground));
	/* R */ inline CBrush Background() const reflect_as(this->hbrBackground);
public: // Property - Menu
	/* W */ inline auto    &Menu(LPCTSTR lpszMenuName) reflect_to_child(this->lpszMenuName = lpszMenuName);
	/* W */ inline auto    &Menu(WORD wID) reflect_to_child(this->lpszMenuName = MAKEINTRESOURCE(wID));
	/* S */ inline LPCTSTR &MenuName() reflect_as(this->lpszMenuName);
	/* R */ inline LPCTSTR  MenuName() const reflect_as(this->lpszMenuName);
	/* R */ inline CMenu    Menu() const reflect_as(Module().Menu(this->lpszMenuName));
public: // Property - Name
	/* W */ inline auto        &Name(LPCTSTR lpszClassName) reflect_to_child(this->lpszClassName = lpszClassName);
	/* S */ inline LPCTSTR     &Name() reflect_as(this->lpszClassName);
	/* R */ inline const String Name() const reflect_as(CString(this->lpszClassName, MaxLenClass));
public:
	inline ATOM Register()                 const reflect_as(RegisterClass(this));
	inline bool Unregister(ATOM classAtom) const reflect_as(UnregisterClass(MAKEINTRESOURCE(classAtom), hInstance));
	inline bool GetInfo()                        reflect_as(GetClassInfo(hInstance, lpszClassName, this));
	inline bool GetInfo(ATOM classAtom)          reflect_as(GetClassInfo(hInstance, MAKEINTRESOURCE(classAtom), this));
	inline LPWNDCLASS      operator&()           reflect_as(this);
	inline const WNDCLASS *operator&()     const reflect_as(this);
};
#pragma region ClassEx
template<class = void>
struct ClassExBase;
using ClassEx = ClassExBase<>;
template<class AnyChild>
struct ClassExBase : public ChainExtend<ClassExBase<AnyChild>, AnyChild>,
	protected WNDCLASSEX {
	using Child = KChain<ClassExBase, AnyChild>;
	ClassExBase() : WNDCLASSEX{ 0 } {
		this->cbSize = sizeof(WNDCLASSEX);
		this->cbClsExtra = sizeof(Child) - sizeof(WNDCLASSEX);
	}
public: // Property - Styles
	/* W */ inline auto   &Styles(CStyle style) reflect_to_child(this->style = style.yield());
	/* S */ inline CStyle &Styles() reflect_as(reuse_as<CStyle>(this->style));
	/* R */ inline CStyle  Styles() const reflect_as(reuse_as<CStyle>(this->style));
public: // Property - WndProc
	/* W */ inline auto       &WndProc(WNDPROC lpfnWndProc) reflect_to_child(this->lpfnWndProc = lpfnWndProc);
	/* R */ inline WX::WndProc WndProc() const reflect_as(this->lpfnWndProc);
public: // Property - WndExtra
	/* W */ inline auto &WndExtra(int cbWndExtra) reflect_to_child(this->cbWndExtra = cbWndExtra);
	/* S */ inline int  &WndExtra() reflect_as(this->cbWndExtra);
	/* R */ inline int   WndExtra() const reflect_as(this->cbWndExtra);
public: // Property - Module
	/* W */ inline auto       &Module(HINSTANCE hModule) reflect_to_child(this->hInstance = hModule);
	/* S */ inline WX::Module &Module() reflect_as(WX::Module::Attach(this->hInstance));
	/* R */ inline CModule     Module() const reflect_as(this->hInstance);
public: // Property - Icon
	/* W */ inline auto     &Icon(LPCTSTR lpszName) reflect_to_child(this->hIcon = Module().Icon(lpszName));
	/* W */ inline auto     &Icon(CIcon icon) reflect_to_child(this->hIcon = icon);
	/* W */ inline auto     &Icon(WORD wID) reflect_to_child(this->hIcon = Module().Icon(wID));
	/* S */ inline WX::Icon &Icon() reflect_as(WX::Icon::Attach(this->hIcon));
	/* R */ inline CIcon     Icon() const reflect_as(this->hIcon);
public: // Property - Cursor
	/* W */ inline auto       &Cursor(LPCTSTR lpszName) reflect_to_child(this->hCursor = Module().Cursor(lpszName));
	/* W */ inline auto       &Cursor(HCURSOR hCursor) reflect_to_child(this->hCursor = hCursor);
	/* W */ inline auto       &Cursor(WORD wID) reflect_to_child(this->hCursor = Module().Cursor(wID));
	/* S */ inline WX::Cursor &Cursor() reflect_as(WX::Cursor::Attach(this->hCursor));
	/* R */ inline CCursor     Cursor() const reflect_as(this->hCursor);
public: // Property - Background
	/* W */ inline auto  &Background(HBRUSH hbrBackground) reflect_to_child(this->hbrBackground = hbrBackground);
	/* W */ inline auto  &Background(SysColor sc) reflect_to_child(this->hbrBackground = force_cast<HBRUSH>(sc.yield()));
	/* S */ inline Brush &Background() reflect_as(Brush::Attach(this->hbrBackground));
	/* R */ inline CBrush Background() const reflect_as(this->hbrBackground);
public: // Property - Menu
	/* W */ inline auto    &Menu(LPCTSTR lpszMenuName) reflect_to_child(this->lpszMenuName = lpszMenuName);
	/* W */ inline auto    &Menu(WORD wID) reflect_to_child(this->lpszMenuName = MAKEINTRESOURCE(wID));
	/* S */ inline LPCTSTR &MenuName() reflect_as(this->lpszMenuName);
	/* R */ inline LPCTSTR  MenuName() const reflect_as(this->lpszMenuName);
	/* R */ inline CMenu    Menu() const reflect_as(Module().Menu(this->lpszMenuName));
public: // Property - Name
	/* W */ inline auto        &Name(LPCTSTR lpszClassName) reflect_to_child(this->lpszClassName = lpszClassName);
	/* S */ inline LPCTSTR     &Name() reflect_as(this->lpszClassName);
	/* R */ inline const String Name() const reflect_as(CString(this->lpszClassName, MaxLenClass));
public: // Property - IconSm
	/* W */ inline auto     &IconSm(LPCTSTR lpszName) reflect_to_child(this->hIcon = Module().Icon(lpszName));
	/* W */ inline auto     &IconSm(CIcon iconSm) reflect_to_child(this->hIconSm = iconSm);
	/* W */ inline auto     &IconSm(WORD wID) reflect_to_child(this->hIconSm = Module().Icon(wID));
	/* S */ inline WX::Icon &IconSm() reflect_as(WX::Icon::Attach(this->hIconSm));
	/* R */ inline CIcon     IconSm() const reflect_as(this->hIconSm);
public:
	inline ATOM Register()                 const reflect_as(RegisterClassEx(this));
	inline bool Unregister(ATOM classAtom) const reflect_as(UnregisterClass(MAKEINTRESOURCE(classAtom), hInstance));
	inline bool GetInfo()                        reflect_as(GetClassInfoEx(hInstance, lpszClassName, this));
	inline bool GetInfo(ATOM classAtom)          reflect_as(GetClassInfoEx(hInstance, MAKEINTRESOURCE(classAtom), this));
	inline WNDCLASSEX        operator&()         reflect_as(this);
	inline const WNDCLASSEX *operator&()   const reflect_as(this);
};
#pragma endregion
#pragma endregion

#pragma region Styles
enum_flags(WindowStyle, LONG,
	Overlapped          = WS_OVERLAPPED,
	Popup               = WS_POPUP,
	Child               = WS_CHILD,
	Minimize            = WS_MINIMIZE,
	Visible             = WS_VISIBLE,
	Disabled            = WS_DISABLED,
	Clipsiblings        = WS_CLIPSIBLINGS,
	ClipChildren        = WS_CLIPCHILDREN,
	Maximize            = WS_MAXIMIZE,
	Caption             = WS_CAPTION,
	Border              = WS_BORDER,
	DlgFrame            = WS_DLGFRAME,
	VScroll             = WS_VSCROLL,
	HScroll             = WS_HSCROLL,
	SysMenu             = WS_SYSMENU,
	ThickFrame          = WS_THICKFRAME,
	Group               = WS_GROUP,
	TabStop             = WS_TABSTOP,
	MinimizeBox         = WS_MINIMIZEBOX,
	MaximizeBox         = WS_MAXIMIZEBOX,
	Tiled               = WS_TILED,
	Iconic              = WS_ICONIC,
	SizeBox             = WS_SIZEBOX,
	TiledWindow         = WS_TILEDWINDOW,
	PopupWindow         = WS_POPUPWINDOW,
	OverlappedWindow    = WS_OVERLAPPEDWINDOW);
using WS = WindowStyle;
using WStyle = WindowStyle;
enum_flags(WindowStyleEx, DWORD,
	DlgModalFrame       = WS_EX_DLGMODALFRAME,
	NoParentNotify      = WS_EX_NOPARENTNOTIFY,
	TopMost             = WS_EX_TOPMOST,
	AcceptFiles         = WS_EX_ACCEPTFILES,
	Transparent         = WS_EX_TRANSPARENT,
	MdiChild            = WS_EX_MDICHILD,
	ToolWindow          = WS_EX_TOOLWINDOW,
	WindowEdge          = WS_EX_WINDOWEDGE,
	ClientEdge          = WS_EX_CLIENTEDGE,
	ContextHelp         = WS_EX_CONTEXTHELP,
	Right               = WS_EX_RIGHT,
	Left                = WS_EX_LEFT,
	ReadingR2L          = WS_EX_RTLREADING,
	ReadingL2R          = WS_EX_LTRREADING,
	LeftScrollBar       = WS_EX_LEFTSCROLLBAR,
	RightScrollBar      = WS_EX_RIGHTSCROLLBAR,
	ControlParent       = WS_EX_CONTROLPARENT,
	StaticEdge          = WS_EX_STATICEDGE,
	AppWindow           = WS_EX_APPWINDOW,
	Layered             = WS_EX_LAYERED,
	NoInheritLayout     = WS_EX_NOINHERITLAYOUT,
	NoRedirectionBitmap = WS_EX_NOREDIRECTIONBITMAP,
	LayoutR2L           = WS_EX_LAYOUTRTL,
	Composited          = WS_EX_COMPOSITED,
	NoActivate          = WS_EX_NOACTIVATE,
	OverlappedWindow    = WS_EX_OVERLAPPEDWINDOW,
	PaletteWindow       = WS_EX_PALETTEWINDOW);
using WSEX = WindowStyleEx;
using WStyleEx = WindowStyleEx;
#pragma endregion

template<class AnyChild>
class WindowBase;
using Window = WindowBase<void>;
template<class AnyChild, class Style = WStyle, class StyleEx = WStyleEx>
struct CreateStruct : public ChainExtend<CreateStruct<AnyChild, Style, StyleEx>, AnyChild>,
	protected CREATESTRUCT {
	CreateStruct() : CREATESTRUCT{ 0 } {}
	CreateStruct(const CREATESTRUCT *lpCreate) : CREATESTRUCT(*lpCreate) {}
public: // Property - Param
	/* W */ inline auto &Param(LPVOID lpCreateParams) reflect_to_child(this->lpCreateParams = lpCreateParams);
	template<class AnyType = void>
	/* R */ inline AnyType *Param() reflect_as((AnyType *)this->lpCreateParams);
public: // Property - Module
	/* W */ inline auto       &Module(HINSTANCE hInstance) reflect_to_child(this->hInstance = hInstance);
	/* S */ inline WX::Module &Module() reflect_as(Module::Attach(this->hInstance));
	/* R */ inline CModule     Module() const reflect_as(this->hInstance);
public: // Property - Menu
	/* W */ inline auto     &Menu(HMENU hMenu) reflect_to_child(this->hMenu = hMenu);
	/* S */ inline WX::Menu &Menu() reflect_as(Menu::Attach(this->hMenu));
	/* R */ inline CMenu     Menu() const reflect_as(this->hMenu);
public: // Property - Parent
	/* W */ inline auto &Parent(HWND hwndParent) reflect_to_child(this->hwndParent = hwndParent);
	/* S */ Window      &Parent();
	/* R */ Window       Parent() const;
public: // Property - Styles
	/* W */ inline auto  &Styles(Style style) reflect_to_child(this->style = style.yield());
	/* S */ inline Style &Styles() reflect_as(reuse_as<Style>(this->style));
	/* R */ inline Style  Styles() const reflect_as(reuse_as<Style>(this->style));
public: // Property - Caption
	/* W */ inline auto        &Caption(LPCTSTR name) reflect_to_child(this->lpszName = name);
	/* S */ inline LPCTSTR     &Caption() reflect_as(this->lpszName);
	/* R */ inline const String Caption() const reflect_as(CString(this->lpszName, MaxLenTitle));
public: // Property - Class
	/* W */ inline auto        &Class(LPCTSTR name) reflect_to_child(this->lpszClass = name);
	/* W */ inline auto        &Class(ATOM wClassAtom) reflect_to_child(this->lpszClass = MAKEINTATOM(wClassAtom));
	/* S */ inline LPCTSTR     &Class() reflect_as(this->lpszClass);
	/* R */ inline const String Class() const reflect_as(CString(this->lpszClass, MaxLenClass));
public: // Property - StylesEx
	/* W */ inline auto    &StylesEx(StyleEx dwExStyle) reflect_to_child(this->dwExStyle = dwExStyle.yield());
	/* S */ inline StyleEx &StylesEx() reflect_as(reuse_as<StyleEx>(this->dwExStyle));
	/* R */ inline StyleEx  StylesEx() const reflect_as(reuse_as<StyleEx>(this->dwExStyle));
public: // Property - Size
	/* W */ inline auto &Size(LSize size) reflect_to_child(this->cx = size.cx, this->cy = size.cy);
	/* R */ inline LSize Size() const reflect_as({ this->cx, this->cy });
public: // Property - Position
	/* W */ inline auto  &Position(LPoint pos) reflect_to_child(this->x = pos.x, this->y = pos.y);
	/* R */ inline LPoint Position() const reflect_as({ this->x, this->y });
public: // Property - Rect
	/* W */ inline auto &Rect(LRect rc) reflect_to_child(auto &&sz = rc.size(); auto &&pt = rc.bottom_left(); this->x = pt.x, this->y = pt.y, this->cx = sz.cx, this->cy = sz.cy);
	/* R */ inline LRect Rect() const reflect_as({ Position(), Size() });
public: // Property - ClientRect
	/* W */ inline auto &ClientRect(LRect rc) assert_reflect_as(AdjustWindowRectEx(rc, this->style, this->hMenu, this->dwExStyle), this->Rect(rc));
	/* W */ inline auto &ClientRect(LRect rc, UINT dpi) assert_reflect_as(AdjustWindowRectExForDpi(rc, this->style, this->hMenu, this->dwExStyle, dpi), this->Rect(rc));
public: // Property - ClientSize
	/* W */ inline auto &ClientSize(LSize sz) reflect_as(ClientRect({ Position(), sz }));
	/* W */ inline auto &ClientSize(LSize sz, UINT dpi) reflect_as(ClientRect({ Position(), sz }, dpi));
public:
	inline HWND Create() const assert_reflect_as(auto h = CreateWindowEx(dwExStyle, lpszClass, lpszName, style, x, y, cx, cy, hwndParent, hMenu, hInstance, lpCreateParams), h);
	inline LPCREATESTRUCT      operator&()       reflect_as(this);
	inline const CREATESTRUCT *operator&() const reflect_as(this);
};

template<class AnyChild>
class WindowBase : public ChainExtend<WindowBase<AnyChild>, AnyChild> {
	HWND hWnd = O;
public:
	using super = WindowBase;
	using Child = KChain<WindowBase, AnyChild>;

	WindowBase() {}
	WindowBase(HWND hWnd) : hWnd(hWnd) {}
	~WindowBase() reflect_to(Destroy());

	using Style = WStyle;
	using StyleEx = WStyleEx;

#pragma region Classes
protected:
	static ATOM _ClassAtom;
	static HINSTANCE _hClassModule;
	static const String &&_ClassName;
protected:
	template<class = void>
	struct ClassBase;
	using  Class = ClassBase<>;
	template<class _AnyChild>
	struct ClassBase :
		public ChainExtend<ClassBase<_AnyChild>, _AnyChild>,
		public WX::ClassBase<KChain<ClassBase<_AnyChild>, _AnyChild>> {
		using Child = KChain<ClassBase<_AnyChild>, _AnyChild>;
		using super = WX::ClassBase<Child>;
		ClassBase() {
			super::WndProc(MainProc<0>);
			super::WndExtra(sizeof(Child));
			super::Name(_ClassName);
		}
	public: // Property - WndExtra (deleted)
		/* W */ inline auto &WndExtra(int) = delete;
	public: // Property - Name (deleted)
		/* W */ inline auto &Name(LPCTSTR) = delete;
	public: // Property - WndProc (deleted)
		/* W */ inline auto &WndProc(WNDPROC) = delete;
	};
protected:
	template<class = void>
	struct ClassExBase;
	using  ClassEx = ClassExBase<>;
	template<class _AnyChild>
	struct ClassExBase :
		public ChainExtend<ClassExBase<_AnyChild>, _AnyChild>,
		public WX::ClassExBase<KChain<ClassExBase<_AnyChild>, _AnyChild>> {
		using Child = KChain<ClassExBase, _AnyChild>;
		using super = WX::ClassExBase<Child>;
		ClassExBase() {
			super::WndProc(MainProc<0>);
			super::WndExtra(sizeof(Child));
			super::Name(_ClassName);
		}
	public: // Property - WndExtra (deleted)
		/* W */ inline auto &WndExtra(int) = delete;
	public: // Property - Name (deleted)
		/* W */ inline auto &Name(LPCTSTR) = delete;
	public: // Property - WndProc (deleted)
		/* W */ inline auto &WndProc(WNDPROC) = delete;
	};
protected:
	subtype_branch(xClass);
	inline void Register() {
		if (_ClassAtom) return;
		subtype_branchof_xClass<Child, ClassEx> cs;
		if constexpr (std::is_same_v<decltype(cs), ClassEx>)
			cs
			.Cursor(IDC_ARROW)
			.Styles(Class::Style::Redraw)
			.Background(SysColor::WindowFrame);
		assert(_ClassAtom = cs.Register());
	}
	inline void Unregister() {
		if (_ClassAtom)
			assert(UnregisterClass(MAKEINTRESOURCE(_ClassAtom), _hClassModule));
		_ClassAtom = 0;
	}
#pragma endregion

#pragma region Creature
protected:
	subtype_branch(xCreate);
public:
	using CreateStruct = WX::CreateStruct<void, Style, StyleEx>;
	template<class _AnyChild = void, class Style = WStyle, class StyleEx = WStyleEx>
	class XCreate :
		public WX::CreateStruct<KChain<XCreate<_AnyChild, Style, StyleEx>, _AnyChild>, Style, StyleEx> {
		WindowBase &target;
	public:
		using Child = KChain<XCreate, _AnyChild>;
		using super = WX::CreateStruct<Child, Style, StyleEx>;
		XCreate(WindowBase &win) : target(win) reflect_to(super::Class(_ClassAtom));
	public: // Property - Class (delete)
		/* W */ inline auto &Class(String) = delete;
		/* W */ inline auto &Class(ATOM) = delete;
		/* R */ inline const String Class() const = delete;
	public:
		inline operator bool() reflect_as(target ? false : bool(target.hWnd = super::Create()));
	};
	inline auto Create() {
		Register();
		return subtype_branchof_xCreate<Child, XCreate<>>(self).Param(this);
	}
	inline bool Destroy() {
		if (self)
			if (!DestroyWindow(self))
				return false;
		hWnd = O;
		return true;
	}
	inline bool Close() {
		if (self)
			if (!CloseWindow(self))
				return false;
		hWnd = O;
		return true;
	}
#pragma endregion

#pragma region Messages Procedure
protected:
	def_memberof(DefProc);
	def_memberof(Callback);
#define MSG_TRANS(msgid, ret, name, ...) \
	def_memberof(name);
#include "msg.inl"
	static LRESULT CallDefProc(HWND hWnd, UINT msgid, WPARAM wParam, LPARAM lParam) {
		if constexpr (member_DefProc_of<Child>::existed) {
			misdef_assert((!std::is_member_pointer_v<decltype(&Child::DefProc)>),
						  "DefProc must be a static method.");
			using tfn_WndProc = LRESULT(HWND, UINT, WPARAM, LPARAM);
			misdef_assert((member_DefProc_of<Child>::template compatible_to<tfn_WndProc>),
						  "DefProc must be compatible to LRESULT(HWND, UINT, WPARAM, LPARAM)");
			return Child::DefProc(hWnd, msgid, wParam, lParam);
		}
		else
			return DefWindowProc(hWnd, msgid, wParam, lParam);
	}
	inline LRESULT HandleNext() const { throw MSG{ 0 }; }
	template<int index = 0>
	static LRESULT CALLBACK MainProc(HWND hWnd, UINT msgid, WPARAM wParam, LPARAM lParam) {
		auto &Wnd = WindowBase::Attach(hWnd);
		auto pThis = (Child *)Wnd.HeapPtr(index);
		switch (msgid) {
			case WM_CREATE: {
				auto lpCreate = (LPCREATESTRUCT)lParam;
				if ((pThis = (Child *)lpCreate->lpCreateParams))
					if (Wnd.HeapPtr(pThis, index)) {
						(HWND &)*force_cast<Window *>(pThis) = hWnd;
						break;
					}
				return -1;
			}
			case WM_COMMAND:
				assert(pThis);
				if (auto wnd = force_cast<Window>(lParam))
					wnd.Send(WM_USER + (UINT)HIWORD(wParam));
				break;
		}
		if (pThis)
			try {
			switch (msgid) {
				case WM_NULL:
					break;
#define _CALL_(name) pThis->name
#define MSG_TRANS(msgid, ret, name, argslist, args, send, call) \
					case msgid: \
						if constexpr (member_##name##_of<Child>::existed) { \
							using fn_type = ret argslist; \
							misdef_assert((member_##name##_of<Child>::template compatible_to<fn_type>), \
										  "Member " #name " must be a method compatible to " #ret #argslist); \
							return call; \
						} break;
#include "msg.inl"
			}
			if constexpr (member_Callback_of<Child>::existed)
				return pThis->Callback(msgid, wParam, lParam);
		} catch (MSG msg) {
			if (msg.message)
				return CallDefProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
		} 
		return CallDefProc(hWnd, msgid, wParam, lParam);
	}
public:
	inline auto Call() {
		class Pack {
			HWND hWnd;
		public:
			Pack(HWND hWnd) : hWnd(hWnd) {}
#define _SEND_(msgid, wparam, lparam) CallDefProc(hWnd, msgid, wparam, lparam) 
#define MSG_TRANS(msgid, ret, name, argslist, args, send, call) \
	inline ret name argslist reflect_as(send);
#include "msg.inl"
		} p = (HWND)self;
		return p;
	}
#pragma endregion

#pragma region Message System
public:
	template<class AnyType = LRESULT, class WParam = WPARAM, class LParam = LPARAM>
	inline AnyType Send(UINT msg, WParam wParam = 0, LParam lParam = 0) const check_reflect_to(auto res = force_cast<AnyType>(SendMessage(self, msg, (WPARAM)wParam, (LPARAM)lParam)), res);
public:
	inline auto Send() {
		class Pack {
			Window win;
		public:
			Pack(HWND hwnd) : win(hwnd) {}
#define _SEND_(msgid, wparam, lparam) win.Send(msgid, wparam, lparam) 
#define MSG_TRANS(msgid, ret, name, argslist, args, send, call) \
	inline ret name argslist reflect_as(send);
#include "msg.inl"
		} p = (HWND)self;
		return p;
	}
#pragma endregion

#pragma region Methods
	inline auto &Show(SW nShow)     reflect_to_child(ShowWindow(self, nShow.yield()));
	inline auto &Hide()             reflect_as(Show(SW::Hide));
	inline auto &ShowNormal()       reflect_as(Show(SW::ShowNormal));
	inline auto &Normal()           reflect_as(Show(SW::Normal));
	inline auto &ShowMinimized()    reflect_as(Show(SW::ShowMinimized));
	inline auto &ShowMaximized()    reflect_as(Show(SW::ShowMaximized));
	inline auto &Maximize()         reflect_as(Show(SW::Maximize));
	inline auto &ShowNoActivate()   reflect_as(Show(SW::ShowNoActivate));
	inline auto &Show()             reflect_as(Show(SW::Show));
	inline auto &Minimize()         reflect_as(Show(SW::Minimize));
	inline auto &ShowMinNoActive()  reflect_as(Show(SW::ShowMinNoActive));
	inline auto &ShowNA()           reflect_as(Show(SW::ShowNA));
	inline auto &Restore()          reflect_as(Show(SW::Restore));
	inline auto &ShowDefault()      reflect_as(Show(SW::ShowDefault));
	inline auto &ForceMinimize()    reflect_as(Show(SW::ForceMinimize));
	inline auto &Max()              reflect_as(Show(SW::Max));

	inline MB MsgBox(LPCTSTR lpCaption = S(""), LPCTSTR lpText = S(""), MB uType = MB::Ok) const reflect_as(WX::MsgBox(lpCaption, lpText, uType, self));

	inline auto &SetFocus() check_reflect_to_child(SetFocus(self));
	inline bool IsFocus() const reflect_as(GetFocus() == self);

	inline static Window Find(LPCTSTR lpszClass, LPCTSTR lpszName) reflect_as(FindWindow(lpszClass, lpszName));
	inline static WindowBase FindByAtom(LPCTSTR lpszName) reflect_as(FindWindow(MAKEINTRESOURCE(_ClassAtom), lpszName));
	inline static WindowBase FindByClass(LPCTSTR lpszName) reflect_as(FindWindow(_ClassName, lpszName));

	inline LPoint ScreenToClient(LPoint pt) const assert_reflect_as(::ScreenToClient(self, &pt), pt);
	inline LPoint ClientToScreen(LPoint pt) const assert_reflect_as(::ClientToScreen(self, &pt), pt);
	inline LRect AdjustRect(LRect rc) const assert_reflect_as(::AdjustWindowRect(&rc, Styles().yield(), this->Menu()), rc);
	inline LRect AdjustRect() const reflect_to(LRect rc, AdjustRect(rc), rc);

	inline void Move(LRect rc, bool bRedraw = true) const assert_reflect_as(::MoveWindow(self, rc.top, rc.left, rc.width(), rc.height(), bRedraw));

	inline auto &Update() assert_reflect_as_child(UpdateWindow(self));
	inline CDC DC() const reflect_as(GetDC(self));

	inline auto &RegisterTouch(bool bFine = true, bool bWantPalm = false) assert_reflect_as_child(RegisterTouchWindow(self, (bFine *TWF_FINETOUCH) | (bWantPalm * TWF_WANTPALM)));
	inline auto &UnregisterTouch() assert_reflect_as_child(UnregisterTouchWindow(self));

	inline TrackMouseEventBox TrackMouse() const reflect_as((HWND)self);

	inline PaintStruct BeginPaint() reflect_as((HWND)self);
#pragma endregion

#pragma region Properties
public: // Property - Iconic
	/* R */ inline bool Iconic() const reflect_as(IsIconic(self));
public: // Property - Visible
	/* W */ inline auto &Visible(bool bVisible) assert_reflect_as_child(ShowWindow(self, SW_SHOW));
	/* R */ inline bool Visible() const reflect_as(IsWindowVisible(self));
public: // Proterty - Enabled
	/* W */ inline auto &Enabled(bool bEnable) assert_reflect_as_child(EnableWindow(self, bEnable));
	/* R */ inline bool Enabled() const reflect_as(IsWindowEnabled(self));
public: // Property - TextLength
	/* R */ inline int TextLength() const reflect_as(GetWindowTextLength(self));
public: // Property - Text
	/* W */ inline auto &Text(LPCTSTR lpString) assert_reflect_as_child(SetWindowText(self, lpString) >= 0);
	/* R */ inline String Text() const {
		auto len = TextLength();
		if (len <= 0) return O;
		auto lpszName = String::Alloc(len);
		assert(len == GetWindowText(self, lpszName, len + 1));
		return{ (size_t)len, lpszName };
	}
public: // Property - Parent
	/* W */ inline auto  &Parent(HWND hParent) check_reflect_to_child(SetParent(self, hParent));
	/* R */ inline Window Parent() const reflect_as(GetParent(self));
public: // Property - Menu
	/* W */ inline auto &Menu(HMENU hMenu) assert_reflect_as_child(SetMenu(self, hMenu));
	/* R */ inline CMenu Menu() const reflect_as(GetMenu(self));
public: // Property - ClientRect
	/* R */ inline LRect ClientRect() const assert_reflect_to(LRect rc, GetClientRect(self, &rc), rc);
public: // Property - ClientSize
	/* R */ inline LSize ClientSize() const assert_reflect_to(LRect rc, GetClientRect(self, &rc), rc.bottom_right());
public: // Property - Position
	/* W */ inline auto  &Position(LPoint ptPosition) assert_reflect_as_child(SetWindowPos(self, O, ptPosition.x, ptPosition.y, 0, 0, SWP_NOSIZE));
	/* R */ inline LPoint Position() const assert_reflect_to(LRect rc, GetWindowRect(self, &rc), rc.top_left());
public: // Property - Size
	/* W */ inline auto &Size(LSize szWin) assert_reflect_as_child(SetWindowPos(self, O, 0, 0, szWin.cx, szWin.cy, SWP_NOMOVE));
	/* R */ inline LSize Size() const assert_reflect_to(LRect rc, GetWindowRect(self, &rc), rc.size());
public: // Property - Rect
	/* W */ inline auto &Rect(LRect rWin) assert_reflect_to_child(LSize sz = rWin.size(), SetWindowPos(self, O, rWin.top, rWin.left, sz.cx, sz.cy, SWP_NOZORDER));
	/* R */ inline LRect Rect() const assert_reflect_to(LRect rWin, GetWindowRect(self, rWin), rWin);
public: // Property - IconBig
	/* W */ inline auto &IconBig(HICON hIcon) reflect_to_child(Send<HICON>(WM_SETICON, ICON_BIG, hIcon));
	/* R */ inline CIcon IconBig() const reflect_as(Send<HICON>(WM_GETICON, ICON_BIG));
public: // Property - IconSmall
	/* W */ inline auto &IconSmall(HICON hIcon) reflect_to_child(Send<HICON>(WM_SETICON, ICON_SMALL, hIcon));
	/* R */ inline CIcon IconSmall() const reflect_as(Send<HICON>(WM_GETICON, ICON_SMALL));
public: // Property - Styles
	/* W */ inline auto &Styles(Style style) check_reflect_to_child(SetWindowLongPtr(self, GWL_STYLE, style.yield()));
	/* R */ inline Style Styles() const reflect_as(force_cast<Style>(GetWindowLongPtr(self, GWL_STYLE)));
public: // Property - StylesEx 
	/* W */ inline auto   &StylesEx(StyleEx styleEx) check_reflect_to_child(SetWindowLongPtr(self, GWL_EXSTYLE, styleEx.yield()));
	/* R */ inline StyleEx StylesEx() const reflect_as(force_cast<StyleEx>(GetWindowLongPtr(self, GWL_EXSTYLE)));
public: // Property - ID
	/* W */ inline auto    &ID(LONG_PTR uId) check_reflect_to_child(SetWindowLongPtr(self, GWLP_ID, uId));
	/* R */ inline LONG_PTR ID() const reflect_as(force_cast<LONG_PTR>(GetWindowLongPtr(self, GWLP_ID)));
public: // Property - Module
	/* W */ inline auto   &Module(HINSTANCE hInstance) check_reflect_to_child(SetWindowLongPtr(self, GWLP_HINSTANCE, force_cast<LONG_PTR>(hInstance)));
	/* R */ inline CModule Module() const reflect_as(force_cast<HINSTANCE>(GetWindowLongPtr(self, GWLP_HINSTANCE)));
public: // Property - UserData
	/* W */ inline auto &UserData(void *pData) check_reflect_to_child(SetWindowLongPtr(self, GWLP_USERDATA, force_cast<LONG_PTR>(pData)));
	/* R */ inline void *UserData() const reflect_as(force_cast<void *>(GetWindowLongPtr(self, GWLP_USERDATA)));
public: // Property - Long 0
	/* W */ inline auto &HeapPtr(void *lpHeap, int nIndex) check_reflect_to_child(SetWindowLongPtr(self, nIndex, (LONG_PTR)lpHeap));
	template<class AnyType = void>
	/* R */ inline AnyType *HeapPtr(int nIndex) const reflect_as(force_cast<AnyType *>(GetWindowLongPtr(self, nIndex)));
public: // Property - ClassName
	/* R */ inline String ClassName() const {
		auto lpszClassName = String::Alloc(MaxLenClass);
		int len = GetClassName(self, lpszClassName, MaxLenClass);
		if (len <= 0) return O;
		lpszClassName = String::Realloc(len, lpszClassName);
		return { (size_t)len, lpszClassName };
	}
public: // Property - ClassMenuName
	/* W */ inline auto   &ClassMenuName(UINT uID) check_reflect_to_child(SetClassLongPtr(self, GCLP_MENUNAME, (LONG_PTR)MAKEINTRESOURCE(uID)));
	/* W */ inline auto   &ClassMenuName(LPCTSTR lpMenuName) check_reflect_to_child(SetClassLongPtr(self, GCLP_MENUNAME, (LONG_PTR)lpMenuName));
	/* R */ inline LPCTSTR ClassMenuName() const reflect_as(force_cast<LPCTSTR>(GetClassLongPtr(self, GCLP_MENUNAME)));
public: // Property - ClassBackground
	/* W */ inline auto  &ClassBackground(HBRUSH hBrush) check_reflect_to_child(SetClassLongPtr(self, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush));
	/* R */ inline CBrush ClassBackground() const reflect_as(force_cast<HBRUSH>(GetClassLongPtr(self, GCLP_HBRBACKGROUND)));
public: // Property - ClassCursor
	/* W */ inline auto   &ClassCursor(HCURSOR hCursor) check_reflect_to_child(SetClassLongPtr(self, GCLP_HCURSOR, (LONG_PTR)hCursor));
	/* R */ inline CCursor ClassCursor() const reflect_as(force_cast<HCURSOR>(GetClassLongPtr(self, GCLP_HCURSOR)));
public: // Property - ClassIcon
	/* W */ inline auto &ClassIcon(HICON hIcon) check_reflect_to_child(SetClassLongPtr(self, GCLP_HICON, (LONG_PTR)hIcon));
	/* R */ inline CIcon ClassIcon() const reflect_as(force_cast<HICON>(GetClassLongPtr(self, GCLP_HICON)));
public: // Property - ClassModule
	/* W */ inline auto   &ClassModule(HINSTANCE hModule) check_reflect_to_child(SetClassLongPtr(self, GCLP_HMODULE, (LONG_PTR)hModule));
	/* R */ inline CModule ClassModule() const reflect_as(force_cast<HINSTANCE>(GetClassLongPtr(self, GCLP_HMODULE)));
public: // Property - ClassWndExtra
	/* W */ inline auto     &ClassWndExtra(ULONG_PTR hModule) check_reflect_to_child(SetClassLongPtr(self, GCL_CBWNDEXTRA, (LONG_PTR)hModule));
	/* R */ inline ULONG_PTR ClassWndExtra() const reflect_as(GetClassLongPtr(self, GCL_CBWNDEXTRA));
public: // Property - ClassClsExtra
	/* W */ inline auto     &ClassClsExtra(ULONG_PTR hModule) check_reflect_to_child(SetClassLongPtr(self, GCL_CBCLSEXTRA, (LONG_PTR)hModule));
	/* R */ inline ULONG_PTR ClassClsExtra() const reflect_as(GetClassLongPtr(self, GCL_CBCLSEXTRA));
public: // Property - ClassWndProc
	/* W */ inline auto   &ClassWndProc(WNDPROC WndProc) check_reflect_to_child(SetClassLongPtr(self, GCLP_WNDPROC, (LONG_PTR)WndProc));
	/* R */ inline WndProc ClassWndProc() const reflect_as({ force_cast<WNDPROC>(GetClassLongPtr(self, GCLP_WNDPROC)), self });
public: // Property - ClassIconSm
	/* W */ inline auto &ClassIconSm(HICON hIcon) check_reflect_to_child(SetClassLongPtr(self, GCLP_HICONSM, (LONG_PTR)hIcon));
	/* R */ inline CIcon ClassIconSm() const reflect_as(force_cast<HICON>(GetClassLongPtr(self, GCLP_HICONSM)));
public: // Property - ClassAtom
	/* W */ inline auto &ClassAtom(ATOM atom) check_reflect_to_child(SetClassWord(self, GCW_ATOM, atom));
	/* R */ inline ATOM  ClassAtom() const reflect_as(GetClassWord(self, GCW_ATOM));
#pragma endregion

	inline void *operator new(size_t uSize, void *ptr) reflect_as(ptr);
	inline void *operator new(size_t uSize) assert_reflect_as(auto lpHeap = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Child)), lpHeap);
	inline void operator delete(void *ptr) assert(HeapFree(GetProcessHeap(), 0, ptr));

	inline static Window &Attach(HWND &hWnd) reflect_as(reuse_as<Window>(hWnd));

	inline operator bool() const reflect_as(IsWindow(self));
	inline operator HWND() const reflect_as(hWnd);

	inline AnyChild *operator*() reflect_as(HeapPtr());
	inline const AnyChild *operator*() const reflect_as(HeapPtr());
	inline operator Window &() reflect_as(reuse_as<Window>(self));
	inline operator const Window() const reflect_as(hWnd);
};
template<class AnyChild, class Style, class StyleEx>
inline Window &CreateStruct<AnyChild, Style, StyleEx>::Parent() reflect_as(Window::Attach(this->hwndParent));
template<class AnyChild, class Style, class StyleEx>
inline Window CreateStruct<AnyChild, Style, StyleEx>::Parent() const reflect_as(this->hwndParent);
template<class AnyChild, class SubXCreate, class Style, class StyleEx>
using WXCreate = typename WindowBase<AnyChild>::template XCreate<SubXCreate, Style, StyleEx>;
template<class AnyChild> ATOM WindowBase<AnyChild>::_ClassAtom = 0;
template<class AnyChild> HINSTANCE WindowBase<AnyChild>::_hClassModule = O;
template<class AnyChild> const String &&WindowBase<AnyChild>::_ClassName = Fits(typeid(typename WindowBase<AnyChild>::Child).name(), MaxLenClass);
#define Window_Based(name) name : public WindowBase<name>
#define WxCreate() public: struct xCreate : XCreate<xCreate>
#define WxClass() public: struct xClass : ClassExBase<xClass>
#pragma endregion

class Console {
protected:
	Window con;
	HANDLE hOut, hErr, hIn;
public:
	using super = Window;

	Console() { if (!Load()) Alloc(); }
	Console(DWORD pid) { Attach(pid); }
	~Console() { Free(); }

	inline bool Load() {
		con = GetConsoleWindow();
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		hErr = GetStdHandle(STD_ERROR_HANDLE);
		hIn = GetStdHandle(STD_INPUT_HANDLE);
		return true;
	}
	inline void Select() {
		static FILE *fout = O, *ferr = O, *fin = O;
		freopen_s(&fout, "CONOUT$", "w", stdout);
		freopen_s(&ferr, "CONERR$", "w", stderr);
		freopen_s(&fin, "CONIN$", "w", stdin);
	}

#pragma region Allocator
	inline bool Attach(DWORD pid) reflect_as(AttachConsole(pid) ? Load() : false);
	inline bool Alloc() reflect_as(AllocConsole() ? Load() : false);
	inline void Free() {
		FreeConsole(); (HWND &)self = O, hOut = hErr = hIn = O;
	}
#pragma endregion

#pragma region Properties
public: // Property - Attr
	/* W */ inline auto &Attr(WORD wAttr) assert_reflect_as_self(SetConsoleTextAttribute(hOut, wAttr));
	/* R */ inline WORD  Attr() const assert_reflect_to(CONSOLE_SCREEN_BUFFER_INFO csbi, GetConsoleScreenBufferInfo(hOut, &csbi), csbi.wAttributes);
public: // Property - Title
	/* W */ inline auto  &Title(LPCTSTR lpTitle) assert_reflect_as_self(SetConsoleTitle(lpTitle));
	/* R */ inline String Title() const {
		String title((size_t)MaxLenTitle);
		int len = GetConsoleTitle(title, MaxLenTitle + 1);
		if (len <= 0) return O;
		title.Resize(len);
		return title;
	}
public: // Property - CurPos
	/* W */ inline auto  &CurPos(LPoint p) assert_reflect_as_self(SetConsoleCursorPosition(hOut, COORD({ (short)p.x, (short)p.y })));
	/* R */ inline LPoint CurPos() const assert_reflect_to(CONSOLE_SCREEN_BUFFER_INFO csbi, GetConsoleScreenBufferInfo(hOut, &csbi), { csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y });
public: // Property - CurVis
	/* W */ inline auto &CurVis(bool bVisible) assert_reflect_to_self(CONSOLE_CURSOR_INFO cci({ 0 }); cci.bVisible = bVisible, SetConsoleCursorInfo(hOut, &cci));
	/* R */ inline bool  CurVis() const assert_reflect_to(CONSOLE_CURSOR_INFO cci({ 0 }), GetConsoleCursorInfo(hOut, &cci), cci.bVisible);
#pragma endregion

#pragma region Read Write
protected:
	inline DWORD _Write(HANDLE hOut, LPCWSTR lpszString, DWORD uLength) assert_reflect_as(WriteConsoleW(hOut, lpszString, uLength, &uLength, O), uLength);
	inline DWORD _Write(HANDLE hOut, LPCSTR lpszString, DWORD uLength) assert_reflect_as(WriteConsoleA(hOut, lpszString, uLength, &uLength, O), uLength);
public:
	inline DWORD Log(bool b) reflect_as(b ? Log(_T("true")) : Log(_T("false")));
	inline DWORD Log(double f) reflect_as(Log(nX(".5d", f)));
	inline DWORD Log(long i) reflect_as(Log(nX("d", i)));
	inline DWORD Log(int32_t i) reflect_as(Log(nX("d", i)));
	inline DWORD Log(int64_t i) reflect_as(Log(nX("d", i)));
	inline DWORD Log(uint8_t i) reflect_as(Log(nX("d", i)));
	inline DWORD Log(uint16_t i) reflect_as(Log(nX("d", i)));
	inline DWORD Log(uint32_t i) reflect_as(Log(nX("d", i)));
	inline DWORD Log(uint64_t i) reflect_as(Log(nX("d", i)));
	inline DWORD Log(DWORD i) reflect_as(Log(nX("d", i)));
	inline DWORD Log(char chs) reflect_as(Log((arrayof<char, 1>&)chs));
	inline DWORD Log(wchar_t chs) reflect_as(Log((arrayof<wchar_t, 1>&)chs));
	inline DWORD Log(const String &str) reflect_as(_Write(hOut, str, (DWORD)str.Length()));
	template<class AnyType>
	inline DWORD Log(const AnyType &t) reflect_as(Log((String)t));
	template<size_t len>
	inline DWORD Log(const char(&Chars)[len]) reflect_as(_Write(hOut, Chars, len));
	template<size_t len>
	inline DWORD Log(const wchar_t(&Chars)[len]) reflect_as(_Write(hOut, Chars, len));
	template<class... Args>
	inline DWORD Log(const Args& ...args) reflect_as((Log(args) + ...));
public:
	inline DWORD Err(bool b) reflect_as(b ? Err(_T("true")) : Err(_T("false")));
	inline DWORD Err(double f) reflect_as(Err(nX(".5d", f)));
	inline DWORD Err(long i) reflect_as(Err(nX("d", i)));
	inline DWORD Err(int32_t i) reflect_as(Err(nX("d", i)));
	inline DWORD Err(int64_t i) reflect_as(Err(nX("d", i)));
	inline DWORD Err(uint8_t i) reflect_as(Err(nX("d", i)));
	inline DWORD Err(uint16_t i) reflect_as(Err(nX("d", i)));
	inline DWORD Err(uint32_t i) reflect_as(Err(nX("d", i)));
	inline DWORD Err(uint64_t i) reflect_as(Err(nX("d", i)));
	inline DWORD Err(DWORD i) reflect_as(Err(nX("d", i)));
	inline DWORD Err(char chs) reflect_as(Err((arrayof<char, 1> &)chs));
	inline DWORD Err(wchar_t chs) reflect_as(Err((arrayof<wchar_t, 1> &)chs));
	inline DWORD Err(const String &str) reflect_as(_Write(hErr, str, (DWORD)str.Length()));
	template<class AnyType>
	inline DWORD Err(const AnyType &t) reflect_as(Err((String)t));
	template<size_t len>
	inline DWORD Err(const char(&Chars)[len]) reflect_as(_Write(hErr, Chars, len));
	template<size_t len>
	inline DWORD Err(const wchar_t(&Chars)[len]) reflect_as(_Write(hErr, Chars, len));
	template<class... Args>
	inline DWORD Err(const Args& ...args) reflect_as((Err(args) + ...));
public:
	inline auto &operator[](LPoint p) reflect_as(CurPos(p));
	inline auto &operator[](bool bCurVis) reflect_as(CurVis(bCurVis));
	template<class... Args>
	inline auto &operator()(const Args& ...args) reflect_to_self((Log(args), ...));
#pragma endregion
};

}
