#pragma once

#include "security.h"

namespace WX {

#pragma region File
enum_flags(FileAccess, HandleAccess,
	enum_default No      = 0,
	ReadData             = FILE_READ_DATA,        // file & pipe
	ListDirectory        = FILE_LIST_DIRECTORY,   // directory
	WriteData            = FILE_WRITE_DATA,       // file & pipe
	AddFile              = FILE_ADD_FILE,         // directory
	AppendData           = FILE_APPEND_DATA,      // file
	AddSubdirectory      = FILE_ADD_SUBDIRECTORY, // directory
	CreatePipeInstance   = FILE_CREATE_PIPE_INSTANCE, // named pipe
	ReadEA               = FILE_READ_EA,  // file & directory
	WriteEA              = FILE_WRITE_EA, // file & directory
	Execute              = FILE_EXECUTE,  // file
	Traverse             = FILE_TRAVERSE, // directory
	DeleteChild          = FILE_DELETE_CHILD,     // directory
	ReadAttributes       = FILE_READ_ATTRIBUTES,  // all
	WriteAttributes      = FILE_WRITE_ATTRIBUTES, // all
	enum_complex GenericRead    = FILE_GENERIC_READ,
	enum_complex GenericWrite   = FILE_GENERIC_WRITE,
	enum_complex GenericExecute = FILE_GENERIC_EXECUTE,
	enum_complex All            = FILE_ALL_ACCESS);
enum_flags(FileAttribute, DWORD,
	ReadOnly              = FILE_ATTRIBUTE_READONLY,
	Hidden                = FILE_ATTRIBUTE_HIDDEN,
	System                = FILE_ATTRIBUTE_SYSTEM,
	Directory             = FILE_ATTRIBUTE_DIRECTORY,
	Archive               = FILE_ATTRIBUTE_ARCHIVE,
	Device                = FILE_ATTRIBUTE_DEVICE,
	Normal                = FILE_ATTRIBUTE_NORMAL,
	Temporary             = FILE_ATTRIBUTE_TEMPORARY,
	SparseFile            = FILE_ATTRIBUTE_SPARSE_FILE,
	ReparsePoint          = FILE_ATTRIBUTE_REPARSE_POINT,
	Compressed            = FILE_ATTRIBUTE_COMPRESSED,
	Offline               = FILE_ATTRIBUTE_OFFLINE,
	NotContentIndexed     = FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
	Encrypted             = FILE_ATTRIBUTE_ENCRYPTED,
	IntegrityStream       = FILE_ATTRIBUTE_INTEGRITY_STREAM,
	Virtual               = FILE_ATTRIBUTE_VIRTUAL,
	NoScrubData           = FILE_ATTRIBUTE_NO_SCRUB_DATA,
	EA                    = FILE_ATTRIBUTE_EA,
	Pinned                = FILE_ATTRIBUTE_PINNED,
	Unpinned              = FILE_ATTRIBUTE_UNPINNED,
	RecallOnOpen          = FILE_ATTRIBUTE_RECALL_ON_OPEN,
	RecallOnDataAccess    = FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS);
enum_flags(FileFlag, DWORD,
	WriteThrough          = FILE_FLAG_WRITE_THROUGH,
	Overlapped            = FILE_FLAG_OVERLAPPED,
	NoBuffering           = FILE_FLAG_NO_BUFFERING,
	RandomAccess          = FILE_FLAG_RANDOM_ACCESS,
	SequentialScan        = FILE_FLAG_SEQUENTIAL_SCAN,
	DeleteOnClose         = FILE_FLAG_DELETE_ON_CLOSE,
	BackupSemantics       = FILE_FLAG_BACKUP_SEMANTICS,
	PosixSemantics        = FILE_FLAG_POSIX_SEMANTICS,
	SessionAware          = FILE_FLAG_SESSION_AWARE,
	OpenReparsePoint      = FILE_FLAG_OPEN_REPARSE_POINT,
	OpenNoRecall          = FILE_FLAG_OPEN_NO_RECALL,
	FirstPipeInstance     = FILE_FLAG_FIRST_PIPE_INSTANCE,
	OpenRequiringOplock   = FILE_FLAG_OPEN_REQUIRING_OPLOCK,
	IgnoreImpersonatedDevicemap  = FILE_FLAG_IGNORE_IMPERSONATED_DEVICEMAP);
enum_flags(FileShares, DWORD,
	enum_default No = 0,
	Read   = FILE_SHARE_READ,
	Write  = FILE_SHARE_WRITE,
	Delete = FILE_SHARE_DELETE);
enum_class(FileTypes, DWORD, 
	enum_default Unknown = FILE_TYPE_UNKNOWN,
	Disk      = FILE_TYPE_DISK,
	Char      = FILE_TYPE_CHAR,
	Pipe      = FILE_TYPE_PIPE,
	Remote    = FILE_TYPE_REMOTE);
struct FileTime : protected FILETIME {
	FileTime() : FILETIME{ 0 } {}
	FileTime(const FILETIME &ft) : FILETIME(ft) {}
	FileTime(LARGE_INTEGER li) { reuse_as<LARGE_INTEGER>(self) = li; }
	FileTime(const SYSTEMTIME &st) assert(SystemTimeToFileTime(&st, this));
	inline FileTime LocalTime() assert_reflect_to(FILETIME ft, FileTimeToLocalFileTime(this, &ft), ft);
	inline operator SysTime() const assert_reflect_to(SysTime st, FileTimeToSystemTime(this, &st), st);
	inline operator SYSTEMTIME() const assert_reflect_to(SysTime st, FileTimeToSystemTime(this, &st), st);
	inline operator LARGE_INTEGER() const reflect_as(force_cast<LARGE_INTEGER>(*this));
	inline LPFILETIME operator &() reflect_as(this);
	inline const FILETIME *operator &() const reflect_as(this);
};
struct FileTimes {
	FileTime Created, LastAccess, LastWrite;
};
struct FileBaseInfo : protected FILE_BASIC_INFO {
	FileBaseInfo() {}
public: // Property - TimeCreation
	/* W */ inline auto &TimeCreation(FileTime CreationTime) reflect_to_self(this->CreationTime = CreationTime);
	/* R */ inline FileTime TimeCreation() const reflect_as(this->CreationTime);
public: // Property - TimeLastAccess
	/* W */ inline auto &TimeLastAccess(FileTime LastAccessTime) reflect_to_self(this->LastAccessTime = LastAccessTime);
	/* R */ inline FileTime TimeLastAccess() const reflect_as(this->LastAccessTime);
public: // Property - TimeLastWrite
	/* W */ inline auto &TimeLastWrite(FileTime LastWriteTime) reflect_to_self(this->LastWriteTime = LastWriteTime);
	/* R */ inline FileTime TimeLastWrite() const reflect_as(this->LastWriteTime);
public: // Property - TimeChange
	/* W */ inline auto &TimeChange(FileTime ChangeTime) reflect_to_self(this->ChangeTime = ChangeTime);
	/* R */ inline FileTime TimeChange() const reflect_as(this->ChangeTime);
public: // Property - Attributes
	/* W */ inline auto &Attributes(FileAttribute fa) reflect_to_self(this->FileAttributes = fa.yield());
	/* R */ inline FileAttribute Attributes() const reflect_as(force_cast<FileAttribute>(this->FileAttributes));
public:
	inline FILE_BASIC_INFO *operator &() reflect_as(this);
	inline const FILE_BASIC_INFO *operator &() const reflect_as(this);
};
class Handle_Based(File) {
	using super = HandleBase<File>;
	File(HANDLE hFile) : super(hFile) {}
public:
	using Access = FileAccess;
	using Shares = FileShares;
	using Attribute = FileAttribute;
	using Flag = FileFlag;
	using Types = FileTypes;

	File() {}
	File(Null) {}
	File(File &file) : super(file) {}
	File(File &&file) : super(file) {}

	class CreateStruct {
		friend class File;
		LPCTSTR lpFileName;
		Access dwDesiredAccess = Access::No;
		Shares dwShareMode = Shares::No;
		LPSECURITY_ATTRIBUTES lpSecurityAttributes = O;
		DWORD dwCreationDisposition;
		DWORD dwFlagsAndAttributes;
		HANDLE hTemplateFile = O;
		CreateStruct(LPCTSTR lpFileName) : lpFileName(lpFileName) {}
	public:
		inline auto &Accesses(Access dwDesiredAccess) reflect_to_self(this->dwDesiredAccess = dwDesiredAccess);
		inline auto &Share(Shares dwShareMode) reflect_to_self(this->dwShareMode = dwShareMode);
		inline auto &Security(const SecAttr &SecurityAttributes) reflect_to_self(this->lpSecurityAttributes = &SecurityAttributes);
		inline auto &Security(LPSECURITY_ATTRIBUTES lpSecurityAttributes) reflect_to_self(this->lpSecurityAttributes = lpSecurityAttributes);
		inline auto &Template(HANDLE hTemplateFile) reflect_to_self(this->hTemplateFile = hTemplateFile);
		inline auto &Attributes(Attribute dwAttributes) reflect_to_self(this->dwFlagsAndAttributes = dwAttributes.yield());
		inline auto &Flags(Flag dwFlags) reflect_to_self(this->dwFlagsAndAttributes = dwFlags.yield());
	public:
		inline auto &CreateNew() reflect_to_self(this->dwCreationDisposition = CREATE_NEW);
		inline auto &CreateAlways() reflect_to_self(this->dwCreationDisposition = CREATE_ALWAYS);
		inline auto &OpenExisting() reflect_to_self(this->dwCreationDisposition = OPEN_EXISTING);
		inline auto &OpenAlways() reflect_to_self(this->dwCreationDisposition = OPEN_ALWAYS);
		inline auto &TruncateExisting() reflect_to_self(this->dwCreationDisposition = TRUNCATE_EXISTING);
	public:
		inline operator File() reflect_as(CreateFile(lpFileName, dwDesiredAccess.yield(), dwShareMode.yield(), lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile));
	};
	inline static CreateStruct Create(LPCTSTR lpFileName) reflect_as(lpFileName);

	inline static bool Delete(LPCTSTR lpFileName) reflect_as(DeleteFile(lpFileName));
	inline static bool Copy(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, bool bFailIfExists = false) reflect_as(CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists));

	inline DWORD Read(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead) assert_reflect_as(ReadFile(self, lpBuffer, dwNumberOfBytesToRead, &dwNumberOfBytesToRead, O), dwNumberOfBytesToRead);
	inline DWORD Write(LPCVOID lpBuffer, DWORD dwNumberOfBytesToRead) assert_reflect_as(WriteFile(self, lpBuffer, dwNumberOfBytesToRead, &dwNumberOfBytesToRead, O), dwNumberOfBytesToRead);
	inline auto &FlushBuffers() assert_reflect_as_self(FlushFileBuffers(self));

public: // Property - Size
	/* W */ inline uint64_t Size() const assert_reflect_to(uint64_t size = 0, GetFileSizeEx(self, (PLARGE_INTEGER)&size), size);
public: // Property - Time
	/* W */ inline FileTimes Times() const assert_reflect_to(FileTimes ts, GetFileTime(self, &ts.Created, &ts.LastAccess, &ts.LastWrite), ts);
public: // Property - Type
	/* W */ inline Types Type() const check_reflect_to(auto type = GetFileType(self), force_cast<Types>(type));

};
#pragma endregion

}
