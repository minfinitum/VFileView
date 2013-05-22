#pragma once

#include <windows.h>
#include <string>
#include <ntsecapi.h>

//#include <ntstatus.h>
//#include <winternl.h>

// Structure Definitions

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );        \
    (p)->RootDirectory = r;                           \
    (p)->Attributes = a;                              \
    (p)->ObjectName = n;                              \
    (p)->SecurityDescriptor = s;                      \
    (p)->SecurityQualityOfService = NULL;             \
}

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define NT_INFORMATION(Status) ((ULONG)(Status) >> 30 == 1)
#define NT_WARNING(Status) ((ULONG)(Status) >> 30 == 2)
#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)
#define APPLICATION_ERROR_MASK           0x20000000
#define ERROR_SEVERITY_SUCCESS           0x00000000
#define ERROR_SEVERITY_INFORMATIONAL     0x40000000
#define ERROR_SEVERITY_WARNING           0x80000000
#define ERROR_SEVERITY_ERROR             0xC0000000
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L) // ntsubauth
//#define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005L)
//#define STATUS_UNSUCCESSFUL              ((NTSTATUS)0xC0000001L)
//#define STATUS_NOT_IMPLEMENTED           ((NTSTATUS)0xC0000002L)
//#define STATUS_INVALID_INFO_CLASS        ((NTSTATUS)0xC0000003L)
//#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)
//#define STATUS_INVALID_PARAMETER         ((NTSTATUS)0xC000000DL)
//#define STATUS_NO_SUCH_DEVICE            ((NTSTATUS)0xC000000EL)
//#define STATUS_NO_SUCH_FILE              ((NTSTATUS)0xC000000FL)
//#define STATUS_INVALID_DEVICE_REQUEST    ((NTSTATUS)0xC0000010L)
//#define STATUS_END_OF_FILE               ((NTSTATUS)0xC0000011L)
//#define STATUS_NO_MEDIA_IN_DEVICE        ((NTSTATUS)0xC0000013L)
//#define STATUS_UNRECOGNIZED_MEDIA        ((NTSTATUS)0xC0000014L)
//#define STATUS_MORE_PROCESSING_REQUIRED  ((NTSTATUS)0xC0000016L)
//#define STATUS_ACCESS_DENIED             ((NTSTATUS)0xC0000022L)
//#define STATUS_BUFFER_TOO_SMALL          ((NTSTATUS)0xC0000023L)
//#define STATUS_OBJECT_NAME_NOT_FOUND     ((NTSTATUS)0xC0000034L)
//#define NT_STATUS_OBJECT_PATH_SYNTAX_BAD ((NTSTATUS)0xC000003BL)

// Valid values for the Attributes field
//
// This handle can be inherited by child processes of the current process.
#define OBJ_INHERIT				0x00000002L

// This flag only applies to objects that are named within the Object Manager.
// By default, such objects are deleted when all open handles to them are closed.
// If this flag is specified, the object is not deleted when all open handles are
// closed. Drivers can use ZwMakeTemporaryObject to delete permanent objects.
#define OBJ_PERMANENT			0x00000010L

// Only a single handle can be open for this object.
#define OBJ_EXCLUSIVE			0x00000020L

// If this flag is specified, a case-insensitive comparison is used when
// matching the ObjectName parameter against the names of existing objects.
// Otherwise, object names are compared using the default system settings.
#define OBJ_CASE_INSENSITIVE	0x00000040L

typedef enum _FILE_INFORMATION_CLASS {
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileCopyOnWriteInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileOleClassIdInformation,
    FileOleStateBitsInformation,
    FileNetworkOpenInformation,
    FileObjectIdInformation,
    FileOleAllInformation,
    FileOleDirectoryInformation,
    FileContentIndexInformation,
    FileInheritContentIndexInformation,
    FileOleInformation,
    FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080

#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_FOR_RECOVERY                  0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800

#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000

typedef struct _FILE_BOTH_DIR_INFORMATION {
    ULONG  NextEntryOffset;
    ULONG  FileIndex;
    LARGE_INTEGER  CreationTime;
    LARGE_INTEGER  LastAccessTime;
    LARGE_INTEGER  LastWriteTime;
    LARGE_INTEGER  ChangeTime;
    LARGE_INTEGER  EndOfFile;
    LARGE_INTEGER  AllocationSize;
    ULONG  FileAttributes;
    ULONG  FileNameLength;
    ULONG  EaSize;
    CCHAR  ShortNameLength;
    WCHAR  ShortName[12];
    WCHAR  FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

typedef struct _FILE_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef struct _FILE_FULL_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG Unknown;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaInformationLength;
    WCHAR FileName[1];
} FILE_FULL_DIRECTORY_INFORMATION, *PFILE_FULL_DIRECTORY_INFORMATION;

typedef struct _FILE_BOTH_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG Unknown;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaInformationLength;
    UCHAR AlternateNameLength;
    WCHAR AlternateName[12];
    WCHAR FileName[1];
} FILE_BOTH_DIRECTORY_INFORMATION, *PFILE_BOTH_DIRECTORY_INFORMATION;

typedef struct _FILE_NAMES_INFORMATION {
    ULONG NextEntryOffset;
    ULONG Unknown;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

typedef struct FILE_BASIC_INFORMATION {
    LARGE_INTEGER  CreationTime;
    LARGE_INTEGER  LastAccessTime;
    LARGE_INTEGER  LastWriteTime;
    LARGE_INTEGER  ChangeTime;
    ULONG  FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

typedef struct _FILE_STREAM_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG StreamNameLength;
    LARGE_INTEGER StreamSize;
    LARGE_INTEGER StreamAllocationSize;
    WCHAR StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

// ----------------------------------------------------------------------------
// winternl.h

typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef
VOID
(NTAPI *PIO_APC_ROUTINE) (
                          IN PVOID ApcContext,
                          IN PIO_STATUS_BLOCK IoStatusBlock,
                          IN ULONG Reserved
                          );

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

// ----------------------------------------------------------------------------
// Method definitions

// NtCreateFile
typedef NTSTATUS (STDAPICALLTYPE * FPNtCreateFile)
(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions,
    IN PVOID EaBuffer OPTIONAL,
    IN ULONG EaLength
    );

// NtOpenFile
typedef NTSTATUS (STDAPICALLTYPE * FPNtOpenFile)
(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG ShareAccess,
    IN ULONG OpenOptions
    );

// NtReadFile
typedef NTSTATUS (STDAPICALLTYPE * FPNtReadFile)
(
     IN HANDLE               FileHandle,
     IN HANDLE               Event OPTIONAL,
     IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
     IN PVOID                ApcContext OPTIONAL,
     OUT PIO_STATUS_BLOCK    IoStatusBlock,
     OUT PVOID               Buffer,
     IN ULONG                Length,
     IN PLARGE_INTEGER       ByteOffset OPTIONAL,
     IN PULONG               Key OPTIONAL
     );

// NtDeleteFile
typedef NTSTATUS (STDAPICALLTYPE * FPNtDeleteFile)
(
     IN POBJECT_ATTRIBUTES  ObjectAttributes
     );

// NtClose
typedef NTSTATUS (STDAPICALLTYPE * FPNtClose)
(
    IN HANDLE FileHandle
    );

// NtQueryDirectoryFile
typedef NTSTATUS (STDAPICALLTYPE * FPNtQueryDirectoryFile)
(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG FileInformationLength,
    IN FILE_INFORMATION_CLASS FileInformationClass,
    IN BOOLEAN ReturnSingleEntry,
    IN PUNICODE_STRING FileName OPTIONAL,
    IN BOOLEAN RestartScan
    );

// NtQueryInformationFile
typedef NTSTATUS (STDAPICALLTYPE * FPNtQueryInformationFile)
(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
    );

// RtlInitUnicodeString
typedef void (STDAPICALLTYPE * FPRtlInitUnicodeString)
(
    IN OUT PUNICODE_STRING DestinationString,
    IN LPCWSTR SourceString
    );

typedef struct _TIME_FIELDS
{
    USHORT Year;
    USHORT Month;
    USHORT Day;
    USHORT Hour;
    USHORT Minute;
    USHORT Second;
    USHORT Milliseconds;
    USHORT Weekday;
} TIME_FIELDS, *PTIME_FIELDS;

typedef void (STDAPICALLTYPE * FPRtlTimeToTimeFields)
(
    IN PLARGE_INTEGER  Time,
    IN PTIME_FIELDS  TimeFields
    );



#define		DLL_NAME_NTDLL			L"ntdll.dll"


class NtDllLib
{
public:
    NtDllLib()
    {
        m_hDll = NULL;
        unloadDll();
    }

    ~NtDllLib()
    {
        unloadDll();
    }

    static NtDllLib * NtDllLib::instance()
    {
        static NtDllLib ntdlllib;
        return &ntdlllib;
    }

    bool isLoaded()
    {
        return (m_hDll != NULL);
    }

    bool loadDll(const wchar_t * szPath)
    {
        // already opened
        if (m_hDll)
            unloadDll();

        // Validate OS


        std::wstring location = szPath;
        if (location[location.length() -1] != L'\\' && location[location.length() -1] != L'/')
            location += L"\\";

        location += DLL_NAME_NTDLL;
        if (GetFileAttributesW(location.c_str()) == INVALID_FILE_ATTRIBUTES)
        {
            location = L".\\";
            location += DLL_NAME_NTDLL;
        }

        m_hDll = LoadLibraryW(location.c_str());
        if ( ! m_hDll)
            return false;

        m_fpNtCreateFile = (FPNtCreateFile)GetProcAddress(m_hDll, "NtCreateFile");
        m_fpNtOpenFile = (FPNtOpenFile)GetProcAddress(m_hDll, "NtOpenFile");
        m_fpNtReadFile = (FPNtReadFile)GetProcAddress(m_hDll, "NtReadFile");
        m_fpNtDeleteFile = (FPNtDeleteFile)GetProcAddress(m_hDll, "NtDeleteFile");
        m_fpNtClose = (FPNtClose)GetProcAddress(m_hDll, "NtClose");

        m_fpNtQueryDirectoryFile = (FPNtQueryDirectoryFile)GetProcAddress(m_hDll, "NtQueryDirectoryFile");
        m_fpNtQueryInformationFile = (FPNtQueryInformationFile)GetProcAddress(m_hDll, "NtQueryInformationFile");

        m_fpRtlInitUnicodeString = (FPRtlInitUnicodeString)GetProcAddress(m_hDll, "RtlInitUnicodeString");
        m_fpRtlTimeToTimeFields = (FPRtlTimeToTimeFields)GetProcAddress(m_hDll, "RtlTimeToTimeFields");

        bool bRetVal = true;

        if(m_fpNtCreateFile == NULL || m_fpNtOpenFile == NULL || m_fpNtReadFile == NULL || m_fpNtDeleteFile == NULL || m_fpNtClose == NULL)
            bRetVal = false;

        if(m_fpNtQueryDirectoryFile == NULL || m_fpNtQueryInformationFile == NULL)
            bRetVal = false;

        if(m_fpRtlInitUnicodeString == NULL || m_fpRtlTimeToTimeFields == NULL)
            bRetVal = false;

        return bRetVal;
    }

    void unloadDll()
    {
        m_fpRtlTimeToTimeFields = NULL;
        m_fpRtlInitUnicodeString = NULL;

        m_fpNtClose = NULL;
        m_fpNtDeleteFile = NULL;
        m_fpNtReadFile = NULL;
        m_fpNtOpenFile = NULL;
        m_fpNtCreateFile = NULL;

        if(m_hDll != NULL)
            FreeLibrary(m_hDll);
        m_hDll = NULL;
    }

    NTSTATUS NtCreateFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess,
        POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock,
        PLARGE_INTEGER AllocationSize, ULONG FileAttributes,
        ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions,
        PVOID EaBuffer, ULONG EaLength)
    {
        if(!m_fpNtCreateFile)
            return -1;
        return m_fpNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock,
            AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
    }

    NTSTATUS NtOpenFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess,
        POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock,
        ULONG ShareAccess, ULONG OpenOptions)
    {
        if(!m_fpNtOpenFile)
            return -1;
        return m_fpNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
    }

    NTSTATUS NtReadFile(HANDLE FileHandle, HANDLE Event,
        PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock,
        PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset, PULONG Key)
    {
        if(!m_fpNtReadFile)
            return -1;
        return m_fpNtReadFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
    }

    NTSTATUS NtDeleteFile(POBJECT_ATTRIBUTES ObjectAttributes)
    {
        if(!m_fpNtDeleteFile)
            return -1;
        return m_fpNtDeleteFile(ObjectAttributes);
    }

    NTSTATUS NtClose(HANDLE FileHandle)
    {
        if(!m_fpNtClose)
            return -1;
        return m_fpNtClose(FileHandle);
    }

    NTSTATUS NtQueryDirectoryFile(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine,
        PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation,
        ULONG FileInformationLength, FILE_INFORMATION_CLASS FileInformationClass, BOOLEAN ReturnSingleEntry,
        PUNICODE_STRING FileName, BOOLEAN RestartScan)
    {
        if(!m_fpNtQueryDirectoryFile)
            return -1;
        return m_fpNtQueryDirectoryFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation,
            FileInformationLength, FileInformationClass, ReturnSingleEntry, FileName, RestartScan);
    }

    NTSTATUS NtQueryInformationFile(HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock,
        PVOID FileInformation, ULONG Length, FILE_INFORMATION_CLASS FileInformationClass)
    {
        if(!m_fpNtQueryInformationFile)
            return -1;
        return m_fpNtQueryInformationFile(FileHandle, IoStatusBlock, FileInformation, Length, FileInformationClass);
    }

    void RtlInitUnicodeString(PUNICODE_STRING DestinationString, LPCWSTR SourceString)
    {
        if(!m_fpRtlInitUnicodeString)
            return;
        m_fpRtlInitUnicodeString(DestinationString, SourceString);
    }

    void RtlTimeToTimeFields(PLARGE_INTEGER Time, PTIME_FIELDS TimeFields)
    {
        if(!m_fpRtlTimeToTimeFields)
            return;
        m_fpRtlTimeToTimeFields(Time, TimeFields);
    }

private:
    HMODULE m_hDll;

    FPNtCreateFile m_fpNtCreateFile;
    FPNtOpenFile m_fpNtOpenFile;
    FPNtReadFile m_fpNtReadFile;
    FPNtDeleteFile m_fpNtDeleteFile;

    FPNtClose m_fpNtClose;

    FPNtQueryDirectoryFile m_fpNtQueryDirectoryFile;
    FPNtQueryInformationFile m_fpNtQueryInformationFile;

    FPRtlInitUnicodeString m_fpRtlInitUnicodeString;
    FPRtlTimeToTimeFields m_fpRtlTimeToTimeFields;
};
