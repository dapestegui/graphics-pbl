
// Chess-replayView.cpp : implementation of the CChessreplayView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Chess-replay.h"
#endif

#include "Chess-replayDoc.h"
#include "Chess-replayView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChessreplayView

IMPLEMENT_DYNCREATE(CChessreplayView, CView)

BEGIN_MESSAGE_MAP(CChessreplayView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CChessreplayView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CChessreplayView construction/destruction

CChessreplayView::CChessreplayView()
{
	// TODO: add construction code here

}

CChessreplayView::~CChessreplayView()
{
}

BOOL CChessreplayView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CChessreplayView drawing

void CChessreplayView::OnDraw(CDC* /*pDC*/)
{
	CChessreplayDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CChessreplayView printing


void CChessreplayView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CChessreplayView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CChessreplayView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CChessreplayView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CChessreplayView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CChessreplayView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CChessreplayView diagnostics

#ifdef _DEBUG
void CChessreplayView::AssertValid() const
{
	CView::AssertValid();
}

void CChessreplayView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CChessreplayDoc* CChessreplayView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CChessreplayDoc)));
	return (CChessreplayDoc*)m_pDocument;
}
#endif //_DEBUG


// CChessreplayView message handlers
