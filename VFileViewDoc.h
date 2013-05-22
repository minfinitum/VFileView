// NtFileViewDoc.h : interface of the CNtFileViewDoc class
//


#pragma once


class CVFileViewDoc : public CDocument
{
protected: // create from serialization only
    CVFileViewDoc();
    DECLARE_DYNCREATE(CVFileViewDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

// Implementation
public:
    virtual ~CVFileViewDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
};


