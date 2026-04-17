#pragma once


// CValorDebtDlg 对话框

class CValorDebtDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CValorDebtDlg)

public:
	CValorDebtDlg(int itab,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CValorDebtDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ValOrDebtDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeNameedit();
	CEdit m_editname;
	CString m_strname;
	int m_nCurTab;                               // 0=资产，1=负债
};
