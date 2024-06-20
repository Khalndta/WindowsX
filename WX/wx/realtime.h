#pragma once
#include <heapapi.h>
#include <psapi.h>

#include "security.h"

namespace WX {

#pragma region Event
enum_flags(EventAccess, HandleAccess,
	All    = EVENT_ALL_ACCESS,
	Modify = EVENT_MODIFY_STATE);
class Handle_Based(Event) {
	Event(HANDLE h) : super(h) {}
public:
	using super = HandleBase<Event>;
	using Access = EventAccess;

	Event() : Event(CreateStruct().operator WX::Event()) {}
	Event(Null) {}
	Event(Event &evt) : super(evt) {}

	class CreateStruct {
		friend class Event;
		String name;
		bool bInitialState = false;
		bool bManualReset = false;
		LPSECURITY_ATTRIBUTES pSA = O;
		CreateStruct(String name = O) : name(name) {}
	public:
		inline auto &Preset(bool bInitialState = true) reflect_to_self(this->bInitialState = bInitialState);
		inline auto &AutoReset(bool bAutoReset = true) reflect_to_self(this->bManualReset = !bAutoReset);
		inline auto &ManualReset(bool bManualReset = true) reflect_to_self(this->bManualReset = bManualReset);
		inline auto &Security(const SecAttr &sa) reflect_to_self(this->pSA = &sa);
		inline auto &Security(LPSECURITY_ATTRIBUTES pSA) reflect_to_self(this->pSA = pSA);
		inline auto &Name(String name) reflect_to_self(this->name = name);
	public:
		inline operator Event() const assert_reflect_as(auto h = CreateEvent(pSA, bManualReset, bInitialState, name), h);
	};
	inline static CreateStruct Create(String name = O) reflect_as(name);

	class OpenStruct {
		friend class Event;
		Access dwDesiredAccess = Access::Modify;
		BOOL bInheritHandle = false;
		String name = O;
		OpenStruct(String name) : name(name) {}
	public:
		inline auto &Accesses(Access dwDesiredAccess) reflect_to_self(this->dwDesiredAccess = dwDesiredAccess);
		inline auto &Inherit(bool bInheritHandle = true) reflect_to_self(this->bInheritHandle = bInheritHandle);
	public:
		inline operator Event() const assert_reflect_as(auto h = OpenEvent(dwDesiredAccess.yield(), bInheritHandle, name), h);
	};
	inline static OpenStruct Open(String name = O) reflect_as(name);

	inline Event&Set() assert_reflect_as_self(SetEvent(self));
	inline Event&Reset() assert_reflect_as_self(ResetEvent(self));

	inline auto&operator=(bool bState) reflect_as(bState ? Set() : Reset());
};
#pragma endregion

#pragma region Mutex
enum_flags(MutexAccess, HandleAccess,
	All    = MUTEX_ALL_ACCESS,
	Modify = MUTEX_MODIFY_STATE);
class Handle_Based(Mutex) {
	Mutex(HANDLE h) : super(h) {}
public:
	using super = HandleBase<Mutex>;
	using Access = MutexAccess;

	Mutex() {}
	Mutex(Null) {}

	~Mutex() { ReleaseMutex(self); }

	class CreateStruct {
		friend class Mutex;
		String name;
		bool bInitialState = false;
		LPSECURITY_ATTRIBUTES lpMutexAttributes = O;
		CreateStruct(String name = O) : name(name) {}
	public:
		inline auto &Preset(bool bInitialState = true) reflect_to_self(this->bInitialState = bInitialState);
		inline auto &Security(const SecAttr &MutexAttributes) reflect_to_self(this->lpMutexAttributes = &MutexAttributes);
		inline auto &Security(LPSECURITY_ATTRIBUTES lpMutexAttributes) reflect_to_self(this->lpMutexAttributes = lpMutexAttributes);
		inline auto &Name(String name) reflect_to_self(this->name = name);
	public:
		inline Mutex Create() const assert_reflect_as(auto h = CreateMutex(lpMutexAttributes, bInitialState, name), h);
		inline operator Mutex() const reflect_as(Create());
	};
	inline static CreateStruct Create(String name = O) reflect_as(name);

	class OpenStruct {
		friend class Mutex;
		Access dwDesiredAccess = Access::Modify;
		bool bInheritHandle = false;
		String name = O;
		OpenStruct(String name) : name(name) {}
	public:
		inline auto &Accesses(Access dwDesiredAccess) reflect_to_self(this->dwDesiredAccess = dwDesiredAccess);
		inline auto &Inherit(bool bInheritHandle = true) reflect_to_self(this->bInheritHandle = bInheritHandle);
	public:
		inline operator Mutex() const assert_reflect_as(auto h = OpenMutex(dwDesiredAccess.yield(), bInheritHandle, name), h);
	};
	inline static OpenStruct Open(String name = O) reflect_as(name);

	inline auto&Release() assert_reflect_as_self(ReleaseMutex(self));
};
#pragma endregion

#pragma region ThreadBase
enum_flags(ThreadAccess, Handle::Access,
	Resume            = THREAD_RESUME,
	Terminate         = THREAD_TERMINATE,
	SuspendResume     = THREAD_SUSPEND_RESUME,
	GetContext        = THREAD_GET_CONTEXT,
	SetContext        = THREAD_SET_CONTEXT,
	QueryInfo         = THREAD_QUERY_INFORMATION,
	QueryInfoLimit    = THREAD_QUERY_LIMITED_INFORMATION,
	SetInfo           = THREAD_SET_INFORMATION,
	SetInfoLimit      = THREAD_SET_LIMITED_INFORMATION,
	SetToken          = THREAD_SET_THREAD_TOKEN,
	Impersonate       = THREAD_IMPERSONATE,
	ImpersonateDirect = THREAD_DIRECT_IMPERSONATION,
	All               = THREAD_ALL_ACCESS);
template<class AnyChild = void>
class ThreadBase;
using Thread = ThreadBase<>;
template<class AnyChild>
class ThreadBase : public ChainExtend<ThreadBase<AnyChild>, AnyChild>,
	public HandleBase<ThreadBase<AnyChild>> {
	def_memberof(Start);
	ThreadBase(HANDLE hThread) : super(hThread) {}
	static DWORD WINAPI Proc(LPVOID lpThis) {
		using FuncTypeRet = DWORD();
		if constexpr (member_Start_of<AnyChild>::template compatible_to<FuncTypeRet>)
			reflect_as(((AnyChild *)lpThis)->Start())
		else {
			static_assert(member_Start_of<AnyChild>::template compatible_to<void()>);
			((AnyChild *)lpThis)->Start();
			return 0;
		}
	}
public:
	using super = HandleBase<ThreadBase>;
	using Access = ThreadAccess;

	ThreadBase() {
		if constexpr (!std::is_void_v<AnyChild>)
			static_assert(member_Start_of<AnyChild>::existed);
	}
	ThreadBase(Null) {}
	~ThreadBase() reflect_to(super::WaitForSignal());

	class OpenStruct {
		friend class ThreadBase;
		Access dwDesiredAccess = Access::All;
		bool bInheritHandle = false;
		DWORD dwThreadId = 0;
		OpenStruct(DWORD dwThreadId) : dwThreadId(dwThreadId) {}
	public: // Property - Accesses
		/* W */ inline auto &Accesses(Access dwDesiredAccess) reflect_to_self(this->dwDesiredAccess = dwDesiredAccess);
		/* W */ inline auto Inherit(bool bInheritHandle) reflect_to_self(this->bInheritHandle = bInheritHandle);
	public:
		inline operator Thread() const reflect_as(OpenThread(dwDesiredAccess.yield(), bInheritHandle, dwThreadId));
	};
	inline static OpenStruct Open(DWORD dwProcessId) reflect_as(dwProcessId);

	class CreateStruct {
		friend class ThreadBase;
		ThreadBase *pThis;
		LPSECURITY_ATTRIBUTES lpThreadAttributes = O;
		SIZE_T dwStackSize = 0;
		DWORD dwCreationFlags = STACK_SIZE_PARAM_IS_A_RESERVATION;
		CreateStruct(ThreadBase &_this) : pThis(&_this) {}
	public:
		~CreateStruct() reflect_to(this->Create());
		inline auto &Security(const SecAttr &ThreadAttributes) reflect_to_self(this->lpThreadAttributes = &ThreadAttributes);
		inline auto &Security(LPSECURITY_ATTRIBUTES lpThreadAttributes) reflect_to_self(this->lpThreadAttributes = lpThreadAttributes);
		inline auto &StackSize(size_t dwStackSize) reflect_to_self(this->dwStackSize = dwStackSize, this->dwCreationFlags &= ~STACK_SIZE_PARAM_IS_A_RESERVATION);
		inline auto &Suspend(bool bSuspend) reflect_to_self(this->dwCreationFlags = bSuspend ? (this->dwCreationFlags | CREATE_SUSPENDED) : (this->dwCreationFlags & ~CREATE_SUSPENDED));
		inline ThreadBase &Create() assert_reflect_as((this->pThis->hObject = CreateThread(this->lpThreadAttributes, this->dwStackSize, Proc, this->pThis, this->dwCreationFlags, O)), *pThis);
	};
	inline CreateStruct Create() reflect_to_self();

	inline auto &Suspend() assert_reflect_as_child(SuspendThread(self));
	inline auto &Resume()  assert_reflect_as_child(ResumeThread(self));
	inline bool Terminate(DWORD dwExitCode = 0) {
		if (this->hObject)
			if (!::TerminateThread(this->hObject, dwExitCode))
				return false;
		this->hObject = O;
		return true;
	}

	inline static void Exit(DWORD dwExitCode = 0) reflect_to(ExitThread(dwExitCode));

public: // Property - ID
	/* W */ inline DWORD ID() const assert_reflect_as(auto id = GetThreadId(self), id);
public: // Property - ExitCode
	/* W */ inline DWORD ExitCode() const assert_reflect_to(DWORD dwExitCode, GetExitCodeThread(self, &dwExitCode), dwExitCode);
public: // Property - StillActive
	/* W */ inline bool StillActive() const {
		try {
			if (ExitCode() == STILL_ACTIVE)
				return super::WaitForSignal(0);
		} catch (...) {}
		return false;
	}

};
#define Thread_Bsaed(name) name : public ThreadBase<name>
#pragma endregion

#pragma region Process
enum_flags(ProccessCreateFlag, DWORD,
	enum_default Default         = 0,

	DebugProcess                 = DEBUG_PROCESS,
	DebugOnlyThisProcess         = DEBUG_ONLY_THIS_PROCESS,
	CreateSuspended              = CREATE_SUSPENDED,
	DetachedProcess              = DETACHED_PROCESS,

	CreateNewConsole             = CREATE_NEW_CONSOLE,
	NormalPriorityClass          = NORMAL_PRIORITY_CLASS,
	IdlePriorityClass            = IDLE_PRIORITY_CLASS,
	HighPriorityClass            = HIGH_PRIORITY_CLASS,

	RealtimePriorityClass        = REALTIME_PRIORITY_CLASS,
	CreateNewProcessGroup        = CREATE_NEW_PROCESS_GROUP,
	CreateUnicodeEnvironment     = CREATE_UNICODE_ENVIRONMENT,
	CreateSeparateWowVdm         = CREATE_SEPARATE_WOW_VDM,

	CreateSharedWowVdm           = CREATE_SHARED_WOW_VDM,
	CreateForceDos               = CREATE_FORCEDOS,
	BelowNormalPriorityClass     = BELOW_NORMAL_PRIORITY_CLASS,
	AboveNormalPriorityClass     = ABOVE_NORMAL_PRIORITY_CLASS,

	InheritParentAffinity        = INHERIT_PARENT_AFFINITY,
	InheritCallerPriority        = INHERIT_CALLER_PRIORITY,
	CreateProtectedProcess       = CREATE_PROTECTED_PROCESS,
	ExtendedStartupinfoPresent   = EXTENDED_STARTUPINFO_PRESENT,

	ProcessModeBackgroundBegin   = PROCESS_MODE_BACKGROUND_BEGIN,
	ProcessModeBackgroundEnd     = PROCESS_MODE_BACKGROUND_END,
	CreateSecureProcess          = CREATE_SECURE_PROCESS,

	CreateBreakawayFromJob       = CREATE_BREAKAWAY_FROM_JOB,
	CreatePreserveCodeAuthzLevel = CREATE_PRESERVE_CODE_AUTHZ_LEVEL,
	CreateDefaultErrorMode       = CREATE_DEFAULT_ERROR_MODE,
	CreateNoWindow               = CREATE_NO_WINDOW,

	ProfileUser                  = PROFILE_USER,
	ProfileKernel                = PROFILE_KERNEL,
	ProfileServer                = PROFILE_SERVER,
	CreateIgnoreSystemDefault    = CREATE_IGNORE_SYSTEM_DEFAULT);
enum_flags(ProcessAccess, DWORD,
	Terminate                 = PROCESS_TERMINATE,
	CreateThread              = PROCESS_CREATE_THREAD,
	SetSessionid              = PROCESS_SET_SESSIONID,
	VmOperation               = PROCESS_VM_OPERATION,
	VmRead                    = PROCESS_VM_READ,
	VmWrite                   = PROCESS_VM_WRITE,
	DupHandle                 = PROCESS_DUP_HANDLE,
	CreateProcess             = PROCESS_CREATE_PROCESS,
	SetQuota                  = PROCESS_SET_QUOTA,
	SetInformation            = PROCESS_SET_INFORMATION,
	QueryInformation          = PROCESS_QUERY_INFORMATION,
	SuspendResume             = PROCESS_SUSPEND_RESUME,
	QueryLimitedInformation   = PROCESS_QUERY_LIMITED_INFORMATION,
	SetLimitedInformation     = PROCESS_SET_LIMITED_INFORMATION,
	enum_complex All          = PROCESS_ALL_ACCESS);
class Environments {
	LPTCH lpEnv = O;
	Environments(LPTCH lpEnv) : lpEnv(lpEnv) {}
public:
	class Variable {
		friend class Environments;
		const String name;
		const String value;
	public:
		Variable() {}
		//Variable(const Variable &var) : name(var.name), value(var.value) {}
		Variable(const String &entry) {
			LPCTSTR lpName = entry, pName = lpName;
			while (*pName++ != '=');
			name = +CString(pName - lpName - 1, lpName);
			value = +CString(pName, entry.Length() - name.Length());
		}
		Variable(const String &name, const String &value) reflect_to(this->name = name; this->value = value);
	public: // Property - Name
		/* W */ inline Variable &Name(const String &name) reflect_to_self(this->name = name);
		/* R */ inline const String &Name() const reflect_as(this->name);
	public: // Property - Value
		/* W */ inline Variable &Value(const String &value) reflect_to_self(this->value = value);
		/* R */ inline const String &Value() const reflect_as(this->value);
	public: // Property - ValueExpend
		inline String ValueExpend() const {
			if (!value) return O;
			DWORD lenDst;
			assert((lenDst = ExpandEnvironmentStrings(value, O, 0)));
			if (lenDst - 1 == value.Length()) return &value;
			String eval((size_t)lenDst - 1);
			ExpandEnvironmentStrings(value, eval, lenDst);
			return eval;
		}
	public:
		inline auto &operator=(const String &value) reflect_to_self(Value(value));
		inline operator bool() const reflect_as(name);
		inline operator String() const reflect_as(&Value());
		inline String operator+() const reflect_as(ValueExpend());
		inline bool operator!=(Null) const reflect_as(name);
		inline bool operator==(Null) const reflect_as(!name);
	};

	Environments() {}
	Environments(Null) {}
	Environments(Environments &env) : lpEnv(env.lpEnv) reflect_to(env.lpEnv = O);
	Environments(Environments &&env) : lpEnv(env.lpEnv) reflect_to(env.lpEnv = O);

	~Environments() reflect_to(Free());

	inline static Environments From(std::initializer_list<const String> envs) {
		size_t maxlen = 1;
		for (const String &env : envs)
			if (auto len = env.Length())
				maxlen += env.Length() + 1;
		if (maxlen <= 1) return O;
		LPTCH lpEnv = (LPTCH)Heap::Now()->Alloc(maxlen * sizeof(TCHAR)), pEnv = lpEnv;
		for (const String &env : envs)
			if (auto len = env.Length()) {
				CopyMemory(pEnv, (LPCTSTR)env, len * sizeof(TCHAR));
				pEnv += len;
				*pEnv++ = '\0';
			}
		*pEnv = '\0';
		return lpEnv;
	}
	inline static Environments From(std::initializer_list<const Variable> vars) {
		size_t maxlen = 1;
		for (const Variable &var : vars)
			if (var)
				maxlen += var.name.Length() + var.value.Length() + 2;
		if (maxlen <= 1) return O;
		LPTCH lpEnv = (LPTCH)Heap::Now()->Alloc(maxlen * sizeof(TCHAR)), pEnv = lpEnv;
		for (const Variable &var : vars)
			if (var) {
				CopyMemory(pEnv, (LPCTSTR)var.name, var.name.Length() * sizeof(TCHAR));
				pEnv += var.name.Length();
				*pEnv++ = '=';
				CopyMemory(pEnv, (LPCTSTR)var.value, var.value.Length() * sizeof(TCHAR));
				pEnv += var.value.Length();
				*pEnv++ = '\0';
			}
		*pEnv = '\0';
		return lpEnv;
	}
	inline static Environments Current() assert_reflect_as(auto lpEnv = GetEnvironmentStrings(), lpEnv);

	inline bool Free() {
		if (lpEnv)
			if (!Heap::Now()->Free(lpEnv))
				return true;
		lpEnv = O;
		return false;
	}

	inline void Use() const assert_reflect_as(SetEnvironmentStrings(lpEnv));

public: // Property - Count
	/* R */ inline size_t Count() const {
		if (!lpEnv) return 0;
		size_t count = 0;
		for (auto ch = lpEnv; *ch != '\0'; ++ch) {
			while (*ch != '\0') ++ch;
			++count;
		}
		return count;
	}
public: // Property - Entries
	/* R */ inline std::vector<String> Entries() const {
		size_t count = Count();
		std::vector<String> list(count);
		auto lpEnv = this->lpEnv;
		for (const String &e : list)
			lpEnv += (e = CString(lpEnv, MaxLenNotice)).Length() + 1;
		return list;
	}
public: // Property - Variables
	/* R */ inline std::vector<Variable> Variables() const {
		size_t count = Count();
		std::vector<Variable> list(count);
		auto lpEnv = this->lpEnv;
		for (const Variable &e : list) {
			auto lpName = lpEnv;
			while (*lpEnv++ != '=');
			e.name = CString(lpEnv - lpName - 1, lpName);
			e.value = CString(lpEnv, MaxLenNotice);
			lpEnv += e.value.Length() + 1;
		}
		return list;
	}

public:
	inline operator bool() const reflect_as(lpEnv);
};
using EnvVar = Environments::Variable;
class CurrentEnvironment {
	class Variable {
		friend class CurrentEnvironment;
		const String name;
	public:
		Variable(const String &name) : name(+name) {}
	public: // Property - Value
		/* W */ inline Variable &Value(const String &value) assert_reflect_as_self(SetEnvironmentVariable(name, value.str_safe()));
		/* R */ inline String Value() const {
			DWORD len;
			assert((len = GetEnvironmentVariable(name, O, 0)));
			String value((size_t)len - 1);
			GetEnvironmentVariable(name, value, len);
			return value;
		}
	public: // Property - ValueExpend
		inline String ValueExpend() const {
			DWORD lenSrc;
			assert((lenSrc = GetEnvironmentVariable(name, O, 0)));
			String value((size_t)lenSrc - 1);
			GetEnvironmentVariable(name, value, lenSrc);
			DWORD lenDst;
			assert((lenDst = ExpandEnvironmentStrings(value, O, 0)));
			if (lenSrc == lenDst) return value;
			String eval((size_t)lenDst - 1);
			ExpandEnvironmentStrings(value, eval, lenDst);
			return eval;
		}
	public:
		inline auto &operator=(const String &value) reflect_to_self(Value(value));
		inline operator bool() const reflect_as(GetEnvironmentVariable(name, O, 0));
		inline operator String() const reflect_as(Value());
		inline String operator+() const reflect_as(ValueExpend());
		inline bool operator!=(Null) const reflect_as(GetEnvironmentVariable(name, O, 0) != 0);
		inline bool operator==(Null) const reflect_as(GetEnvironmentVariable(name, O, 0) == 0);
	};
public:
	inline operator Environments() reflect_as(Environments::Current());
	inline Variable operator[](const String &str) reflect_as(str);
	inline CurrentEnvironment &operator=(const Environments &env) reflect_to_self(env.Use());
} Environment;
enum_flags(StartupFlag, DWORD,
	UseShowWindow       = STARTF_USESHOWWINDOW,
	UseSize             = STARTF_USESIZE,
	UsePosition         = STARTF_USEPOSITION,
	UseCountChars       = STARTF_USECOUNTCHARS,
	UseFillAttribute    = STARTF_USEFILLATTRIBUTE,
	UseStdHandles       = STARTF_USESTDHANDLES,
	UseHotKey           = STARTF_USEHOTKEY,

	RunFullScreen       = STARTF_RUNFULLSCREEN,

	ForceOnFeedback     = STARTF_FORCEONFEEDBACK,
	ForceOffFeedback    = STARTF_FORCEOFFFEEDBACK,

	TitleIsLinkName     = STARTF_TITLEISLINKNAME,
	TitleIsAppID        = STARTF_TITLEISAPPID,
	PreventPinning      = STARTF_PREVENTPINNING,
	UntrustedSource     = STARTF_UNTRUSTEDSOURCE);
class StartupInfo : protected STARTUPINFO {
public:
	StartupInfo() : STARTUPINFO{ 0 } reflect_to(this->cb = sizeof(*this));
// public: // Property - Desktop
// 	/* W */ inline auto &Desktop(LPWSTR lpDesktop) reflect_to_self(this->lpDesktop = lpDesktop);
// 	/* R */ inline auto  Desktop() const reflect_as(this->lpDesktop);
// public: // Property - Title
// 	/* W */ inline auto &Title(LPWSTR lpTitle) reflect_to_self(this->lpTitle = lpTitle);
// 	/* R */ inline auto  Title() const reflect_as(this->lpTitle);
public: // Property - Position
	/* W */ inline auto  &Position(LPoint pos) reflect_to_self(this->dwFlags |= STARTF_USEPOSITION, this->dwX = pos.x, this->dwY = pos.y);
	/* R */ inline LPoint Position() const reflect_as({ (LONG)this->dwX, (LONG)this->dwY });
public: // Property - Size
	/* W */ inline auto &Size(LSize size) reflect_to_self(this->dwFlags |= STARTF_USESIZE, this->dwXSize = size.cx, this->dwYSize = size.cy);
	/* R */ inline LSize Size() const reflect_as({ (LONG)this->dwXSize, (LONG)this->dwYSize });
public: // Property - CountCmhars
	/* W */ inline auto &CountChars(LSize size) reflect_to_self(this->dwFlags |= STARTF_USECOUNTCHARS, this->dwXCountChars = size.cx, this->dwYCountChars = size.cy);
	/* R */ inline LSize CountChars() const reflect_as({ (LONG)this->dwXCountChars, (LONG)this->dwYCountChars });
public: // Property - FillAttribute
	/* W */ inline auto &FillAttribute(DWORD dwFillAttribute) reflect_to_self(this->dwFlags |= STARTF_USEFILLATTRIBUTE, this->dwFillAttribute = dwFillAttribute);
	/* R */ inline auto  FillAttribute() const reflect_as(this->dwFillAttribute);
public: // Property - Flags
	/* R */ inline StartupFlag Flags() const reflect_as(force_cast<StartupFlag>(this->dwFlags));
public: // Property - ShowWindow
	/* W */ inline auto &ShowWindow(SW wShowWindow) reflect_to_self(this->dwFlags |= STARTF_USESHOWWINDOW, this->wShowWindow = wShowWindow.yield());
	/* R */ inline SW    ShowWindow() const reflect_as(force_cast<SW>(this->wShowWindow));
public: // Property - StdInput
	/* W */ inline auto &StdInput(HANDLE hStdInput) reflect_to_self(this->dwFlags |= STARTF_USESTDHANDLES, this->hStdInput = hStdInput);
	/* R */ inline auto  StdInput() const reflect_as(this->hStdInput);
public: // Property - StdOutput
	/* W */ inline auto &StdOutput(HANDLE hStdOutput) reflect_to_self(this->dwFlags |= STARTF_USESTDHANDLES, this->hStdOutput = hStdOutput);
	/* R */ inline auto  StdOutput() const reflect_as(this->hStdOutput);
public: // Property - StdError
	/* W */ inline auto &StdError(HANDLE hStdError) reflect_to_self(this->dwFlags |= STARTF_USESTDHANDLES, this->hStdError = hStdError);
	/* R */ inline auto  StdError() const reflect_as(this->hStdError);
public:
	inline LPSTARTUPINFO operator&() reflect_as(this);
};
class Handle_Based(Process) {
	using super = HandleBase<Process>;
	Process(HANDLE h) : super(h) {}
public:
	using Access = ProcessAccess;
	using CreateFlag = ProccessCreateFlag;

	class CreateStruct {
		friend class Process;
		LPCTSTR lpApplicationName;
		String CommandLine;
		LPSECURITY_ATTRIBUTES lpProcessAttributes = O;
		LPSECURITY_ATTRIBUTES lpThreadAttributes = O;
		bool bInheritHandles = false;
		CreateFlag dwCreationFlags = CreateFlag::Default;
		LPVOID lpEnvironment = O;
		LPCTSTR lpCurrentDirectory = O;
		StartupInfo startupInfo;
		_Out_ PROCESS_INFORMATION processInfo = { O };
		CreateStruct(LPCTSTR lpApplicationName, const String &CommandLine) : lpApplicationName(lpApplicationName), CommandLine(+CommandLine) {}
	public:
		inline auto &Security(const SecAttr &ProcessAttributes) reflect_to_self(this->lpProcessAttributes = &ProcessAttributes);
		inline auto &Security(LPSECURITY_ATTRIBUTES lpProcessAttributes) reflect_to_self(this->lpProcessAttributes = lpProcessAttributes);
		inline auto &SecurityThread(const SecAttr &ThreadAttributes) reflect_to_self(this->lpThreadAttributes = &ThreadAttributes);
		inline auto &SecurityThread(LPSECURITY_ATTRIBUTES lpThreadAttributes) reflect_to_self(this->lpThreadAttributes = lpThreadAttributes);
		inline auto &Inherit(bool bInheritHandles) reflect_to_self(this->bInheritHandles = bInheritHandles);
	public:
		inline auto &DebugProcess() reflect_to_self(this->dwCreationFlags |= CreateFlag::DebugProcess);
		inline auto &DebugOnlyThisProcess() reflect_to_self(this->dwCreationFlags |= CreateFlag::DebugOnlyThisProcess);
		inline auto &Suspended() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateSuspended);
		inline auto &DetachedProcess() reflect_to_self(this->dwCreationFlags |= CreateFlag::DetachedProcess);

		inline auto &NewConsole() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateNewConsole);
		inline auto &NormalPriorityClass() reflect_to_self(this->dwCreationFlags |= CreateFlag::NormalPriorityClass);
		inline auto &IdlePriorityClass() reflect_to_self(this->dwCreationFlags |= CreateFlag::IdlePriorityClass);
		inline auto &HighPriorityClass() reflect_to_self(this->dwCreationFlags |= CreateFlag::HighPriorityClass);

		inline auto &RealtimePriorityClass() reflect_to_self(this->dwCreationFlags |= CreateFlag::RealtimePriorityClass);
		inline auto &NewProcessGroup() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateNewProcessGroup);
		inline auto &UnicodeEnvironment() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateUnicodeEnvironment);
		inline auto &SeparateWowVdm() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateSeparateWowVdm);

		inline auto &SharedWowVdm() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateSharedWowVdm);
		inline auto &ForceDos() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateForceDos);
		inline auto &BelowNormalPriorityClass() reflect_to_self(this->dwCreationFlags |= CreateFlag::BelowNormalPriorityClass);
		inline auto &AboveNormalPriorityClass() reflect_to_self(this->dwCreationFlags |= CreateFlag::AboveNormalPriorityClass);

		inline auto &InheritParentAffinity() reflect_to_self(this->dwCreationFlags |= CreateFlag::InheritParentAffinity);
		inline auto &InheritCallerPriority() reflect_to_self(this->dwCreationFlags |= CreateFlag::InheritCallerPriority);
		inline auto &ProtectedProcess() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateProtectedProcess);
		inline auto &ExtendedStartupinfoPresent() reflect_to_self(this->dwCreationFlags |= CreateFlag::ExtendedStartupinfoPresent);

		inline auto &ProcessModeBackgroundBegin() reflect_to_self(this->dwCreationFlags |= CreateFlag::ProcessModeBackgroundBegin);
		inline auto &ProcessModeBackgroundEnd() reflect_to_self(this->dwCreationFlags |= CreateFlag::ProcessModeBackgroundEnd);
		inline auto &SecureProcess() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateSecureProcess);

		inline auto &BreakawayFromJob() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateBreakawayFromJob);
		inline auto &PreserveCodeAuthzLevel() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreatePreserveCodeAuthzLevel);
		inline auto &DefaultErrorMode() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateDefaultErrorMode);
		inline auto &NoWindow() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateNoWindow);

		inline auto &ProfileUser() reflect_to_self(this->dwCreationFlags |= CreateFlag::ProfileUser);
		inline auto &ProfileKernel() reflect_to_self(this->dwCreationFlags |= CreateFlag::ProfileKernel);
		inline auto &ProfileServer() reflect_to_self(this->dwCreationFlags |= CreateFlag::ProfileServer);
		inline auto &IgnoreSystemDefault() reflect_to_self(this->dwCreationFlags |= CreateFlag::CreateIgnoreSystemDefault);
	public:
		// inline auto &Environment();
		// inline auto &CurrentDirectory();
	public:
		inline operator Process() assert_reflect_as(CreateProcess(
			lpApplicationName, CommandLine,
			lpProcessAttributes, lpThreadAttributes, bInheritHandles,
			dwCreationFlags.yield(), lpEnvironment, lpCurrentDirectory,
			&startupInfo, &processInfo
		), processInfo.hProcess);
	};
	inline static CreateStruct Create(LPCTSTR lpApplicationName, const String &CommandLine = O) reflect_as({lpApplicationName, CommandLine});

	class OpenStruct {
		friend class Process;
		Access dwDesiredAccess = Access::All;
		bool bInheritHandle = false;
		DWORD dwProcessId;
		OpenStruct(DWORD dwProcessId) : dwProcessId(dwProcessId) {}
	public:
		inline auto &Accesses(Access dwDesiredAccess) reflect_to_self(this->dwDesiredAccess = dwDesiredAccess);
		inline auto &Inherit(bool bInheritHandle = true) reflect_to_self(this->bInheritHandle = bInheritHandle);
	public:
		inline operator Process() reflect_as(OpenProcess(dwDesiredAccess.yield(), bInheritHandle, dwProcessId));
	};
	inline OpenStruct Open(DWORD dwProcessId) reflect_as(dwProcessId);

	inline bool Terminate(UINT uExitCode = 0) {
		if (super::hObject)
			if (!TerminateProcess(super::hObject, uExitCode))
				return false;
		super::hObject = O;
		return true;
	}
	inline static void Exit(UINT uExitCode = 0) reflect_to(ExitProcess(uExitCode));

	inline static Process Current() reflect_as(GetCurrentProcess());

public: // Property - ExitCode
	/* R */ inline auto ExitCode() const reflect_to(DWORD dwCode, (dwCode = GetExitCodeProcess(self, &dwCode)), dwCode);
public: // Property - ID
	/* R */ inline auto ID() const reflect_as(GetProcessId(self));
public: // Property - Memory
	/* W */ inline auto Memory() const assert_reflect_to(PROCESS_MEMORY_COUNTERS pmc, GetProcessMemoryInfo(self, &pmc, sizeof(pmc)), pmc);
public: // Property - WorkingSetSize
	/* W */ inline auto &WorkingSetSize(SIZE_T Min, SIZE_T Max) assert_reflect_as_self(SetProcessWorkingSetSize(self, Min, Max));
	/* R */ inline auto  WorkingSetSize() const assert_reflect_to(struct _B_(SIZE_T Min, Max;) size, GetProcessWorkingSetSize(self, &size.Min, &size.Max), size);
public: // Property - WorkingSetSizeEx
	//GetProcessWorkingSetSizeEx
	//SetProcessWorkingSetSizeEx
public: // Property - Informations
	//GetProcessInformation
	//SetProcessInformation
public: // Property - PriorityBoost
	//GetProcessPriorityBoost
	//SetProcessPriorityBoost
public: // Property - ShutdownParameters
	//GetProcessShutdownParameters
	//SetProcessShutdownParameters
public: // Property - MitigationPolicy
	//GetProcessMitigationPolicy
	//SetProcessMitigationPolicy

public: // Property - CommandLine
	/* R */ inline static String CommandLine() reflect_as(+CString(GetCommandLine(), MaxLenNotice));
public: // Property - Environment
	///* R */ inline static auto Environment() {
	//	return SetEnvironmentStrings();
	//}
	///* R */ inline static auto EnvironmentVariable() {
	//	return GetEnvironmentVariable(;
	//}
};
#pragma endregion

}
