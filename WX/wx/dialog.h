#pragma once

#include "window.h"

namespace WX {

class ChooserColor : protected CHOOSECOLOR {
protected:
	COLORREF crCustColors[16] = { 0 };
	using ColorSet = arrayof<ColorRGB, 16>;
	using CColorSet = const arrayof<ColorRGB, 16>;
public:

	ChooserColor() : CHOOSECOLOR{ 0 } {
		lStructSize = sizeof(CHOOSECOLOR);
		this->lpCustColors = crCustColors;
	}

#pragma region Style
	enum_flags(Style, DWORD,
		InitRGB              = CC_RGBINIT,
		FullOpen             = CC_FULLOPEN,
		PreventFullOpen      = CC_PREVENTFULLOPEN,
		ShowHelp             = CC_SHOWHELP,
		EnableHook           = CC_ENABLEHOOK,
		EnableTemplate       = CC_ENABLETEMPLATE,
		EnableTemplateHandle = CC_ENABLETEMPLATEHANDLE,
		SolidColor           = CC_SOLIDCOLOR,
		AnyColor             = CC_ANYCOLOR);
#pragma endregion

#pragma region Properties
public: // Property - Parent
	/* W */ inline auto  &Parent(HWND hWnd) reflect_to_self(this->hwndOwner = hWnd);
	/* R */ inline Window Parent() const reflect_as(this->hwndOwner);
public: // Property - Module
	/* W */ inline auto   &Module(HINSTANCE hMod) reflect_to_self(this->hInstance = (HWND &)hMod);
	/* R */ inline CModule Module() const reflect_as((HINSTANCE &)this->hInstance);
public: // Property - Parent
	/* R */ inline CColorSet *CustColors() const reflect_as((CColorSet *)this->lpCustColors);
public: // Property - Styles
	/* W */ inline auto &Styles(Style style) reflect_to_self(this->Flags = style.yield());
	/* R */ inline Style Styles() const reflect_as(force_cast<Style>(this->Flags));
public: // Property - Name
	/* W */ inline auto        &Name(LPCTSTR lpTemplateName) reflect_to_self(this->lpTemplateName = lpTemplateName);
	/* R */ inline const String Name() const reflect_as(CString(this->lpTemplateName, MaxLenTitle));
public: // Property - Result
	/* W */ inline auto    &Result(COLORREF rgb) reflect_to_self(this->rgbResult = rgb);
	/* R */ inline ColorRGB Result() const reflect_as(this->rgbResult);
//	LPCCHOOKPROC lpfnHook;
#pragma endregion

	inline bool Choose() reflect_as(ChooseColor(this));
};

class ChooserFont : protected CHOOSEFONT {
protected:
	FontLogic crLogFont;
public:

	ChooserFont() : CHOOSEFONT{ 0 } {
		this->lStructSize = sizeof(CHOOSEFONT);
		this->lpLogFont = &crLogFont;
	}

#pragma region Style
	enum_flags(Style, DWORD,
		ScreenFonts          = CF_SCREENFONTS,
		PrinterFonts         = CF_PRINTERFONTS,
		ShowHelp             = CF_SHOWHELP,
		EnableHook           = CF_ENABLEHOOK,
		EnableTemplate       = CF_ENABLETEMPLATE,
		EnableTemplateHandle = CF_ENABLETEMPLATEHANDLE,
		InitTologFontStruct  = CF_INITTOLOGFONTSTRUCT,
		UseStyle             = CF_USESTYLE,
		Effects              = CF_EFFECTS,
		Apply                = CF_APPLY,
		Ansionly             = CF_ANSIONLY,
		NoVectorFonts        = CF_NOVECTORFONTS,
		NoSimulations        = CF_NOSIMULATIONS,
		LimitSize            = CF_LIMITSIZE,
		FixedPitchOnly       = CF_FIXEDPITCHONLY,
		WYSIWYG              = CF_WYSIWYG,
		ForceFontExist       = CF_FORCEFONTEXIST,
		ScalableOnly         = CF_SCALABLEONLY,
		TTOnly               = CF_TTONLY,
		NoFaceSel            = CF_NOFACESEL,
		NoStyleSel           = CF_NOSTYLESEL,
		NoSizeSel            = CF_NOSIZESEL,
		SelectScript         = CF_SELECTSCRIPT,
		NoScriptSel          = CF_NOSCRIPTSEL,
		NoVertFonts          = CF_NOVERTFONTS,
		InActiveFonts        = CF_INACTIVEFONTS);
	enum_flags(FontType, WORD,
		Simulated = SIMULATED_FONTTYPE,
		Printer   = PRINTER_FONTTYPE,
		Screen    = SCREEN_FONTTYPE,
		Bold      = BOLD_FONTTYPE,
		Italic    = ITALIC_FONTTYPE,
		Regular   = REGULAR_FONTTYPE);
#pragma endregion

#pragma region Properties
public: // Property - Parent
	/* W */ inline auto  &Parent(HWND hWnd) reflect_to_self(this->hwndOwner = hWnd);
	/* R */ inline Window Parent() const reflect_as(this->hwndOwner);
public: // Property - Module
	/* W */ inline auto   &Module(HINSTANCE hMod) reflect_to_self(this->hInstance = hMod);
	/* R */ inline CModule Module() const reflect_as((HINSTANCE &)this->hInstance);
public: // Property - Styles
	/* W */ inline auto &Styles(Style style) reflect_to_self(this->Flags = style.yield());
	/* R */ inline Style Styles() const reflect_as(force_cast<Style>(this->Flags));
public: // Property - Name
	/* W */ inline auto        &Name(LPCTSTR name) reflect_to_self(this->lpTemplateName = name);
	/* R */ inline const String Name() const reflect_as(CString(this->lpTemplateName, MaxLenTitle));
public: // Property - LogFont
	/* R */ inline const FontLogic &LogFont() const reflect_as(crLogFont);
public: // Property - FontTypes
	/* W */ inline auto    &FontTypes(FontType ft) reflect_to_self(this->nFontType = ft.yield());
	/* R */ inline FontType FontTypes() const reflect_as(force_cast<FontType>(this->nFontType));
public: // Property - SizeMin
	/* W */ inline auto &SizeMin(INT nSizeMin) reflect_to_self(this->nSizeMin = nSizeMin);
	/* R */ inline INT   SizeMin() const reflect_as(this->nSizeMin);
public: // Property - SizeMax
	/* W */ inline auto &SizeMax(INT nSizeMax) reflect_to_self(this->nSizeMax = nSizeMax);
	/* R */ inline INT   SizeMax() const reflect_as(this->nSizeMax);
public: // Property - PointSize
	/* W */ inline auto &PointSize(INT iPointSize) reflect_to_self(this->iPointSize = iPointSize);
	/* R */ inline INT   PointSize() const reflect_as(this->iPointSize);
public: // Property - Color
	/* W */ inline auto    &Color(COLORREF rgbColors) reflect_to_self(this->rgbColors = rgbColors);
	/* R */ inline ColorRGB Color() const reflect_as(this->rgbColors);
public: // Property - 
	// LPCFHOOKPROC    lpfnHook;
	// LPARAM          lCustData;
	// HDC             hDC;
	// LPWSTR          lpszStyle;
#pragma endregion

	inline bool Choose() reflect_as(ChooseFont(this));
};

class ChooserFile : protected OPENFILENAME {
protected:
	String strCustomFilter;
	String strFile{ (size_t)MaxLenPath };
	String strFileTitle;
public:
	ChooserFile() : OPENFILENAME{ 0 } {
		this->lStructSize = sizeof(OPENFILENAME);
		CustomFilter(strCustomFilter);
		File(strFile);
		FileTitle(strFileTitle);
	}

#pragma region Style
	enum_flags(Style, DWORD,
		ReadOnly             = OFN_READONLY,
		OverwritePrompt      = OFN_OVERWRITEPROMPT,
		HideReadOnly         = OFN_HIDEREADONLY,
		NoChangeDir          = OFN_NOCHANGEDIR,
		ShowHelp             = OFN_SHOWHELP,
		EnableHook           = OFN_ENABLEHOOK,
		EnableTemplate       = OFN_ENABLETEMPLATE,
		EnableTemplateHandle = OFN_ENABLETEMPLATEHANDLE,
		NoValidate           = OFN_NOVALIDATE,
		AllowMultiSelect     = OFN_ALLOWMULTISELECT,
		ExtensionDifferent   = OFN_EXTENSIONDIFFERENT,
		PathMustExist        = OFN_PATHMUSTEXIST,
		FileMustExist        = OFN_FILEMUSTEXIST,
		CreatePrompt         = OFN_CREATEPROMPT,
		ShareAware           = OFN_SHAREAWARE,
		NoReadOnlyReturn     = OFN_NOREADONLYRETURN,
		NoTestFileCreate     = OFN_NOTESTFILECREATE,
		NoNetworkButton      = OFN_NONETWORKBUTTON,
		NoLongNames          = OFN_NOLONGNAMES,
		Explorer             = OFN_EXPLORER,
		NodeReferenceLinks   = OFN_NODEREFERENCELINKS,
		LongNames            = OFN_LONGNAMES,
		EnableIncludeNotify  = OFN_ENABLEINCLUDENOTIFY,
		EnableSizing         = OFN_ENABLESIZING,
		DontAddToRecent      = OFN_DONTADDTORECENT,
		ForcesHowHidden      = OFN_FORCESHOWHIDDEN
	);
#pragma endregion

#pragma region Properties
public: // Property - Parent
	/* W */ inline auto  &Parent(HWND hWnd) reflect_to_self(this->hwndOwner = hWnd);
	/* R */ inline Window Parent() const reflect_as(this->hwndOwner);
public: // Property - Module
	/* W */ inline auto   &Module(HINSTANCE hMod) reflect_to_self(this->hInstance = hMod);
	/* R */ inline CModule Module() const reflect_as((HINSTANCE &)this->hInstance);
public: // Properties - Style
	/* W */ inline auto &Styles(Style Flags) reflect_to_self(this->Flags = Flags.yield());
	/* R */ inline Style Styles() const reflect_as(force_cast<Style>(this->Flags));
public: // Properties - FileOffset
	/* R */ inline WORD FileOffset() const reflect_as(this->nFileOffset);
public: // Properties - FileExtension
	/* R */ inline WORD FileExtension() const reflect_as(this->nFileExtension);
public: // Properties - File
	/* W */ inline ChooserFile &File(String &strFile) reflect_to_self(this->lpstrFile = this->strFile = strFile, this->nMaxFile = (DWORD)this->strFile.Length());
	/* W */ inline ChooserFile &File(String &&strFile) reflect_to_self(this->lpstrFile = this->strFile = strFile, this->nMaxFile = (DWORD)this->strFile.Length());
	/* R */ inline auto        &File() const reflect_as(this->strFile);
public: // Properties - FileTitle
	/* W */ inline ChooserFile &FileTitle(String &strFileTitle) reflect_to_self(this->lpstrFileTitle = this->strFileTitle = strFileTitle, this->nMaxFileTitle = (DWORD)this->strFileTitle);
	/* W */ inline ChooserFile &FileTitle(String &&strFileTitle) reflect_to_self(this->lpstrFileTitle = this->strFileTitle = strFileTitle, this->nMaxFileTitle = (DWORD)this->strFileTitle);
	/* R */ inline auto        &FileTitle() const reflect_as(this->strFileTitle);
public: // Properties - CustomFilter
	/* W */ inline ChooserFile &CustomFilter(String &strCustomFilter) reflect_to_self(this->lpstrCustomFilter = this->strCustomFilter = strCustomFilter, this->nMaxCustFilter = (DWORD)this->strCustomFilter.Length());
	/* W */ inline ChooserFile &CustomFilter(String &&strCustomFilter) reflect_to_self(this->lpstrCustomFilter = this->strCustomFilter = strCustomFilter, this->nMaxCustFilter = (DWORD)this->strCustomFilter.Length());
	/* R */ inline auto        &CustomFilter() const reflect_as(this->strCustomFilter);
public: // Properties - Filter
	/* W */ inline auto   &Filter(LPCTSTR lpstrFilter) reflect_to_self(this->lpstrFilter = lpstrFilter);
	/* R */ inline LPCTSTR Filter() const reflect_as(this->lpstrFilter);
public: // Properties - FilterIndex
	/* W */ inline auto &FilterIndex(DWORD nFilterIndex) reflect_to_self(this->nFilterIndex = nFilterIndex);
	/* R */ inline DWORD FilterIndex() const reflect_as(this->nFilterIndex);
public: // Properties - InitialDir
	/* W */ inline auto        &InitialDir(LPCTSTR lpstrInitialDir) reflect_to_self(this->lpstrInitialDir = lpstrInitialDir);
	/* R */ inline const String InitialDir() const reflect_as(CString(this->lpstrInitialDir, MaxLenPath));
public: // Properties - Title
	/* W */ inline auto        &Title(LPCTSTR lpstrTitle) reflect_to_self(this->lpstrTitle = lpstrTitle);
	/* R */ inline const String Title() const reflect_as(CString(this->lpstrTitle, MaxLenTitle));
public: // Properties - DefExt
	/* R */ inline const String DefExt() const reflect_as(CString(this->lpstrDefExt, MaxLenTitle));
public: // Property - Name
	/* W */ inline auto        &Name(LPCTSTR name) reflect_to_self(this->lpTemplateName = name);
	/* R */ inline const String Name() const reflect_as(CString(this->lpTemplateName, MaxLenTitle));
#pragma endregion

	inline bool OpenFile() reflect_as(GetOpenFileName(this));
	inline bool SaveFile() reflect_as(GetSaveFileName(this));
};

#pragma region Dialog Template
bool PushHeap(void *&pHeap, size_t &maxSize, const void *pData, size_t dataSize) {
	if (dataSize > maxSize) return true;
	memcpy_s(pHeap, maxSize, pData, dataSize);
	maxSize -= dataSize;
	(uint8_t *&)pHeap += dataSize;
	return false;
}
template<class AnyType>
bool PushHeap(void *&pHeap, size_t &maxSize, const AnyType &type) reflect_as(PushHeap(pHeap, maxSize, &type, sizeof(AnyType)));
template<>
bool PushHeap<String>(void *&pHeap, size_t &maxSize, const String &str) reflect_as(PushHeap(pHeap, maxSize, str, str.Size()));

class DCtl {
	DLGITEMTEMPLATE dit = { 0 };
	WORD classId = 0;
	String className;
	String caption;
	WORD sizeParam = 0;
public:
	enum_class(C, WORD, 
		Button    = 0x0080, // See Microsoft Learn
		Edit      = 0x0081,
		Static    = 0x0082,
		ListBox   = 0x0083,
		Scrollbar = 0x0084,
		ComboBox  = 0x0085);
public:
	DCtl(C classId) : classId(classId.yield()) {}
	DCtl(const DCtl &dc) : dit(dc.dit), classId(dc.classId),
		className(&dc.className), caption(&dc.caption), sizeParam(dc.sizeParam) {}
	DCtl(LPCTSTR pCaption, C classId = C::Static) :
		classId(classId.yield()), caption(+CString(pCaption, MaxLenTitle)) {}
	DCtl(LPCTSTR pCaption, WORD id) : caption(+CString(pCaption, MaxLenTitle)) { dit.id = id; }
public:
	inline auto &Style(DWORD style) reflect_to_self(dit.style = style);
	inline auto &StyleEx(DWORD exStyle) reflect_to_self(dit.dwExtendedStyle = exStyle);
	inline auto &Position(short x, short y) reflect_to_self(dit.x = x, dit.y = y);
	inline auto &Size(short cx, short cy) reflect_to_self(dit.cx = cx, dit.cy = cy);
	inline auto &ID(WORD id) reflect_to_self(dit.id = id);
	inline auto &Class(LPCTSTR pClassname) reflect_to_self(className = +CString(pClassname, MaxLenClass));
	inline auto &Caption(LPCTSTR pCaption) reflect_to_self(caption = +CString(pCaption, MaxLenTitle));
private:
	friend class DBox;
	size_t HeapSize() {
		return
			sizeof(dit) +
			(classId ? sizeof(WORD) + sizeof(classId) : className.Size()) +
			caption.Size() +
			sizeof(sizeParam) + sizeParam;
	}
	bool PushToHeap(void *&pHeap, size_t &maxSize) {
		if (PushHeap(pHeap, maxSize, dit))
			return true;
		if (classId) {
			WORD aClassId[2] = { 0xFFFF, classId };
			if (PushHeap(pHeap, maxSize, aClassId))
				return true;
		}
		elif (PushHeap(pHeap, maxSize, className))
			return true;
		if (PushHeap(pHeap, maxSize, caption))
			return true;
		if (PushHeap(pHeap, maxSize, sizeParam))
			return true;
		return 0;
	}
};
class DBox {
	DLGTEMPLATE *pHeap = nullptr;
	DLGTEMPLATE dt = { 0 };
	WORD menuId = 0;
	String menuName;
	WORD classId = 0;
	String className;
	String caption;
	std::vector<DCtl> dits;
public:
	DBox() {}
	DBox(LPCTSTR pCaption) : caption(+CString(pCaption, MaxLenTitle)) {}
	~DBox() {
		if (pHeap) {
			free(pHeap);
			pHeap = nullptr;
		}
	}
public:
	inline auto &Style(DWORD style) reflect_to_self(dt.style = style);
	inline auto &StyleEx(DWORD exStyle) reflect_to_self(dt.dwExtendedStyle = exStyle);
	inline auto &Position(short x, short y) reflect_to_self(dt.x = x, dt.y = y);
	inline auto &Size(short cx, short cy) reflect_to_self(dt.cx = cx, dt.cy = cy);
	inline auto &Caption(LPCTSTR pCaption) reflect_to_self(caption = +CString(pCaption, MaxLenTitle));
private:
	size_t HeapSize0() {
		return
			sizeof(dt) +
			(menuId ? sizeof(WORD) + sizeof(menuId) : menuName.Size()) +
			(classId ? sizeof(WORD) + sizeof(classId) : className.Size()) +
			caption.Size() +
			/************* style & DS_SETFONT ************/ 0;
	}
	bool PushToHeap0(void *&pHeap, size_t &maxSize) {
		if (PushHeap(pHeap, maxSize, dt))
			return true;
		if (menuId) {
			WORD aMenuId[2] = { 0xFFFF, menuId };
			if (PushHeap(pHeap, maxSize, aMenuId))
				return true;
		}
		elif (PushHeap(pHeap, maxSize, menuName))
			return true;
		if (classId) {
			WORD aClassId[2] = { 0xFFFF, classId };
			if (PushHeap(pHeap, maxSize, aClassId))
				return true;
		}
		elif (PushHeap(pHeap, maxSize, className))
			return true;
		if (PushHeap(pHeap, maxSize, caption))
			return true;
		return false;
	}
private:
	inline size_t HeapSize() {
		auto size = HeapSize0();
		for (auto &dit : dits)
			size += (dit.HeapSize() + 3) & ~3; // Align to DWORD // See Microsoft Learn for more
		return size;
	}
	inline bool PushToHeap(void *&pHeap, size_t &maxSize) {
		if (PushToHeap0(pHeap, maxSize))
			return true;
		for (auto &dit : dits) {
			if (dit.PushToHeap(pHeap, maxSize))
				return true;
			// Align to DWORD // See Microsoft Learn for more
			(uint8_t *&)pHeap += 3;
			(LONG_PTR &)pHeap &= ~3;
		}
		return false;
	}
public:
	inline auto &add(DCtl &&dc) reflect_to_self(dits.push_back(dc));
	inline operator LPDLGTEMPLATE() {
		if (pHeap) return pHeap;
		auto maxSize = HeapSize();
		auto pHeap = this->pHeap = (LPDLGTEMPLATE)malloc(maxSize);
		if (PushToHeap((void *&)pHeap, maxSize)) {
			DBox::~DBox();
			return nullptr;
		}
		this->pHeap->cdit = (WORD)dits.size();
		return this->pHeap;
	}
};
#pragma endregion

}
