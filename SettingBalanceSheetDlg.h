#pragma once
#include "pch.h"
#include <vector>
#include "resource.h"

class CSettingBalanceSheetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingBalanceSheetDlg)

public:
	CSettingBalanceSheetDlg(CWnd* pParent = nullptr);
	virtual ~CSettingBalanceSheetDlg();

	void SetConfig(BalanceSheetConfig* pConfig);

	enum { IDD = IDD_SETTING_BALANCESHEET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

private:
	BalanceSheetConfig* m_pConfig;               // 指向主窗口配置
	std::vector<CategoryItem>* m_pCurrVec;       // 当前编辑的 vector（资产或负债）
	std::vector<FieldMeta> m_currFields;         // 当前 Tab 下的可用字段

	CTabCtrl m_tab;
	CListBox m_listCategory;
	CListCtrl m_listFields;
	CButton m_btnAdd;
	CButton m_btnDel;
	CButton m_btnRename;

	int m_nCurTab;                               // 0=资产，1=负债
	int m_nSelCategory;                          // 当前选中类别索引（-1 表示无）

	void LoadCategoriesToList();
	void LoadFieldsToList(int nCategoryIndex = -1);
	void OnTabChanged();
	void OnFieldCheckChanged(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLbnSelchangeListCategory();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnRename();
	afx_msg void OnLvnItemchangedListFields(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOK();
};