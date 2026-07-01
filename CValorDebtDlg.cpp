// CValorDebtDlg.cpp
#include "pch.h"
#include "ValueInversting.h"
#include "CValorDebtDlg.h"
#include "afxdialogex.h"

extern const std::vector<FieldMeta> g_AllFields; // 全局字段元数据

IMPLEMENT_DYNAMIC(CValorDebtDlg, CDialogEx)
#define IDC_CHECK_FIELD2 1711
CValorDebtDlg::CValorDebtDlg(int itab, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ValOrDebtDlg, pParent)
	, m_nCurTab(itab)
	, m_strname(_T(""))
{
}

CValorDebtDlg::~CValorDebtDlg()
{
}

void CValorDebtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NameEdit, m_strname);
}

BEGIN_MESSAGE_MAP(CValorDebtDlg, CDialogEx)
	ON_EN_CHANGE(IDC_NameEdit, &CValorDebtDlg::OnEnChangeNameedit)
	ON_BN_CLICKED(IDOK, &CValorDebtDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CValorDebtDlg::OnEnChangeNameedit()
{
	UpdateData(TRUE);
}

BOOL CValorDebtDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rcList;
	CRect rcClient;
	GetClientRect(rcClient);
	if (GetDlgItem(IDC_NameEdit) && GetDlgItem(IDOK))
	{
		CRect rcEdit;
		GetDlgItem(IDC_NameEdit)->GetWindowRect(rcEdit);
		ScreenToClient(rcEdit);
		rcList.top = rcEdit.bottom + 3;
		rcList.left = rcEdit.left;
		rcList.right = rcClient.right;
		CRect rcOk;
		GetDlgItem(IDOK)->GetWindowRect(rcOk);
		ScreenToClient(rcOk);
		rcList.bottom = rcOk.top - 1;
	}
	// 2. 动态创建 CMyCheckListBox
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL |
		LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOTIFY;

	// 使用原 ID 或新 ID 均可，这里沿用 IDC_CHECK_FIELDS 保持消息映射兼容
	if (m_checkFields.Create(dwStyle, rcList, this, IDC_CHECK_FIELD2))
	{
		LoadAvailableFields();
		// 3. 同步对话框字体（动态控件默认使用系统字体，需手动继承）
		m_checkFields.SetFont(GetFont());
		m_checkFields.SetItemHeight(18);
		// 设置初始勾选状态
		for (int i = 0; i < m_checkFields.GetCount(); ++i)
		{
			BOOL bChecked = FALSE;
			CString key = m_availableFields[i].key;
			for (const auto& selKey : m_selectedFieldKeys)
			{
				if (key == selKey)
				{
					bChecked = TRUE;
					break;
				}
			}

			m_checkFields.SetCheck(i, bChecked);
		}
	}
	// 发送 WM_CHANGEUISTATE 消息隐藏焦点矩形（直接使用系统宏）
	// 注意：此消息通常发送给对话框窗口，而不是子控件
	//::SendMessage(GetSafeHwnd(), WM_CHANGEUISTATE, MAKELONG(UIS_SET, UISF_HIDEFOCUS), 0);
	return TRUE;
}

void CValorDebtDlg::LoadAvailableFields()
{
	m_checkFields.ResetContent();
	m_availableFields.clear();

	CString sectionFilter = (m_nCurTab == 0) ? _T("assets") : _T("liabilities");

	for (const auto& fm : g_AllFields)
	{
		CString sec = fm.section;
		if (sec.Find(sectionFilter) != -1)
		{
			m_availableFields.push_back(fm);
			m_checkFields.AddString(fm.display);
		}
	}
}

void CValorDebtDlg::SetSelectedFields(const std::vector<CString>& selectedKeys)
{
	m_selectedFieldKeys = selectedKeys;
}

std::vector<CString> CValorDebtDlg::GetSelectedFields() 
{
	return m_resultFields;
}

void CValorDebtDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	if (m_strname.IsEmpty())
	{
		MessageBox(_T("请输入类别名称"), _T("提示"), MB_ICONWARNING);
		return;
	}
	// 保存勾选的字段
	m_resultFields.clear();
	for (int i = 0; i < m_checkFields.GetCount(); ++i)
	{
		if (m_checkFields.GetCheck(i) && i < (int)m_availableFields.size())
		{
			m_resultFields.push_back(m_availableFields[i].key);
		}
	}
	CDialogEx::OnOK();
}