#pragma once

#include <CommCtrl.h>

#include "./window.h"

namespace WX {

#pragma region CommCtl
template<class AnyChild>
class CommCtl : public ChainExtend<CommCtl<AnyChild>, AnyChild>,
	public WindowBase<AnyChild> {
	def_memberof(CtlClassName);
	subtype_branch(xCreate); 
public:
	using super = WindowBase<AnyChild>;

	CommCtl() { static_assert(!std::is_void_v<AnyChild>); }

	static WndProc DefProc;

	struct xClass : super::template ClassExBase<xClass> {
		xClass() {
			this->lpszClassName = AnyChild::CtlClassName;
			assert(this->GetInfo());
			this->lpszClassName = AnyChild::_ClassName;
			DefProc = this->lpfnWndProc;
			this->lpfnWndProc = AnyChild::template MainProc<GWLP_USERDATA>;
		}
	};

	template<class _AnyChild, class Style, class StyleEx>
	struct XCreate :
		public WindowBase<AnyChild>::template XCreate<KChain<XCreate<_AnyChild, Style, StyleEx>, _AnyChild>, Style, StyleEx> {
		using super = typename WindowBase<AnyChild>::template XCreate<KChain<XCreate, _AnyChild>, Style, StyleEx>;
		XCreate(CommCtl &ctl) : super(ctl) {}
		inline operator bool() {
			this->style |= WS_CHILD | WS_VISIBLE;
			return this->Create();
		}
	};
	inline auto Create() = delete;
	inline auto Create(HWND hParent) {
		super::Register();
		using CreateX = XCreate<void, typename AnyChild::Style, typename AnyChild::StyleEx>;
		return subtype_branchof_xCreate<AnyChild, CreateX>(self).Parent(hParent).Param(this);
	}
};
template<class AnyChild>
WndProc CommCtl<AnyChild>::DefProc;
#define CommCtl_Based(name) template<class AnyChild> name : public ChainExtend<name<AnyChild>, AnyChild>, public CommCtl<KChain<name<AnyChild>, AnyChild>>
#pragma endregion

#pragma region Static
template<class AnyChild>
class StaticCtl;
using Static = StaticCtl<void>;
enum_flags(StaticStyle, WStyle,
	Left            = SS_LEFT,
	Center          = SS_CENTER,
	Right           = SS_RIGHT,
	Icon            = SS_ICON,
	BlackRect       = SS_BLACKRECT,
	GrayRect        = SS_GRAYRECT,
	WhiteRect       = SS_WHITERECT,
	BlackFrame      = SS_BLACKFRAME,
	GrayFrame       = SS_GRAYFRAME,
	WhiteFrame      = SS_WHITEFRAME,
	UserItem        = SS_USERITEM,
	Simple          = SS_SIMPLE,
	LeftNoWordWrap  = SS_LEFTNOWORDWRAP,
	OwnerDraw       = SS_OWNERDRAW,
	Bitmap          = SS_BITMAP,
	EnhMetaFile     = SS_ENHMETAFILE,
	EtchedHorz      = SS_ETCHEDHORZ,
	EtchedVert      = SS_ETCHEDVERT,
	EtchedFrame     = SS_ETCHEDFRAME,
	TypeMask        = SS_TYPEMASK,
	RealSizeControl = SS_REALSIZECONTROL,
	NoPrefix        = SS_NOPREFIX,
	Notify          = SS_NOTIFY,
	CenterImage     = SS_CENTERIMAGE,
	RightJust       = SS_RIGHTJUST,
	RealSizeImage   = SS_REALSIZEIMAGE,
	Sunken          = SS_SUNKEN,
	EditControl     = SS_EDITCONTROL,
	EndEllipsis     = SS_ENDELLIPSIS,
	PathEllipsis    = SS_PATHELLIPSIS,
	WordEllipsis    = SS_WORDELLIPSIS,
	EllipsisMask    = SS_ELLIPSISMASK);
CommCtl_Based(class StaticCtl) {
public:
	static constexpr auto CtlClassName = WC_STATIC;
	using super = CommCtl<KChain<StaticCtl<AnyChild>, AnyChild>>;

	StaticCtl() {}

	using Style = StaticStyle;

#pragma region Properties
public: // Property - Icon
	/* W */ inline auto &Icon(HICON hIcon) reflect_to_child(super::Send(STM_SETICON, hIcon));
	/* R */ inline CIcon Icon() const reflect_as(super::template Send<HICON>(STM_GETICON));
public: // Property - ImageBitmap
	/* W */ inline auto    &ImageBitmap() reflect_to_child(super::Send(STM_SETIMAGE, IMAGE_BITMAP));
	/* R */ inline CBitmap  ImageBitmap() const reflect_as(super::template Send<HBITMAP>(STM_GETIMAGE, IMAGE_BITMAP));
public: // Property - ImageIcon
	/* W */ inline auto &ImageIcon(HICON hIcon) reflect_to_child(super::Send(STM_SETIMAGE, IMAGE_BITMAP));
	/* R */ inline CIcon ImageIcon() const reflect_as(super::template Send<HICON>(STM_GETIMAGE, IMAGE_BITMAP));
public: // Property - ImageCursor
	/* W */ inline auto   &ImageCursor(HCURSOR hCursor) reflect_to_child(super::Send(STM_SETIMAGE, IMAGE_CURSOR, hCursor));
	/* R */ inline CCursor ImageCursor() const reflect_as(super::template Send<HCURSOR>(STM_GETIMAGE, IMAGE_CURSOR));
//public: // Property - EnhMetaFile
//	/* W */ inline auto &EnhMetaFile(HENHMETAFILE hEnhMetaFile) reflect_to_child(super::Send(STM_SETIMAGE, IMAGE_CURSOR, hCursor));
//	/* R */ inline EnhMetaFile() const reflect_as(super::Send<HCURSOR>(STM_GETIMAGE, IMAGE_CURSOR));
//	HENHMETAFILE
#pragma endregion
};
#pragma endregion

#pragma region Button
template<class AnyChild>
class ButtonCtl;
using Button = ButtonCtl<void>;
enum_flags(ButtonStyle, WStyle,
	PushButton       = BS_PUSHBUTTON,
	DefPushButton    = BS_DEFPUSHBUTTON,
	CheckBox         = BS_CHECKBOX,
	AutoCheckBox     = BS_AUTOCHECKBOX,
	RadioButton      = BS_RADIOBUTTON,
	State3           = BS_3STATE,
	AutoState3       = BS_AUTO3STATE,
	GroupBox         = BS_GROUPBOX,
	UserButton       = BS_USERBUTTON,
	AutoRadioButton  = BS_AUTORADIOBUTTON,
	PushBox          = BS_PUSHBOX,
	OwnerDraw        = BS_OWNERDRAW,
	TypeMask         = BS_TYPEMASK,
	LeftText         = BS_LEFTTEXT,
	Text             = BS_TEXT,
	Icon             = BS_ICON,
	Bitmap           = BS_BITMAP,
	Left             = BS_LEFT,
	Right            = BS_RIGHT,
	Center           = BS_CENTER,
	Top              = BS_TOP,
	Bottom           = BS_BOTTOM,
	VCenter          = BS_VCENTER,
	PushLike         = BS_PUSHLIKE,
	MultiLine        = BS_MULTILINE,
	Notify           = BS_NOTIFY,
	Flat             = BS_FLAT);
enum_flags(ButtonState, SHORT,
	Unchecked     = BST_UNCHECKED,
	Checked       = BST_CHECKED,
	Indeterminate = BST_INDETERMINATE,
	Pushed        = BST_PUSHED,
	Focus         = BST_FOCUS);
struct ButtonSplitInfo : protected BUTTON_SPLITINFO {
	ButtonSplitInfo() : BUTTON_SPLITINFO{ 0 } {}
	ButtonSplitInfo(const BUTTON_SPLITINFO &bs) : BUTTON_SPLITINFO(bs) {}
public: // mask
	// MKAS BCSIF_GLYPH 
public: // Property - Glyph
//		HIMAGELIST  himlGlyph;
public: // Property - Styles
	enum_flags(Style, UINT,
		NoSplit =            BCSS_NOSPLIT,
		Stretch =            BCSS_STRETCH,
		AlignLeft =          BCSS_ALIGNLEFT,
		Image =              BCSS_IMAGE);
	/* W */ inline auto &Styles(Style style) reflect_to_self(this->uSplitStyle = style.yield());
	/* R */ inline Style Styles() const reflect_as(reuse_as<Style>(this->uSplitStyle));
public: // Property - Size
	/* W */ inline auto &Size(SIZE size) reflect_to_self(this->size = size);
	/* R */ inline LSize Size() const reflect_as(reuse_as<LSize>(this->size));
};
struct ButtonImageList : protected BUTTON_IMAGELIST {
	ButtonImageList() : BUTTON_IMAGELIST{ 0 } {}
public: // Property - Margin
	/* W */ inline auto &Margin(LRect margin) reflect_to_self(this->margin = margin);
	/* R */ inline LRect Margin() const reflect_as(this->margin);
public: // Property - Align
	enum_class(Aligns, UINT, 
		Left     = BUTTON_IMAGELIST_ALIGN_LEFT,
		Right    = BUTTON_IMAGELIST_ALIGN_RIGHT,
		Top      = BUTTON_IMAGELIST_ALIGN_TOP,
		Bottom   = BUTTON_IMAGELIST_ALIGN_BOTTOM,
		Center   = BUTTON_IMAGELIST_ALIGN_CENTER);
	/* W */ inline auto   &Align(Aligns align) reflect_to_self(this->uAlign = align.yield());
	/* R */ inline Aligns  Align() const reflect_as(reuse_as<Aligns>(this->uAlign));
};
CommCtl_Based(class ButtonCtl) {
	def_memberof(wmailBox);
public:
	static constexpr auto CtlClassName = WC_BUTTON;
	using super = CommCtl<KChain<ButtonCtl<AnyChild>, AnyChild>>;

	ButtonCtl() {}

	using Style = ButtonStyle;
	using State = ButtonState;
	using BSplitInfo = ButtonSplitInfo;
	using BImageList = ButtonImageList;

#define MSG_TRANS(msgid, ret, name, ...) \
	def_memberof(name);
#define WX_BUTTON
#include "msg_ctl.inl"

	inline LRESULT Callback(UINT msgid, WPARAM wParam, LPARAM lParam) {
		switch (msgid) {
			case WM_NULL:
				break;
#define _CALL_(name) child.name
#define MSG_TRANS(msgid, ret, name, argslist, args, send, call) \
			case msgid: \
				if constexpr (member_##name##_of<AnyChild>::existed) { \
					using fn_type = ret argslist; \
					misdef_assert(member_##name##_of<AnyChild>:: \
						template compatible_to<fn_type>, \
						"Member " #name " must be a method compatible to " #ret #argslist); \
					return call; \
				} break;
#define WX_BUTTON
#include "msg_ctl.inl"
		}
		if constexpr (member_wmailBox_of<AnyChild>::existed) {
			LRESULT res;
			if (static_cast<AnyChild * >(this)->wmailBox(res, msgid, wParam, lParam))
				return res;
		}
		return super::HandleNext();
	}

#pragma region Properties
public: // Property - IdealSize
	/* R */ inline LSize IdealSize() const assert_reflect_to(LSize sz, Button_GetIdealSize(self, &sz), sz);
public: // Property - Note // style with BS_COMMANDLINK only
	/* W */ inline auto  &Note(LPCTSTR lpszNote) assert_reflect_as_child(Button_SetNote(self, lpszNote));
	/* R */ inline String Note() const {
		auto len = Button_GetNoteLength(self);
		if (len <= 0) return O;
		String note = len;
		assert(Button_GetNote(self, note, len + 1));
		return note;
	}
public: // Property - TextMargin
	/* W */ inline auto &TextMargin(RECT margin) assert_reflect_as_child(Button_SetTextMargin(self, &margin));
	/* R */ inline LRect TextMargin() const assert_reflect_to(LRect rc, Button_GetTextMargin(self, &rc), rc);
public: // Property - SplitInfo
	/* W */ inline auto      &SplitInfo(BUTTON_SPLITINFO bsp) assert_reflect_as_child(Button_SetSplitInfo(self, &bsp));
	/* R */ inline BSplitInfo SplitInfo() const assert_reflect_to(BSplitInfo bsp, Button_GetSplitInfo(self, &bsp), bsp);
public: // Property - ImageList
	/* W */ inline auto      &ImageList(BUTTON_IMAGELIST &iml) assert_reflect_as_child(Button_SetImageList(self, &iml));
	/* R */ inline BImageList ImageList() const assert_reflect_to(BImageList iml, Button_GetImageList(self, &iml), iml);
//public: // Property - Image
//	/* W */ inline auto &Image() reflect_to_child(super::Send(BM_SETIMAGE));
//	/* R */ inline auto &Image() const {}
public: // Property - Check
	/* W */ inline auto &Check(State s) reflect_to_child(super::Send(BM_SETCHECK, s.yield()));
	/* R */ inline State Check() const reflect_as(super::template Send<State>(BM_GETCHECK));
public: // Property - States
	/* W */ inline auto &States(State s) reflect_to_child(super::Send(BM_SETSTATE, s.yield()));
	/* R */ inline State States() const reflect_as(super::template Send<State>(BM_GETSTATE));
public: // Property - Styles
	/* W */ inline auto &Styles(Style style, bool bRedraw = true) reflect_to_child(super::Send(BM_SETSTYLE, style.yield(), bRedraw));
public: // Property - DontClick
	/* W */ inline auto &DontClick(bool bDontClick) reflect_to_child(super::Send(BM_SETDONTCLICK, bDontClick));
#pragma endregion
};
//template<>
//class ButtonCtl<void> : public ButtonCtl<ButtonCtl<void>> {
//	using super = ButtonCtl<ButtonCtl<void>>;
//	friend class ButtonCtl<ButtonCtl<void>>;
//	WMailSlot wmailBox;
//public:
//#define _CALL_(...) evf
//#define MSG_TRANS(msgid, ret, name, argslist, args, send, call) \
//	template<class EventFunc> \
//	inline auto &name(EventFunc evf) { \
//		using EventFuncStd = ret argslist; \
//		static_assert(static_compatible<EventFunc, EventFuncStd>, \
//					  "Error event function type of " #name \
//					  " must be compatible to " #ret #argslist); \
//		wmailBox.Attach(msgid, \
//			[evf](WPARAM wParam, LPARAM lParam) reflect_as(call);); \
//		retself; }
//#define WX_BUTTON
//#include "wx_messages.inl"
//};
#pragma endregion

#pragma region ScrollBar
template<class AnyChild = void>
class ScrollBarCtl;
using ScrollBar = ScrollBarCtl<>;
enum_flags(ScrollBarStyle, WStyle,
	Horz                    = SBS_HORZ,
	Vert                    = SBS_VERT,
	TopAlign                = SBS_TOPALIGN,
	LeftAlign               = SBS_LEFTALIGN,
	BottomAlign             = SBS_BOTTOMALIGN,
	RightAlign              = SBS_RIGHTALIGN,
	SizeBoxTopLeftAlign     = SBS_SIZEBOXTOPLEFTALIGN,
	SizeBoxBottomRightAlign = SBS_SIZEBOXBOTTOMRIGHTALIGN,
	SizeBox                 = SBS_SIZEBOX,
	SizeGrip                = SBS_SIZEGRIP);
struct ScrollInfo : protected SCROLLINFO {
	ScrollInfo() : SCROLLINFO{ 0 } { this->cbSize = sizeof(SCROLLINFO); }
public:
	//	UINT    fMask;
public:
	//int     nMin;
	//int     nMax;
	//UINT    nPage;
	//int     nPos;
	//int     nTrackPos;
};
struct ScrollBarInfo : protected SCROLLBARINFO {
	ScrollBarInfo() : SCROLLBARINFO{ 0 } { this->cbSize = sizeof(SCROLLBARINFO); }
public: // Property - Rect
	/* W */ inline auto &Rect(LRect rc) reflect_to_self(this->rcScrollBar = rc);
	/* R */ inline LRect Rect() const reflect_as(this->rcScrollBar);
public: // Property - LineButton
	/* W */ inline auto &LineButton(int dxyLineButton) reflect_to_self(this->dxyLineButton = dxyLineButton);
	/* R */ inline int   LineButton() const reflect_as(this->dxyLineButton);
public: // Property - ThumbTop
	/* W */ inline auto &ThumbTop(int xyThumbTop) reflect_to_self(this->xyThumbTop = xyThumbTop);
	/* R */ inline int   ThumbTop() const reflect_as(this->xyThumbTop);
public: // Property - ThumbBottom
	/* W */ inline auto &ThumbBottom(int xyThumbBottom) reflect_to_self(this->xyThumbBottom = xyThumbBottom);
	/* R */ inline int   ThumbBottom() const reflect_as(this->xyThumbBottom);
public: // Property - 
	//DWORD rgstate[CCHILDREN_SCROLLBAR + 1];
};
CommCtl_Based(class ScrollBarCtl) {
public:
	static constexpr auto CtlClassName = WC_SCROLLBAR;
	using super = CommCtl<KChain<ScrollBarCtl<AnyChild>, AnyChild>>;

	ScrollBarCtl() {}

	using Style = ScrollBarStyle;
	using SInfo = ScrollInfo;
	using SBarInfo = ScrollBarInfo;

#pragma region Properties
public:
//	ESB_DISABLE_RIGHT
// #define SBM_ENABLE_ARROWS           0x00E4 /*not in win3.1 */
public: // Property - Position
	/* W */ inline auto &Position(int Pos, bool bRedraw = true) reflect_to_child(super::Send(SBM_SETPOS, Pos, bRedraw));
	/* R */ inline int   Position() const reflect_as(super::Send(SBM_GETPOS));
public: // Property - Range
	/* W */ inline auto &Range(int MinPos, int MaxPos, bool bRedraw = true) reflect_to_child(super::Send(bRedraw ? SBM_SETRANGEREDRAW : SBM_SETRANGE, MinPos, MaxPos));
	/* R */ inline auto  Range() const assert_reflect_to(struct{ _M_(int Min, Max); } rgn, super::Send(SBM_GETRANGE, &rgn.Min, &rgn.Max), rgn);
public: // Property - ScrollInfo
	/* W */ inline auto  &Info(const SInfo &i, bool bRedraw = true) reflect_to_self(super::Send(SBM_SETSCROLLINFO, bRedraw, &i));
	/* R */ inline SInfo  Info() const assert_reflect_to(SInfo i, super::Send(SBM_GETSCROLLINFO, 0, &i), i);
public:
	/* R */ inline SBarInfo BarInfo() const assert_reflect_to(SBarInfo i, super::Send(SBM_GETSCROLLBARINFO, 0, &i), i);
#pragma endregion
};
#pragma endregion

#pragma region StatusBar
template<class AnyChild>
class StatusBarCtl;
using StatusBar = StatusBarCtl<void>;
enum_flags(StatusBarTexts, uint16_t,
	Default         = 0,
	OwnerDraw       = SBT_OWNERDRAW,
	NoBorders       = SBT_NOBORDERS,
	PopOut          = SBT_POPOUT,
	RTLReading      = SBT_RTLREADING,
	NoTabParsing    = SBT_NOTABPARSING);
CommCtl_Based(class StatusBarCtl) {
public:
	static constexpr LPCTSTR CtlClassName = STATUSCLASSNAME;
	using super = CommCtl<KChain<StatusBarCtl<AnyChild>, AnyChild>>;
	using Style = WStyle;
	using StyleEx = WStyleEx;
	using Texts = StatusBarTexts;

	StatusBarCtl() {}

	inline void FixSize() { super::Send(WM_SIZE); }

#pragma region Properties
public: // Property - Parts
	template<size_t Len>
	/* W */ inline bool SetParts(const int (&Slice)[Len]) reflect_as(super::Send(SB_SETPARTS, Len, Slice));
	/* W */ inline bool SetParts(const int *pSlices, int Len) reflect_as(super::Send(SB_SETPARTS, Len, pSlices));
	template<size_t Len>
	/* R */ inline bool GetParts(int (&Slice)[Len]) const reflect_as(super::Send(SB_GETPARTS, Len, Slice));
	/* R */ inline bool GetParts(int *pSlices, int Len) const reflect_as(super::Send(SB_GETPARTS, Len, pSlices));
public: // Property - Borders
	/* R */ inline auto Borders() const assert_reflect_to(struct { int _M_(BorderH, BorderV, GapH); } borders, super::Send(SB_GETBORDERS, 0, &borders), borders);
public: // Property - MinHeight
	/* W */ inline auto&MinHeight(int MinHeight) reflect_to_self(super::Send(SB_SETMINHEIGHT, MinHeight));
public: // Property - Simple
	/* W */ inline auto&Simple(bool bSimple) reflect_to_self(super::Send(SB_SIMPLE, bSimple));
	/* R */ inline bool Simple() const reflect_as(super::Send(SB_SIMPLE));
public: // Property - Rect
	/* W */ inline LRect Rect(uint8_t nPart) assert_reflect_to(LRect rc, super::Send(SB_GETRECT, nPart, &rc), rc);
public: // Property - Icon
	/* W */ inline auto &Icon(uint8_t nPart, HICON hIcon) reflect_to_self(super::Send(SB_SETICON, nPart, hIcon));
	/* R */ inline CIcon Icon(uint8_t nPart) const reflect_as((HICON)super::Send(SB_GETICON, nPart));
public: // Property - TextLength
	/* R */ inline uint16_t TextLength(uint8_t nPart) const reflect_as(super::template Send<uint16_t>(SB_GETTEXTLENGTH, nPart));
	/* R */ inline Texts    TextStyle(uint8_t nPart) const reflect_as(super::template Send<Texts>(SB_GETTEXTLENGTH, nPart) >> 16);
public: // Property - Text
	/* W */ inline bool   Text(uint8_t nPart, LPCTSTR text, Texts style = Texts::Default) reflect_as(super::Send(SB_SETTEXT, nPart | style.yield(), (LPARAM)(LPCTSTR)text));
	/* R */ inline String Text(uint8_t nPart) const {
		auto len = TextLength(nPart);
		if (len <= 0) return O;
		String str = len;
		if (super::Send(SB_GETTEXT, nPart, str))
			return str;
		return O;
	}
public: // Property - TipText
	/* W */ inline auto  &TipText(WORD id, LPCTSTR tip) reflect_to_self(super::Send(SB_SETTIPTEXT, id, (LPARAM)(LPCTSTR)tip));
	/* R */ inline String TipText(WORD id) const {
		String str = MaxLenNotice;
		if (super::Send(SB_GETTIPTEXT, MAKEWPARAM(id, MaxLenNotice + 1), str))
			return str.Trunc();
		return O;
	}
public: // Property - UnicodeFormat
	/* W */ inline auto &UnicodeFormat(bool bUnicode) reflect_to_self(super::Send(SB_SETUNICODEFORMAT, bUnicode));
	/* R */ inline bool  UnicodeFormat() const reflect_as(super::Send(SB_GETUNICODEFORMAT));
public: // Property - BkColor
	/* R */ inline ColorRGB BkColor() const reflect_as(super::template Send<ColorRGB>(SB_SETBKCOLOR));
#pragma endregion
};
#pragma endregion

#if 0
#pragma region ToolTip
template<class AnyChild = void>
class ToolTipCtl;
enum_flags(ToolTipStyle, WStyle,
	AlwaysTip           = TTS_ALWAYSTIP,
	NoPrefix            = TTS_NOPREFIX,
	NoAnimate           = TTS_NOANIMATE,
	NoFade              = TTS_NOFADE,
	Balloon             = TTS_BALLOON,
	Close               = TTS_CLOSE,
	UseVisualStyle      = TTS_USEVISUALSTYLE);
struct ToolTipInfo : protected TTTOOLINFO {
	ToolTipInfo() : TTTOOLINFO{ 0 } { this->cbSize = sizeof(TTTOOLINFO); }
};
using ToolTip = ToolTipCtl<>;
CommCtl_Based(class ToolTipCtl) {
public:
	static constexpr auto CtlClassName = TOOLTIPS_CLASS;
	using super = CommCtl<KChain<ToolTipCtl<AnyChild>, AnyChild>>;

	ToolTipCtl() {}

	using Style = ToolTipStyle;
	using CInfo = ToolTipInfo;

#pragma region Methods
	inline auto&Activate(bool bActivate = false) reflect_to_child(super::Send(TTM_ACTIVATE, bActivate));
	inline bool Add(const CInfo &i) reflect_as(super::Send(TTM_ADDTOOL, 0, &i));
	inline void Del(const CInfo &i) { super::Send(TTM_DELTOOL, 0, &i); }
	inline void UpdateTip(const CInfo &i) { super::Send(TTM_UPDATETIPTEXT, 0, &i); }
	//inline CInfo HitTest(HWND hwnd, POINT pt) {
	//	TTHITTESTINFO tti = { 0 };
	//	tti.hwnd = hwnd, tti.pt = pt;
	//	if (super::Send(TTM_HITTEST))
	//		return tti.ti;
	//	return {};
	//}
//#define TTM_ENUMTOOLSW          (WM_USER +58)
//#define TTM_NEWTOOLRECTA        (WM_USER + 6)
//#define TTM_RELAYEVENT          (WM_USER + 7) // Win7: wParam = GetMessageExtraInfo() when relaying WM_MOUSEMOVE
//#define TTM_POP                 (WM_USER + 28)
//#define TTM_POPUP               (WM_USER + 34)
//#define TTM_UPDATE              (WM_USER + 29)
//#define TTM_ADJUSTRECT          (WM_USER + 31)
//#define TTM_WINDOWFROMPOINT     (WM_USER + 16)
//#define TTM_TRACKACTIVATE       (WM_USER + 17)  // wParam = TRUE/FALSE start end  lparam = LPTOOLINFO
//#define TTM_TRACKPOSITION       (WM_USER + 18)  // lParam = dwPos
#pragma endregion

#pragma region Properties
//public: // Property - Info
//	/* W */ inline auto &Info(CInfo i) { super::Send(TTM_SETTOOLINFO, 0, &i); retchild; }
//	/* R */ inline CInfo Info() const {
//		CInfo i;
//		super::Send(TTM_GETTOOLINFO);
//		return i;
//	}
//public: // Property - Text
//	/* R */ inline String Text() const {
//		String str;
//		super::Send(TTM_GETTEXT, str);
//		return str;
//	}
//public: // Property - ToolCount
//	/* R */ inline int ToolCount() const reflect_as(super::Send(TTM_GETTOOLCOUNT));
//public: // Property - CurrentTool
//	/* R */ inline CInfo CurrentTool() const { CInfo i; assert(super::Send(TTM_GETCURRENTTOOL, 0, &i)); return i; }
//public: // Property - DelayTime
//	/* W */ inline auto &DelayTime(CInfo i) { super::Send(TTM_SETDELAYTIME); retchild; }
//	/* R */ inline CInfo DelayTime() const { CInfo i; super::Send(TTM_GETDELAYTIME); return i; }
//public: // Property - TipBkColor
//	/* W */ inline auto    &TipBkColor(ColorRGB rgb) { super::Send(TTM_SETTIPBKCOLOR); retchild; }
//	/* R */ inline ColorRGB TipBkColor() const reflect_as(super::Send(TTM_GETTIPBKCOLOR));
//public: // Property - TipTextColor
//	/* W */ inline auto &TipTextColor(int width) { super::Send(TTM_SETTIPTEXTCOLOR); retchild; }
//	/* R */ inline int   TipTextColor() const reflect_as(super::Send(TTM_GETTIPTEXTCOLOR));
//public: // Property - MaxTipWidth
//	/* W */ inline auto &MaxTipWidth(int width) { super::Send(TTM_SETMAXTIPWIDTH); retchild; }
//	/* R */ inline int   MaxTipWidth() const reflect_as(super::Send(TTM_GETMAXTIPWIDTH));
//public: // Property - Margin
//	/* W */ inline auto &Margin(int width) { super::Send(TTM_SETMARGIN); retchild; }
//	/* R */ inline int   Margin() const reflect_as(super::Send(TTM_GETMARGIN));
//public: // Property - Title
//	/* W */ inline auto &Title(LPCTSTR lpString) { super::Send(TTM_SETTITLE); retchild; }
//	/* R */ inline int   Title() const {
//		super::Send(TTM_GETTITLE);
//	}
//public: // Property - BubbleSize
//	/* R */ inline int BubbleSize() const { super::Send(TTM_GETBUBBLESIZE); }
//public: // Property - WindowThem
//	/* W */ inline auto &WindowTheme() { super::Send(TTM_SETWINDOWTHEME); retchild; }
#pragma endregion
};
#pragma endregion

#pragma region ReBar
struct ColorScheme : protected COLORSCHEME {
	ColorScheme() : COLORSCHEME{ 0 } { this->dwSize = sizeof(self); }
public: // Property - Highlight
	/* W */ inline auto &Highlight(COLORREF cr) reflect_to_self(this->clrBtnHighlight = clrBtnHighlight);
	/* R */ inline ColorRGB Highlight() const reflect_as(this->clrBtnHighlight);
public: // Property - Shadow
	/* W */ inline auto &Shadow(COLORREF cr) reflect_to_self(this->clrBtnShadow = clrBtnShadow);
	/* R */ inline ColorRGB Shadow() const reflect_as(this->clrBtnShadow);
public:
	inline operator COLORSCHEME&() reflect_to_self();
	inline operator const COLORSCHEME &() const reflect_to_self();
	inline LPCOLORSCHEME operator&() reflect_as(this);
	inline const COLORSCHEME *operator&() const reflect_as(this);
};
template<class AnyChild>
class ReBarCtl;
using ReBar = ReBarCtl<void>;
enum_flags(ReBarBandStyle, UINT,
	Break          = RBBS_BREAK,
	FixedSize      = RBBS_FIXEDSIZE,
	ChildEdge      = RBBS_CHILDEDGE,
	Hidden         = RBBS_HIDDEN,
	NoVert         = RBBS_NOVERT,
	FixedBmp       = RBBS_FIXEDBMP,
	VariableHeight = RBBS_VARIABLEHEIGHT,
	GripperAlways  = RBBS_GRIPPERALWAYS,
	NoGripper      = RBBS_NOGRIPPER,
	UseChevron     = RBBS_USECHEVRON,
	HideTitle      = RBBS_HIDETITLE,
	TopAlign       = RBBS_TOPALIGN);
struct ReBarBandInfo : protected REBARBANDINFO {
	ReBarBandInfo() : REBARBANDINFO{ 0 } { this->cbSize = sizeof(REBARBANDINFO); }
	using Style = ReBarBandStyle;
public:
	UINT        fMask;
public: // Property - Styles
	/* W */ inline auto &Styles(Style style) reflect_to_self(this->fStyle = style.yield());
	/* R */ inline Style Styles() const reflect_as(force_cast<Style>(this->fStyle));
public: // Property - Foreground
	/* W */ inline auto    &Foreground(COLORREF clrFore) reflect_to_self(this->clrFore = clrFore);
	/* R */ inline ColorRGB Foreground() const reflect_as(this->clrFore);
public: // Property - Background
	/* W */ inline auto    &Background(COLORREF clrBack) reflect_to_self(this->clrBack = clrBack);
	/* R */ inline ColorRGB Background() const reflect_as(this->clrBack);
//public: // Property - Text
//	LPWSTR      lpText;
//	UINT        cch;
public:
	int         iImage;
public: // Property - Child
	/* W */ inline auto  &Child(HWND hwndChild) reflect_to_self(this->hwndChild = hwndChild);
	/* R */ inline Window Child() const reflect_as(this->hwndChild);
public:
	UINT        cxMinChild;
	UINT        cyMinChild;
	UINT        cx;
	HBITMAP     hbmBack;
	UINT        wID;
	UINT        cyChild;
	UINT        cyMaxChild;
	UINT        cyIntegral;
	UINT        cxIdeal;
	LPARAM      lParam;
	UINT        cxHeader;
	RECT        rcChevronLocation;    // the rect is in client co-ord wrt hwndChild
	UINT        uChevronState; // STATE_SYSTEM_*
public:
	inline operator REBARBANDINFO &() reflect_to_self();
	inline operator const REBARBANDINFO &() const reflect_to_self();
	inline REBARBANDINFO *operator&() reflect_as(this);
	inline const REBARBANDINFO *operator&() const reflect_as(this);
};
enum_flags(ReBarStyle, WStyle,
	ToolTips                  = RBS_TOOLTIPS,
	VarHeight                 = RBS_VARHEIGHT,
	BandBorders               = RBS_BANDBORDERS,
	FixedOrder                = RBS_FIXEDORDER,
	RegisterDrop              = RBS_REGISTERDROP,
	AutoSize                  = RBS_AUTOSIZE,
	VerticalGripper           = RBS_VERTICALGRIPPER,
	DblClkToggle              = RBS_DBLCLKTOGGLE);
CommCtl_Based(class ReBarCtl) {
public:
	static constexpr auto CtlClassName = REBARCLASSNAME;
	using super = CommCtl<KChain<ReBarCtl<AnyChild>, AnyChild>>;

	ReBarCtl() {}

	using Style = ReBarStyle;
	using BandInfo = ReBarBandInfo;

#define RB_INSERTBAND  (WM_USER +  1)

#define RB_MOVEBAND     (WM_USER + 39)

#define RB_SIZETORECT   (WM_USER +  23) // resize the rebar/break bands and such to this rect (lparam)
#define RB_BEGINDRAG    (WM_USER + 24)
#define RB_ENDDRAG      (WM_USER + 25)
#define RB_DRAGMOVE     (WM_USER + 26)

#define RB_IDTOINDEX    (WM_USER +  16) // wParam == id

#define RB_PUSHCHEVRON      (WM_USER + 43)

#define RB_HITTEST      (WM_USER +  8)

public: // Property - BandCount
	/* R */ inline UINT  BandCount() const reflect_as(super::template Send<UINT>(RB_GETBANDCOUNT));

#pragma region Band
	inline void  BandDelete(UINT ind) assert_reflect_as_child(super::Send(RB_DELETEBAND, ind));
	inline auto &BandMinimize(UINT ind) reflect_to_child(super::Send(RB_MINIMIZEBAND, ind));
	inline auto &BandMaximize(UINT ind) reflect_to_child(super::Send(RB_MAXIMIZEBAND, ind));
public: // Property - BandVisible
	/* R */ inline auto &BandVisible(UINT ind, bool bVisible) assert_reflect_as_child(super::template Send<bool>(RB_SHOWBAND, ind, bVisible));
//public: // Property - BandInfo
//	/* W */ inline bool  BandInfo(UINT ind); // RB_SETBANDINFO
//	/* R */ inline bool  BandInfo(UINT ind) const; // RB_GETBANDINFO
public: // Property - BandBorders
	/* R */ inline LRect BandBorders(UINT ind) const reflect_to(LRect rc; super::Send(RB_GETBANDBORDERS, ind, &rc), rc);
public: // Property - BandMargins
	/* R */ inline LRect BandMargins(UINT ind) const reflect_to(LRect rc; super::Send(RB_GETBANDMARGINS, ind, &rc), rc);
public: // Property - BandWidth
	/* W */ inline auto &BandWidth(UINT ind, UINT width) assert_reflect_as_child(super::template Send<bool>(RB_SETBANDWIDTH, ind, width));
public: // Property - BandRect
	/* R */ inline bool  BandRect(UINT ind) const assert_reflect_to(LRect rc, super::template Send<bool>(RB_GETRECT, ind, &rc), rc);
public: // Property - BandRowHeight
	/* R */ inline UINT  BandRowHeight(UINT ind) const reflect_as(super::template Send<UINT>(RB_GETROWHEIGHT, ind));
#pragma endregion

#pragma region Properties
//public: // Property - 
//	/* W */ inline auto &Info(); // RB_GETBARINFO
//	/* R */ inline bool  Info() const; // RB_SETBARINFO
public: // Property - Parent
	/* R */ inline auto &Parent(HWND hParent) const reflect_to_child(super::Send(RB_SETPARENT, hParent));
public: // Property - RowCount
	/* R */ inline UINT  RowCount() const reflect_as(super::template Send<UINT>(RB_GETROWCOUNT));
public: // Property - ToolTips
//	/* W */ inline auto   &ToolTips() reflect_as(super::Send(RB_SETTOOLTIPS));
	/* R */ inline ToolTip ToolTips() const reflect_as(super::template Send<HWND>(RB_GETTOOLTIPS));
public: // Property - BkColor
	/* W */ inline auto    &BkColor(COLORREF cr) reflect_to_child(super::Send(RB_SETBKCOLOR, 0, cr));
	/* R */ inline COLORREF BkColor() const reflect_as(super::template Send<COLORREF>(RB_GETBKCOLOR));
public: // Property - TextColor
	/* W */ inline auto    &TextColor(COLORREF cr) reflect_to_child(super::Send(RB_SETTEXTCOLOR, 0, cr));
	/* R */ inline COLORREF TextColor() const reflect_as(super::template Send<COLORREF>(RB_GETTEXTCOLOR));
public: // Property - BarHeight
	/* R */ inline UINT  BarHeight() const reflect_as(super::template Send<UINT>(RB_GETBARHEIGHT));
public: // Property - Palette
	/* W */ inline auto    &Palette(HPALETTE hPal) reflect_to_child(super::Send(RB_SETPALETTE, 0, hPal));
	/* R */ inline CPalette Palette() const reflect_as(super::template Send<HPALETTE>(RB_GETPALETTE));
public: // Property - ColorScheme
	/* W */ inline auto &ColorScheme(COLORSCHEME cs) reflect_to_child(super::Send(RB_SETCOLORSCHEME, 0, &cs));
	/* R */ inline ::ColorScheme ColorScheme() const assert_reflect_to(::ColorScheme cs, super::template Send<bool>(RB_GETCOLORSCHEME, 0, &cs), cs);
//public: // Property - DropTarget
//	/* R */ inline auto DropTarget(IDropTarget); // RB_GETDROPTARGET
public: // Property - UnicodeFormat
	/* W */ inline auto &UnicodeFormat(bool bUnicode) reflect_to_child(Send(RB_SETUNICODEFORMAT, bUnicode));
	/* R */ inline bool  UnicodeFormat() const assert_reflect_as(Send<bool>(RB_SETUNICODEFORMAT));
//public: // Property - WindowTheme
//	/* W */ inline auto &WindowTheme(); // RB_SETWINDOWTHEME
#pragma endregion
};
#pragma endregion
#endif

#pragma region Edit
template<class AnyChild = void>
class EditCtl;
using Edit = EditCtl<>;
enum_flags(EditStyle, WStyle,
	Left              = ES_LEFT,
	Center            = ES_CENTER,
	Right             = ES_RIGHT,
	MultiLine         = ES_MULTILINE,
	UpperCase         = ES_UPPERCASE,
	LowerCase         = ES_LOWERCASE,
	Password          = ES_PASSWORD,
	AutoVScroll       = ES_AUTOVSCROLL,
	AutoHScroll       = ES_AUTOHSCROLL,
	NoHideSel         = ES_NOHIDESEL,
	OEMConvert        = ES_OEMCONVERT,
	ReadOnly          = ES_READONLY,
	WantReturn        = ES_WANTRETURN,
	Number            = ES_NUMBER);
enum_flags(EditMargin, UINT,
	Left     = EC_LEFTMARGIN,
	Right    = EC_RIGHTMARGIN,
	FontInfo = EC_USEFONTINFO);
struct EditBalloonTip : protected EDITBALLOONTIP {
	EditBalloonTip() : EDITBALLOONTIP{ 0 } { this->cbStruct = sizeof(EDITBALLOONTIP); }
	//LPCTSTR pszTitle;
	//LPCTSTR pszText;
	//INT     ttiIcon; // From TTI_*
};
CommCtl_Based(class EditCtl) {
public:
	static constexpr auto CtlClassName = WC_EDIT;
	using super = CommCtl<KChain<EditCtl<AnyChild>, AnyChild>>;

	EditCtl() {}

	using Style = EditStyle;
	using Balloon = EditBalloonTip;

#pragma region Methods
///#define EM_SCROLL               0x00B5
	inline auto &ScrollCaret() reflect_to_child(super::Send(EM_SCROLLCARET));
	inline bool  LineScroll(int nLines) reflect_as(super::Send(EM_LINESCROLL, 0, nLines));
	inline auto &ReplaceSel(LPCTSTR lpString, bool bUndo = false) reflect_to_child(super::Send(EM_REPLACESEL, bUndo, lpString));
	inline bool  Undo() reflect_as(super::Send(EM_UNDO));
	inline auto &EmptyUndoBuffer() reflect_to_child(super::Send(EM_EMPTYUNDOBUFFER));
//#define EM_LINEFROMCHAR         0x00C9
//#define EM_POSFROMCHAR          0x00D6
//#define EM_CHARFROMPOS          0x00D7
	inline auto&BalloonTip(const Balloon &tip) const assert_reflect_as_child(Edit_ShowBalloonTip(self, &tip));
	inline auto&BalloonTip() const assert_reflect_as_child(Edit_HideBalloonTip(self));
//#define Edit_GetFileLineFromChar(hwndCtl, characterIndex) \
//        (DWORD)SNDMSG((hwndCtl), EM_FILELINEFROMCHAR, (WPARAM)(characterIndex), 0)
#pragma endregion

#pragma region Properties
public: // Property - Sel
	/* W */ inline auto &Sel(int to, int from) reflect_to_child(super::Send(EM_SETSEL, to, from));
	/* R */ inline auto  Sel() const reflect_to(struct { _M_(int to = 0, from = 0); } r; super::Send(EM_GETSEL, &r.from, &r.to), r);
public: // Property - Rect
	/* W */ inline auto &Rect(LRect rc) reflect_to_child(super::Send(EM_SETRECT, O, rc));
	/* R */ inline LRect Rect() const assert_reflect_to(LRect rc, super::Send(EM_GETRECT, O, &rc), rc);
public: // Property - RectNp
	/* W */ inline auto &RectNp(LRect rc) reflect_to_child(super::Send(EM_SETRECTNP, O, rc));
public: // Property - Modify
	/* W */ inline auto &Modify(bool bModified) reflect_to_child(super::Send(EM_SETMODIFY, bModified));
	/* R */ inline bool  Modify() const reflect_as(super::Send(EM_GETMODIFY));
public: // Property - Handle
	/* W */ inline auto  &Handle(HLOCAL hLocal) reflect_to_child(super::Send(EM_SETHANDLE, hLocal));
	/* R */ inline HLOCAL Handle() const reflect_as(super::template Send<HLOCAL>(EM_GETHANDLE));
public: // Property - Thumb
	/* R */ inline int Thumb() const reflect_as(super::Send(EM_GETTHUMB));
public: // Property - LineCount
	/* R */ inline UINT LineCount() const reflect_as(super::Send(EM_GETLINECOUNT));
public: // Property - LineIndex
	/* R */ inline int LineIndex(int index) reflect_as(super::Send(EM_LINEINDEX, index));
public: // Property - LineLength
	/* R */ inline int LineLength(int nLine) reflect_as(super::Send(EM_LINELENGTH, nLine));
public: // Property - Line
	/* W */ inline String Line(int nLine) const {
		auto len = LineLength(nLine);
		assert(len > 0);
		if (len == 0) return O;
		String line = len;
		assert(super::Send(EM_GETLINE, nLine, line));
		return line;
	}
public: // Property - FmtLines
	/* W */ inline auto &FmtLines(bool bFmtLines) reflect_to_child(assert(super::Send(EM_FMTLINES, bFmtLines) == bFmtLines));
public: // Property - LimitText
	/* W */ inline auto &LimitText(UINT limit) reflect_to_child(super::Send(EM_SETLIMITTEXT, limit));
	/* R */ inline UINT  LimitText() const reflect_as(super::Send(EM_GETLIMITTEXT));
public: // Property - CanUndo
	/* R */ inline bool CanUndo() const reflect_as(super::Send(EM_CANUNDO));
public: // Property - TabStops
//	/* W */ inline auto &TabStops() reflect_to_child(super::Send(EM_SETTABSTOPS));
public: // Property - FirstVisibleLine
	/* R */ inline int FirstVisibleLine() const reflect_as(super::Send(EM_GETFIRSTVISIBLELINE));
public: // Property - ReadOnly
	/* W */ inline auto &ReadOnly(bool bReadOnly) reflect_to_child(assert(super::Send(EM_SETREADONLY, bReadOnly)));
//public: // Property - WordBreakProc
//	/* W */ inline auto &WordBreakProc() reflect_to_child(super::Send(EM_SETWORDBREAKPROC));
//	/* R */ inline auto &WordBreakProc() const reflect_as(super::Send(EM_GETWORDBREAKPROC));
public: // Property - PasswordChar
	/* W */ inline auto &PasswordChar(TCHAR chr) reflect_to_child(super::Send(EM_SETPASSWORDCHAR, chr));
	/* R */ inline TCHAR PasswordChar() const reflect_as(super::Send(EM_GETPASSWORDCHAR));
//public: // Property - Margins
//	using Margin = EditMargin;
//	/* W */ inline auto &Margins(Margin m) reflect_to_child(super::Send(EM_SETMARGINS, m.yield(), ));
//	/* R */ inline auto &Margins() const reflect_as(super::Send(EM_GETMARGINS));
//public: // Property - ImeStatus
//	/* W */ inline auto &ImeStatus() reflect_to_child(super::Send(EM_SETIMESTATUS));
//	/* R */ inline auto &ImeStatus() const reflect_as(super::Send(EM_GETIMESTATUS));
public: // Property - CueBanner
	/* W */ inline auto  &CueBanner(LPCTSTR lpString, bool bFocused = false) reflect_to_child(Edit_SetCueBannerTextFocused(self, lpString, bFocused));
	/* R */ inline String CueBanner() const assert_reflect_to(String cbt = MaxLenNotice, Edit_GetCueBannerText(self, cbt, MaxLenNotice + 1), cbt.Trunc());
//public: // Property - ExtendedStyle
//	/* W */ inline auto &ExtendedStyle(TCHAR chr) reflect_to_child(Edit_SetExtendedStyle(self, ));
//	/* R */ inline TCHAR ExtendedStyle() const reflect_as(Edit_GetExtendedStyle(self));
public: // Property - SearchWeb
	/* W */ inline auto &SearchWeb(bool bSearchWeb) reflect_to_child(Edit_EnableSearchWeb(self, bSearchWeb));
public: // Property - CaretIndex
	/* W */ inline auto &CaretIndex(DWORD newCaretIndex) reflect_to_child(Edit_SetCaretIndex(self, newCaretIndex));
	/* R */ inline DWORD CaretIndex() const reflect_as(Edit_GetCaretIndex(self));
public: // Property - Zoom
	/* W */ inline auto &Zoom(int Numerator, int Denominator) reflect_to_child(Edit_SetZoom(self, Numerator, Denominator));
	/* R */ inline auto  Zoom() const assert_reflect_to(struct { _M_(int Numerator, Denominator); } z, Edit_GetZoom(self, &z.Numerator, &z.Denominator), z);
public: // Property - FileLineIndex
	/* R */ inline int FileLineIndex(int index) const reflect_as(Edit_GetFileLineIndex(self, index));
public: // Property - FileLineLength
	/* R */ inline int FileLineLength(int nLine) const reflect_as(Edit_GetFileLineLength(self, nLine));
public: // Property - FileLine
	/* R */ inline String FileLine(int nLine) const {
		auto len = FileLineLength(nLine);
		assert(len > 0);
		if (len == 0) return O;
		String line = len;
		assert(Edit_GetFileLine(self, nLine, line));
		return line;
	}
public: // Property - FileLineCount
	/* R */ inline DWORD FileLineCount() const reflect_as(Edit_GetFileLineCount(self));
#pragma endregion
};
#pragma endregion

}
