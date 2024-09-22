#include <iostream>
#include <string>

#include <windows.h>
#include <sddl.h>

BOOL createLowSecurityDescriptor(SECURITY_ATTRIBUTES &secAttr) {
	
	PACL pSacl = NULL;

	PSECURITY_DESCRIPTOR pSD = O;
	ULONG size;
	
	ConvertStringSecurityDescriptorToSecurityDescriptor(_T("S:(ML;;NW;;;LW)"), SDDL_REVISION_1, &pSD, &size);

	BOOL fSaclPresent = FALSE;
	BOOL fSaclDefaulted = FALSE;
	GetSecurityDescriptorSacl(
		pSD,
		&fSaclPresent,
		&pSacl,
		&fSaclDefaulted);

	InitializeSecurityDescriptor(secAttr.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);

	SetSecurityDescriptorSacl(secAttr.lpSecurityDescriptor, TRUE, pSacl, FALSE);

	return TRUE;
}

//HANDLE createOrOpenMutex(const TCHAR *gMutexName) {
//	srlog_tracer(::createOrOpenMutex);
//
//	HANDLE hMutex = NULL;
//
//
//	BOOL succ = FALSE;
//	DWORD lassErr = 0;
//	SECURITY_ATTRIBUTES secAttr;
//	SECURITY_DESCRIPTOR secDesc;
//
//	OSVERSIONINFO osvi;
//	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//	succ = GetVersionEx(&osvi);
//	//bool bIsWindowsXPOrLater = ( (osvi.dwMajorVersion > 5) || ( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1) ));
//	bool bIsVistaOrLater = (osvi.dwMajorVersion > 5);
//	if (bIsVistaOrLater) {
//		secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
//		secAttr.bInheritHandle = FALSE;
//		secAttr.lpSecurityDescriptor = &secDesc;
//
//		succ = createLowSecurityDescriptor(secAttr);
//		srlog_info(_T("::createOrOpenMutex | createLowSecurityDescriptor return:%d"), succ);
//	}
//	else {
//
//		if (InitializeSecurityDescriptor(&secDesc, SECURITY_DESCRIPTOR_REVISION)) //Revision level
//		{
//			if (SetSecurityDescriptorDacl(&secDesc,
//										  TRUE, // DACL presence
//										  NULL, // DACL (NULL DACL means all access granted)
//										  FALSE)) // default DACL
//			{
//				secAttr.nLength = sizeof(SECURITY_DESCRIPTOR);
//				secAttr.lpSecurityDescriptor = (LPVOID)&secDesc;
//				secAttr.bInheritHandle = FALSE;
//				// now you can fill param LPSECURITY_ATTRIBUTES while &secAttr
//			}
//		}
//	}
//
//	hMutex = CreateMutex(&secAttr, FALSE, gMutexName);
//
//	return hMutex;
//}

int main(int argc, char *argv[]) {

	return 0;
}
