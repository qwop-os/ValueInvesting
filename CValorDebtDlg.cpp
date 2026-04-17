// CValorDebtDlg.cpp: 实现文件
//

#include "pch.h"
#include "ValueInversting.h"
#include "CValorDebtDlg.h"
#include "afxdialogex.h"


// CValorDebtDlg 对话框

IMPLEMENT_DYNAMIC(CValorDebtDlg, CDialogEx)

CValorDebtDlg::CValorDebtDlg(int itab,CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ValOrDebtDlg, pParent)
	, m_nCurTab(itab)
{

}

CValorDebtDlg::~CValorDebtDlg()
{
}

void CValorDebtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NameEdit, m_editname);
}


BEGIN_MESSAGE_MAP(CValorDebtDlg, CDialogEx)
	ON_EN_CHANGE(IDC_NameEdit, &CValorDebtDlg::OnEnChangeNameedit)
END_MESSAGE_MAP()


// CValorDebtDlg 消息处理程序


void CValorDebtDlg::OnEnChangeNameedit()
{
	// TODO:  在此添加控件通知处理程序代码
	m_editname.GetWindowText(m_strname);
}
