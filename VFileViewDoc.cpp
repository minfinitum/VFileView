// NtFileViewDoc.cpp : implementation of the CNtFileViewDoc class
//

#include "stdafx.h"
#include "VFileView.h"
#include "VFileViewdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNtFileViewDoc

IMPLEMENT_DYNCREATE(CVFileViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CVFileViewDoc, CDocument)
END_MESSAGE_MAP()


// CNtFileViewDoc construction/destruction

CVFileViewDoc::CVFileViewDoc()
{
    // TODO: add one-time construction code here

}

CVFileViewDoc::~CVFileViewDoc()
{
}

BOOL CVFileViewDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    return TRUE;
}

// CNtFileViewDoc serialization
void CVFileViewDoc::Serialize(CArchive& ar)
{
}

// CNtFileViewDoc diagnostics
#ifdef _DEBUG
void CVFileViewDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CVFileViewDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CNtFileViewDoc commands
