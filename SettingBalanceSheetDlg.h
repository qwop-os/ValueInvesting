#pragma once

// SettingBalanceSheetDlg 对话框

class SettingBalanceSheetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SettingBalanceSheetDlg)

public:
	SettingBalanceSheetDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SettingBalanceSheetDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTING_BALANCESHEET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
