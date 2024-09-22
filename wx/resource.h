#pragma once

#include <vector>

#include "./type.h"
#include "./file.h"

namespace WX {

#pragma region GDI Objectjs

#pragma region GObject
template<class AnyChild, class BaseHandle = HGDIOBJ>
class GObjectBase;
using GObject = GObjectBase<void>;
template<class AnyChild, class BaseHandle>
class GObjectBase : public ChainExtend<GObjectBase<AnyChild, BaseHandle>, AnyChild> {
	misuse_assert((std::is_convertible_v<BaseHandle, HGDIOBJ>),
				 "BaseHandle must can be converted to HANDLE");
public:
	using Child = KChain<GObjectBase, AnyChild>;
protected:
	friend union RefAs<GObjectBase>;
	mutable HGDIOBJ hobj = O;
	GObjectBase(HGDIOBJ h) : hobj(h) {}
	GObjectBase(const GObjectBase &h) : hobj(h.hobj) reflect_to(h.hobj = O);
public:
	GObjectBase() {}
	GObjectBase(Null) {}
	GObjectBase(GObjectBase &obj) : hobj(obj.hobj) { obj.hobj = O; }
	GObjectBase(GObjectBase &&obj) : hobj(obj.hobj) { obj.hobj = O; }
	~GObjectBase() reflect_to(Delete());

	inline bool Delete() {
		if (hobj)
			if (!DeleteObject(hobj))
				return false;
		hobj = O;
		return true;
	}

	inline operator BaseHandle() const reflect_as(reuse_as<BaseHandle>(hobj));

	inline Child &operator=(Child &obj) reflect_to_child(std::swap(obj.hobj, hobj));
	inline Child &operator=(Child &&obj) reflect_to_child(std::swap(obj.hobj, hobj));
	inline const Child &operator=(const Child &obj) const reflect_to_child(std::swap(obj.hobj, hobj));

	inline static auto &Attach(BaseHandle &hObj) reflect_as(reuse_as<AnyChild>(hObj));
	inline static const auto &Attach(const BaseHandle &hObj) reflect_as(reuse_as<const AnyChild>(hObj));
};
using CGObject = RefAs<GObject>;
#define BaseOf_GDI(name, idt) name : public WX::GObjectBase<name, idt>
#pragma endregion

#pragma region Bitmap
enum_class(BitmapCompressions, DWORD,
	RGB         = BI_RGB,
	RLE8        = BI_RLE8,
	RLE4        = BI_RLE4,
	BitFields   = BI_BITFIELDS,
	JPEG        = BI_JPEG,
	PNG         = BI_PNG);
#pragma pack(2)
struct BitmapHeader : protected BITMAPFILEHEADER, protected BITMAPINFO {
	BitmapHeader() : BITMAPFILEHEADER{ 0 }, BITMAPINFO{ { 0 } } {
		this->bfType = *(const WORD *)"BM";
		this->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		this->bmiHeader.biCompression = BI_RGB;
	}
public: // Property - PaletteSize
	/* W */ inline auto &PaletteSize(DWORD dwSize) reflect_to_self(this->bfOffBits = dwSize + sizeof(*this) - 4);
	/* R */ inline DWORD PaletteSize() const reflect_as(this->bfOffBits - sizeof(*this) - 4);
public: // Property - ColorsSize
	/* W */ inline auto &ColorsSize(DWORD dwSize) reflect_to_self(this->bfSize = this->bfOffBits + dwSize);
	/* R */ inline DWORD ColorsSize() const reflect_as(this->bfSize - this->bfOffBits);
public: // Property - FileSize
	/* R */ inline DWORD FileSize() const reflect_as(this->bfSize);
public: // Property - Width
	/* W */ inline auto &Width(LONG biWidth) reflect_to_self(this->bmiHeader.biWidth = biWidth);
	/* R */ inline LONG  Width() const reflect_as(this->bmiHeader.biWidth);
public: // Property - Height
	/* W */ inline auto &Height(LONG biHeight) reflect_to_self(this->bmiHeader.biHeight = biHeight);
	/* R */ inline LONG  Height() const reflect_as(this->bmiHeader.biHeight);
public: // Property - Size
	/* W */ inline auto &Size(LSize sz) reflect_to_self(bmiHeader.biWidth = sz.cx, bmiHeader.biHeight = sz.cy);
	/* R */ inline LSize Size() const reflect_as({ bmiHeader.biWidth, bmiHeader.biHeight });
public: // Property - Compression
	/* W */ inline auto &Compression(BitmapCompressions biCompression) reflect_to_self(this->bmiHeader.biCompression = biCompression.yield());
	/* R */ inline BitmapCompressions Compression() const reflect_as(force_cast<BitmapCompressions>(this->bmiHeader.biCompression));
public: // Property - SizeImage
	/* R */ inline DWORD SizeImage() const reflect_as(this->bmiHeader.biSizeImage);
public: // Property - PixelsPerMeter
	/* W */ inline auto &PixelsPerMeter(LSize sz) reflect_to_self(bmiHeader.biXPelsPerMeter = sz.cx, bmiHeader.biYPelsPerMeter = sz.cy);
	/* R */ inline LSize PixelsPerMeter() const reflect_as({ bmiHeader.biXPelsPerMeter, bmiHeader.biYPelsPerMeter });
public: // Property - Planes
	/* W */ inline auto &Planes(WORD biPlanes) reflect_to_self(this->bmiHeader.biPlanes = biPlanes);
	/* R */ inline WORD  Planes() const reflect_as(this->bmiHeader.biPlanes);
public: // Property - BitsPerPixel
	/* W */ inline auto &BitsPerPixel(WORD biBitCount) reflect_to_self(this->bmiHeader.biBitCount = biBitCount);
	/* R */ inline WORD  BitsPerPixel() const reflect_as(this->bmiHeader.biBitCount);
public: // Property - ColorUsed
	/* W */ inline auto &ColorUsed(DWORD biClrUsed) reflect_to_self(this->bmiHeader.biClrUsed = biClrUsed);
	/* R */ inline DWORD ColorUsed() const reflect_as(this->bmiHeader.biClrUsed);
public: // Property - ColorImportant
	/* W */ inline auto &ColorImportant(DWORD biClrImportant) reflect_to_self(this->bmiHeader.biClrImportant = biClrImportant);
	/* R */ inline DWORD ColorImportant() const reflect_as(this->bmiHeader.biClrImportant);
public: // Property - PaletteStart
	// /* W */ inline auto PaletteStart() reflect_to_self(this->bmiColors);
	/* R */ inline auto PaletteStart() const reflect_as(this->bmiColors);
public: // Property - Colors
	// /* W */ inline void *ColorsStart() reflect_to_self(((uint8_t *)PaletteStart()) + PaletteSize());
	/* R */ inline void *ColorsStart() const reflect_as(((uint8_t *)PaletteStart()) + PaletteSize());
public:
	inline operator LPBITMAPFILEHEADER() reflect_as(this);
	inline operator const BITMAPFILEHEADER *() const reflect_as(this);
	inline operator LPBITMAPINFO() reflect_as(this);
	inline operator const BITMAPINFO *() const reflect_as(this);
};
#pragma pack()
struct BitmapLogic : protected BITMAP {
	BitmapLogic() : BITMAP{ 0 } {}
public: // Property - Width
	/* W */ inline auto &Width(LONG bmWidth) reflect_to_self(this->bmWidth = bmWidth);
	/* R */ inline LONG  Width() const reflect_as(this->bmWidth);
public: // Property - Height
	/* W */ inline auto &Height(LONG bmHeight) reflect_to_self(this->bmHeight = bmHeight);
	/* R */ inline LONG  Height() const reflect_as(this->bmHeight);
public: // Property - Size
	/* W */ inline auto &Size(LSize sz) reflect_to_self(this->bmWidth = sz.cx, this->bmHeight = sz.cy);
	/* R */ inline LSize Size() const reflect_as({ this->bmWidth, this->bmHeight });
public: // Property - WidthBytes
	/* W */ inline auto &WidthBytes(LONG bmWidthBytes) reflect_to_self(this->bmWidthBytes = bmWidthBytes);
	/* R */ inline LONG  WidthBytes() const reflect_as(this->bmWidthBytes);
public: // Property - Planes
	/* W */ inline auto &Planes(WORD bmPlanes) reflect_to_self(this->bmPlanes = bmPlanes);
	/* R */ inline WORD  Planes() const reflect_as(this->bmPlanes);
public: // Property - BitsPerPixel
	/* W */ inline auto &BitsPerPixel(WORD bmBitsPixel) reflect_to_self(this->bmBitsPixel = bmBitsPixel);
	/* R */ inline WORD  BitsPerPixel() const reflect_as(this->bmBitsPixel);
public: // Property - Colors
	/* W */ inline auto &Colors(LPVOID bmBits) reflect_to_self(this->bmBits = bmBits);
	/* R */ inline LPVOID Colors() const reflect_as(this->bmBits);
public:
	inline HBITMAP Create() const assert_reflect_as(auto h = CreateBitmapIndirect(this), h);
	inline operator HBITMAP() const reflect_as(Create());
	inline BITMAP *operator&() reflect_as(this);
	inline const BITMAP *operator&() const reflect_as(this);
};
class BaseOf_GDI(Bitmap, HBITMAP) {
public:
	using super = GObjectBase<Bitmap, HBITMAP>;
	using Header = BitmapHeader;
	using File = BitmapHeader;
	using Logic = BitmapLogic;
protected:
	friend union RefAs<Bitmap>;
	Bitmap(HBITMAP h) : super(h) {}
	Bitmap(const Bitmap &b) : super(b) {}
public:
	Bitmap() {}
	Bitmap(Null) {}
	Bitmap(Bitmap &b) : super(b) {}
	Bitmap(Bitmap &&b) : super(b) {}
	~Bitmap() reflect_to(Destroy());

	using super::operator=;

	inline Logic Log() const reflect_to(Logic log, GetObject(self, sizeof(log), reinterpret_cast<LPSTR>(&log)), log);

	class CreateStruct {
		int nWidth, nHeight;
		UINT nPlanes = 1, nBitsPerPixel = 32;
		LPCVOID lpBits = O;
	public:
		CreateStruct(LSize s) : nWidth(s.cx), nHeight(s.cy) {}
	public: // Property - Width
		/* W */ inline auto &Width(LONG nWidth) reflect_to_self(this->nWidth = nWidth);
		/* R */ inline LONG  Width() const reflect_as(this->nWidth);
	public: // Property - Height
		/* W */ inline auto &Height(LONG nHeight) reflect_to_self(this->nHeight = nHeight);
		/* R */ inline LONG  Height() const reflect_as(this->nHeight);
	public: // Property - Size
		/* W */ inline auto &Size(LSize sz) reflect_to_self(this->nWidth = sz.cx, this->nHeight = sz.cy);
		/* R */ inline LSize Size() const reflect_as({ this->nWidth, this->nHeight });
	public: // Property - Planes
		/* W */ inline auto &Planes(UINT nPlanes) reflect_to_self(this->nPlanes = nPlanes);
		/* R */ inline UINT  Planes() const reflect_as(this->nPlanes);
	public: // Property - BitsPerPixel
		/* W */ inline auto &BitsPerPixel(UINT nBitsPerPixel) reflect_to_self(this->nBitsPerPixel = nBitsPerPixel);
		/* R */ inline UINT  BitsPerPixel() const reflect_as(this->nBitsPerPixel);
	public: // Property - Colors
		/* W */ inline auto &Colors(LPCVOID lpBits) reflect_to_self(this->lpBits = lpBits);
		/* R */ inline LPCVOID Colors() const reflect_as(this->lpBits);
	public:
		inline Bitmap Create() const assert_reflect_as(auto h = CreateBitmap(nWidth, nHeight, nPlanes, nBitsPerPixel, lpBits), h);
		inline operator Bitmap() const reflect_as(Create());
	};
	inline static CreateStruct Create(LSize s = 0) reflect_as(s);

	inline static Bitmap Create(const BITMAPINFO *pbi, void **ppbits, HANDLE hSection = O, DWORD offset = 0) assert_reflect_as(auto h = CreateDIBSection(O, pbi, DIB_RGB_COLORS, ppbits, hSection, offset), h);
	inline static Bitmap Create(const BITMAPINFO *pbi, HANDLE hSection = O, DWORD offset = 0) assert_reflect_as(auto h = CreateDIBSection(O, pbi, DIB_RGB_COLORS, O, hSection, offset), h);

	inline static Bitmap From(HDC hDC, LSize s) reflect_as(CreateCompatibleBitmap(hDC, s.cx, s.cy));

	inline bool Destroy() reflect_as(super::Delete());

	inline auto &Restretch(LSize sz) assert_reflect_as_self((super::hobj = (HBITMAP)CopyImage(self, IMAGE_BITMAP, sz.cx, sz.cy, LR_DEFAULTSIZE)));
	inline Bitmap StretchNew(LSize sz) const assert_reflect_as(auto h = (HBITMAP)CopyImage(self, IMAGE_BITMAP, sz.cx, sz.cy, LR_DEFAULTCOLOR), h);
	inline Bitmap Clone() const reflect_as(StretchNew(Size()));

#pragma region Properties
public: // Property - Size
	/* R */ inline LSize Size() const assert_reflect_to(LSize sz, GetBitmapDimensionEx(self, &sz), sz);
public: // Property - SizeBytes
	/* R */ inline size_t SizeBytes() const {
		auto &&log = Log();
		return (((log.Width() * log.Planes() /* nPlanes */ * log.BitsPerPixel() /* nBitCount */ + 15) >> 4) << 1) * log.Height();
	}
public: // Property - Colors
	/* W */ inline auto &GetColors(void *lpBits, DWORD cb) const assert_reflect_as_self(GetBitmapBits(self, cb, lpBits));
	/* R */ inline auto &SetColors(const void *lpBits, DWORD cb) assert_reflect_as_self(SetBitmapBits(self, cb, lpBits));
#pragma endregion

};
using BitmapLogic = Bitmap::Logic;
using CBitmap = RefAs<Bitmap>;
#pragma endregion

#pragma region Pen
struct Pen;
using CPen = RefAs<Pen>;
struct BaseOf_GDI(Pen, HPEN) {
public:
	using super = GObjectBase<Pen, HPEN>;
protected:
	friend union RefAs<Pen>;
	Pen(HPEN h) : super(h) {}
	Pen(const Pen &p) : super(p) {}
public:
	Pen() {}
	Pen(Null) {}
	Pen(Pen &p) : super(p) {}
	Pen(Pen &&p) : super(p) {}
	
	using super::operator=;

	inline static Pen White() reflect_as((HPEN)::GetStockObject(WHITE_PEN));
	inline static Pen Black() reflect_as((HPEN)::GetStockObject(BLACK_PEN));
	inline static Pen Null()  reflect_as((HPEN)::GetStockObject(NULL_PEN));
	inline static Pen DC()    reflect_as((HPEN)::GetStockObject(DC_PEN));

	inline static Pen CreateSolid(COLORREF rgb, int nWidth = 0)       assert_reflect_as(auto h = CreatePen(PS_SOLID, nWidth, rgb), h);
	inline static Pen CreateDash(COLORREF rgb, int nWidth = 0)        assert_reflect_as(auto h = CreatePen(PS_DASH, nWidth, rgb), h);
	inline static Pen CreateDot(COLORREF rgb, int nWidth = 0)         assert_reflect_as(auto h = CreatePen(PS_DOT, nWidth, rgb), h);
	inline static Pen CreateDashDot(COLORREF rgb, int nWidth = 0)     assert_reflect_as(auto h = CreatePen(PS_DASHDOT, nWidth, rgb), h);
	inline static Pen CreateDashDotDot(COLORREF rgb, int nWidth = 0)  assert_reflect_as(auto h = CreatePen(PS_DASHDOTDOT, nWidth, rgb), h);
	inline static Pen Create(COLORREF rgb, int nWidth = 0)            assert_reflect_as(auto h = CreatePen(PS_NULL, nWidth, rgb), h);
	inline static Pen CreateInsideFrame(COLORREF rgb, int nWidth = 0) assert_reflect_as(auto h = CreatePen(PS_INSIDEFRAME, nWidth, rgb), h);
	inline static Pen CreateUserStyle(COLORREF rgb, int nWidth = 0)   assert_reflect_as(auto h = CreatePen(PS_USERSTYLE, nWidth, rgb), h);
	inline static Pen CreateAlternate(COLORREF rgb, int nWidth = 0)   assert_reflect_as(auto h = CreatePen(PS_ALTERNATE, nWidth, rgb), h);
};
#pragma endregion

#pragma region Brush
enum_class(SysColor, int,
	ScrollBar               = COLOR_SCROLLBAR,
	Background              = COLOR_BACKGROUND,
	ActiveCaption           = COLOR_ACTIVECAPTION,
	InactiveCaption         = COLOR_INACTIVECAPTION,
	Menu                    = COLOR_MENU,
	Window                  = COLOR_WINDOW,
	WindowFrame             = COLOR_WINDOWFRAME,
	MenuText                = COLOR_MENUTEXT,
	WindowText              = COLOR_WINDOWTEXT,
	CaptionText             = COLOR_CAPTIONTEXT,
	ActiveBorder            = COLOR_ACTIVEBORDER,
	InactiveBorder          = COLOR_INACTIVEBORDER,
	AppWorkspace            = COLOR_APPWORKSPACE,
	HighLight               = COLOR_HIGHLIGHT,
	HighLightText           = COLOR_HIGHLIGHTTEXT,
	BtnFace                 = COLOR_BTNFACE,
	BtnShadow               = COLOR_BTNSHADOW,
	GrayText                = COLOR_GRAYTEXT,
	BtnText                 = COLOR_BTNTEXT,
	InactiveCaptionText     = COLOR_INACTIVECAPTIONTEXT,
	BtnHighlight            = COLOR_BTNHIGHLIGHT,
	DkShadow3D              = COLOR_3DDKSHADOW,
	Light3D                 = COLOR_3DLIGHT,
	InfoText                = COLOR_INFOTEXT,
	InfoBk                  = COLOR_INFOBK,
	HotLight                = COLOR_HOTLIGHT,
	GradientActiveCaption   = COLOR_GRADIENTACTIVECAPTION,
	GradientInactiveCaption = COLOR_GRADIENTINACTIVECAPTION,
	MenuHiLight             = COLOR_MENUHILIGHT,
	MenuBar                 = COLOR_MENUBAR);
enum_class(HatchStyle, int,
	Horizontal              = HS_HORIZONTAL,    /* ----- */
	Vertical                = HS_VERTICAL,      /* ||||| */
	FDiagonal               = HS_FDIAGONAL,     /* \\\\\ */
	BDiagonal               = HS_BDIAGONAL,     /* ///// */
	Cross                   = HS_CROSS,         /* +++++ */
	DiagCross               = HS_DIAGCROSS      /* xxxxx */);
struct Brush;
using CBrush = RefAs<Brush>;
struct BaseOf_GDI(Brush, HBRUSH) {
public:
	using super = GObjectBase<Brush, HBRUSH>;
protected:
	friend union RefAs<Brush>;
	Brush(HBRUSH h) : super(h) {}
	Brush(const Brush &b) : super(b) {}
public:
	Brush() {}
	Brush(Null) {}
	Brush(Brush &b) : super(b) {}
	Brush(Brush &&b) : super(b) {}
	
	using super::operator=;

	inline static Brush SysColor(WX::SysColor sc) assert_reflect_as(auto hbr = GetSysColorBrush(sc.yield()), hbr);

	inline static Brush White()   reflect_as((HBRUSH)::GetStockObject(WHITE_BRUSH));
	inline static Brush LitGray() reflect_as((HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	inline static Brush Gray()    reflect_as((HBRUSH)::GetStockObject(GRAY_BRUSH));
	inline static Brush DkGray()  reflect_as((HBRUSH)::GetStockObject(DKGRAY_BRUSH));
	inline static Brush Black()   reflect_as((HBRUSH)::GetStockObject(BLACK_BRUSH));
	inline static Brush Null()    reflect_as((HBRUSH)::GetStockObject(NULL_BRUSH));
	inline static Brush DC()      reflect_as((HBRUSH)::GetStockObject(DC_BRUSH));

	inline static Brush CreateSolid(COLORREF rgb)                assert_reflect_as(auto h = CreateSolidBrush(rgb), h);
	inline static Brush CreatePattern(HBITMAP hbm)               assert_reflect_as(auto h = CreatePatternBrush(hbm), h);
	inline static Brush CreateHatch(COLORREF rgb, HatchStyle hs) assert_reflect_as(auto h = CreateHatchBrush(hs.yield(), rgb), h);
};
#pragma endregion

#pragma region Palette
class Palette;
class PaletteEntry : protected PALETTEENTRY {
	friend class Palette;
public:
	PaletteEntry() {}
	PaletteEntry(Null) {}
	PaletteEntry(const PALETTEENTRY &entry) : PALETTEENTRY(entry) {}
	PaletteEntry(BYTE r, BYTE g, BYTE b, BYTE f = 0) : PALETTEENTRY{ r, g, b, f } {}
	PaletteEntry(RGBColor rgb, BYTE f = 0) : PALETTEENTRY{ rgb.Red(), rgb.Green(), rgb.Blue(), f } {}
	PaletteEntry(COLORREF rgb, BYTE f = 0) : PaletteEntry((RGBColor)rgb, f) {}
public: // Property - Red
	/* W */ inline auto &Red(BYTE bRed) reflect_to_self(this->peRed = bRed);
	/* R */ inline BYTE  Red() const reflect_as(this->peRed);
public: // Property - Green
	/* W */ inline auto &Green(BYTE bGreen) reflect_to_self(this->peGreen = bGreen);
	/* R */ inline BYTE  Green() const reflect_as(this->peGreen);
public: // Property - Blue
	/* W */ inline auto &Blue(BYTE bBlue) reflect_to_self(this->peBlue = bBlue);
	/* R */ inline BYTE  Blue() const reflect_as(this->peBlue);
public:
	inline operator RGBColor() const reflect_as({ Red(), Green(), Blue() });
	inline LPPALETTEENTRY operator&() reflect_as(this);
	inline const PALETTEENTRY *operator&() const reflect_as(this);
};
using PalEntry = PaletteEntry;
using CPalette = RefAs<Palette>;
class BaseOf_GDI(Palette, HPALETTE) {
public:
	using Entry = PaletteEntry;
	using super = GObjectBase<Palette, HPALETTE>;
protected:
	friend union RefAs<Palette>;
	Palette(HPALETTE h) : super(h) {}
	Palette(const Palette &p) :super(p) {}
public:
	Palette() {}
	Palette(Null) {}
	Palette(Palette &pal) : super(pal) {}
	Palette(Palette &&pal) : super(pal) {}
	Palette(const Entry *lpEntries, UINT nCount) {
		assert(1 < nCount && nCount <= 256);
		AutoPointer<Heap, LOGPALETTE> hPal;
		hPal.Alloc(sizeof(LOGPALETTE) + (nCount - 1) * sizeof(Entry));
		auto pPal = &hPal;
		pPal->palVersion = 0x300;
		pPal->palNumEntries = (WORD)nCount;
		CopyMemory(&pPal->palPalEntry, lpEntries, sizeof(Entry) * nCount);
		assert(this->hobj = CreatePalette(pPal));
	}
	Palette(std::initializer_list<COLORREF> entries) : Palette((const Entry *)entries.begin(), (UINT)entries.size()) {}
	Palette(std::initializer_list<Entry> entries) : Palette(entries.begin(), (UINT)entries.size()) {}
	Palette(const std::vector<Entry> &entries) : Palette(entries.data(), (UINT)entries.size()) {}

	using super::operator=;

	inline static CPalette Default() reflect_as((HPALETTE)::GetStockObject(DEFAULT_PALETTE));

	inline auto &SetEntries(UINT iStart, const Entry *pPalEntries, UINT cEntries = 1) assert_reflect_as_self(SetPaletteEntries(self, iStart, cEntries, pPalEntries));
	inline auto &GetEntries(UINT iStart, Entry *pPalEntries, UINT cEntries = 1) const assert_reflect_as_self(GetPaletteEntries(self, iStart, cEntries, pPalEntries));

	inline UINT NearestIndex(COLORREF cr) const reflect_as(GetNearestPaletteIndex(self, cr));

#pragma region Properties
public: // Property - Entries
	/* W */ inline auto &Entries(std::vector<Entry> entries) {
		auto count = std::min((size_t)Count(), entries.size());
		assert(count <= 256);
		if (count > 0)
			SetEntries(0, entries.data(), (WORD)(count - 1));
		retself;
	}
	/* R */ inline std::vector<Entry> Entries() const {
		size_t count = Count();
		std::vector<Entry> entries(count);
		if (count > 0)
			GetEntries(0, entries.data(), (WORD)count);
		return entries;
	}
public: // Property - NumEntries
	/* W */ inline auto &Count(WORD nNumEntries) assert_reflect_as_self(ResizePalette(self, nNumEntries));
	/* R */ inline WORD  Count() const assert_reflect_to(WORD w = 0, GetObject(self, sizeof(WORD), &w), w);
public: // Property - Size
	/* R */ inline size_t Size() const reflect_as(Count() * sizeof(Entry));
#pragma endregion

	inline Entry operator[](size_t ind) const reflect_to(Entry entry; GetEntries((UINT)ind, (Entry *)&entry), entry);
};
using CPalette = RefAs<Palette>;
#pragma endregion

#pragma region DC
enum_class(MapModes, int,
	Text             = MM_TEXT,
	LoMetric         = MM_LOMETRIC,
	HiMetric         = MM_HIMETRIC,
	LoEnglish        = MM_LOENGLISH,
	HiEnglish        = MM_HIENGLISH,
	Twips            = MM_TWIPS,
	Isotropic        = MM_ISOTROPIC,
	Anisotropic      = MM_ANISOTROPIC);
enum_flags(Rop, DWORD,
	SrcCopy              = SRCCOPY,     /* dest = source                   */
	SrcPaint             = SRCPAINT,    /* dest = source OR dest           */
	SrcAnd               = SRCAND,      /* dest = source AND dest          */
	SrcInvert            = SRCINVERT,   /* dest = source XOR dest          */
	SrcErase             = SRCERASE,    /* dest = source AND (NOT dest )   */
	SrcCopyNot           = NOTSRCCOPY,  /* dest = (NOT source)             */
	SrcEraseNot          = NOTSRCERASE, /* dest = (NOT src) AND (NOT dest) */
	MergeCopy            = MERGECOPY,   /* dest = (source AND pattern)     */
	MergePaint           = MERGEPAINT,  /* dest = (NOT source) OR dest     */
	PatCopy              = PATCOPY,     /* dest = pattern                  */
	PatPaint             = PATPAINT,    /* dest = DPSnoo                   */
	PatInvert            = PATINVERT,   /* dest = pattern XOR dest         */
	DstInvert            = DSTINVERT,   /* dest = (NOT dest)               */
	Blackness            = BLACKNESS,   /* dest = BLACK                    */
	Whiteness            = WHITENESS,   /* dest = WHITE                    */
	NoMirrorBitmap                = NOMIRRORBITMAP, /* Do not Mirror the bitmap in this call */
	CaptureBlt                    = CAPTUREBLT/* Include layered windows */);
enum_class(Stretches, int,
	And  = BLACKONWHITE,
	Or   = WHITEONBLACK,
	Del  = COLORONCOLOR,
	Half = HALFTONE);
class DeviceCap;
using CDC = RefAs<DeviceCap>;
class BaseOf_GDI(DeviceCap, HDC) {
public:
	using super = GObjectBase<DeviceCap, HDC>;
protected:
	friend union RefAs<DeviceCap>;
	DeviceCap(HDC h) : super(h) {}
	DeviceCap(const DeviceCap &d) : super(d) {}
public:
	DeviceCap() {}
	DeviceCap(Null) {}
	DeviceCap(DeviceCap &dc) : super(dc) {}
	DeviceCap(DeviceCap &&dc) : super(dc) {}

	using super::operator=;

	inline static DeviceCap CreateCompatible(HDC hdc = O) assert_reflect_as(auto hobj = CreateCompatibleDC(hdc), hobj);

	inline bool Delete() {
		if (this->hobj)
			if (!DeleteDC(self))
				return false;
		this->hobj = O;
		return true;
	}
	inline auto Release(HWND hWnd) reflect_as(ReleaseDC(hWnd, self));

//	inline auto &BltStretch(LPoint dstStart, LSize dstSize, Rop rop = Rop::SrcCopy)
	inline auto &BltStretch(LPoint dstStart, LSize dstSize, HDC hdcSrc, LPoint srcStart, LSize srcSize, Rop rop = Rop::SrcCopy) assert_reflect_as_self(::StretchBlt(self, dstStart.x, dstStart.y, dstSize.cx, dstSize.cy, hdcSrc, srcStart.x, srcStart.y, srcSize.cx, srcSize.cy, rop.yield()));
	inline auto &BltStretch(LRect dst, HDC hdcSrc, LRect src, Rop rop = Rop::SrcCopy) reflect_to_self(BltStretch(dst.top_left(), dst.size(), hdcSrc, src.top_left(), src.size(), rop));
	inline auto &BltBit(LPoint dstStart, LSize dstSize, HDC hdcSrc, LPoint srcStart = 0, Rop rop = Rop::SrcCopy) assert_reflect_as_self(::BitBlt(self, dstStart.x, dstStart.y, dstSize.cx, dstSize.cy, hdcSrc, srcStart.x, srcStart.y, rop.yield()));
	inline auto &BltBit(LRect rc, HDC hdcSrc, LPoint srcStart = 0, Rop rop = Rop::SrcCopy) reflect_to_self(BltBit(rc.top_left(), rc.size(), hdcSrc, srcStart, rop));

	inline CGObject Select(HGDIOBJ ho) assert_reflect_as((ho = SelectObject(self, ho)) != HGDI_ERROR, ho);
	inline CPalette Palette(HPALETTE hPal, bool bForceBkgd = false) check_reflect_to(hPal = SelectPalette(self, hPal, bForceBkgd), hPal);

	inline auto&DrawIcon(HICON hIcon, LPoint p = 0) assert_reflect_as_self(::DrawIcon(self, p.x, p.y, hIcon));
//	inline bool DrawText(String text, Rect r = 0) reflect_as(DrawText(self, text.c_str(), text.size(), r, ));
	inline auto&DrawPixel(COLORREF rgb, LPoint p) assert_reflect_as_self(SetPixel(self, p.x, p.y, rgb) != -1);
	template<size_t len>
	inline auto&DrawPolyline(const LPoint(&pts)[len]) assert_reflect_as_self(Polyline(self, pts, len));
	inline auto&DrawPie(LRect rc, LPoint start, LPoint end) assert_reflect_as_self(Pie(self, rc.left, rc.top, rc.right, rc.bottom, start.x, start.y, end.x, end.y));
	inline auto&DrawEllipse(LRect rc) assert_reflect_as_self(Ellipse(self, rc.left, rc.top, rc.right, rc.bottom));
	inline auto&DrawFocus(LRect rc) assert_reflect_as_self(DrawFocusRect(self, rc));
	inline auto&Invert(LRect rc) assert_reflect_as_self(InvertRect(self, rc));
	inline auto&Fill(LRect rc, HBRUSH hbr) assert_reflect_as_self(FillRect(self, rc, hbr));
	inline auto&Fill(HBRUSH hbr) reflect_as(Fill(Size(), hbr));

#pragma region Properties
public: // Property - PenColor
	/* W */ inline auto    &PenColor(COLORREF rgb) assert_reflect_as_self(SetDCPenColor(self, rgb));
	/* R */ inline RGBColor PenColor() const reflect_as(GetDCPenColor(self));
public: // Property - BkColor
	/* W */ inline auto    &BkColor(COLORREF rgb) assert_reflect_as_self(SetBkColor(self, rgb));
	/* R */ inline RGBColor BkColor() const reflect_as(GetBkColor(self));
public: // Property - ViewOrg
	/* W */ inline auto  &ViewOrg(LPoint pt) assert_reflect_as_self(SetViewportOrgEx(self, pt.x, pt.y, O));
	/* R */ inline LPoint ViewOrg() const assert_reflect_to(LPoint pt, GetViewportOrgEx(self, &pt), pt);
public: // Property - ViewExt
	/* W */ inline auto &ViewExt(LSize sz) assert_reflect_as_self(SetViewportExtEx(self, sz.cx, sz.cy, O));
	/* R */ inline LSize ViewExt() const assert_reflect_to(LSize sz, GetViewportExtEx(self, &sz), sz);
public: // Property - WindowOrg
	/* W */ inline auto  &WindowOrg(LPoint pt) assert_reflect_as_self(SetWindowOrgEx(self, pt.x, pt.y, O));
	/* R */ inline LPoint WindowOrg() const assert_reflect_to(LPoint pt, GetWindowOrgEx(self, &pt), pt);
public: // Property - WindowExt
	/* W */ inline auto &WindowExt(LSize sz) assert_reflect_as_self(SetWindowExtEx(self, sz.cx, sz.cy, O));
	/* R */ inline LSize WindowExt() const assert_reflect_to(LSize sz, GetWindowExtEx(self, &sz), sz);
public: // Property - MapMode
	/* W */ inline auto    &MapMode(MapModes mode) assert_reflect_as_self(SetMapMode(self, mode.yield()));
	/* R */ inline MapModes MapMode() const reflect_as(force_cast<MapModes>(GetMapMode(self)));
public: // Property - Size
	/* R */ inline LSize Size() const reflect_as({ GetDeviceCaps(self, HORZRES), GetDeviceCaps(self, VERTRES) });
public: // Property - PaletteRealize
	/* R */ inline UINT PaletteRealize() const reflect_as(RealizePalette(self));
public: // Property - StretchMode
	/* W */ inline auto     &StretchMode(Stretches mode) assert_reflect_as_self(SetStretchBltMode(self, mode.yield()));
	/* R */ inline Stretches StretchMode() const assert_reflect_as(auto mode = GetStretchBltMode(self), force_cast<Stretches>(mode));
#pragma endregion

	inline auto&operator()(HGDIOBJ ho) reflect_to_self(Select(ho));
	inline auto&operator()(Bitmap &ho) reflect_to_self(Select(ho));
	inline auto&operator()(const Pen &ho) reflect_to_self(Select(ho));
	inline auto&operator()(const Brush &ho) reflect_to_self(Select(ho));
	inline auto&operator()(const WX::Palette hPal, bool bForceBkgd = false) reflect_to_self(Palette(hPal, bForceBkgd));
};
using DC = DeviceCap;
#pragma endregion

#pragma endregion

#pragma region Icon & Cursor

#pragma region Icon
struct IconInfo : protected ICONINFO {
public:
	IconInfo(bool fIcon = true) : ICONINFO{ 0 } reflect_to(this->fIcon = fIcon);
	IconInfo(const IconInfo &i) : ICONINFO{ i } {}
	IconInfo(const ICONINFO &i) : ICONINFO{ i } {}
	~IconInfo() {
		if (hbmMask) Bitmap::Attach(hbmMask).Delete();
		if (hbmColor) Bitmap::Attach(hbmColor).Delete();
	}
public: // Property - Hotspot
	/* W */ inline auto &Hotspot(LPoint s) reflect_to_self(this->xHotspot = s.x, this->yHotspot = s.y);
	/* R */ inline LPoint Hotspot() const reflect_as({ (long)this->xHotspot, (long)this->yHotspot });
public: // Property - Masks
	/* W */ inline auto &Mask(HBITMAP hbmMask) reflect_to_self(this->hbmMask = hbmMask);
	/* R */ inline Bitmap &Mask() const reflect_as(Bitmap::Attach(const_cast<HBITMAP &>(this->hbmMask)));
public: // Property - Colors
	/* W */ inline auto &Colors(HBITMAP hbmColor) reflect_to_self(this->hbmColor = hbmColor);
	/* R */ inline Bitmap &Colors() const reflect_as(Bitmap::Attach(const_cast<HBITMAP &>(this->hbmColor)));
public:
	inline PICONINFO operator&() reflect_as(this);
	inline const ICONINFO *operator&() const reflect_as(this);
};
struct IconInfoEx : protected ICONINFOEX {
public:
	IconInfoEx() : ICONINFOEX{ 0 } reflect_to(this->cbSize = sizeof(*this); fIcon = true);
	IconInfoEx(const IconInfoEx &i) : ICONINFOEX{ i } {}
	IconInfoEx(const ICONINFOEX &i) : ICONINFOEX{ i } {}
	~IconInfoEx() {
		if (hbmMask) Bitmap::Attach(hbmMask).Delete();
		if (hbmColor) Bitmap::Attach(hbmColor).Delete();
	}
public: // Property - Hotspot
	/* W */ inline auto &Hotspot(LSize s) reflect_to_self(this->xHotspot = s.cx, this->yHotspot = s.cy);
	/* R */ inline LSize Hotspot() const reflect_as({ (long)this->xHotspot, (long)this->yHotspot });
public: // Property - Masks
	/* W */ inline auto   &Masks(HBITMAP hbmMask) reflect_to_self(this->hbmMask = hbmMask);
	/* R */ inline Bitmap &Masks() const reflect_as(Bitmap::Attach(const_cast<HBITMAP &>(this->hbmMask)));
public: // Property - Colors
	/* W */ inline auto   &Colors(HBITMAP hbmColor) reflect_to_self(this->hbmColor = hbmColor);
	/* R */ inline Bitmap &Colors() const reflect_as(Bitmap::Attach(const_cast<HBITMAP &>(this->hbmColor)));
public: // Property - ResourceID
	/* W */ inline auto &ResourceID(WORD wResID) reflect_to_self(this->wResID = wResID);
	/* R */ inline WORD  ResourceID() const reflect_as(this->wResID);
public: // Property - ModuleName
	//	WCHAR   szModName[MAX_PATH];
	/* R */ inline const String ModuleName() const reflect_as(CString(this->szModName, CountOf(this->szModName)));
public: // Property - ResourceName
	//	WCHAR   szResName[MAX_PATH];
	/* R */ inline const String ResourceName() const reflect_as(CString(this->szResName, CountOf(this->szResName)));
public:
	inline PICONINFOEX operator&() reflect_as(this);
	inline const ICONINFOEX *operator&() const reflect_as(this);
};
class Icon {
public:
	using Info = IconInfo;
	using InfoEx = IconInfoEx;
protected:
	friend union RefAs<Icon>;
	mutable HICON hIcon = O;
	Icon(HICON h) : hIcon(h) {}
	Icon(const Icon &i) : hIcon(i.hIcon) reflect_to(i.hIcon = O);
public:
	Icon() {}
	Icon(Null) {}
	Icon(Icon &i) : hIcon(i.hIcon) reflect_to(i.hIcon = O);
	Icon(Icon &&i) : hIcon(i.hIcon) reflect_to(i.hIcon = O);
	~Icon() reflect_to(Destroy());

	class CreateStruct {
		HINSTANCE hInstance = GetModuleHandle(O);
		int nWidth, nHeight;
		BYTE nPlanes = 1, nBitsPerPixel = 32;
		LPCBYTE lpANDbits = O, lpXORbits = O;
	public:
		CreateStruct(LPCBYTE lpANDbits, LPCBYTE lpXORbits) :
			lpANDbits(lpANDbits), lpXORbits(lpXORbits) {}
	public: // Property - Module
		/* W */ inline auto &Module(HINSTANCE hInstance) reflect_to_self(this->hInstance = hInstance);
//		/* R */ inline WX::Module Module() reflect_as(this->hInstance);
	public: // Property - Width
		/* W */ inline auto &Width(LONG nWidth) reflect_to_self(this->nWidth = nWidth);
		/* R */ inline LONG  Width() const reflect_as(this->nWidth);
	public: // Property - Height
		/* W */ inline auto &Height(LONG nHeight) reflect_to_self(this->nHeight = nHeight);
		/* R */ inline LONG  Height() const reflect_as(this->nHeight);
	public: // Property - Size
		/* W */ inline auto &Size(LSize sz) reflect_to_self(this->nWidth = sz.cx, this->nHeight = sz.cy);
		/* R */ inline LSize Size() const reflect_as({ this->nWidth, this->nHeight });
	public: // Property - Planes
		/* W */ inline auto &Planes(UINT nPlanes) reflect_to_self(this->nPlanes = nPlanes);
		/* R */ inline UINT  Planes() const reflect_as(this->nPlanes);
	public: // Property - BitsPerPixel
		/* W */ inline auto &BitsPerPixel(UINT nBitsPerPixel) reflect_to_self(this->nBitsPerPixel = nBitsPerPixel);
		/* R */ inline UINT  BitsPerPixel() const reflect_as(this->nBitsPerPixel);
	public: // Property - Masks
		/* W */ inline auto   &Mask(LPCVOID lpBits) reflect_to_self(this->lpANDbits = (LPCBYTE)lpBits);
		/* R */ inline LPCBYTE Mask() const reflect_as(this->lpANDbits);
	public: // Property - Colors
		/* W */ inline auto   &Colors(LPCVOID lpBits) reflect_to_self(this->lpXORbits = (LPCBYTE)lpBits);
		/* R */ inline LPCBYTE Colors() const reflect_as(this->lpXORbits);
	public:
		inline Icon Create() const assert_reflect_as(auto h = CreateIcon(hInstance, nWidth, nHeight, nPlanes, nBitsPerPixel, lpANDbits, lpXORbits), h);
	};
	inline static CreateStruct Create(LPCBYTE lpANDbits, LPCBYTE lpXORbits = O) reflect_as({ lpANDbits, lpXORbits });
	inline static Icon Create(HBITMAP hbmColor, HBITMAP hbmMask = O) assert_reflect_as(auto h = CreateIconIndirect(&IconInfo().Colors(hbmColor).Mask(hbmMask)), h);

	inline bool Destroy() {
		if (hIcon)
			if (!DestroyIcon(hIcon))
				return false;
		hIcon = O;
		return true;
	}

#pragma region Properties
public: // Property - Info
	/* R */ inline Info Informations() const assert_reflect_to(ICONINFO i, GetIconInfo(hIcon, &i), i);
public: // Property - InfoEx
	/* R */ inline InfoEx InformationsEx() const assert_reflect_to(ICONINFOEX i, GetIconInfoEx(hIcon, &i), i);
public: // Property - Masks
	/* R */ inline Bitmap Mask() const reflect_as(Informations().Mask());
public: // Property - Colors
	/* R */ inline Bitmap Colors() const reflect_as(Informations().Colors());
#pragma endregion

	inline operator HICON() const reflect_as(hIcon);

	inline Icon operator+() const assert_reflect_as(auto h = CopyIcon(hIcon), h);

	inline auto &operator=(Icon &i) reflect_to_self(std::swap(hIcon, i.hIcon));
	inline auto &operator=(Icon &&i) reflect_to_self(std::swap(hIcon, i.hIcon));
	inline auto &operator=(const Icon &i) const reflect_to_self(std::swap(hIcon, i.hIcon));

	inline static Icon &Attach(HICON &hIcon) reflect_as(reuse_as<Icon>(hIcon));
	inline static const Icon &Attach(const HICON &hIcon) reflect_as(reuse_as<const Icon>(hIcon));
};
using CIcon = RefAs<Icon>;
#pragma endregion

#pragma region Cursor
class Cursor : public Icon {
public:
	using super = Icon;
protected:
	friend union RefAs<Cursor>;
	Cursor(HCURSOR h) : super((HICON)h) {}
	Cursor(const Cursor &c) : Icon(c) {}
public:
	Cursor() {}
	Cursor(Null) {}
	Cursor(Cursor &i) : super(i) {}
	Cursor(Cursor &&i) : super(i) {}
	~Cursor() reflect_to(Destroy());

	class CreateStruct {
		HINSTANCE hInstance = GetModuleHandle(O);
		int nWidth = 0, nHeight = 0, xHotspot = 0, yHotspot = 0;
		LPCBYTE lpANDbits, lpXORbits;
	public:
		CreateStruct(LPCBYTE lpANDbits, LPCBYTE lpXORbits, LPoint Hotspot) :
			xHotspot(Hotspot.x), yHotspot(Hotspot.y), lpANDbits(lpANDbits), lpXORbits(lpXORbits) {}
	public:
		inline auto &Module(HINSTANCE hInstance) reflect_to_self(this->hInstance = hInstance);
		inline auto &Hotspot(LSize s) reflect_to_self(this->xHotspot = s.cx, this->yHotspot = s.cy);
		inline auto &Width(LONG nWidth) reflect_to_self(this->nWidth = nWidth);
		inline auto &Height(LONG nHeight) reflect_to_self(this->nHeight = nHeight);
		inline auto &Size(LSize sz) reflect_to_self(this->nWidth = sz.cx, this->nHeight = sz.cy);
	public:
		inline Cursor Create() const assert_reflect_as(auto h = CreateCursor(hInstance, xHotspot, yHotspot, nWidth, nHeight, lpANDbits, lpXORbits), h);
		inline operator Cursor() const reflect_as(Create());
	};
	inline static CreateStruct Create(LPCBYTE lpANDbits, LPCBYTE lpXORbits = O, LPoint Hotspot = 0) reflect_as({ lpANDbits, lpXORbits, Hotspot });
	inline static Cursor Create(HBITMAP hbmColor, HBITMAP hbmMask = O, LPoint Hotspot = 0) assert_reflect_as(auto h = CreateIconIndirect(&IconInfo(true).Colors(hbmColor).Mask(hbmMask)), h);

	inline bool Destroy() {
		if (super::hIcon)
			if (!DestroyCursor(super::hIcon))
				return false;
		super::hIcon = O;
		return true;
	}

#pragma region Properties
public: // Property - Hotspot
	/* R */ inline LPoint Hotspot() const reflect_as(Informations().Hotspot());
#pragma endregion

	inline operator HCURSOR() const reflect_as((HCURSOR)super::hIcon);

	inline Cursor operator+() const assert_reflect_as(auto h = CopyCursor(super::hIcon), h);

	inline auto &operator=(Cursor &i) reflect_to_self(std::swap(hIcon, i.hIcon));
	inline auto &operator=(Cursor &&i) reflect_to_self(std::swap(hIcon, i.hIcon));
	inline auto &operator=(const Cursor &i) const reflect_to_self(std::swap(hIcon, i.hIcon));

	inline static Cursor &Attach(HCURSOR &hCursor) reflect_as(reuse_as<Cursor>(hCursor));
	inline static const Cursor &Attach(const HCURSOR &hCursor) reflect_as(reuse_as<const Cursor>(hCursor));
};
using CCursor = RefAs<Cursor>;
#pragma endregion

#pragma endregion

#pragma region Menu
class Menu;
using CMenu = RefAs<Menu>;
enum_class(MenuBmp, HBITMAP,
	Callback      = HBMMENU_CALLBACK,
	No            = 0,
	System        = HBMMENU_SYSTEM,
	BarRestore    = HBMMENU_MBAR_RESTORE,
	BarMinimize   = HBMMENU_MBAR_MINIMIZE,
	BarClose      = HBMMENU_MBAR_CLOSE,
	BarCloseD     = HBMMENU_MBAR_CLOSE_D,
	BarMinimizeD  = HBMMENU_MBAR_MINIMIZE_D,
	PopupClose    = HBMMENU_POPUP_CLOSE,
	PopupRestore  = HBMMENU_POPUP_RESTORE,
	PopupMaximize = HBMMENU_POPUP_MAXIMIZE,
	PopupMinimize = HBMMENU_POPUP_MINIMIZE);
enum_flags(MenuItemType, UINT,
	String        = MFT_STRING,
	Bitmap        = MFT_BITMAP,
	BarBreak      = MFT_MENUBARBREAK,
	Break         = MFT_MENUBREAK,
	OwnerDraw     = MFT_OWNERDRAW,
	RadioCheck    = MFT_RADIOCHECK,
	Separator     = MFT_SEPARATOR,
	RightOrder    = MFT_RIGHTORDER,
	RightJustify  = MFT_RIGHTJUSTIFY);
enum_flags(MenuItemState, UINT,
	Default       = MFS_DEFAULT,
	Grayed        = MFS_GRAYED,
	Enabled       = MFS_ENABLED,
	Disabled      = MFS_DISABLED,
	Checked       = MFS_CHECKED,
	Unchecked     = MFS_UNCHECKED,
	Hilite        = MFS_HILITE,
	Unhilite      = MFS_UNHILITE);
enum_flags(MenuStyle, DWORD,
	NoCheck       = MNS_NOCHECK,
	Modeless      = MNS_MODELESS,
	DragDrop      = MNS_DRAGDROP,
	AutoDismiss   = MNS_AUTODISMISS,
	NotifyByPos   = MNS_NOTIFYBYPOS,
	CheckOrBmp    = MNS_CHECKORBMP);
class MenuItem {
	friend class Menu;
	HMENU hMenu;
	UINT uID, flags;
	MenuItem(HMENU hMenu, UINT uID, UINT flags) :
		hMenu(hMenu), uID(uID), flags(flags) {}
	using Type = MenuItemType;
	using State = MenuItemState;
public:
	inline void Delete(UINT uID) assert(DeleteMenu(hMenu, uID, flags) || (flags = 0));
	inline void Remove(UINT uID) assert(RemoveMenu(hMenu, uID, flags) || (flags = 0));
	inline auto&Hilite(HWND hWnd, bool bHilite = true) assert_reflect_as_self(HiliteMenuItem(hWnd, hMenu, uID, flags | (bHilite ? MF_HILITE : MF_UNHILITE)));
	CMenu Sub(int nPos);
public: // Property - Enable
	/* W */ inline auto &Enable(bool bEnable) assert_reflect_as_self(EnableMenuItem(hMenu, uID, flags | (bEnable ? MF_ENABLED : MF_DISABLED)) >= 0);
public: // Property - Check
	/* W */ inline auto &Check(bool bChecked) assert_reflect_as_self(CheckMenuItem(hMenu, uID, flags | (bChecked ? MF_CHECKED : MF_UNCHECKED)) >= 0);
#define MENUITEM_PROPERTY(name, mask, memb, type, in, conv) \
	/* W */ inline auto &name(type memb) assert_reflect_to_self(MENUITEMINFO mii({ 0 }); mii.cbSize = sizeof(mii); mii.fMask = mask; mii.memb = in, SetMenuItemInfo(hMenu, uID, flags, &mii)); \
	/* R */ inline type  name() const assert_reflect_to(MENUITEMINFO mii({ 0 }); mii.cbSize = sizeof(mii); mii.fMask = mask, GetMenuItemInfo(hMenu, uID, flags, &mii), conv(mii.memb))
public: // Property - Types
	MENUITEM_PROPERTY(Types, MIIM_TYPE, fType, Type, fType.yield(), reuse_as<Type>);
public: // Property - State
	MENUITEM_PROPERTY(States, MIIM_STATE, fState, State, fState.yield(), reuse_as<State>);
public: // Property - ID
	MENUITEM_PROPERTY(ID, MIIM_ID, wID, UINT, wID, _M_);
public: // Property - Checked
	MENUITEM_PROPERTY(Checked, MIIM_CHECKMARKS, hbmpChecked, CBitmap, hbmpChecked, _M_);
public: // Property - Unchecked
	MENUITEM_PROPERTY(Unchecked, MIIM_CHECKMARKS, hbmpUnchecked, CBitmap, hbmpUnchecked, _M_);
public: // Property - Bitmap
	MENUITEM_PROPERTY(Bitmap, MIIM_BITMAP, hbmpItem, CBitmap, hbmpItem, _M_);
	/* W */ inline auto &Bitmap(MenuBmp hbmpItem) assert_reflect_to_self(MENUITEMINFO mii({ 0 }); mii.cbSize = sizeof(mii); mii.fMask = MIIM_BITMAP; mii.hbmpItem = hbmpItem.yield(), SetMenuItemInfo(hMenu, uID, flags, &mii));
public: // Property - UserData
	MENUITEM_PROPERTY(UserData, MIIM_DATA, dwItemData, void *, (ULONG_PTR)dwItemData, reuse_as<void *>);
public: // Property - SubMenu
	/* W */ auto &SubMenu(CMenu);
	/* R */ CMenu SubMenu() const;
public: // Property - String
	/* W */ inline auto &String(const String &str) {
		MENUITEMINFO mii({ 0 });
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = const_cast<LPTSTR>((LPCTSTR)str);
		assert(SetMenuItemInfo(hMenu, uID, flags, &mii));
		retself;
	}
	/* R */ inline WX::String String() const {
		MENUITEMINFO mii({ 0 });
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_STRING;
		mii.cch = MaxLenNotice;
		WX::String data((size_t)mii.cch);
		mii.dwTypeData = data;
		assert(GetMenuItemInfo(hMenu, uID, flags, &mii));
		return data;
	}
};
class Menu {
public:
	using Item = MenuItem;
	using Style = MenuStyle;
protected:
	friend union RefAs<Menu>;
	mutable HMENU hMenu = O;
	Menu(HMENU h) : hMenu(h) {}
	Menu(const Menu &m) : hMenu(m.hMenu) reflect_to(m.hMenu = O);
public:
	Menu() {}
	Menu(Null) {}
	Menu(Menu &m) : hMenu(m) reflect_to(m.hMenu = O);
	Menu(Menu &&m) : hMenu(m) reflect_to(m.hMenu = O);
	~Menu() reflect_to(Destroy());

#pragma region Methods
	inline static Menu Create()      assert_reflect_as(auto h = CreateMenu(), h);
	inline static Menu CreatePopup() assert_reflect_as(auto h = CreatePopupMenu(), h);

	inline bool Destroy() {
		if (hMenu)
			if (!DestroyMenu(hMenu))
				return false;
		hMenu = O;
		return true;
	}

	inline auto&String(LPCTSTR lpString, UINT_PTR uID = 0, bool bEnable = true) assert_reflect_as_self(AppendMenu(hMenu, MF_STRING | (bEnable ? MF_ENABLED : MF_DISABLED), uID, lpString));
	inline auto&Popup(LPCTSTR lpString, HMENU hPopup) assert_reflect_as_self(AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopup, lpString));
	inline auto&Check(LPCTSTR lpString, UINT_PTR uID = 0, bool bChecked = false) assert_reflect_as_self(AppendMenu(hMenu, (bChecked ? MF_CHECKED : MF_UNCHECKED), uID, lpString));
	inline auto&Separator() assert_reflect_as_self(AppendMenu(hMenu, MF_SEPARATOR, 0, O));

	inline auto&String(UINT uPosition, LPCTSTR lpString, UINT_PTR uID = 0, bool bEnable = true) assert_reflect_as_self(InsertMenu(hMenu, uPosition, MF_STRING | (bEnable ? MF_ENABLED : MF_DISABLED), uID, lpString));
	inline auto&Popup(UINT uPosition, LPCTSTR lpString, HMENU hPopup) assert_reflect_as_self(InsertMenu(hMenu, uPosition, MF_POPUP, (UINT_PTR)hPopup, lpString));
	inline auto&Check(UINT uPosition, LPCTSTR lpString, UINT_PTR uID = 0, bool bChecked = false) assert_reflect_as_self(InsertMenu(hMenu, uPosition, (bChecked ? MF_CHECKED : MF_UNCHECKED), uID, lpString));
	inline auto&Separator(UINT uPosition) assert_reflect_as_self(InsertMenu(hMenu, uPosition, MF_SEPARATOR, 0, O));

	inline auto&Radio(UINT idFirst, UINT idLast, UINT idCheck) assert_reflect_as_self(CheckMenuRadioItem(hMenu, idFirst, idLast, idCheck, MF_BYCOMMAND));
	inline auto&RadioIndex(UINT indFirst, UINT indLast, UINT indCheck) assert_reflect_as_self(CheckMenuRadioItem(hMenu, indFirst, indLast, indCheck, MF_BYPOSITION));
	
	enum_flags(Tracks, UINT,
		LeftButton          = TPM_LEFTBUTTON,
		RightButton         = TPM_RIGHTBUTTON,
		LeftAlign           = TPM_LEFTALIGN,
		CenterAlign         = TPM_CENTERALIGN,
		RightAlign          = TPM_RIGHTALIGN,
		TopAlign            = TPM_TOPALIGN,
		VCenterAlign        = TPM_VCENTERALIGN,
		BottomAlign         = TPM_BOTTOMALIGN,
		Horizontal          = TPM_HORIZONTAL,
		Vertical            = TPM_VERTICAL,
		NoNotify            = TPM_NONOTIFY,
		ReturnCmd           = TPM_RETURNCMD,
		Recurse             = TPM_RECURSE,
		HorPosAnimation     = TPM_HORPOSANIMATION,
		HorNegAnimation     = TPM_HORNEGANIMATION,
		VerPosAnimation     = TPM_VERPOSANIMATION,
		VerNegAnimation     = TPM_VERNEGANIMATION,
		NoAnimation         = TPM_NOANIMATION,
		LayoutRtl           = TPM_LAYOUTRTL,
		WorkArea            = TPM_WORKAREA);
	inline bool Track(Tracks tt, POINT pt, HWND hParent) const reflect_as(TrackPopupMenu(hMenu, tt.yield(), pt.x, pt.y, 0, hParent, O));

	//inline auto Modify(UINT uID) reflect_as(ModifyMenu(hMenu, uID, MF_BYCOMMAND, ));
#pragma endregion

//#pragma region Properties
//public: // Property - Count
//	inline auto Count() const reflect_as(GetMenuItemCount(hMenu));
//#define MENU_PROPERTY(name, mask, memb, type, in) \
//	/* W */ inline auto &name(type memb) assert_reflect_to_self(MENUINFO mi({ 0 }); mi.cbSize = sizeof(mi); mi.fMask = mask; mi.memb = in, SetMenuInfo(hMenu, &mi)); \
//	/* R */ inline type  name() const assert_reflect_to(MENUINFO mi({ 0 }); mi.cbSize = sizeof(mi); mi.fMask = mask, GetMenuInfo(hMenu, &mi), reuse_as<type>(mi.memb));
//public: // Property - Styles
//	MENU_PROPERTY(Styles, MIM_STYLE, dwStyle, Style, dwStyle.yield());
//public: // Property - MaxY
//	MENU_PROPERTY(MaxY, MIM_MAXHEIGHT, cyMax, UINT, cyMax);
//public: // Property - Background
//	MENU_PROPERTY(Background, MIM_BACKGROUND, hbrBack, CBrush, hbrBack);
//public: // Property - ContextHelpID
//	MENU_PROPERTY(ContextHelpID, MIM_HELPID, dwContextHelpID, DWORD, dwContextHelpID);
//public: // Property - UserData
//	MENU_PROPERTY(UserData, MIM_DATA, dwMenuData, void *, (ULONG_PTR)dwMenuData);
//#undef MENU_PROPERTY
//#pragma endregion

	inline Item operator[](UINT ind) reflect_as({ hMenu, ind, MF_BYPOSITION });
	inline const Item operator[](UINT ind) const reflect_as({ hMenu, ind, MF_BYPOSITION });

	inline Item operator()(UINT ind) reflect_as({ hMenu, ind, MF_BYCOMMAND });
	inline const Item operator()(UINT ind) const reflect_as({ hMenu, ind, MF_BYCOMMAND });

	inline operator bool() const reflect_as(IsMenu(hMenu));
	inline operator HMENU() const reflect_as(hMenu);
	inline operator LPARAM() const reflect_as((LPARAM)hMenu);

	inline auto &operator=(Menu &m) reflect_to_self(std::swap(hMenu, m.hMenu));
	inline auto &operator=(const Menu &m) const reflect_to_self(std::swap(hMenu, m.hMenu));

	inline static Menu &Attach(HMENU &hMenu) reflect_as(reuse_as<Menu>(hMenu));
	inline static const Menu &Attach(const HMENU &hMenu) reflect_as(reuse_as<const Menu>(hMenu));
};
MENUITEM_PROPERTY(MenuItem::SubMenu, MIIM_SUBMENU, hSubMenu, CMenu, hSubMenu, _M_);
inline CMenu MenuItem::Sub(int nPos) reflect_as({ GetSubMenu(hMenu, nPos) });
#undef MENUITEM_PROPERTY
#pragma endregion

#pragma region Module
class Module {
protected:
	friend union RefAs<Module>;
	mutable HINSTANCE hInst = O;
	Module(HINSTANCE h) : hInst(h) {}
	Module(const Module &m) : hInst(m.hInst) reflect_to(m.hInst = O);
public:
	Module(Null) {}
	Module(Module &m) : hInst(m) reflect_to(m.hInst = O);
	Module(Module &&m) : hInst(m) reflect_to(m.hInst = O);
	Module(LPCTSTR lpModuleName = O) : hInst(GetModuleHandle(lpModuleName)) {}
	~Module() reflect_to(Free());

	inline static Module Handle(LPCTSTR lpModuleName) reflect_as(GetModuleHandle(lpModuleName));
	inline static Module Library(LPCTSTR lpLibFileName) assert_reflect_as(auto hInst = LoadLibrary(lpLibFileName), hInst);

	inline bool Free() {
		if (hInst)
			if (!::FreeLibrary(hInst))
				return false;
		hInst = O;
		return true;
	}

	inline CMenu Menu(LPCTSTR lpszName) const reflect_as(LoadMenu(self, lpszName));
	inline CMenu Menu(WORD wID) const reflect_as(LoadMenu(self, MAKEINTRESOURCE(wID)));

	inline CBitmap Bitmap(LPCTSTR lpszName) const reflect_as(LoadBitmap(self, lpszName));
	inline CBitmap Bitmap(WORD wID) const reflect_as(LoadBitmap(self, MAKEINTRESOURCE(wID)));

	inline CIcon Icon(LPCTSTR lpszName) const reflect_as(LoadIcon(self, lpszName));
	inline CIcon Icon(WORD wID) const reflect_as(LoadIcon(self, MAKEINTRESOURCE(wID)));

	inline CCursor Cursor(LPCTSTR lpszName) const reflect_as(LoadCursor(self, lpszName));
	inline CCursor Cursor(WORD wID) const reflect_as(LoadCursor(self, MAKEINTRESOURCE(wID)));

	inline WX::String String(WORD wID) const {
		auto len = LoadString(self, wID, O, 0);
		assert(len >= 0);
		WX::String str((size_t)len);
		LoadString(self, wID, str, len);
		return str;
	}

	template<class AnyFunc>
	inline AnyFunc *Proc(LPCSTR lpName) reflect_as((AnyFunc  *)::GetProcAddress(self, lpName));

	inline operator bool() const reflect_as(hInst ? hInst != INVALID_HANDLE_VALUE : false);
	inline operator HINSTANCE() const reflect_as(hInst);
	inline operator LPARAM() const reflect_as((LPARAM)hInst);

	inline auto &operator=(Module &m) reflect_to_self(std::swap(hInst, m.hInst));
	inline auto &operator=(Module &&m) reflect_to_self(std::swap(hInst, m.hInst));
	inline auto &operator=(const Module &m) const reflect_to_self(std::swap(hInst, m.hInst));

	inline static Module &Attach(HINSTANCE &hInst) reflect_as(reuse_as<Module>(hInst));
	inline static const Module &Attach(const HINSTANCE &hInst) reflect_as(reuse_as<const Module>(hInst));
};
using CModule = RefAs<Module>;
#pragma endregion

#pragma region Font
enum_class(CharSet, int,
	ANSI           = ANSI_CHARSET,
	Default        = DEFAULT_CHARSET,
	Symbol         = SYMBOL_CHARSET,
	ShiftJIS       = SHIFTJIS_CHARSET,
	Hangeul        = HANGEUL_CHARSET,
	Hangul         = HANGUL_CHARSET,
	GB2312         = GB2312_CHARSET,
	ChineseBig5    = CHINESEBIG5_CHARSET,
	OEM            = OEM_CHARSET,
	Johab          = JOHAB_CHARSET,
	Hebrew         = HEBREW_CHARSET,
	Arabic         = ARABIC_CHARSET,
	Greek          = GREEK_CHARSET,
	Turkish        = TURKISH_CHARSET,
	Vietnamese     = VIETNAMESE_CHARSET,
	Thai           = THAI_CHARSET,
	EastEurope     = EASTEUROPE_CHARSET,
	Russian        = RUSSIAN_CHARSET,
	Mac            = MAC_CHARSET,
	Baltic         = BALTIC_CHARSET);
enum_class(OutPrecis, BYTE, 
	Default        = OUT_DEFAULT_PRECIS,
	String         = OUT_STRING_PRECIS,
	Character      = OUT_CHARACTER_PRECIS,
	Stroke         = OUT_STROKE_PRECIS,
	TrueType       = OUT_TT_PRECIS,
	Device         = OUT_DEVICE_PRECIS,
	Raster         = OUT_RASTER_PRECIS,
	TrueTypeOnly   = OUT_TT_ONLY_PRECIS,
	Outline        = OUT_OUTLINE_PRECIS,
	ScreenOutline  = OUT_SCREEN_OUTLINE_PRECIS,
	PostScriptOnly = OUT_PS_ONLY_PRECIS);
enum_class(ClipPrecis, BYTE, 
	Default        = CLIP_DEFAULT_PRECIS,
	Character      = CLIP_CHARACTER_PRECIS,
	Stroke         = CLIP_STROKE_PRECIS,
	Mask           = CLIP_MASK,
	LH_Angles      = CLIP_LH_ANGLES,
	TT_Always      = CLIP_TT_ALWAYS,
	DFA_Disable    = CLIP_DFA_DISABLE,
	Embedded       = CLIP_EMBEDDED);
struct FontLogic: protected LOGFONT {
	FontLogic() : LOGFONT{ 0 } {}
	FontLogic(const LOGFONT &lf) : LOGFONT(lf) {}
public: // Properties - Height
	/* W */ inline auto &Height(LONG lfHeight) reflect_to_self(this->lfHeight = lfHeight);
	/* R */ inline LONG  Height() const reflect_as(this->lfHeight);
public: // Properties - Width
	/* W */ inline auto &Width(LONG lfWidth) reflect_to_self(this->lfWidth = lfWidth);
	/* R */ inline LONG  Width() const reflect_as(this->lfWidth);
public: // Properties - Escapement
	/* W */ inline auto &Escapement(LONG lfEscapement) reflect_to_self(this->lfEscapement = lfEscapement);
	/* R */ inline LONG  Escapement() const reflect_as(this->lfEscapement);
public: // Properties - Orientation
	/* W */ inline auto &Orientation(LONG lfOrientation) reflect_to_self(this->lfOrientation = lfOrientation);
	/* R */ inline LONG  Orientation() const reflect_as(this->lfOrientation);
public: // Properties - Weight
	/* W */ inline auto &Weight(LONG lfWeight) reflect_to_self(this->lfWeight = lfWeight);
	/* R */ inline LONG  Weight() const reflect_as(this->lfWeight);
public: // Properties - Italic
	/* W */ inline auto &Italic(bool lfItalic) reflect_to_self(this->lfItalic = lfItalic);
	/* R */ inline bool  Italic() const reflect_as(this->lfItalic);
public: // Properties - Underline
	/* W */ inline auto &Underline(BYTE lfUnderline) reflect_to_self(this->lfUnderline = lfUnderline);
	/* R */ inline bool  Underline() const reflect_as(this->lfUnderline);
public: // Properties - StrikeOut
	/* W */ inline auto &StrikeOut(BYTE lfStrikeOut) reflect_to_self(this->lfStrikeOut = lfStrikeOut);
	/* R */ inline bool  StrikeOut() const reflect_as(this->lfStrikeOut);
public: // Properties - CharSet
	/* W */ inline auto &CharSet(BYTE lfCharSet) reflect_to_self(this->lfCharSet = lfCharSet);
	/* R */ inline BYTE  CharSet() const reflect_as(this->lfCharSet);
public: // Properties - OutPrecision
	/* W */ inline auto     &OutPrecision(OutPrecis lfOutPrecision) reflect_to_self(this->lfOutPrecision = lfOutPrecision.yield());
	/* R */ inline OutPrecis OutPrecision() const reflect_as(reuse_as<OutPrecis>(this->lfOutPrecision));
public: // Properties - ClipPrecision
	/* W */ inline auto&ClipPrecision(BYTE lfClipPrecision) reflect_to_self(this->lfClipPrecision = lfClipPrecision);
	/* R */ inline BYTE ClipPrecision() const reflect_as(this->lfClipPrecision);
public: // Properties - Quality
	/* W */ inline auto&Quality(BYTE lfQuality) reflect_to_self(this->lfQuality = lfQuality);
	/* R */ inline BYTE Quality() const reflect_as(this->lfQuality);
public: // Properties - PitchAndFamily
	/* W */ inline auto&PitchAndFamily(BYTE lfPitchAndFamily) reflect_to_self(this->lfPitchAndFamily = lfPitchAndFamily);
	/* R */ inline BYTE PitchAndFamily() const reflect_as(this->lfPitchAndFamily);
public: // Properties - FaceName
	/* W */ inline auto &FaceName(String name) assert_reflect_as_self(SUCCEEDED(StringCchCopy(this->lfFaceName, name.Length() + 1, name)));
	/* R */ inline const String FaceName() const reflect_as(CString(this->lfFaceName, LF_FACESIZE));
public:
	inline LPLOGFONT operator&() reflect_as(this);
	inline const LOGFONT *operator&() const reflect_as(this);
};
class BaseOf_GDI(Font, HFONT) {
public:
	using super = GObjectBase<Font, HFONT>;
	using Logic = FontLogic;
protected:
	Font(const Font &f) : super(f) {}
public:
	Font() {}
	Font(Font &f) : super(f) {}
	Font(Font &&f) : super(f) {}

	using super::operator=;

	inline auto&Create(const Logic &LogFont) reflect_to_self(this->hobj = CreateFontIndirect(&LogFont));

//#define OEM_FIXED_FONT      10
//#define ANSI_FIXED_FONT     11
//#define ANSI_VAR_FONT       12
//#define SYSTEM_FONT         13
//#define DEVICE_DEFAULT_FONT 14
//#define SYSTEM_FIXED_FONT   16
//#define DEFAULT_GUI_FONT    17
};
#pragma endregion

inline Bitmap ClipBitmap(const Bitmap &bmp, LRect rc) {
	auto &&size = rc.size();
	Bitmap nBmp = Bitmap::Create(size).BitsPerPixel(bmp.Log().BitsPerPixel());
	DC srcDC = DC::CreateCompatible(), dstDC = DC::CreateCompatible();
	srcDC.Select(nBmp);
	dstDC.Select(bmp);
	srcDC.BltBit(0, size, dstDC);
	return nBmp;
}

inline void SaveToFile(DC &dc, const Palette &pal, const Bitmap &bmp, File &file) {
	auto &&log = bmp.Log();
	bool usePal = log.BitsPerPixel() < 24 && pal;
	auto usage = usePal ? DIB_PAL_COLORS : DIB_RGB_COLORS;
	auto palSize = usePal ? (DWORD)std::min(pal.Size(), (size_t)4 << log.BitsPerPixel()) : 0;
	BitmapHeader header;
	header.Size(log.Size());
	header.Planes(log.Planes());
	header.BitsPerPixel(log.BitsPerPixel());
	assert(GetDIBits(dc, bmp, 0, log.Height(), O, header, usage));
	header.PaletteSize(palSize);
	header.ColorsSize(header.SizeImage());
	assert(file.Write(&header, sizeof(header) - 4) == sizeof(header) - 4);
	assert(file.Write(pal.Entries().data(), palSize) == palSize);
	AutoPointer<Heap> hBits(header.SizeImage());
	assert(GetDIBits(dc, bmp, 0, log.Height(), &hBits, header, usage));
	assert(file.Write(&hBits, header.SizeImage()) == header.SizeImage());
}

}
