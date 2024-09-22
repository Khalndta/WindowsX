#include "./wx/window.h"

#define DLL_EXPORTS "dllexports.inl"
#include "./wx/dll.inl"

REG_FUNC(uint8_t, DLL_Notice, const wchar_t *lpString) {
	return WX::MsgBox(L"Notice", lpString).yield();
}
