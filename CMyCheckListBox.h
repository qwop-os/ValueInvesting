// CMyCheckListBox.h
#pragma once
#include<afxwin.h>
#include "pch.h"
class CMyCheckListBox : public CCheckListBox
{
	DECLARE_DYNAMIC(CMyCheckListBox)

public:
	CMyCheckListBox();
	virtual ~CMyCheckListBox();

	// 自定义设置行高的方法
	void SetItemHeight(int nHeight);
	DECLARE_MESSAGE_MAP()

protected:
	int m_nItemHeight;
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
};