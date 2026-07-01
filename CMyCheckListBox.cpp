// CMyCheckListBox.cpp
#include "pch.h"
#include "CMyCheckListBox.h"
#include <uxtheme.h>
#pragma comment(lib, "uxtheme.lib")

IMPLEMENT_DYNAMIC(CMyCheckListBox, CCheckListBox)

CMyCheckListBox::CMyCheckListBox() : m_nItemHeight(16) {}
CMyCheckListBox::~CMyCheckListBox() {}

void CMyCheckListBox::SetItemHeight(int nHeight)
{
	m_nItemHeight = nHeight;
	CCheckListBox::SetItemHeight(0, m_nItemHeight);
}

BEGIN_MESSAGE_MAP(CMyCheckListBox, CCheckListBox)
END_MESSAGE_MAP()

/*
	case WM_DRAWITEM:
		ASSERT(pResult == NULL);       // no return value expected
		PreDrawItem((LPDRAWITEMSTRUCT)lParam);
		// PreDrawItem调用DrawItem绘制文本（不包括复选框）
*/

void CMyCheckListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType != ODT_LISTBOX) {
		Default(); return;
	}

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	rcItem.left--;
	BOOL bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED);
	BOOL bFocus = (lpDrawItemStruct->itemState & ODS_FOCUS);

	// 1. 绘制项背景（处理选中/悬停/焦点状态）
	if (bSelected) {
		dc.FillSolidRect(rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else {
		dc.FillSolidRect(rcItem, ::GetSysColor(COLOR_WINDOW));
		dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	}

	// 2. 绘制文本
	CString strText;
	GetText(lpDrawItemStruct->itemID, strText);
	CRect rcText = rcItem;
	rcText.left = rcItem.left + 4; // 留出复选框右侧间距
	dc.DrawText(strText, rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//// 3. 绘制焦点虚线框
	//if (bFocus) {
	//	rcItem.DeflateRect(1, 1);
	//	dc.DrawFocusRect(rcItem);
	//}

	dc.Detach();
}
