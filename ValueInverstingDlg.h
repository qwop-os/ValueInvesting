
// ValueInverstingDlg.h: 头文件
//

#pragma once

#include<map>
#include<vector>
using std::map;
using std::vector;
// CValueInverstingDlg 对话框
class CValueInverstingDlg : public CDialogEx
{
// 构造
public:
	CValueInverstingDlg(CWnd* pParent = nullptr);	// 标准构造函数
	void InitDebtVal();
	void InitFont(LOGFONT*lpfont);
	void InitMaxMin();
	double GetValueByPoint(CPoint pt);
	CString GetNameByPoint(CPoint pt);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VALUEINVERSTING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	map<CString,double>m_mpDebt;
	map<CString, double>m_mpVal;
	vector<std::pair<CString,double>>m_vNameValue;
	CFont m_font;
	double m_dbMax{ 1.0f };
	double m_dbMin{0.0f};
	CRect m_rcDraw;
	CRect m_rcToolTip;
	int m_iRealWidthPillar{ 10 };
	int m_iLeftStartX{ 30 };
	int m_iBold{ 30 };
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
