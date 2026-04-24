#pragma once
#include "CMyCheckListBox.h"

// CValorDebtDlg 对话框
class CValorDebtDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CValorDebtDlg)

public:
	CValorDebtDlg(int itab,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CValorDebtDlg();
	BOOL OnInitDialog();
	// 设置初始选中的字段（可选）
	void SetSelectedFields(const std::vector<CString>& selectedKeys);
	// 获取用户勾选的字段
	std::vector<CString> GetSelectedFields();

	void LoadAvailableFields();            // 根据 m_nCurTab 加载可用字段

	afx_msg void OnEnChangeNameedit();
	afx_msg void OnBnClickedOk();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ValOrDebtDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nCurTab;                               // 0=资产，1=负债
	CMyCheckListBox m_checkFields;
	std::vector<FieldMeta> m_availableFields;   // 当前 Tab 下的所有字段
	std::vector<CString> m_selectedFieldKeys;   // 初始勾选的字段 key
	CString m_strname;
	std::vector<CString> m_resultFields;
};
