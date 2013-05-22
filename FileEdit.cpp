// FileEdit.cpp : implementation file
//

#include "stdafx.h"
#include "VFileView.h"
#include "fileedit.h"
#include "filesystem.h"
#include "utils.h"

#include <sstream>
#include <iomanip>


// CFileEdit

IMPLEMENT_DYNAMIC(CFileEdit, CEdit)

CFileEdit::CFileEdit()
{
}

CFileEdit::~CFileEdit()
{
}

BEGIN_MESSAGE_MAP(CFileEdit, CEdit)
END_MESSAGE_MAP()

void CFileEdit::initialise()
{
    m_font.CreatePointFont(10, L"Courier");

    SetFont(&m_font, FALSE);
}

// CFileEdit message handlers
void CFileEdit::UpdatePathInfo(PathInfo &path, VEC_PATH &children)
{
    ClearFileInfo();

    // Display directory information
    std::wstringstream str;
    str.str(L"");

    str << L"Directory: " << path.sName.c_str() << L"\r\n";
    str << L"Created : " << Utils::systemTimeToLocalTimeString(path.CreationTime) << L"\r\n";
    str << L"Last Access: " << Utils::systemTimeToLocalTimeString(path.LastAccessTime) << L"\r\n";
    str << L"Last Write: " << Utils::systemTimeToLocalTimeString(path.LastWriteTime) << L"\r\n";
    str << L"Attributes: " << CFileSystem::attributesToString(path.Attributes) << L"\r\n";
    str << L"\r\n";

    __int64 directories = 0;
    __int64 directoryStreams = 0;
    __int64 files = 0;
    __int64 fileStreams = 0;

    for(auto child = children.begin(); child != children.end(); ++child) {

        if(child->Attributes & FILE_ATTRIBUTE_DIRECTORY) {
            directories ++;
        } else {
            files ++;
            fileStreams += child->streams.size();
        }
    }

    str << L"Directories: " << directories << L"\r\n";
    str << L"Directory Streams: " << path.streams.size() << L"\r\n";

    str << L"Files: " << files << L"\r\n";
    str << L"File Streams: " << fileStreams << L"\r\n";

    SetWindowText(str.str().c_str());
}

void CFileEdit::UpdatePathInfo(const std::wstring &sPath)
{
    ClearFileInfo();

    CFileSystem fs;
    PathInfo path;

    if(fs.queryDirectoryInfo(sPath, path))
    {
        // Display directory information
        std::wstringstream str;
        str.str(L"");

        str << L"Directory: " << sPath << L"\r\n";
        str << L"Created : " << Utils::systemTimeToLocalTimeString(path.CreationTime) << L"\r\n";
        str << L"Last Access: " << Utils::systemTimeToLocalTimeString(path.LastAccessTime) << L"\r\n";
        str << L"Last Write: " << Utils::systemTimeToLocalTimeString(path.LastWriteTime) << L"\r\n";
        str << L"Attributes: " << CFileSystem::attributesToString(path.Attributes) << L"\r\n";

        str << L"\r\n";
        bool more = true;
        bool first = true;
        __int64 numofdir = 0;
        __int64 numoffile = 0;
        __int64 numoffilestreams = 0;

        do {
            PathInfo info;

            if(first) {
                more = fs.findFirst(sPath, info);
                first = false;
            } else {
                more = fs.findNext(info);
            }

            if(more) {
                if(info.Attributes & FILE_ATTRIBUTE_DIRECTORY) {
                    numofdir++;
                } else {
                    numoffile++;
                    numoffilestreams += info.streams.size();
                }
            }

        } while(more);

        str << L"Directories: " << numofdir << L"\r\n";
        str << L"Directory Streams: " << path.streams.size() << L"\r\n";
        str << L"Files: " << numoffile << L"\r\n";
        str << L"File Streams: " << numoffilestreams << L"\r\n";

        SetWindowText(str.str().c_str());
    }
}

void CFileEdit::UpdateFileInfo(const PathInfo &file)
{
    ClearFileInfo();

    const int nMaxLen = 128;
    VEC_FILEDATA vfd (nMaxLen, 0);
    DWORD nBytesRead = 0;
    CFileSystem fs;
    if(fs.readFile(file.sPath, nMaxLen, vfd, nBytesRead) && nBytesRead != 0)
        SetBinary(&vfd[0], nBytesRead);

}

void CFileEdit::ClearFileInfo()
{
    SetWindowText(L"");
}

void CFileEdit::SetBinary(unsigned char *pData, int nSize)
{
    const int nInterval = 16;

    std::wstringstream str;
    str.str(L"");

    wchar_t c = 0;
    int iterBin = 0;
    int textStartPos = 0;
    for(iterBin = 0; iterBin < nSize; iterBin++)
    {
        // add index
        if(iterBin % nInterval == 0)
            str << std::hex << std::setw(4) << std::setfill(L'0') << iterBin << L"  ";

        // add binary value
        str << std::hex << std::setw(2) << std::setfill(L'0') << pData[iterBin];

        // add text value - check if next value start of next interval
        if(iterBin != 0 && (iterBin + 1) % nInterval == 0)
        {
            str << L"  ";

            for(int iterText = textStartPos; iterText < iterBin + 1; iterText++)
            {
                c = pData[iterText];
                str << (wchar_t) (!(iswcntrl(c)) ? c : L'.');
            }

            // Index to start text on next interval
            textStartPos = iterBin + 1;

            str << L"\r\n";
        }
        else
        {
            str << L" ";
        }
    }

    if(iterBin % nInterval != 0)
    {
        // pad binary data
        int nNumBinaryValues = nInterval - (iterBin % nInterval);
        for(int iterPad = 0; iterPad < nNumBinaryValues; iterPad++)
            str << L"   ";

        str << L" ";

        // add text value
        for(int iterText = textStartPos; iterText < iterBin; iterText++)
        {
            c = pData[iterText];
            str << (wchar_t) (!(iswcntrl(c)) ? c : L'.');
        }

        str << L"\r\n";
    }

    if(str.str().length() > 0)
    {
        SetWindowText(str.str().c_str());
    }
}
