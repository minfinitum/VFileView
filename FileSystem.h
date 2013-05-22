#pragma once

#include <windows.h>
#include <string>
#include <vector>

#include "NtDllLib.h"

class ItemInfo
{
public:
    ItemInfo()
    {
        ZeroMemory(&CreationTime, sizeof(CreationTime));
        ZeroMemory(&LastAccessTime, sizeof(LastAccessTime));
        ZeroMemory(&LastWriteTime, sizeof(LastWriteTime));
        ZeroMemory(&ChangeTime, sizeof(ChangeTime));
        Attributes = 0;

        bCovert = false;
    }

    std::wstring sName;
    std::wstring sPath;

    SYSTEMTIME CreationTime;
    SYSTEMTIME LastAccessTime;
    SYSTEMTIME LastWriteTime;
    SYSTEMTIME ChangeTime;
    ULONG  Attributes;

    bool bCovert;
};


class FileInfo
    : public ItemInfo
{
public:
    FileInfo()
        :ItemInfo()
    {
        Size.QuadPart = 0;
        AllocationSize.QuadPart = 0;
    }

    LARGE_INTEGER Size;
    LARGE_INTEGER AllocationSize;
};

#define VEC_FILES	std::vector<FileInfo>

class PathInfo : public FileInfo
{
public:
    PathInfo()
    {

    }

    VEC_FILES streams;
};

#define		VEC_PATH		std::vector<PathInfo>


struct DriveInfo
{
    std::wstring sPath;
};

#define		VEC_DRIVE		std::vector<DriveInfo>


#define		VEC_FILEDATA	std::vector<unsigned char>

class CFileSystem
{
public:
    enum FS_TYPE
    {
        FS_FILE = 1,
        FS_DIRECTORY = 2
    };

public:
    CFileSystem(void);
    virtual ~CFileSystem(void);

    bool getLogicalDrives(VEC_DRIVE &drives);
    bool getDrives(VEC_DRIVE &drives);

    bool queryDirectoryInfo(const std::wstring &sPath, PathInfo &directory);
    bool queryFileInfo(const std::wstring &sPath, PathInfo &file);

    // expensive use fileFirst, findNext
    bool getDirectories(const std::wstring &sPath, VEC_PATH &directories);
    bool getFiles(const std::wstring &sPath, VEC_PATH &files);
    bool getFilesAndDirectories(const std::wstring &sPath, VEC_PATH &paths);

    bool findFirst(const std::wstring &sPath, PathInfo &info);
    bool findNext(PathInfo &info);


    bool getDirectoryStreams(const std::wstring &sPath, VEC_FILES &streams);
    bool getFileStreams(const std::wstring &sPath, VEC_FILES &streams);

    bool hasFiles(const std::wstring &sPath);
    bool hasDirectories(const std::wstring &sPath);

    bool readFile(const std::wstring &sPath, DWORD nSize, VEC_FILEDATA &data, DWORD &nReadBytes);
    bool deleteFile(const std::wstring &sPath);
    bool deleteFileOnReboot(const std::wstring &sPath);


    static std::wstring attributesToString(int nAttr);
    static std::wstring formatBytes(__int64 nSize);
    static void rtlTimeToSystemTime(const LARGE_INTEGER &rtlt, SYSTEMTIME &st);
    static void timeFieldsToSystemTime(const TIME_FIELDS &tf, SYSTEMTIME &st);

protected:
    bool queryFiles(const std::wstring &sPath, bool bFindFirstItem, VEC_PATH &files);
    bool queryDirectories(const std::wstring &sPath, bool bFindFirstItem, VEC_PATH &paths);
    bool queryFilesAndDirectories(const std::wstring &sPath, bool bFindFirstItem, VEC_PATH &files);

protected:
    bool queryDirectoryFile(const std::wstring &sPath, int fsType, bool bFindFirstItem, VEC_PATH &paths);

private:
    static NtDllLib m_ntdlllib;

    HANDLE m_hEnumPath;
    std::wstring m_sEnumPath;
};
