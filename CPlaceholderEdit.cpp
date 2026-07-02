#include "pch.h"
#include "CPlaceholderEdit.h"
#include "Resource.h"

IMPLEMENT_DYNAMIC(CPlaceholderEdit, CEdit)

CPlaceholderEdit::CPlaceholderEdit()
	: m_bShowPlaceholder(FALSE)
{
	LOGFONT lf = { 0 };
	InitFont(&lf);
	m_font.CreateFontIndirect(&lf);
	m_brBackground.CreateSolidBrush(RGB(224, 224, 224));
}

CPlaceholderEdit::~CPlaceholderEdit()
{
}

void CPlaceholderEdit::SetPlaceholder(LPCTSTR lpszText)
{
	m_strPlaceholder = lpszText;
	UpdatePlaceholderState();
	Invalidate();   // 立即刷新
}

void CPlaceholderEdit::UpdatePlaceholderState()
{
	// 当编辑框内容为空，并且没有焦点时，显示提示
	CString strText;
	GetWindowText(strText);
	BOOL bEmpty = strText.IsEmpty();
	BOOL bHasFocus = (GetFocus() == this);
	m_bShowPlaceholder = bEmpty && !bHasFocus && !m_strPlaceholder.IsEmpty();
}

BEGIN_MESSAGE_MAP(CPlaceholderEdit, CEdit)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_WM_CTLCOLOR_REFLECT()   // 反射消息
END_MESSAGE_MAP()

HBRUSH CPlaceholderEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// 对于反射消息，不需要再调用基类（CEdit 不处理反射）
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetBkMode(TRANSPARENT);
		// 使用静态画刷或成员画刷
		static CBrush br(RGB(224, 224, 224));
		return (HBRUSH)br;
	}
	return NULL;  // 或者返回默认画刷
}

void CPlaceholderEdit::InitFont(LOGFONT*lpfont)
{
	lpfont->lfHeight = 14;               // 字体高度（逻辑单位）
	lpfont->lfWidth = 0;                  // 宽度（0 表示根据纵横比自动匹配）
	lpfont->lfEscapement = 0;             // 文本行与 x 轴的角度（十分之一度），0 为水平
	lpfont->lfOrientation = 0;            // 字符基线角度（通常与 Escapement 相同）
	lpfont->lfWeight = FW_NORMAL;         // 字体粗细（FW_BOLD 为粗体）
	lpfont->lfItalic = FALSE;              // ★ 关键：设置为 TRUE 表示斜体
	lpfont->lfUnderline = FALSE;
	lpfont->lfStrikeOut = FALSE;
	lpfont->lfCharSet = DEFAULT_CHARSET;
	lpfont->lfOutPrecision = OUT_DEFAULT_PRECIS;
	lpfont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lpfont->lfQuality = DEFAULT_QUALITY;
	lpfont->lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy_s(lpfont->lfFaceName, LF_FACESIZE, _T("宋体")); // 字体名称
}

void CPlaceholderEdit::OnPaint()
{
	CPaintDC dc(this); // 获取设备上下文

	// 1. 调用默认窗口过程，绘制编辑框的边框、背景及文本内容
	DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, 0);
	// 获取编辑框客户区矩形
	CRect rect;
	GetClientRect(&rect);
	//dc.FillSolidRect(rect, RGB(224, 224, 224));
	// 如果需要显示提示，则在其上绘制灰色文字
	if (m_bShowPlaceholder)
	{
		// 设置字体（与编辑框相同）
		CFont* pOldFont = dc.SelectObject(&m_font);

		// 设置文本颜色为灰色
		dc.SetTextColor(RGB(128, 128, 128));
		dc.SetBkMode(TRANSPARENT);

		// 绘制提示文本
		dc.DrawText(m_strPlaceholder, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		// 恢复字体
		dc.SelectObject(pOldFont);
	}
}

void CPlaceholderEdit::OnSetFocus(CWnd* pOldWnd)
{
	// 获得焦点时，若显示提示，则清除提示并清空编辑框（用户准备输入）
	if (m_bShowPlaceholder)
	{
		// 注意：不清除文本，否则用户输入时会覆盖提示？我们采用另一种方式：
		// 在获得焦点时直接将提示清除（即设置空文本），并刷新。
		// 但为了更好的用户体验，可以保留空文本，让用户直接输入。
		// 这里我们只是更新状态并重绘（提示消失）
		UpdatePlaceholderState();
		Invalidate();
	}
	// 调用基类
	CEdit::OnSetFocus(pOldWnd);
}

void CPlaceholderEdit::OnKillFocus(CWnd* pNewWnd)
{
	// 失去焦点时更新状态（若空则显示提示）
	UpdatePlaceholderState();
	Invalidate();
	CEdit::OnKillFocus(pNewWnd);
}

void CPlaceholderEdit::OnChange()
{
	// 内容变化时更新状态
	UpdatePlaceholderState();
	Invalidate();
}
