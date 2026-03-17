
// ValueInverstingDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ValueInversting.h"
#include "ValueInverstingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CValueInverstingDlg 对话框



CValueInverstingDlg::CValueInverstingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VALUEINVERSTING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CValueInverstingDlg::InitDebtVal()
{
	m_mpVal.clear();
	m_mpDebt.clear();
	m_vNameValue.clear();
	m_mpVal.insert({ "现金",520.010 });
	m_mpVal.insert({ "应收款",88.105 });
	m_mpVal.insert({ "预付款",0.212 });
	m_mpVal.insert({ "存货",558.580 });
	m_mpVal.insert({ "其他流动性资产",47.392 });
	m_mpVal.insert({ "长期投资",10.100 });
	m_mpVal.insert({ "固定资产",247.040 });
	m_mpVal.insert({ "无形资产&商誉",88.670 });
	m_mpVal.insert({ "其他非流动性资产",61.290 });

	m_mpDebt.insert({"短期借款",0.570 });
	m_mpDebt.insert({ "应付款",81.940 });
	m_mpDebt.insert({ "预收款",77.490 });
	m_mpDebt.insert({ "薪酬&税",73.110 });
	m_mpDebt.insert({ "其他流动性负债",10.310 });
	m_mpDebt.insert({ "长期借款",0.000 });
	m_mpDebt.insert({ "其他非流动性负债",2.690 });
	for (const auto& elempair : m_mpVal)
	{
		m_vNameValue.push_back({ elempair.first,elempair.second });
	}
	for (const auto& elempair : m_mpDebt)
	{
		m_vNameValue.push_back({ elempair.first,elempair.second });
	}
}

double CValueInverstingDlg::GetValueByPoint(CPoint pt)
{
	int len = m_vNameValue.size();
	int iIndex = 0;
	int iStartX = 30 - 15;
	iIndex = (pt.x - iStartX) / 40;
	if (pt.x < 30)
		return m_vNameValue[0].second;

	if (iIndex > len - 1)
		return m_vNameValue[len - 1].second;

	return m_vNameValue[iIndex].second;
}

CString CValueInverstingDlg::GetNameByPoint(CPoint pt)
{
	int len = m_vNameValue.size();
	int iIndex = 0;
	int iStartX = 30 - 15;
	iIndex = (pt.x - iStartX) / 40;
	if (pt.x < 30)
		return m_vNameValue[0].first;

	if (iIndex > len - 1)
		return m_vNameValue[len - 1].first;

	return m_vNameValue[iIndex].first;
}

void CValueInverstingDlg::InitMaxMin()
{
	for (const auto&pair: m_mpDebt)
	{
		if (pair.second > m_dbMax)
			m_dbMax = pair.second;
		if (pair.second < m_dbMin)
			m_dbMin = pair.second;
	}
	for (const auto&pair : m_mpVal)
	{
		if (pair.second > m_dbMax)
			m_dbMax = pair.second;
		if (pair.second < m_dbMin)
			m_dbMin = pair.second;
	}
}

void CValueInverstingDlg::InitFont(LOGFONT*lpfont)
{
	lpfont->lfHeight = 16;               // 字体高度（逻辑单位）
	lpfont->lfWidth = 0;                  // 宽度（0 表示根据纵横比自动匹配）
	lpfont->lfEscapement = 600;             // 文本行与 x 轴的角度（十分之一度），0 为水平
	lpfont->lfOrientation = 600;            // 字符基线角度（通常与 Escapement 相同）
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
void CValueInverstingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CValueInverstingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CValueInverstingDlg 消息处理程序

BOOL CValueInverstingDlg::OnInitDialog()
{
	// TODO: 在此添加额外的初始化代码
	SetWindowText("贵州茅台资产负债表");
	InitDebtVal();
	InitMaxMin();
	LOGFONT lf = { 0 };
	InitFont(&lf);
	m_font.CreateFontIndirect(&lf);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CValueInverstingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CValueInverstingDlg::OnPaint()
{
	CRect rcClient;
	GetClientRect(rcClient);
	CPaintDC dcOrigin(this);
	//CMemDC memDC(dcOrigin,rcClient);  // 创建内存 DC，自动与 dc 兼容
	//CDC& dc = memDC.GetDC(); // 获取用于绘制的内存 DC 引用
	CDC& dc = dcOrigin;
	//// 2. 创建内存 DC 和位图
	//CDC dc;
	//dc.CreateCompatibleDC(&dcOrigin);
	//CBitmap bmp;
	//bmp.CreateCompatibleBitmap(&dcOrigin, rcClient.Width(),rcClient.Height());
	//CBitmap* pOldBmp = dc.SelectObject(&bmp);
	dc.FillSolidRect(rcClient, RGB(255,255,255));
	int iOldMode = dc.SetBkMode(TRANSPARENT);
	CFont* pOldFont = dc.SelectObject(&m_font);
	int iMaxHeightFont = static_cast<int>(dc.GetTextExtent("其他非流动性资产").cx * 1.8/2);
	int iBottom = rcClient.bottom - iMaxHeightFont;
	m_rcDraw = rcClient;
	m_rcDraw.bottom = iBottom;
	CPen pen(PS_SOLID,1,RGB(110,112,121));
	CPen* pOldPen = dc.SelectObject(&pen);
	dc.MoveTo(1, 0);
	dc.LineTo(1, iBottom);
	dc.LineTo(rcClient.right, iBottom);
	int iBold = m_iBold;
	int iLeft = m_iLeftStartX;
	int iMaxHeightPillar = static_cast<int>(iBottom * 0.8f);
	int iRealHeightPillar = static_cast<int>(iMaxHeightPillar * (m_mpVal["现金"] / (m_dbMax - m_dbMin)));
	int iRealWidthPillar = 10;
	int iWidthX = iBold;
	for (auto it = m_mpVal.begin(); ;)
	{
		int iHeightY = static_cast<int>(dc.GetTextExtent(it->first).cx *1.73 / 2);
		int iTextWidthX = static_cast<int>(dc.GetTextExtent(it->first).cx / 2);
		iRealHeightPillar = static_cast<int>(iMaxHeightPillar * (it->second / (m_dbMax - m_dbMin)));
		CRect rcPillar{ iLeft,iBottom - iRealHeightPillar,iLeft + iRealWidthPillar,iBottom };
		dc.FillSolidRect(rcPillar, RGB(52, 152, 219));
		dc.TextOut(iLeft - iTextWidthX, iBottom + iHeightY, it->first);
		auto beforeit = it;
		++it;
		if (it == m_mpVal.end())
		{
			iLeft = rcPillar.right + iWidthX;
			break;
		}
		iLeft = rcPillar.right + iWidthX;
	}

	for (auto it = m_mpDebt.begin();;)
	{
		int iHeightY = static_cast<int>(dc.GetTextExtent(it->first).cx *1.73 / 2);
		int iTextWidthX = static_cast<int>(dc.GetTextExtent(it->first).cx / 2);
		iRealHeightPillar = static_cast<int>(iMaxHeightPillar * (it->second / (m_dbMax - m_dbMin)));
		CRect rcPillar{ iLeft,iBottom - iRealHeightPillar,iLeft + iRealWidthPillar,iBottom };
		dc.FillSolidRect(rcPillar, RGB(219, 152, 52));
		dc.TextOut(iLeft - iTextWidthX, iBottom + iHeightY, it->first);
		auto beforeit = it;
		++it;
		if (it == m_mpDebt.end())
		{
			iLeft = rcPillar.right + iWidthX;
			break;
		}
		iLeft = rcPillar.right + iWidthX;
	}

	//绘制提示框
	CPoint ptCur;
	GetCursorPos(&ptCur);
	//TRACE("CursorPos ptx:%d pty:%d\n", ptCur.x, ptCur.y);
	ScreenToClient(&ptCur);
	//TRACE("ClientPos ptx:%d pty:%d\n", ptCur.x, ptCur.y);

	CString strText;
	double dbVal = GetValueByPoint(ptCur);
	CString strName = GetNameByPoint(ptCur);
	//TRACE("ClientPos Val:%.3f\n", dbVal);
	strText.Format("%s:%.3f", strName, dbVal);
	CSize textSize = dc.GetTextExtent(strText);
	int nWidth = textSize.cx + 20;
	int nHeight = textSize.cy + 10;
	CPoint tipPos = ptCur; // 计算位置
	m_rcToolTip.left = ptCur.x + 5;
	m_rcToolTip.top = ptCur.y + 2;
	m_rcToolTip.right = m_rcToolTip.left + nWidth;
	m_rcToolTip.bottom = m_rcToolTip.top + nHeight;
	// 2. 创建内存 DC 和位图
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, nWidth, nHeight);
	CBitmap* pOldBmp = memDC.SelectObject(&bmp);

	// 3. 在内存 DC 上绘制不透明内容
	memDC.FillSolidRect(0, 0, nWidth, nHeight, RGB(255, 255, 255)); // 白色背景
	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(RGB(0, 0, 0));
	CFont* pOldFont2 = memDC.SelectObject(GetFont()); // 使用对话框字体
	memDC.DrawText(strText, CRect(0, 0, nWidth, nHeight), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	memDC.SelectObject(pOldFont2);

	// 4. 使用 AlphaBlend 混合到目标 DC
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 192; // 透明度 (0-255), 180 表示约 70% 不透明
	bf.AlphaFormat = 0;            // 不使用每个像素的 Alpha 通道，使用 SourceConstantAlpha
	dc.AlphaBlend(m_rcToolTip.left, m_rcToolTip.top, nWidth, nHeight,
		&memDC, 0, 0, nWidth, nHeight, bf);

	// 5. 清理
	memDC.SelectObject(pOldBmp);
	

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);
	dc.SetBkMode(iOldMode);
	//dcOrigin.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dc, 0, 0, SRCCOPY);
	//dc.SelectObject(pOldBmp);
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CValueInverstingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CValueInverstingDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_rcDraw.PtInRect(point))
	{
		InvalidateRect(m_rcToolTip);
		//InvalidateRect(m_rcDraw);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}
