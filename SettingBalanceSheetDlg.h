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
	BalanceSheetConfig SaveConfig();
	// 处理 ESC 键，可重写 OnCancel
	virtual void OnCancel();
	enum { IDD = IDD_SETTING_BALANCESHEET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	BalanceSheetConfig* m_pConfig{nullptr};               // 指向主窗口配置
	BalanceSheetConfig m_ConfigOrigin;					  // 指向主窗口配置副本
	std::vector<CategoryItem>* m_pCurrVec;       // 当前编辑的 vector（资产或负债）
	std::vector<FieldMeta> m_currFields;         // 当前 Tab 下的可用字段

	CListBox m_listCategory;
	CListCtrl m_listFields;
	CButton m_btnAdd;
	CButton m_btnDel;
	CButton m_btnRename;

	int m_nCurTab;                               // 0=资产，1=负债
	CRect m_rcVal;								 // 资产
	CRect m_rcDebt;								 // 负债
	int m_nSelCategory;                          // 当前选中类别索引（-1 表示无）
	bool m_bCheckBoxEnabled;					// 复选框是否可操作（当有有效类别时为 true）

	void LoadCategoriesToList();
	void LoadFieldsToList(int nCategoryIndex = -1);
	void OnTabChanged();
	void OnFieldCheckChanged(NMHDR* pNMHDR, LRESULT* pResult);
	bool IsModifiy();							// 是否修改配资

	afx_msg void OnLbnSelchangeListCategory();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnRename();
	afx_msg void OnLvnItemchangedListFields(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOK();

	afx_msg void OnClickListFields(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangingListFields(NMHDR* pNMHDR, LRESULT* pResult);
public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};