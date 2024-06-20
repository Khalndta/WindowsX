#include "./wx/window.h"
#include "./wx/control.h"
#include "./wx/dialog.h"
#include "./wx/security.h"
#include "./wx/realtime.h"
#include "./wx/file.h"


using namespace WX;

Console con;

#pragma region SimDisp

auto
&&d_ = "d"_nx,
&&d02_ = "02d"_nx,
&&d4_ = " 4d"_nx;

typedef struct {
	uint8_t Left : 1; // MK_LBUTTON
	uint8_t Middle : 1; // MK_MBUTTON
	uint8_t Right : 1; // MK_RBUTTON
	uint8_t Shift : 1; // MK_SHIFT
	uint8_t Control : 1; // MK_CONTROL
	uint8_t Leave : 1;
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

struct Window_Based(SiDiWindow) {
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
		inline void OnPaint() {
			auto &&ps = BeginPaint();
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
	WX::Menu menu;
	inline auto Create() {
		return super::Create()
			.Styles(WS::MinimizeBox | WS::Caption | WS::SysMenu | WS::ClipChildren)
			.Size(500)
			.Position(100)
			.Menu(menu.Create()
				  .Popup(S("&Screen"), MenuPopup()
						 .String(S("&Print Screen"), IDM_PRINTSCREEN)
						 .Separator()
						 .String(S("&Resize Screen"), IDM_RESIZE, false)
						 .String(S("&Invert Screen"), IDM_INVERT, false)
						 .Separator()
						 .Popup(S("&Cursor Control"), MenuPopup()
								.Check(S("&Hide Cursor On Screen"), IDM_HIDECURSOR, false)
								.Check(S("&Lock Mouse On Screen"), IDM_LOCKCURSOR))
						 .Separator()
						 .String(S("&Exit"), IDM_EXIT))
				  .Popup(S("&Event"), MenuPopup()
						 .Check(S("Mask &Mouse Event"), IDM_MASK_MOUSE, panel.bMaskMouse)
						 .Check(S("Mask &Touch Event"), IDM_MASK_TOUCH, panel.bMaskTouch)
						 .Check(S("Mask &Keyboard Event"), IDM_MASK_KEYBOARD, panel.bMaskKeyboard))
				  .Popup(S("&Help"), MenuPopup()
						 .String(S("&About"), IDM_ABOUT)));
	}
	StatusBar sbar;
	inline void OnSize(UINT state, int cx, int cy) {
		auto sz = Size();
		sbar.SetParts({ sz.cx / 6, 2 * sz.cx / 6, 3 * sz.cx / 6, 4 * sz.cx / 6, -1 });
		sz = panel.Size();
		sbar.Text(0, Cats(d_(sz.cx), S("x"), d_(sz.cy)));
		sbar.FixSize();
	}
	inline void OnDestroy() { PostQuitMessage(0); }
	inline bool OnCreate(LPCREATESTRUCT lpCreate) {
		if (!sbar.Create(self))
			return false;
		auto sz = ClientSize() - Border.bottom_right() - Border.top_left();
		if (sbar.Enabled())
			sz.cy -= sbar.Size().cy;
		if (!panel.Create()
			.Position(Border.top_left())
			.Size(sz))
			return false;
		return true;
	}
	inline void OnCommand(int id, HWND hwndCtl, UINT codeNotify) {
		switch (id) {
		case IDM_PRINTSCREEN:
			SaveScreen();
			break;
		case IDM_EXIT:
			Destroy();
			break;
		case IDM_RESIZE:
			break;
		case IDM_INVERT:
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
	bool bLastIconic = false;
	bool bResizeable = false;
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
		panel.Size(sz);
		return true;
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
			sbar.Text(4, S("Mouse leave"));
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
public:
	inline void SaveScreen() {
		auto &&st = SysTime::SystemTime();
		ChooserFile cf;
		cf
			.File(Cats(d_(st.wYear), S("-"), d02_(st.wMonth), S("-"), d02_(st.wDay), S("~"),
						d02_(st.wHour), d02_(st.wMinute), d02_(st.wSecond), S(".bmp")))
			.Parent(self)
			.Styles(ChooserFile::Style::Explorer)
			.Title(S("Printscreen to bitmap"))
			.Filter(S("Bitmap file (*.bmp)\0*.bmp*\0\0"));
		if (cf.SaveFile())
			MsgBox(S("Error"), S("Save bitmap failed"), MB::IconError);
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
	inline void SetOnMouse(tSimDisp_OnMouse pfnOnMouse)
	{ this->pfnOnMouse = pfnOnMouse; }
};

bool test_window() {
	Handle h;
	SiDiWindow p;

	if (!p.Create().Caption(S("Window X By Nurtas Ihram")))
		return false;
	
	p.Update();
	p.Show();
	p.Resizeable(true);

	MSG msg;
 	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

#pragma endregion

void test_sa() {
	Event evt = Event::Create().Security(SecAttr().Descriptor(SecDesc().Discretion(AceList{
		AccessExplicit()
			.Permissions(AccessPermission::Read)
			.Trust(SecID(SecAuthorIDs::World, SECURITY_WORLD_RID))
			.TrustBy(TrustTypes::WellKnownGroup),
		AccessExplicit()
			.Permissions(AccessPermission::AllAccess)
			.Trust(SecID(SecAuthorIDs::NT, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS))
			.TrustBy(TrustTypes::Group)
	})));
	if (evt)
		con("\nCreated\n");
}

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
	con.Log(Environment[T("path")].ValueExpend());
	Process prc = Process::Create(O, T("cmd.exe ")).NewConsole();
	Sleep(5000);
	prc.Terminate();
}

void test_file() {
	File file = File::Create(T("WX.cpp")).OpenExisting();
	con.Log("Size: ", file.Size(), '\n');
}

constexpr auto word_t = ZRegex::Words(L"aA123  ");
constexpr auto word3_t = ZRegex::Rept<ZRegex::And<ZRegex::Words, ZRegex::Blanks>, 3>(L"aA123  aA123 asd ");
constexpr auto word3_ = word3_t.Matched;
constexpr auto word3_1 = ZRegex::ReptMax<ZRegex::And<ZRegex::Words, ZRegex::Blanks>>(L"aA123  aA123 asd asd ");
constexpr auto sizePaa = word3_1.Matched;
constexpr auto sizeP = word3_1.Times;
char buf[word3_1.Times] = { 0 };

int main(int argc, char *argv[]) {
	con.Title(T("Windows X"));
	con.Log(word3_1.Times, '\n');
	try {
//		test_sa();
		test_file();
//		test_thread();
//		test_proc();
//		test_window();
	} catch (Exception err) {
		con.Log('\n', " : ", CString(err.lpszLine, 1024), '\n');
	}

	getchar();
	return 0;
}
