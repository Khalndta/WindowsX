﻿#include "./wx/window.h"
#include "./wx/control.h"
#include "./wx/dialog.h"
#include "./wx/realtime.h"
#include "./wx/file.h"

using namespace WX;

Console con;

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

inline Bitmap ClipBitmap(const Bitmap &bmp, LRect rc) {
	auto &&size = rc.size();
	Bitmap nBmp = Bitmap::Create(size).BitsPerPixel(bmp.Log().BitsPerPixel());
	DC srcDC = DC::CreateCompatible(), dstDC = DC::CreateCompatible();
	srcDC.Select(nBmp);
	dstDC.Select(bmp);
	srcDC.BltBit(0, size, dstDC);
	return nBmp;
}

#pragma region SimDisp

auto
&&d02_ = "02d"_nx,
&&d4_ = " 4d"_nx;

typedef struct {
	uint8_t Left    : 1; // MK_LBUTTON
	uint8_t Middle  : 1; // MK_MBUTTON
	uint8_t Right   : 1; // MK_RBUTTON
	uint8_t Shift   : 1; // MK_SHIFT
	uint8_t Control : 1; // MK_CONTROL
	uint8_t Leave   : 1;
} tSimDisp_MouseKey;

typedef uint8_t(*tSimDisp_OnResize)(uint16_t nSizeX, uint16_t nSizeY);
typedef void(*tSimDisp_OnMouse)(int16_t xPos, int16_t yPos, int16_t zPos, tSimDisp_MouseKey MouseKeys);

static DC &TayKit(DC &dc, LPoint org, int r) {
	auto &&brushWhite = Brush::White(),
		 &&brushBlack = Brush::Black();
	auto &&penWhite = Pen::White(),
		 &&penBlack = Pen::Black();
	auto &&rect0 = org + LRect{ -r, -r, r, r };
	--r;
	auto &&rect1 = org + LRect{ -r / 2, -r, r / 2, 0 };
	auto &&p1 = org + LPoint{ 0, -r },
		 &&p2 = org + LPoint{ 0,  r };
	dc(penBlack);
	dc(brushWhite).DrawPie(rect0, p1, p2);
	dc(brushBlack).DrawPie(rect0, p2, p1);
	dc(brushWhite)(penWhite).DrawEllipse(rect1);
	dc(brushBlack)(penBlack).DrawEllipse(rect1 + LPoint{ 0, r });
	rect1 = (rect1 - org) / 3 + org;
	dc(brushBlack)(penBlack).DrawEllipse(rect1 - LPoint{ 0, (r - 1) / 3 });
	dc(brushWhite)(penWhite).DrawEllipse(rect1 + LPoint{ 0, (r - 1) / 3 * 2 });
	return dc;
}

class Window_Based(SiDiWindow) {
	friend class WindowBase<SiDiWindow>;
	LRect Border = 20;
	struct Window_Based(GraphPanel) {
		SiDiWindow &parent;
		bool bMaskKeyboard = false,
			bMaskMouse = false,
			bMaskTouch = false,
			bHideCursor = false;
		GraphPanel(SiDiWindow &parent) : parent(parent) {}
		WxClass() {
			xClass() {
				Styles(CStyle::Redraw);
				Cursor(IDC_ARROW);
			}
		};
		inline auto Create() {
			return super::Create()
				.Parent(parent)
				.Styles(WStyle::Child | WStyle::Visible);
		}
		WX::DC dcBmp = WX::DC::CreateCompatible();
		void *pbits = O;
		File bmpBuff = File().CreateMapping()
			.Security(InheritHandle)
			.Size(screenSize.cx * screenSize.cy * 4)
			.Protect(PageAccess::ReadWrite);
		Bitmap bmp = Bitmap::Create(
			BitmapHeader()
				.Size(screenSize)
				.BitsPerPixel(32)
				.Planes(1),
			&pbits, bmpBuff);
		inline bool OnCreate(CreateStruct cs) {
			memset(pbits, ~0, screenSize.cx * screenSize.cy * 4);
			dcBmp.Select(bmp);
			return true;
		}
		inline void OnPaint() {
			auto &&ps = BeginPaint();
			auto &&sz = Size();
			DC()->BltBit(0, sz, dcBmp);
		}
		inline bool SaveToFile(LPCTSTR lpFilename) {
			try {
				File file = File::Create(lpFilename).CreateAlways().Accesses(FileAccess::GenericWrite);
				::SaveToFile(DC(), Palette::Default(), ClipBitmap(bmp.Clone(), Size()), file);
				return true;
			}
			catch (WX::Exception err) {
				con.Log(_T("Error: "), err);
			}
			return false;
		}
#pragma region Mouse
		bool OnSetCursor(HWND hwndCursor, UINT codeHitTest, UINT msg) {
			if (!bHideCursor) 
				return false;
			SetCursor(nullptr);
			return true;
		}
		inline void OnMouseMove(int x, int y, UINT keyFlags) {
			if (TrackMouse().Flags(TME::Flag::Hover | TME::Flag::Leave))
				parent.OnPanelMouse(x, y, keyFlags);
		}
		inline void OnLButtonUp(int x, int y, UINT keyFlags) { parent.OnPanelMouse(x, y, keyFlags); }
		inline void OnMButtonUp(int x, int y, UINT keyFlags) { parent.OnPanelMouse(x, y, keyFlags); }
		inline void OnRButtonUp(int x, int y, UINT keyFlags) { parent.OnPanelMouse(x, y, keyFlags); }
		inline void OnLButtonDown(int x, int y, UINT keyFlags) { parent.OnPanelMouse(x, y, keyFlags); }
		inline void OnMButtonDown(int x, int y, UINT keyFlags) { parent.OnPanelMouse(x, y, keyFlags); }
		inline void OnRButtonDown(int x, int y, UINT keyFlags) { parent.OnPanelMouse(x, y, keyFlags); }
		inline void OnMouseLeave() { parent.OnPanelMouse(-1, -1, 0); }
		inline void OnMouseWheel(int x, int y, int z, UINT fwKeys) {
			auto &&pt = ScreenToClient({ x, y });
			parent.OnPanelMouse(pt.x, pt.y, fwKeys, z);
		}
#pragma endregion
	} panel = self;
	WxClass() {
		xClass() {
			Styles(CStyle::Redraw);
			Cursor(IDC_ARROW);
			Background(SysColor::Window);
			auto makeIcon = [](int size) -> WX::Icon {
				Bitmap bmpColor = Bitmap().Create(size);
				Bitmap bmpMask = Bitmap::Create(size).BitsPerPixel(1);
				TayKit(::DC()
						.CreateCompatible()(bmpColor)
						.Fill(Brush::Black()), size / 2, size / 2);
				::DC()
					.CreateCompatible()(bmpMask)
					.Fill(Brush::White())
					(Brush::Black()).DrawEllipse({ 0, 0, size, size });
				return ::Icon::Create().Colors(bmpColor).Masks(bmpMask);
			};
			Icon(makeIcon(128));
			IconSm(makeIcon(48));
		}
	};
	enum eMenu {
		IDM_PRINTSCREEN,
		IDM_EXIT,
		IDM_RESIZE,
		IDM_INVERT,
		IDM_HIDECURSOR,
		IDM_LOCKCURSOR,
		IDM_MASK_MOUSE,
		IDM_MASK_TOUCH,
		IDM_MASK_KEYBOARD,
		IDM_ABOUT
	};
	WX::Menu menu = WX::Menu::Create();
	StatusBar sbar;
#pragma region Events
	inline auto Create() {
		return super::Create()
			.Styles(WS::MinimizeBox | WS::Caption | WS::SysMenu | WS::ClipChildren)
			.Size(500)
			.Position(100)
			.Menu(menu
				  .Popup(S("&Screen"), Menu::CreatePopup()
						 .String(S("&Print Screen"), IDM_PRINTSCREEN)
						 .Separator()
						 .String(S("&Resize Screen"), IDM_RESIZE, bResizeable)
						 .String(S("&Invert Screen"), IDM_INVERT, bResizeable)
						 .Separator()
						 .Popup(S("&Cursor Control"), Menu::CreatePopup()
								.Check(S("&Hide Cursor On Screen"), IDM_HIDECURSOR, false)
								.Check(S("&Lock Mouse On Screen"), IDM_LOCKCURSOR))
						 .Separator()
						 .String(S("&Exit"), IDM_EXIT))
				  .Popup(S("&Event"), Menu::CreatePopup()
						 .Check(S("Mask &Mouse Event"), IDM_MASK_MOUSE, panel.bMaskMouse)
						 .Check(S("Mask &Touch Event"), IDM_MASK_TOUCH, panel.bMaskTouch)
						 .Check(S("Mask &Keyboard Event"), IDM_MASK_KEYBOARD, panel.bMaskKeyboard))
				  .Popup(S("&Help"), Menu::CreatePopup()
						 .String(S("&About"), IDM_ABOUT)));
	}
	inline bool OnCreate(CreateStruct cs) {
		if (!sbar.Create(self)) return false;
		auto rc = AdjustRect(cs.Rect() + LSize(Border.bottom_right() + Border.top_left()) + LSize(0, sbar.Size().cy));
		if (!panel.Create()
			.Position(Border.top_left())
			.Size(cs.Size())) return false;
		Move(rc);
		return true;
	}
	inline void OnDestroy() { PostQuitMessage(0); }
	inline void OnSize(UINT state, int cx, int cy) {
		auto sz = Size();
		sbar.SetParts({ sz.cx / 6, 2 * sz.cx / 6, 3 * sz.cx / 6, 4 * sz.cx / 6, -1 });
		sz = panel.Size();
		sbar.Text(0, Cats(sz.cx, S("x"), sz.cy));
		sbar.FixSize();
	}
	bool bLastIconic = false, bResizeable = false;
	tSimDisp_OnResize pfnOnResize = O;
	inline bool OnWindowPosChanging(LPWINDOWPOS Pos) {
		if (Pos->flags & SWP_NOSIZE)
			return false;
		if (bLastIconic) {
			bLastIconic = false;
			return false;
		}
		if (Iconic()) {
			bLastIconic = true;
			return false;
		}
		auto &&border = Size() - ClientSize() + Border.top_left() + Border.bottom_right();
		if (sbar.Enabled()) border.cy += sbar.Size().cy;
		LSize sz = { Pos->cx - border.cx, Pos->cy - border.cy };
		if (sz.cx <= 0) Pos->cx = border.cx + 1, sz.cx = 1;
		if (sz.cy <= 0) Pos->cy = border.cy + 1, sz.cy = 1;
		if (pfnOnResize)
			if (!pfnOnResize((uint16_t)sz.cx, (uint16_t)sz.cy))
				return false;
		panel.Size(sz);
		return true;
	}
	inline void OnCommand(int id, HWND hwndCtl, UINT codeNotify) {
		switch (id) {
		case IDM_PRINTSCREEN:
			PrintScreen();
			break;
		case IDM_EXIT:
			Destroy();
			break;
		case IDM_RESIZE:
			OnResizeBox();
			break;
		case IDM_INVERT:
			Resize(~panel.Size());
			break;
		case IDM_HIDECURSOR: HideCursor(!panel.bHideCursor); break;
		case IDM_LOCKCURSOR:
			break;
		case IDM_MASK_MOUSE: MaskMouse(!panel.bMaskMouse); break;
		case IDM_MASK_TOUCH: MaskTouch(!panel.bMaskTouch); break;
		case IDM_MASK_KEYBOARD: MaskKeyboard(!panel.bMaskKeyboard); break;
		case IDM_ABOUT:
			MsgBox(
				S("About"),
				S("Simulator Display for embedded OS test\n")
				S("\tBy Nurtas Ihram"),
				MB::Ok | MB::IconInformation);
			break;
		}
	}
	tSimDisp_OnMouse pfnOnMouse = O;
	inline void OnPanelMouse(int x, int y, UINT keyFlags, int z = 0) {
		sbar.Text(1, Cats(S("X: "), d4_(x)));
		sbar.Text(2, Cats(S("Y: "), d4_(y)));
		sbar.Text(3, Cats(S("Z: "), d4_(z)));
		tSimDisp_MouseKey mk;
		mk.Left = bool(keyFlags & MK_LBUTTON);
		mk.Middle = bool(keyFlags & MK_MBUTTON);
		mk.Right = bool(keyFlags & MK_RBUTTON);
		mk.Control = bool(keyFlags & MK_CONTROL);
		mk.Shift = bool(keyFlags & MK_SHIFT);
		if (x < 0 || y < 0) {
			sbar.Text(4, _T("Mouse leave"));
			mk.Leave = 1;
		}
		else
			sbar.Text(4, Cats(
				mk.Left ? S("L") : S(" "),
				mk.Middle ? S("M") : S(" "),
				mk.Right ? S("R") : S(" "),
				mk.Control ? S("Ctrl") : S("    "),
				mk.Shift ? S("Shift") : S("     ")));
		if (pfnOnMouse && !panel.bMaskMouse)
			pfnOnMouse(x, y, z, mk);
	}
	inline void OnResizeBox() {
		struct Dialog_Based(ResizeBox) {
			LSize size;
			ResizeBox(LSize size) : size(size) {}
			enum {
				IDE_CX = 0x20,
				IDE_CY
			};
			inline static LPDLGTEMPLATE Forming() {
				static auto &&hDlg = DFact()
					.Style(WS::Caption | WS::SysMenu | WS::Popup)
					.Caption(L"New size")
					.Size({ 145, 75 })
					.Add(DCtl(L"&X:")
						 .Style(WS::Child | WS::Visible | StaticStyle::CenterImage)
						 .Position({ 24, 12 }).Size({ 14, 14 }))
					.Add(DCtl(DClass::Edit)
						 .Style(WS::Child | WS::Visible | EditStyle::Number | WS::TabStop)
						 .Position({ 42, 12 }).Size({ 72, 14 })
						 .ID(IDE_CX))
					.Add(DCtl(L"&Y:")
						 .Style(WS::Child | WS::Visible | StaticStyle::CenterImage)
						 .Position({ 24, 30 }).Size({ 72, 14 }))
					.Add(DCtl(DClass::Edit)
						 .Style(WS::Child | WS::Visible | EditStyle::Number | WS::TabStop)
						 .Position({ 42,30 }).Size({ 72, 14 })
						 .ID(IDE_CY))
					.Add(DCtl(L"&OK", DClass::Button)
						 .Style(WS::Child | WS::Visible | WS::TabStop)
						 .Position({ 18, 48 }).Size({ 50, 14 })
						 .ID(IDOK))
					.Add(DCtl(L"&Cancel", DClass::Button)
						 .Style(WS::Child | WS::Visible | WS::TabStop)
						 .Position({ 72, 48 }).Size({ 50, 14 })
						 .ID(IDCANCEL)).Make();
				return &hDlg;
			}
			inline bool InitDialog() {
				Item(IDE_CX).Int(size.cx);
				Item(IDE_CY).Int(size.cy);
				return true;
			}
			inline void OnCommand(int id, HWND hwndCtl, UINT codeNotify) {
				switch (id) {
					case IDOK:
						try {
							LSize sz = { Item(IDE_CX).Int(), Item(IDE_CY).Int() };
							if (size != sz) size = sz;
							else id = IDCANCEL;
						} catch (...) {
							MsgBox(_T("Failed"), _T("Invalid input"), MB::IconError);
							id = IDCANCEL;
						}
					case IDCANCEL:
						End(id);
						break;
					default:
						break;
				}
			}
			inline void OnClose() { End(IDCANCEL); }
		} rsBox = panel.Size();
		if (rsBox.Box(self) == IDOK)
			if (!Resize(rsBox.size))
				MsgBox(_T("Failed"), _T("Resize error"), MB::IconError);
	}
#pragma endregion
public:
	static const LSize screenSize;
	inline bool PrintScreen(LPCTSTR lpFilename) reflect_as(panel.SaveToFile(lpFilename));
	inline bool PrintScreen() {
		ChooserFile cf;
		cf
			.File(Cats(ReplaceAll(SysTime(), _T(":"), _T("-")), S(".bmp")))
			.Parent(self)
			.Styles(ChooserFile::Style::Explorer)
			.Title(S("Printscreen to bitmap"))
			.Filter(S("Bitmap file (*.bmp)\0*.bmp*\0\0"));
		if (!cf.SaveFile()) return false;
		if (PrintScreen(cf.File())) return true;
		MsgBox(_T("Error"), _T("Save bitmap failed!"), MB::Ok);
		return false;
	}
	inline void Resizeable(bool bResizeable = true) {
		this->bResizeable = bResizeable;
		if (menu) {
			menu(IDM_RESIZE).Enable(bResizeable);
			menu(IDM_INVERT).Enable(bResizeable);
		}
		if (self)
			Styles(bResizeable ? Styles() + WS::SizeBox : Styles() - WS::SizeBox);
	}
	inline bool Resize(LSize sz) {
		if (!bResizeable) return false;
		if (sz.cx == 0 || sz.cy == 0) return false;
		try {
			auto rc = AdjustRect(sz + LSize(Border.bottom_right() + Border.top_left()) + LSize(0, sbar.Size().cy));
			Size(rc);
			return true;
		}
		catch (...) {}
		return false;
	}
	inline void HideCursor(bool bHide) {
		panel.bHideCursor = bHide;
		if (menu)
			menu(IDM_HIDECURSOR).Check(bHide);
	}
	inline void MaskMouse(bool bMask) {
		panel.bMaskMouse = bMask;
		if (menu)
			menu(IDM_MASK_MOUSE).Check(bMask);
	}
	inline void MaskTouch(bool bMask) {
		panel.bMaskTouch = bMask;
		if (menu)
			menu(IDM_MASK_TOUCH).Check(bMask);
	}
	inline void MaskKeyboard(bool bMask) {
		panel.bMaskKeyboard = bMask;
		if (menu)
			menu(IDM_MASK_KEYBOARD).Check(bMask);
	}
	inline void SetOnResize(tSimDisp_OnResize pfnOnResize) { this->pfnOnResize = pfnOnResize; }
	inline void SetOnMouse(tSimDisp_OnMouse pfnOnMouse) { this->pfnOnMouse = pfnOnMouse; }
	inline void Flush() reflect_to(this->Send().OnPaint());
	inline void *GetBits() reflect_as(panel.pbits);
	inline CFile GetBitsMapping() reflect_as(panel.bmpBuff);
	inline LSize ScreenSize() const reflect_as(panel.Size());
	inline LSize ScreenSizeMax() const reflect_as(screenSize);
};

const LSize SiDiWindow::screenSize = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };

bool test_window() {
	try {
		con.Log("Max size - ", SiDiWindow::screenSize.cx, 'x', SiDiWindow::screenSize.cy, '\n');
		SiDiWindow p;

		if (!p.Create().Size({ 400, 300 }).Caption(S("Window X By Nurtas Ihram")))
			return false;

		p.Update();
		p.Show();
		p.Resizeable(true);

		MSG msg;
		while (GetMessage(&msg, 0, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch (WX::Exception err) {
		con.Log(_T("Error: "), err);
	}

	return true;
}

#pragma endregion

void test_thread() {
	struct Thread_Bsaed(__) {
		inline void Start() {
			auto &&pmc = Process::Current().Memory();
			SIZE_T physicalMemUsedByProcess = pmc.WorkingSetSize;
			con("Physical memory used: ", (double)physicalMemUsedByProcess / 1048576., "MB\n");
		}
	} p;
	p.Create();
	p.WaitForSignal();
}

void test_proc() {
	con.Log("\nEnvironments: \n");
	for (const String &s : Environments::Current().Entries())
		con.Log("set ", s, '\n');
	con.Log(Environment[_T("path")].ValueExpend());
	Process prc = Process::Create(O, _T("cmd.exe ")).NewConsole();
	Sleep(5000);
	prc.Terminate();
}

void test_file() {
	File file = File::Create(_T("WX.cpp")).OpenExisting();
	con.Log("Date & Time: ", file.Times().Created, '\n');
	con.Log("Size: ", file.Size(), '\n');
}

// void test_comm() {
// 	for (auto id : Comm::Ports())
// 		con.Log("COM", id, "\n");
// 	Comm com = 15;
// 	com.Setup(2048, 2048);
// 	com.Timeouts(CommTimeout().WriteTotal({ 50, 2000 }));
// 	com.State(_T("baud=115200 parity=N data=8 stop=1"));
// 	com.Events(CommEvent::RxChar);
// 	com.Purge(CommClear::All);
// 	for (;;) {
// 		auto evt = com.WaitEvents();
// 		if (evt == CommEvent::RxChar) {
// 		}
// 	}
// //	con.Log(com.State().BaudRate(), "\n");
// }

int main(int argc, char *argv[]) {
	con.Title(_T("Windows X"));
	con.Log("System Time: ", SysTime(), '\n');
	try {
//		test_sa();
//		test_file();
//		test_thread();
//		test_proc();
//		test_comm();
		test_window();
	} catch (WX::Exception err) {
		con.Log(_T("Error: "), err);
	}
	return 0;
}
