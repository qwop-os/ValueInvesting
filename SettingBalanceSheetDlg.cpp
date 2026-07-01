#include "pch.h"
#include "framework.h"
#include "SettingBalanceSheetDlg.h"
#include "CValorDebtDlg.h"
#include "afxdialogex.h"
#include <set>

// 全局字段元数据（与主对话框一致，或通过头文件共享）
extern const std::vector<FieldMeta> g_AllFields;

IMPLEMENT_DYNAMIC(CSettingBalanceSheetDlg, CDialogEx)

CSettingBalanceSheetDlg::CSettingBalanceSheetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETTING_BALANCESHEET, pParent)
	, m_pConfig(nullptr)
	, m_pCurrVec(nullptr)
	, m_nCurTab(0)
	, m_nSelCategory(-1)
{
	if(m_pConfig==nullptr)
		m_pConfig = new BalanceSheetConfig;
}

CSettingBalanceSheetDlg::~CSettingBalanceSheetDlg()
{
	if (m_pConfig)
		delete m_pConfig;
}

void CSettingBalanceSheetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CATEGORY, m_listCategory);
	DDX_Control(pDX, IDC_LIST_FIELDS, m_listFields);
	DDX_Control(pDX, IDC_BTN_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BTN_DEL, m_btnDel);
	DDX_Control(pDX, IDC_BTN_RENAME, m_btnRename);
}

BEGIN_MESSAGE_MAP(CSettingBalanceSheetDlg, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST_CATEGORY, &CSettingBalanceSheetDlg::OnLbnSelchangeListCategory)
	ON_BN_CLICKED(IDC_BTN_ADD, &CSettingBalanceSheetDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CSettingBalanceSheetDlg::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_RENAME, &CSettingBalanceSheetDlg::OnBnClickedBtnRename)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FIELDS, &CSettingBalanceSheetDlg::OnLvnItemchangedListFields)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FIELDS, &CSettingBalanceSheetDlg::OnClickListFields)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_FIELDS, &CSettingBalanceSheetDlg::OnLvnItemchangingListFields)
	ON_BN_CLICKED(IDC_BUTTON_SaveOk, &CSettingBalanceSheetDlg::OnBnClickedButtonSave)
	ON_WM_CLOSE()   // 映射 WM_CLOSE 消息
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CSettingBalanceSheetDlg::OnClose()
{
	if (!IsModifiy())// 未修改
	{
		EndDialog(IDCANCEL);    // 直接关闭对话框，返回 IDCANCEL
		return;
	}
	// 弹出提示框，两个选项：保存退出 (IDYES) 和 退出 (IDNO)
	int nResult = MessageBox(_T("是否保存当前修改？"), _T("提示"), MB_YESNO | MB_ICONQUESTION);

	if (nResult == IDYES)   
		EndDialog(IDOK);    // 关闭对话框并返回 IDOK
	else if (nResult == IDNO)   
		EndDialog(IDCANCEL);    // 直接关闭对话框，返回 IDCANCEL
	// 注意：不要调用 CDialogEx::OnClose()，因为已经用 EndDialog 结束了模态循环
}

void CSettingBalanceSheetDlg::OnCancel()
{
	if (!IsModifiy())// 未修改
	{
		EndDialog(IDCANCEL);    // 直接关闭对话框，返回 IDCANCEL
		return;
	}
	// 自定义处理：例如弹出询问是否保存的对话框
	int nResult = MessageBox(_T("是否保存更改？"), _T("提示"), MB_YESNO | MB_ICONQUESTION);
	if (nResult == IDYES)
		EndDialog(IDOK);    // 关闭对话框并返回 IDOK
	else if (nResult == IDNO)
		EndDialog(IDCANCEL);    // 直接关闭对话框，返回 IDCANCEL
	// 如果用户点了关闭（X）或其它，不做任何操作（不关闭）
}

void CSettingBalanceSheetDlg::OnLvnItemchangingListFields(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 如果复选框不可操作，且本次变化涉及复选框状态（state image mask）
	if (!m_bCheckBoxEnabled && (pNMLV->uChanged & LVIF_STATE) &&
		(pNMLV->uNewState & LVIS_STATEIMAGEMASK) != (pNMLV->uOldState & LVIS_STATEIMAGEMASK))
	{
		*pResult = 1;   // 阻止改变
		return;
	}

	*pResult = 0;
}

void CSettingBalanceSheetDlg::OnClickListFields(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 如果复选框当前不可操作，直接阻止任何点击导致的复选框变化
	if (!m_bCheckBoxEnabled)
	{
		*pResult = 1;   // 阻止默认行为
		return;
	}

	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CPoint pt = pNMIA->ptAction;
	//m_listFields.ScreenToClient(&pt);

	LVHITTESTINFO ht = { 0 };
	ht.pt = pt;
	int nItem = m_listFields.HitTest(&ht);

	if (nItem != -1 && (ht.flags & LVHT_ONITEM))
	{
		CRect rect;
		m_listFields.GetSubItemRect(nItem, 0, LVIR_LABEL, rect);

		// 复选框区域宽度（可根据需要调整）
		int nCheckBoxWidth = GetSystemMetrics(SM_CXSMICON) + 2;

		// 仅当点击在第一列且坐标在复选框区域时才允许切换
		if (ht.iSubItem == 0 && pt.x <= rect.left + nCheckBoxWidth)
		{
			*pResult = 0;   // 允许默认处理（复选框会切换）
			return;
		}
		else
		{
			*pResult = 1;   // 阻止切换复选框
			return;
		}
	}

	*pResult = 0;
}

void CSettingBalanceSheetDlg::SetConfig(BalanceSheetConfig* pConfig)
{
	m_ConfigOrigin = *pConfig;
	*m_pConfig = *pConfig;
}

BalanceSheetConfig CSettingBalanceSheetDlg::SaveConfig()
{
	return *m_pConfig;
}

BOOL CSettingBalanceSheetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_nCurTab = 0;
	m_pCurrVec = &m_pConfig->assetCategories;

	m_bCheckBoxEnabled = false;   // 初始禁用

	// 初始化字段列表控件
	m_listFields.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | m_listFields.GetExtendedStyle());
	m_listFields.InsertColumn(0, _T("字段键"), LVCFMT_LEFT, 150);
	m_listFields.InsertColumn(1, _T("中文名称"), LVCFMT_LEFT, 200);

	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetFont());
	CRect rcCat;
	m_listCategory.GetWindowRect(rcCat);
	ScreenToClient(rcCat);
	m_rcVal.bottom = rcCat.top - 5;
	m_rcVal.left = rcCat.left;
	m_rcVal.top = m_rcVal.bottom - dc.GetTextExtent("中中").cy - 8;
	m_rcVal.right = m_rcVal.left + dc.GetTextExtent("中中").cx + 16;
	m_rcDebt = m_rcVal;
	m_rcDebt.left = m_rcVal.right;
	m_rcDebt.right = m_rcDebt.left + dc.GetTextExtent("中中").cx + 16;
	dc.SelectObject(pOldFont);
	// 加载当前 Tab 的数据
	OnTabChanged();
	return TRUE;
}

void CSettingBalanceSheetDlg::OnTabChanged()
{
	// 筛选当前 Tab 对应的字段元数据
	m_currFields.clear();
	CString sectionFilter;
	if (m_nCurTab == 0)
		sectionFilter = _T("assets");   // current_assets 和 non_current_assets 均符合
	else
		sectionFilter = _T("liabilities");

	for (const auto& fm : g_AllFields)
	{
		CString sec = fm.section;
		if (sec.Find(sectionFilter) != -1)
			m_currFields.push_back(fm);
	}

	// 刷新类别列表
	LoadCategoriesToList();

	// 默认选中第一个类别
	if (m_listCategory.GetCount() > 0)
	{
		m_listCategory.SetCurSel(0);
		m_nSelCategory = 0;
	}
	else
	{
		m_nSelCategory = -1;
	}
	LoadFieldsToList(m_nSelCategory);

	// 按钮状态
	m_btnDel.EnableWindow(m_nSelCategory != -1);
	m_btnRename.EnableWindow(m_nSelCategory != -1);
}

void CSettingBalanceSheetDlg::LoadCategoriesToList()
{
	m_listCategory.ResetContent();
	if (m_pCurrVec)
	{
		for (const auto& cat : *m_pCurrVec)
		{
			m_listCategory.AddString(cat.name);
		}
	}
}

bool CSettingBalanceSheetDlg::IsModifiy()
{
	if (m_pConfig == nullptr)
		return true;

	// 比较资产类别列表
	if (m_ConfigOrigin.assetCategories.size() != m_pConfig->assetCategories.size())
		return true;

	// 上述判断 基于一级名称顺序不变，如果删除多个一级名称，之后再以不同顺序添加 暂时判断为修改。
	for (size_t i = 0; i < m_ConfigOrigin.assetCategories.size(); ++i)
	{
		const auto& origCat = m_ConfigOrigin.assetCategories[i];
		const auto& curCat = m_pConfig->assetCategories[i];

		// 比较一级名称
		if (origCat.name != curCat.name)
			return true;

		// 比较二级字段列表
		if (origCat.fields.size() != curCat.fields.size())
			return true;
		for (size_t j = 0; j < origCat.fields.size(); ++j)
		{
			if (origCat.fields[j] != curCat.fields[j])
				return true;
		}
	}

	// 比较负债类别列表
	if (m_ConfigOrigin.liabilityCategories.size() != m_pConfig->liabilityCategories.size())
		return true;
	for (size_t i = 0; i < m_ConfigOrigin.liabilityCategories.size(); ++i)
	{
		const auto& origCat = m_ConfigOrigin.liabilityCategories[i];
		const auto& curCat = m_pConfig->liabilityCategories[i];

		if (origCat.name != curCat.name)
			return true;
		if (origCat.fields.size() != curCat.fields.size())
			return true;
		for (size_t j = 0; j < origCat.fields.size(); ++j)
		{
			if (origCat.fields[j] != curCat.fields[j])
				return true;
		}
	}

	// 所有比较都相等
	return false;
}

void CSettingBalanceSheetDlg::LoadFieldsToList(int nCategoryIndex /*= -1*/)
{

	m_listFields.DeleteAllItems();

	// 确定是否有有效类别（用于控制复选框是否可操作）
	BOOL bHasValidCategory = (nCategoryIndex >= 0 && m_pCurrVec && nCategoryIndex < (int)m_pCurrVec->size());
	m_bCheckBoxEnabled = bHasValidCategory;   // 更新标志

	// 获取当前选中类别包含的字段集合（用于勾选）
	std::set<CString> selectedFields;
	if (nCategoryIndex >= 0 && m_pCurrVec && nCategoryIndex < (int)m_pCurrVec->size())
	{
		const CategoryItem& cat = (*m_pCurrVec)[nCategoryIndex];
		for (const auto& f : cat.fields)
			selectedFields.insert(f);
	}

	// 填充所有可用字段
	for (size_t i = 0; i < m_currFields.size(); ++i)
	{
		const FieldMeta& fm = m_currFields[i];
		int nItem = m_listFields.InsertItem((int)i, fm.key);
		m_listFields.SetItemText(nItem, 1, fm.display);
		m_listFields.SetItemData(nItem, (DWORD_PTR)&fm);

		// 设置复选框状态
		BOOL bCheck = selectedFields.find(fm.key) != selectedFields.end();
		m_listFields.SetCheck(nItem, bCheck);
	}
}

void CSettingBalanceSheetDlg::OnLbnSelchangeListCategory()
{
	int nSel = m_listCategory.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_nSelCategory = nSel;
		LoadFieldsToList(nSel);
		m_btnDel.EnableWindow(TRUE);
		m_btnRename.EnableWindow(TRUE);
	}
	else
	{
		m_nSelCategory = -1;
		LoadFieldsToList(-1);
		m_btnDel.EnableWindow(FALSE);
		m_btnRename.EnableWindow(FALSE);
	}
}

void CSettingBalanceSheetDlg::OnBnClickedBtnAdd()
{
	CValorDebtDlg dlgValDebt(m_nCurTab);
	// 设置可用字段（实际上dlg内部会根据m_nCurTab从g_AllFields加载，可以不设置）
	// 设置初始选中为空
	dlgValDebt.SetSelectedFields({});
	if (dlgValDebt.DoModal() == IDOK)
	{
		CString strNewName = dlgValDebt.m_strname;
		if (!strNewName.IsEmpty())
		{
			CategoryItem newCat;
			newCat.name = strNewName;
			newCat.fields = dlgValDebt.GetSelectedFields(); // 获取勾选的字段key
			m_pCurrVec->push_back(newCat);
			LoadCategoriesToList();
			int nNewIndex = (int)m_pCurrVec->size() - 1;
			m_listCategory.SetCurSel(nNewIndex);
			m_nSelCategory = nNewIndex;
			LoadFieldsToList(nNewIndex);
			m_btnDel.EnableWindow(TRUE);
			m_btnRename.EnableWindow(TRUE);
		}
		else
		{
			MessageBox(_T("未输入名字"));
		}
	}
}

void CSettingBalanceSheetDlg::OnBnClickedBtnDel()
{
	if (m_nSelCategory >= 0 && m_nSelCategory < (int)m_pCurrVec->size())
	{
		m_pCurrVec->erase(m_pCurrVec->begin() + m_nSelCategory);
		LoadCategoriesToList();
		if (m_pCurrVec->empty())
		{
			m_nSelCategory = -1;
			LoadFieldsToList(-1);
			m_btnDel.EnableWindow(FALSE);
			m_btnRename.EnableWindow(FALSE);
		}
		else
		{
			int nNewSel = min(m_nSelCategory, (int)m_pCurrVec->size() - 1);
			m_listCategory.SetCurSel(nNewSel);
			m_nSelCategory = nNewSel;
			LoadFieldsToList(nNewSel);
		}
	}
}

void CSettingBalanceSheetDlg::OnBnClickedBtnRename()
{
	if (m_nSelCategory >= 0 && m_nSelCategory < (int)m_pCurrVec->size())
	{
		CValorDebtDlg dlgValDebt(m_nCurTab);
		// 设置当前类别已有的字段作为初始勾选
		const CategoryItem& cat = (*m_pCurrVec)[m_nSelCategory];
		dlgValDebt.SetSelectedFields(cat.fields);
		dlgValDebt.m_strname = cat.name; // 初始化名称
		if (dlgValDebt.DoModal() == IDOK)
		{
			CString strNewName = dlgValDebt.m_strname;
			if (!strNewName.IsEmpty())
			{
				(*m_pCurrVec)[m_nSelCategory].name = strNewName;
				(*m_pCurrVec)[m_nSelCategory].fields = dlgValDebt.GetSelectedFields();
				LoadCategoriesToList();
				m_listCategory.SetCurSel(m_nSelCategory);
				LoadFieldsToList(m_nSelCategory); // 刷新字段列表显示
			}
		}
	}
}

void CSettingBalanceSheetDlg::OnLvnItemchangedListFields(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// 仅处理复选框状态改变
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & 0x2000) != (pNMLV->uOldState & 0x2000))
	{
		OnFieldCheckChanged(pNMHDR, pResult);
	}
	*pResult = 0;
}

void CSettingBalanceSheetDlg::OnFieldCheckChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_nSelCategory < 0 || m_nSelCategory >= (int)m_pCurrVec->size())
		return;

	CategoryItem& cat = (*m_pCurrVec)[m_nSelCategory];
	cat.fields.clear();

	// 遍历所有字段，收集勾选项
	for (int i = 0; i < m_listFields.GetItemCount(); ++i)
	{
		if (m_listFields.GetCheck(i))
		{
			FieldMeta* pFM = (FieldMeta*)m_listFields.GetItemData(i);
			if (pFM)
				cat.fields.push_back(pFM->key);
		}
	}
}

void CSettingBalanceSheetDlg::OnOK()
{
	// 确保当前选中类别的字段已被保存
	if (m_nSelCategory >= 0)
	{
		OnFieldCheckChanged(nullptr, nullptr);
	}
	CDialogEx::OnOK();
}

void CSettingBalanceSheetDlg::OnBnClickedButtonSave()
{
	// 关闭对话框并返回 IDOK
	EndDialog(IDOK);
}


void CSettingBalanceSheetDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CFont* pOldFont = dc.SelectObject(GetFont());
	int iOldBkMod = dc.SetBkMode(TRANSPARENT);
	CPen pen(PS_SOLID, 1, RGB(192, 0, 0));
	dc.Draw3dRect(m_rcVal, RGB(192, 0, 0), RGB(192, 0, 0));
	COLORREF clrText = m_nCurTab == 0 ? RGB(255, 255, 255) : RGB(91, 91, 93);
	COLORREF clrBg = m_nCurTab == 0 ? RGB(192, 0, 0) : RGB(255, 255, 255);
	dc.FillSolidRect(m_rcVal, clrBg);
	dc.SetTextColor(clrText);
	dc.DrawText("资产", m_rcVal, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	clrText = m_nCurTab == 1 ? RGB(255, 255, 255) : RGB(91, 91, 93);
	clrBg = m_nCurTab == 1 ? RGB(192, 0, 0) : RGB(255, 255, 255);
	dc.FillSolidRect(m_rcDebt, clrBg);
	dc.SetTextColor(clrText);
	dc.DrawText("负债", m_rcDebt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	dc.SetBkColor(iOldBkMod);
	dc.SelectObject(pOldFont);
}


void CSettingBalanceSheetDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rcUnion = m_rcVal;
	rcUnion.UnionRect(m_rcVal,m_rcDebt);
	if (m_rcVal.PtInRect(point))
	{
		int nSel = 0;
		if (nSel != m_nCurTab)
		{
			m_nCurTab = nSel;
			m_pCurrVec = (m_nCurTab == 0) ? &m_pConfig->assetCategories : &m_pConfig->liabilityCategories;
			OnTabChanged();
			InvalidateRect(rcUnion, TRUE);
		}
	}
	else if (m_rcDebt.PtInRect(point))
	{
		int nSel = 1;
		if (nSel != m_nCurTab)
		{
			m_nCurTab = nSel;
			m_pCurrVec = (m_nCurTab == 0) ? &m_pConfig->assetCategories : &m_pConfig->liabilityCategories;
			OnTabChanged();
			InvalidateRect(rcUnion, TRUE);
		}
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}
