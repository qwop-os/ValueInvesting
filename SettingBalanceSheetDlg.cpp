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
}

CSettingBalanceSheetDlg::~CSettingBalanceSheetDlg()
{
}

void CSettingBalanceSheetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_LIST_CATEGORY, m_listCategory);
	DDX_Control(pDX, IDC_LIST_FIELDS, m_listFields);
	DDX_Control(pDX, IDC_BTN_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BTN_DEL, m_btnDel);
	DDX_Control(pDX, IDC_BTN_RENAME, m_btnRename);
}

BEGIN_MESSAGE_MAP(CSettingBalanceSheetDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_TAB1, &CSettingBalanceSheetDlg::OnTcnSelchangeTab1)
	ON_LBN_SELCHANGE(IDC_LIST_CATEGORY, &CSettingBalanceSheetDlg::OnLbnSelchangeListCategory)
	ON_BN_CLICKED(IDC_BTN_ADD, &CSettingBalanceSheetDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CSettingBalanceSheetDlg::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_RENAME, &CSettingBalanceSheetDlg::OnBnClickedBtnRename)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FIELDS, &CSettingBalanceSheetDlg::OnLvnItemchangedListFields)
END_MESSAGE_MAP()

void CSettingBalanceSheetDlg::SetConfig(BalanceSheetConfig* pConfig)
{
	m_pConfig = pConfig;
}

BOOL CSettingBalanceSheetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化 Tab 控件
	m_tab.InsertItem(0, _T("资产"));
	m_tab.InsertItem(1, _T("负债"));
	m_nCurTab = 0;
	m_pCurrVec = &m_pConfig->assetCategories;

	// 初始化字段列表控件
	m_listFields.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | m_listFields.GetExtendedStyle());
	m_listFields.InsertColumn(0, _T("字段键"), LVCFMT_LEFT, 150);
	m_listFields.InsertColumn(1, _T("中文名称"), LVCFMT_LEFT, 200);

	// 加载当前 Tab 的数据
	OnTabChanged();
	return TRUE;
}
// 点击切换的资产、负债触发，切换前一个、后一个按钮不触发。（前一个、后一个按钮触发的是UDN_DELTAPOS ）
void CSettingBalanceSheetDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nSel = m_tab.GetCurSel();
	if (nSel != m_nCurTab)
	{
		m_nCurTab = nSel;
		m_pCurrVec = (m_nCurTab == 0) ? &m_pConfig->assetCategories : &m_pConfig->liabilityCategories;
		OnTabChanged();
	}
	*pResult = 0;
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

void CSettingBalanceSheetDlg::LoadFieldsToList(int nCategoryIndex /*= -1*/)
{
	m_listFields.DeleteAllItems();

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

	// 弹出输入框让用户输入新类别名称
	CString strNewName;
	// 使用简易输入对话框（可用 CInputDialog 或自定义）
	// 此处假设有一个输入框函数，实际可用 CDialog 或 AfxInputBox 类似功能
	// 这里用简单对话框示意
	// 若需要完整代码，可自行实现 CInputDlg
	// 简化：直接默认名称
	CValorDebtDlg dlgValDebt(m_nCurTab);
	if (dlgValDebt.DoModal() == IDOK)
	{
		strNewName = dlgValDebt.m_strname;
		if (!strNewName.IsEmpty())
		{
			CategoryItem newCat;
			newCat.name = strNewName;
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
			MessageBox("未输入名字");
		}
	};
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
		// 弹出输入框修改名称，此处简化
		CValorDebtDlg dlgValDebt(m_nCurTab);
		if (dlgValDebt.DoModal() == IDOK)
		{
			CString strNewName;
			strNewName = dlgValDebt.m_strname;

			if (!strNewName.IsEmpty())
			{
				(*m_pCurrVec)[m_nSelCategory].name = strNewName;
				LoadCategoriesToList();
				m_listCategory.SetCurSel(m_nSelCategory);
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