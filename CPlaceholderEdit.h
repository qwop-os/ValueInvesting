#pragma once
#include <afxwin.h>

class CPlaceholderEdit : public CEdit
{
	DECLARE_DYNAMIC(CPlaceholderEdit)

public:
	CPlaceholderEdit();
	virtual ~CPlaceholderEdit();

	// 设置提示文字
	void SetPlaceholder(LPCTSTR lpszText);
	void InitFont(LOGFONT*lpfont);

protected:
	CString m_strPlaceholder;
	BOOL    m_bShowPlaceholder;   // 是否应该显示提示
	CFont	m_font;
	CBrush m_brBackground;
	// 判断是否应显示提示（空且无焦点）
	void UpdatePlaceholderState();

	// 重写
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChange();      // 响应 EN_CHANGE
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
};
