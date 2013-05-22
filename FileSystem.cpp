#include "stdafx.h"
#include "NtDllLib.h"

#include "StdAfx.h"
#include "filesystem.h"

#include <iomanip>
#include <sstream>

NtDllLib CFileSystem::m_ntdlllib;

CFileSystem::CFileSystem(void)
{
    m_hEnumPath = INVALID_HANDLE_VALUE;
    m_sEnumPath = L"";

    if(!m_ntdlllib.isLoaded())
    {
        const int nMaxPath = MAX_PATH * 2;
        wchar_t szSystemPath[nMaxPath] = L"";
        GetSystemDirectoryW(szSystemPath, nMaxPath);

        m_ntdlllib.loadDll(szSystemPath);
    }
}

CFileSystem::~CFileSystem(void)
{
    if(INVALID_HANDLE_VALUE != m_hEnumPath)
        m_ntdlllib.NtClose(m_hEnumPath);
    m_hEnumPath = INVALID_HANDLE_VALUE;
}

bool CFileSystem::getLogicalDrives(VEC_DRIVE &drives)
{
    DWORD nSize = GetLogicalDriveStrings(0, 0);
    std::vector<wchar_t> vDrives(nSize, 0);

    nSize = GetLogicalDriveStrings(nSize, &vDrives[0]);

    size_t nIter = 0;
    DriveInfo info;

    do
    {
        info.sPath = &vDrives[nIter];
        drives.push_back(info);

        nIter += info.sPath.length() + 1;

    } while(nIter < nSize);

    return (drives.size() ? true : false);
}

bool CFileSystem::getDrives(VEC_DRIVE &drives)
{
    HANDLE hDrive = INVALID_HANDLE_VALUE;
    OBJECT_ATTRIBUTES ObjectAttributes;

    wchar_t drive[8] = L"\\??\\a:\\";
    UNICODE_STRING usDrivePath;
    IO_STATUS_BLOCK ioStatusBlock;

    for(wchar_t nIter = L'a'; nIter <= 'z'; nIter++)
    {
        drive[4] = nIter;

        RtlZeroMemory(&usDrivePath, sizeof(usDrivePath));
        m_ntdlllib.RtlInitUnicodeString(&usDrivePath, drive);

        InitializeObjectAttributes(&ObjectAttributes, &usDrivePath, OBJ_CASE_INSENSITIVE, NULL, NULL);

        NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&hDrive, FILE_READ_DATA, &ObjectAttributes, &ioStatusBlock, FILE_SHARE_READ, FILE_DIRECTORY_FILE);
        if(NT_SUCCESS(nStatus))
        {
            DriveInfo info;
            info.sPath = drive;
            info.sPath.erase(0, 4);

            drives.push_back(info);

            m_ntdlllib.NtClose(hDrive);
        }
    }

    return true;
}

bool CFileSystem::queryDirectoryInfo(const std::wstring &sPath, PathInfo &directory)
{
    std::wstring sFullPath = L"\\??\\" + sPath;
    VEC_PATH paths;

    UNICODE_STRING usFolderPath;
    RtlZeroMemory(&usFolderPath, sizeof(usFolderPath));
    m_ntdlllib.RtlInitUnicodeString(&usFolderPath, sFullPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFolderPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    bool bRetVal = false;

    HANDLE hFile = INVALID_HANDLE_VALUE;
    NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&hFile, FILE_READ_ATTRIBUTES, &ObjectAttributes, &ioStatusBlock, FILE_SHARE_READ, FILE_DIRECTORY_FILE);
    if(NT_SUCCESS(nStatus))
    {
        FILE_BASIC_INFORMATION fbi;
        ZeroMemory(&fbi, sizeof(fbi));

        // We pass NO NAME which is the same as *.*
        nStatus = m_ntdlllib.NtQueryInformationFile(hFile, &ioStatusBlock, &fbi, sizeof(fbi), (FILE_INFORMATION_CLASS) FileBasicInformation);

        if(NT_SUCCESS(nStatus))
        {
            bRetVal = true;

            // Parse information
            directory.sPath = sPath;

            TIME_FIELDS tf;
            ZeroMemory(&tf, sizeof(tf));

            m_ntdlllib.RtlTimeToTimeFields(&fbi.CreationTime, &tf);
            timeFieldsToSystemTime(tf, directory.CreationTime);

            m_ntdlllib.RtlTimeToTimeFields(&fbi.LastAccessTime, &tf);
            timeFieldsToSystemTime(tf, directory.LastAccessTime);

            m_ntdlllib.RtlTimeToTimeFields(&fbi.LastWriteTime, &tf);
            timeFieldsToSystemTime(tf, directory.LastWriteTime);

            m_ntdlllib.RtlTimeToTimeFields(&fbi.ChangeTime, &tf);
            timeFieldsToSystemTime(tf, directory.ChangeTime);

            directory.Attributes = fbi.FileAttributes;

            // if we found it via ntdll, and not by getfileattributes
            WIN32_FILE_ATTRIBUTE_DATA wfad;
            ZeroMemory(&wfad, sizeof(wfad));
            directory.bCovert = (GetFileAttributesEx(directory.sPath.c_str(),
                GetFileExInfoStandard, &wfad) == 0 && GetLastError() != 32) ? true : false;

            // attempt to find streams
            getDirectoryStreams(directory.sPath, directory.streams);

        }

        m_ntdlllib.NtClose(hFile);
    }

    return bRetVal;
}

bool CFileSystem::queryFileInfo(const std::wstring &sPath, PathInfo &file)
{
    std::wstring sFullPath = L"\\??\\" + sPath;
    VEC_PATH paths;

    UNICODE_STRING usFolderPath;
    RtlZeroMemory(&usFolderPath, sizeof(usFolderPath));
    m_ntdlllib.RtlInitUnicodeString(&usFolderPath, sFullPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFolderPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    bool bRetVal = false;

    HANDLE hFile = INVALID_HANDLE_VALUE;
    NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&hFile, FILE_READ_ATTRIBUTES, &ObjectAttributes, &ioStatusBlock, FILE_SHARE_READ, FILE_DIRECTORY_FILE);
    if(NT_SUCCESS(nStatus))
    {
        FILE_BASIC_INFORMATION fbi;
        ZeroMemory(&fbi, sizeof(fbi));

        // We pass NO NAME which is the same as *.*
        nStatus = m_ntdlllib.NtQueryInformationFile(hFile, &ioStatusBlock, &fbi, sizeof(fbi), (FILE_INFORMATION_CLASS) FileBasicInformation);

        if(NT_SUCCESS(nStatus))
        {
            bRetVal = true;

            // Parse information
            file.sName = sPath;

            TIME_FIELDS tf;
            ZeroMemory(&tf, sizeof(tf));

            m_ntdlllib.RtlTimeToTimeFields(&fbi.CreationTime, &tf);
            timeFieldsToSystemTime(tf, file.CreationTime);

            m_ntdlllib.RtlTimeToTimeFields(&fbi.LastAccessTime, &tf);
            timeFieldsToSystemTime(tf, file.LastAccessTime);

            m_ntdlllib.RtlTimeToTimeFields(&fbi.LastWriteTime, &tf);
            timeFieldsToSystemTime(tf, file.LastWriteTime);

            m_ntdlllib.RtlTimeToTimeFields(&fbi.ChangeTime, &tf);
            timeFieldsToSystemTime(tf, file.ChangeTime);

            file.Attributes = fbi.FileAttributes;

            WIN32_FILE_ATTRIBUTE_DATA wfad;
            ZeroMemory(&wfad, sizeof(wfad));
            file.bCovert = (GetFileAttributesEx(file.sName.c_str(), GetFileExInfoStandard, &wfad) == 0 && GetLastError() != 32) ? true : false;

            // attempt to find streams
            getFileStreams(file.sPath, file.streams);

        }

        m_ntdlllib.NtClose(hFile);
    }

    return bRetVal;
}

bool CFileSystem::queryFiles(const std::wstring &sPath, bool bFindFirstItem, VEC_PATH &files)
{
    std::wstring sFullPath = L"\\??\\" + sPath;

    UNICODE_STRING usFolderPath;
    RtlZeroMemory(&usFolderPath, sizeof(usFolderPath));
    m_ntdlllib.RtlInitUnicodeString(&usFolderPath, sFullPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFolderPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    HANDLE hDir = INVALID_HANDLE_VALUE;
    NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&hDir, FILE_READ_DATA, &ObjectAttributes, &ioStatusBlock, FILE_SHARE_READ, FILE_DIRECTORY_FILE);
    if(NT_SUCCESS(nStatus))
    {
        ULONG nAllocSize = 4096 * 2;
        std::vector<char> vecData(nAllocSize, 0);

        // Wait for io to complete
        HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        // We pass NO NAME which is the same as *.*
        nStatus = m_ntdlllib.NtQueryDirectoryFile(hDir, hEvent, 0, 0, &ioStatusBlock,
            &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileDirectoryInformation, TRUE, NULL, TRUE);

        if(WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0) {
            CloseHandle(hEvent);
            m_ntdlllib.NtClose(hDir);
            return false;
        }

        if(NT_SUCCESS(nStatus))
        {
            PFILE_DIRECTORY_INFORMATION DirInformation = (PFILE_DIRECTORY_INFORMATION)&vecData[0];

            // Loop over all files
            for (;;)
            {
                // Parse information
                if(!(DirInformation->FileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    PathInfo file;
                    file.sName = std::wstring(DirInformation->FileName, DirInformation->FileNameLength / sizeof(wchar_t));

                    // Must check complete string (hidden unix folders can start with ".")
                    if(!file.sName.empty() && (file.sName != L".") && (file.sName != L".."))
                    {
                        file.bCovert = false;

                        file.sPath = sFullPath.substr(4);
                        if(file.sPath[file.sPath.length() - 1] != L'\\')
                            file.sPath += L'\\';
                        file.sPath += file.sName;

                        TIME_FIELDS tf;
                        ZeroMemory(&tf, sizeof(tf));

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->CreationTime, &tf);
                        timeFieldsToSystemTime(tf, file.CreationTime);

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastAccessTime, &tf);
                        timeFieldsToSystemTime(tf, file.LastAccessTime);

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastWriteTime, &tf);
                        timeFieldsToSystemTime(tf, file.LastWriteTime);

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->ChangeTime, &tf);
                        timeFieldsToSystemTime(tf, file.ChangeTime);

                        file.Size = DirInformation->EndOfFile;
                        file.AllocationSize = DirInformation->AllocationSize;

                        file.Attributes = DirInformation->FileAttributes;

                        WIN32_FILE_ATTRIBUTE_DATA wfad;
                        ZeroMemory(&wfad, sizeof(wfad));
                        file.bCovert = (GetFileAttributesEx(file.sPath.c_str(), GetFileExInfoStandard, &wfad) == 0 && GetLastError() != 32) ? true : false;\

                        // attempt to find streams
                        getFileStreams(file.sPath, file.streams);

                        files.push_back(file);
                    }

                    if(bFindFirstItem)
                        break;
                }

                // If there is no offset in the entry, the buffer has been exhausted.
                if (DirInformation->NextEntryOffset == 0)
                {
                    ZeroMemory(&vecData[0], nAllocSize);
                    ResetEvent(hEvent);

                    nStatus = m_ntdlllib.NtQueryDirectoryFile(hDir, hEvent, 0, 0, &ioStatusBlock,
                        &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileDirectoryInformation, TRUE, NULL, FALSE);

                    if(WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0) {
                        CloseHandle(hEvent);
                        m_ntdlllib.NtClose(hDir);
                        return false;
                    }

                    if(!NT_SUCCESS(nStatus) || ioStatusBlock.Information == 0)
                        break;

                    DirInformation = (PFILE_DIRECTORY_INFORMATION)&vecData[0];
                    continue;
                }

                // Advance to the next entry.
                DirInformation = (PFILE_DIRECTORY_INFORMATION)(((PUCHAR)DirInformation) + DirInformation->NextEntryOffset);
            }
        }

        CloseHandle(hEvent);

        m_ntdlllib.NtClose(hDir);
    }

    return !files.empty();
}

bool CFileSystem::queryDirectories(const std::wstring &sPath, bool bFindFirstItem, VEC_PATH &paths)
{
    std::wstring sFullPath = L"\\??\\" + sPath;

    UNICODE_STRING usFolderPath;
    RtlZeroMemory(&usFolderPath, sizeof(usFolderPath));
    m_ntdlllib.RtlInitUnicodeString(&usFolderPath, sFullPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFolderPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    HANDLE hDir = INVALID_HANDLE_VALUE;
    NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&hDir, FILE_READ_DATA, &ObjectAttributes, &ioStatusBlock, FILE_SHARE_READ, FILE_DIRECTORY_FILE);
    if(NT_SUCCESS(nStatus))
    {
        ULONG nAllocSize = 4096 * 2;
        std::vector<char> vecData(nAllocSize, 0);

        // Wait for io to complete
        HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        // We pass NO NAME which is the same as *.*
        nStatus = m_ntdlllib.NtQueryDirectoryFile(hDir, hEvent, 0, 0, &ioStatusBlock,
            &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileDirectoryInformation, TRUE, NULL, TRUE);

        if(WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0) {
            CloseHandle(hEvent);
            m_ntdlllib.NtClose(hDir);
            return false;
        }

        if(NT_SUCCESS(nStatus))
        {
            PFILE_DIRECTORY_INFORMATION DirInformation = (PFILE_DIRECTORY_INFORMATION)&vecData[0];

            // Loop over all files
            for (;;)
            {
                // Parse information
                if(DirInformation->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    PathInfo file;
                    file.sName = std::wstring(DirInformation->FileName, DirInformation->FileNameLength / sizeof(wchar_t));

                    // Must check complete string (folders can start with ".")
                    if(!file.sName.empty() && (file.sName != L".") && (file.sName != L".."))
                    {
                        file.bCovert = false;

                        file.sPath = sFullPath.substr(4);
                        if(file.sPath[file.sPath.length() - 1] != L'\\')
                            file.sPath += L'\\';
                        file.sPath += file.sName;

                        TIME_FIELDS tf;
                        ZeroMemory(&tf, sizeof(tf));

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->CreationTime, &tf);
                        timeFieldsToSystemTime(tf, file.CreationTime);

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastAccessTime, &tf);
                        timeFieldsToSystemTime(tf, file.LastAccessTime);

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastWriteTime, &tf);
                        timeFieldsToSystemTime(tf, file.LastWriteTime);

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->ChangeTime, &tf);
                        timeFieldsToSystemTime(tf, file.ChangeTime);

                        file.Size = DirInformation->EndOfFile;
                        file.AllocationSize = DirInformation->AllocationSize;

                        file.Attributes = DirInformation->FileAttributes;

                        WIN32_FILE_ATTRIBUTE_DATA wfad;
                        ZeroMemory(&wfad, sizeof(wfad));
                        file.bCovert = (GetFileAttributesEx(file.sPath.c_str(), GetFileExInfoStandard, &wfad) == 0 && GetLastError() != 32) ? true : false;\

                        // attempt to find streams
                        getDirectoryStreams(file.sPath, file.streams);

                        paths.push_back(file);

                        if(bFindFirstItem)
                            break;
                    }
                }

                // If there is no offset in the entry, the buffer has been exhausted.
                if (DirInformation->NextEntryOffset == 0)
                {
                    ZeroMemory(&vecData[0], nAllocSize);
                    ResetEvent(hEvent);

                    nStatus = m_ntdlllib.NtQueryDirectoryFile(hDir, hEvent, 0, 0, &ioStatusBlock,
                        &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileDirectoryInformation, TRUE, NULL, FALSE);

                    if(WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0) {
                        CloseHandle(hEvent);
                        m_ntdlllib.NtClose(hDir);
                        return false;
                    }

                    if(!NT_SUCCESS(nStatus) || ioStatusBlock.Information == 0)
                        break;

                    DirInformation = (PFILE_DIRECTORY_INFORMATION)&vecData[0];
                    continue;
                }

                // Advance to the next entry.
                DirInformation = (PFILE_DIRECTORY_INFORMATION)(((PUCHAR)DirInformation) + DirInformation->NextEntryOffset);
            }
        }

        m_ntdlllib.NtClose(hDir);
    }

    return !paths.empty();
}

bool CFileSystem::queryFilesAndDirectories(const std::wstring &sPath, bool bFindFirstItem, VEC_PATH &files)
{
    std::wstring sFullPath = L"\\??\\" + sPath;

    UNICODE_STRING usFolderPath;
    RtlZeroMemory(&usFolderPath, sizeof(usFolderPath));
    m_ntdlllib.RtlInitUnicodeString(&usFolderPath, sFullPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFolderPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    HANDLE hDir = INVALID_HANDLE_VALUE;
    NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&hDir, FILE_READ_DATA, &ObjectAttributes, &ioStatusBlock, FILE_SHARE_READ, FILE_DIRECTORY_FILE);
    if(NT_SUCCESS(nStatus))
    {
        ULONG nAllocSize = 4096 * 2;
        std::vector<char> vecData(nAllocSize, 0);

        // Wait for io to complete
        HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        // We pass NO NAME which is the same as *.*
        nStatus = m_ntdlllib.NtQueryDirectoryFile(hDir, hEvent, 0, 0, &ioStatusBlock,
            &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileDirectoryInformation, TRUE, NULL, TRUE);

        if(WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0) {
            CloseHandle(hEvent);
            m_ntdlllib.NtClose(hDir);
            return false;
        }

        if(NT_SUCCESS(nStatus))
        {
            PFILE_DIRECTORY_INFORMATION DirInformation = (PFILE_DIRECTORY_INFORMATION)&vecData[0];

            // Loop over all files
            for (;;)
            {
                // Parse information
                PathInfo file;
                file.sName = std::wstring(DirInformation->FileName, DirInformation->FileNameLength / sizeof(wchar_t));

                // Must check complete string (hidden unix folders can start with ".")
                if(!file.sName.empty() && (file.sName != L".") && (file.sName != L".."))
                {
                    file.bCovert = false;

                    file.sPath = sFullPath.substr(4);
                    if(file.sPath[file.sPath.length() - 1] != L'\\')
                        file.sPath += L'\\';
                    file.sPath += file.sName;

                    TIME_FIELDS tf;
                    ZeroMemory(&tf, sizeof(tf));

                    m_ntdlllib.RtlTimeToTimeFields(&DirInformation->CreationTime, &tf);
                    timeFieldsToSystemTime(tf, file.CreationTime);

                    m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastAccessTime, &tf);
                    timeFieldsToSystemTime(tf, file.LastAccessTime);

                    m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastWriteTime, &tf);
                    timeFieldsToSystemTime(tf, file.LastWriteTime);

                    m_ntdlllib.RtlTimeToTimeFields(&DirInformation->ChangeTime, &tf);
                    timeFieldsToSystemTime(tf, file.ChangeTime);

                    file.Size = DirInformation->EndOfFile;
                    file.AllocationSize = DirInformation->AllocationSize;

                    file.Attributes = DirInformation->FileAttributes;

                    WIN32_FILE_ATTRIBUTE_DATA wfad;
                    ZeroMemory(&wfad, sizeof(wfad));
                    file.bCovert = (GetFileAttributesEx(file.sPath.c_str(), GetFileExInfoStandard, &wfad) == 0 && GetLastError() != 32) ? true : false;\

                    if(file.Attributes & FILE_ATTRIBUTE_DIRECTORY)
                        getDirectoryStreams(file.sPath, file.streams);
                    else
                        getFileStreams(file.sPath, file.streams);

                    files.push_back(file);

                    if(bFindFirstItem)
                        break;
                }

                // If there is no offset in the entry, the buffer has been exhausted.
                if (DirInformation->NextEntryOffset == 0)
                {
                    ZeroMemory(&vecData[0], nAllocSize);

                    ResetEvent(hEvent);
                    nStatus = m_ntdlllib.NtQueryDirectoryFile(hDir, hEvent, 0, 0, &ioStatusBlock,
                        &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileDirectoryInformation, TRUE, NULL, FALSE);

                    if(WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0) {
                        CloseHandle(hEvent);
                        m_ntdlllib.NtClose(hDir);
                        return false;
                    }

                    if(!NT_SUCCESS(nStatus) || ioStatusBlock.Information == 0)
                        break;

                    DirInformation = (PFILE_DIRECTORY_INFORMATION)&vecData[0];
                    continue;
                }

                // Advance to the next entry.
                DirInformation = (PFILE_DIRECTORY_INFORMATION)(((PUCHAR)DirInformation) + DirInformation->NextEntryOffset);
            }
        }

        CloseHandle(hEvent);

        m_ntdlllib.NtClose(hDir);
    }

    return !files.empty();
}


bool CFileSystem::queryDirectoryFile(const std::wstring &sPath, int fsType, bool bFindFirstItem, VEC_PATH &paths)
{
    std::wstring sFullPath = L"\\??\\" + sPath;

    UNICODE_STRING usFolderPath;
    RtlZeroMemory(&usFolderPath, sizeof(usFolderPath));
    m_ntdlllib.RtlInitUnicodeString(&usFolderPath, sFullPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFolderPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    HANDLE hDir = INVALID_HANDLE_VALUE;
    NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&hDir, FILE_READ_DATA, &ObjectAttributes, &ioStatusBlock, FILE_SHARE_READ, FILE_DIRECTORY_FILE);
    if(NT_SUCCESS(nStatus))
    {
        ULONG nAllocSize = 4096 * 2;
        std::vector<char> vecData(nAllocSize, 0);

        // Wait for io to complete
        HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        // We pass NO NAME which is the same as *.*
        nStatus = m_ntdlllib.NtQueryDirectoryFile(hDir, hEvent, 0, 0, &ioStatusBlock,
            &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileDirectoryInformation, TRUE, NULL, TRUE);

        if(WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0) {
            CloseHandle(hEvent);
            m_ntdlllib.NtClose(hDir);
            return false;
        }

        if(NT_SUCCESS(nStatus))
        {
            PFILE_DIRECTORY_INFORMATION DirInformation = (PFILE_DIRECTORY_INFORMATION)&vecData[0];

            // Loop over all files
            for (;;)
            {
                // Parse information
                if((fsType & FS_DIRECTORY && DirInformation->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                    (fsType & FS_FILE && !(DirInformation->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
                {
                    PathInfo file;
                    file.sName = std::wstring(DirInformation->FileName, DirInformation->FileNameLength / sizeof(wchar_t));

                    // Must check complete string (hidden unix folders can start with ".")
                    if(!file.sName.empty() && (file.sName != L".") && (file.sName != L".."))
                    {
                        file.bCovert = false;

                        file.sPath = sPath.substr(4);
                        if(file.sPath[file.sPath.length() - 1] != L'\\')
                            file.sPath += L'\\';
                        file.sPath += file.sName;

                        TIME_FIELDS tf;
                        ZeroMemory(&tf, sizeof(tf));

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->CreationTime, &tf);
                        timeFieldsToSystemTime(tf, file.CreationTime);

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastAccessTime, &tf);
                        timeFieldsToSystemTime(tf, file.LastAccessTime);

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastWriteTime, &tf);
                        timeFieldsToSystemTime(tf, file.LastWriteTime);

                        m_ntdlllib.RtlTimeToTimeFields(&DirInformation->ChangeTime, &tf);
                        timeFieldsToSystemTime(tf, file.ChangeTime);

                        file.Size = DirInformation->EndOfFile;
                        file.AllocationSize = DirInformation->AllocationSize;

                        file.Attributes = DirInformation->FileAttributes;

                        WIN32_FILE_ATTRIBUTE_DATA wfad;
                        ZeroMemory(&wfad, sizeof(wfad));
                        file.bCovert = (GetFileAttributesEx(file.sPath.c_str(), GetFileExInfoStandard, &wfad) == 0 && GetLastError() != 32) ? true : false;\

                        if(fsType & FS_FILE && !(DirInformation->FileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                            getFileStreams(file.sPath, file.streams);

                        paths.push_back(file);

                        if(bFindFirstItem)
                            break;
                    }
                }

                // If there is no offset in the entry, the buffer has been exhausted.
                if (DirInformation->NextEntryOffset == 0)
                {
                    ZeroMemory(&vecData[0], nAllocSize);
                    ResetEvent(hEvent);

                    nStatus = m_ntdlllib.NtQueryDirectoryFile(hDir, hEvent, 0, 0, &ioStatusBlock,
                        &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileDirectoryInformation, TRUE, NULL, FALSE);

                    if(WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0) {
                        CloseHandle(hEvent);
                        m_ntdlllib.NtClose(hDir);
                        return false;
                    }

                    if(!NT_SUCCESS(nStatus) || ioStatusBlock.Information == 0)
                        break;

                    DirInformation = (PFILE_DIRECTORY_INFORMATION)&vecData[0];
                    continue;
                }

                // Advance to the next entry.
                DirInformation = (PFILE_DIRECTORY_INFORMATION)(((PUCHAR)DirInformation) + DirInformation->NextEntryOffset);
            }
        }

        CloseHandle(hEvent);

        m_ntdlllib.NtClose(hDir);
    }

    return !paths.empty();
}

bool  CFileSystem::getDirectoryStreams(const std::wstring &sPath, VEC_FILES &streams)
{
    std::wstring sFullPath = L"\\??\\" + sPath;

    std::wstring::size_type nDelimitPos = sFullPath.length() - 1;
    if(sFullPath[nDelimitPos] == L'\\')
        sFullPath.erase(nDelimitPos);

    UNICODE_STRING usFolderPath;
    RtlZeroMemory(&usFolderPath, sizeof(usFolderPath));
    m_ntdlllib.RtlInitUnicodeString(&usFolderPath, sFullPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFolderPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    HANDLE hFile = INVALID_HANDLE_VALUE;
    NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&hFile, FILE_READ_DATA, &ObjectAttributes, &ioStatusBlock, FILE_SHARE_READ, FILE_DIRECTORY_FILE);
    if(NT_SUCCESS(nStatus))
    {
        ULONG nAllocSize = 4096 * 2;
        std::vector<char> vecData(nAllocSize, 0);

        // We pass NO NAME which is the same as *.*
        nStatus = m_ntdlllib.NtQueryInformationFile(hFile, &ioStatusBlock, &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileStreamInformation);

        if(NT_SUCCESS(nStatus))
        {
            FILE_STREAM_INFORMATION *fsi = (FILE_STREAM_INFORMATION *) &vecData[0];

            static const wchar_t * const DefaultStreamName = L"::$DATA";
            static const ULONG DefaultStreamNameLength = 7;

            for(;;)
            {
                std::wstring sStreamFile = std::wstring(fsi->StreamName, fsi->StreamNameLength / sizeof(wchar_t));
                if (!sStreamFile.empty() && sStreamFile != DefaultStreamName)
                {
                    std::wstring::size_type pos = sStreamFile.find(L":$DATA");
                    if(pos != std::wstring::npos)
                        sStreamFile.erase(pos);

                    FileInfo stream;
                    stream.sName = sStreamFile;
                    stream.sPath = sFullPath.substr(4) + sStreamFile;

                    stream.Size = fsi->StreamSize;
                    stream.AllocationSize = fsi->StreamAllocationSize;

                    streams.push_back(stream);
                }
                if ( fsi->NextEntryOffset == 0)
                    break;

                fsi = (FILE_STREAM_INFORMATION *) ( fsi->NextEntryOffset + (byte *) fsi );
            }
        }

        m_ntdlllib.NtClose(hFile);
    }

    return (!streams.empty());
}

bool CFileSystem::getFileStreams(const std::wstring &sPath, VEC_FILES &streams)
{
    std::wstring sFullPath = L"\\??\\" + sPath;

    UNICODE_STRING usFolderPath;
    RtlZeroMemory(&usFolderPath, sizeof(usFolderPath));
    m_ntdlllib.RtlInitUnicodeString(&usFolderPath, sFullPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFolderPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    HANDLE hFile = INVALID_HANDLE_VALUE;
    NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&hFile, FILE_READ_DATA, &ObjectAttributes, &ioStatusBlock, FILE_SHARE_READ, FILE_NON_DIRECTORY_FILE);
    if(NT_SUCCESS(nStatus))
    {
        ULONG nAllocSize = 4096 * 2;
        std::vector<char> vecData(nAllocSize, 0);

        // We pass NO NAME which is the same as *.*
        nStatus = m_ntdlllib.NtQueryInformationFile(hFile, &ioStatusBlock, &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileStreamInformation);

        if(NT_SUCCESS(nStatus))
        {
            FILE_STREAM_INFORMATION *fsi = (FILE_STREAM_INFORMATION *) &vecData[0];

            static const wchar_t * const DefaultStreamName = L"::$DATA";
            static const ULONG DefaultStreamNameLength = 7;

            for(;;)
            {
                std::wstring sStreamFile = std::wstring(fsi->StreamName, fsi->StreamNameLength / sizeof(wchar_t));
                if (!sStreamFile.empty() && sStreamFile != DefaultStreamName)
                {
                    std::wstring::size_type pos = sStreamFile.find(L":$DATA");
                    if(pos != std::wstring::npos)
                        sStreamFile.erase(pos);

                    FileInfo stream;
                    stream.sName = sStreamFile;
                    stream.sPath = sFullPath.substr(4) + sStreamFile;

                    stream.Size = fsi->StreamSize;
                    stream.AllocationSize = fsi->StreamAllocationSize;

                    streams.push_back(stream);
                }
                if ( fsi->NextEntryOffset == 0)
                    break;

                fsi = (FILE_STREAM_INFORMATION *) ( fsi->NextEntryOffset + (byte *) fsi );
            }
        }

        m_ntdlllib.NtClose(hFile);
    }

    return (!streams.empty());
}

bool CFileSystem::getDirectories(const std::wstring &sPath, VEC_PATH &directories)
{
    return queryDirectories(sPath, false, directories);
}

bool CFileSystem::getFiles(const std::wstring &sPath, VEC_PATH &files)
{
    return queryFiles(sPath, false, files);
}

bool CFileSystem::getFilesAndDirectories(const std::wstring &sPath, VEC_PATH &paths)
{
    return queryFilesAndDirectories(sPath, false, paths);
}

bool CFileSystem::hasFiles(const std::wstring &sPath)
{
    VEC_PATH files;
    return queryFiles(sPath, true, files);
}

bool CFileSystem::hasDirectories(const std::wstring &sPath)
{
    VEC_PATH directories;
    return queryDirectories(sPath, true, directories);
}

bool CFileSystem::readFile(const std::wstring &sPath, DWORD nSize, VEC_FILEDATA &data, DWORD &nReadBytes)
{
    std::wstring sFullPath = L"\\??\\" + sPath;
    nReadBytes = 0;

    UNICODE_STRING usFolderPath;
    RtlZeroMemory(&usFolderPath, sizeof(usFolderPath));
    m_ntdlllib.RtlInitUnicodeString(&usFolderPath, sFullPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFolderPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    HANDLE hFile = INVALID_HANDLE_VALUE;
    NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&hFile, GENERIC_READ, &ObjectAttributes,
        &ioStatusBlock, FILE_SHARE_READ, FILE_SEQUENTIAL_ONLY | FILE_NON_DIRECTORY_FILE);
    if(NT_SUCCESS(nStatus))
    {
        // Wait for io to complete
        HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        IO_STATUS_BLOCK ioStatusBlock;
        ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

        LARGE_INTEGER   byteOffset;
        byteOffset.LowPart = 0;
        byteOffset.HighPart = 0;

        if(NT_SUCCESS(m_ntdlllib.NtReadFile(hFile, hEvent, 0, 0, &ioStatusBlock, &data[0], nSize, &byteOffset, 0)))
        {
            nReadBytes = 0;

            if(WaitForSingleObject(hEvent, 1000) == WAIT_OBJECT_0)
                nReadBytes = (DWORD)ioStatusBlock.Information;
        }

        CloseHandle(hEvent);
        m_ntdlllib.NtClose(hFile);
    }

    return ((nReadBytes != 0) ? true : false);
}

bool CFileSystem::deleteFile(const std::wstring &sPath)
{
    std::wstring sFullPath = L"\\??\\" + sPath;

    UNICODE_STRING usFilePath;
    RtlZeroMemory(&usFilePath, sizeof(usFilePath));
    m_ntdlllib.RtlInitUnicodeString(&usFilePath, sFullPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFilePath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    bool bRetVal = NT_SUCCESS(m_ntdlllib.NtDeleteFile(&ObjectAttributes)) ? true : false;

    return bRetVal;
}

bool CFileSystem::deleteFileOnReboot(const std::wstring &sPath)
{
    return (MoveFileEx(sPath.c_str(), NULL,
        MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING)) ? true : false;
}

std::wstring CFileSystem::attributesToString(int nAttr)
{
    std::wstring sValue;

    if(nAttr & FILE_ATTRIBUTE_READONLY)
        sValue += L"r";
    if(nAttr & FILE_ATTRIBUTE_HIDDEN)
        sValue += L"h";
    if(nAttr & FILE_ATTRIBUTE_SYSTEM)
        sValue += L"s";
    if(nAttr & FILE_ATTRIBUTE_DIRECTORY)
        sValue += L"d";
    if(nAttr & FILE_ATTRIBUTE_ARCHIVE)
        sValue += L"a";
    if(nAttr & FILE_ATTRIBUTE_NORMAL)
        sValue += L"n";
    if(nAttr & FILE_ATTRIBUTE_TEMPORARY)
        sValue += L"t";
    if(nAttr & FILE_ATTRIBUTE_COMPRESSED)
        sValue += L"c";

    return sValue;
}

std::wstring CFileSystem::formatBytes(__int64 nSize)
{
    const __int64 nKilo = 1024;
    const __int64 nMega = nKilo * 1024;
    const __int64 nGiga = nMega * 1024;

    std::wostringstream ss;

    if(nSize > nGiga)
    {
        ss << std::setiosflags(std::ios::fixed) << std::setprecision(3);

        ss << ((double) nSize / (double)nGiga) << L" GB";
    }
    else if(nSize > nMega)
    {
        ss << std::setiosflags(std::ios::fixed) << std::setprecision(2);

        ss << ((double) nSize / (double)nMega) << L" MB";
    }
    else if(nSize > nKilo)
    {
        ss << std::setiosflags(std::ios::fixed) << std::setprecision(1);

        ss << ((double) nSize / (double)nKilo) << L" KB";
    }
    else
    {
        ss << std::setiosflags(std::ios::fixed) << std::setprecision(1);

        ss << nSize << L" Bytes";
    }

    return ss.str();
}

void CFileSystem::timeFieldsToSystemTime(const TIME_FIELDS &tf, SYSTEMTIME &st)
{
    st.wYear = tf.Year;
    st.wMonth = tf.Month;
    st.wDayOfWeek = tf.Weekday;
    st.wDay = tf.Day;
    st.wHour = tf.Hour;
    st.wMinute = tf.Minute;
    st.wSecond = tf.Second;
    st.wMilliseconds = tf.Milliseconds;
}

bool CFileSystem::findFirst(const std::wstring &sPath, PathInfo &info)
{
    m_sEnumPath = L"\\??\\" + sPath;

    if(INVALID_HANDLE_VALUE != m_hEnumPath){
        m_ntdlllib.NtClose(m_hEnumPath);
        m_hEnumPath = INVALID_HANDLE_VALUE;
    }

    UNICODE_STRING usFolderPath;
    RtlZeroMemory(&usFolderPath, sizeof(usFolderPath));
    m_ntdlllib.RtlInitUnicodeString(&usFolderPath, m_sEnumPath.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usFolderPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    NTSTATUS nStatus = m_ntdlllib.NtOpenFile(&m_hEnumPath, FILE_READ_DATA, &ObjectAttributes, &ioStatusBlock, FILE_SHARE_READ, FILE_DIRECTORY_FILE);
    if(NT_SUCCESS(nStatus))
    {
        // Wait for io to complete
        HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        ULONG nAllocSize = 4096 * 2;
        std::vector<char> vecData(nAllocSize, 0);

        // We pass NO NAME which is the same as *.*
        nStatus = m_ntdlllib.NtQueryDirectoryFile(m_hEnumPath, hEvent, 0, 0, &ioStatusBlock,
            &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileDirectoryInformation, TRUE, NULL, TRUE);

        if(WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0) {
            CloseHandle(hEvent);
            m_ntdlllib.NtClose(m_hEnumPath);

            m_hEnumPath = INVALID_HANDLE_VALUE;
            return false;
        }

        if(NT_SUCCESS(nStatus))
        {
            PFILE_DIRECTORY_INFORMATION DirInformation = (PFILE_DIRECTORY_INFORMATION)&vecData[0];

            // Parse information
            info.sName = std::wstring(DirInformation->FileName, DirInformation->FileNameLength / sizeof(wchar_t));

            // Must check complete string (folders can start with ".")
            if(info.sName.empty() || (info.sName == L".") || (info.sName == L"..")) {
                CloseHandle(hEvent);

                return findNext(info);
            }

            info.bCovert = false;

            info.sPath = m_sEnumPath.substr(4);
            if(info.sPath[info.sPath.length() - 1] != L'\\')
                info.sPath += L'\\';
            info.sPath += info.sName;

            TIME_FIELDS tf;
            ZeroMemory(&tf, sizeof(tf));

            m_ntdlllib.RtlTimeToTimeFields(&DirInformation->CreationTime, &tf);
            timeFieldsToSystemTime(tf, info.CreationTime);

            m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastAccessTime, &tf);
            timeFieldsToSystemTime(tf, info.LastAccessTime);

            m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastWriteTime, &tf);
            timeFieldsToSystemTime(tf, info.LastWriteTime);

            m_ntdlllib.RtlTimeToTimeFields(&DirInformation->ChangeTime, &tf);
            timeFieldsToSystemTime(tf, info.ChangeTime);

            info.Size = DirInformation->EndOfFile;
            info.AllocationSize = DirInformation->AllocationSize;

            info.Attributes = DirInformation->FileAttributes;

            WIN32_FILE_ATTRIBUTE_DATA wfad;
            ZeroMemory(&wfad, sizeof(wfad));
            info.bCovert = (GetFileAttributesEx(info.sPath.c_str(), GetFileExInfoStandard, &wfad) == 0 && GetLastError() != 32) ? true : false;\

            // attempt to find streams
            getDirectoryStreams(info.sPath, info.streams);
        }

        CloseHandle(hEvent);
    }

    return NT_SUCCESS(nStatus);
}

bool CFileSystem::findNext(PathInfo &info)
{
    if(INVALID_HANDLE_VALUE == m_hEnumPath){
        return false;
    }

    // Wait for io to complete
    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    ULONG nAllocSize = 4096 * 2;
    std::vector<char> vecData(nAllocSize, 0);

    // If there is no offset in the entry, the buffer has been exhausted.
    ZeroMemory(&vecData[0], nAllocSize);

    IO_STATUS_BLOCK ioStatusBlock;
    ZeroMemory(&ioStatusBlock, sizeof(ioStatusBlock));

    NTSTATUS nStatus = m_ntdlllib.NtQueryDirectoryFile(m_hEnumPath, hEvent, 0, 0, &ioStatusBlock,
        &vecData[0], nAllocSize, (FILE_INFORMATION_CLASS) FileDirectoryInformation, TRUE, NULL, FALSE);

    if(WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0 ||
        !NT_SUCCESS(nStatus) || ioStatusBlock.Information == 0) {

        // end enumeration if failure
        CloseHandle(hEvent);
        m_ntdlllib.NtClose(m_hEnumPath);

        m_hEnumPath = INVALID_HANDLE_VALUE;
        return false;
    }

    PFILE_DIRECTORY_INFORMATION DirInformation = (PFILE_DIRECTORY_INFORMATION)&vecData[0];

    // Parse information
    info.sName = std::wstring(DirInformation->FileName, DirInformation->FileNameLength / sizeof(wchar_t));

    // Must check complete string (folders can start with ".")
    if(info.sName.empty() || (info.sName == L".") || (info.sName == L".."))
    {
        CloseHandle(hEvent);

        return findNext(info);
    }

    info.bCovert = false;

    info.sPath = m_sEnumPath.substr(4);
    if(info.sPath[info.sPath.length() - 1] != L'\\')
        info.sPath += L'\\';
    info.sPath += info.sName;

    TIME_FIELDS tf;
    ZeroMemory(&tf, sizeof(tf));

    m_ntdlllib.RtlTimeToTimeFields(&DirInformation->CreationTime, &tf);
    timeFieldsToSystemTime(tf, info.CreationTime);

    m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastAccessTime, &tf);
    timeFieldsToSystemTime(tf, info.LastAccessTime);

    m_ntdlllib.RtlTimeToTimeFields(&DirInformation->LastWriteTime, &tf);
    timeFieldsToSystemTime(tf, info.LastWriteTime);

    m_ntdlllib.RtlTimeToTimeFields(&DirInformation->ChangeTime, &tf);
    timeFieldsToSystemTime(tf, info.ChangeTime);

    info.Size = DirInformation->EndOfFile;
    info.AllocationSize = DirInformation->AllocationSize;

    info.Attributes = DirInformation->FileAttributes;

    WIN32_FILE_ATTRIBUTE_DATA wfad;
    ZeroMemory(&wfad, sizeof(wfad));
    info.bCovert = (GetFileAttributesEx(info.sPath.c_str(), GetFileExInfoStandard, &wfad) == 0 && GetLastError() != 32) ? true : false;

    // attempt to find streams
    if(info.Attributes & FILE_ATTRIBUTE_DIRECTORY) {
        getDirectoryStreams(info.sPath, info.streams);
    } else {
        getFileStreams(info.sPath, info.streams);
    }

    CloseHandle(hEvent);

    return true;
}
