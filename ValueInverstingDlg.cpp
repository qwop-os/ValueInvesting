
// ValueInverstingDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ValueInversting.h"
#include "ValueInverstingDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
#include "SettingBalanceSheetDlg.h"
using namespace std;
using json = nlohmann::json;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
extern const std::vector<FieldMeta> g_AllFields; // 全局字段元数据


CValueInverstingDlg::CValueInverstingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VALUEINVERSTING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}




// 辅助函数：安全获取数值（千元）
long long get_safe_value(const json& j, long long default_val = 0) {
	if (j.is_number_integer() || j.is_number_float()) {
		return j.get<long long>();
	}
	return default_val;
}

void CValueInverstingDlg::SetWindowTitle(CString strTitle)
{
	if (strTitle != m_strTitle)
	{
		m_strTitle = strTitle;
		SetWindowText(m_strTitle);
	}
}

// 递归遍历 JSON，更新 m_mpVal 或 m_mpDebt
// 注意：此函数需要访问类的成员变量，因此可以作为类的成员函数，或者通过参数传入引用
void CValueInverstingDlg::traverse_and_update(const json& j, const CString& path) {
	if (j.is_object()) {
		// 特殊处理：如果对象包含 "total" 字段，将其作为数值处理
		if (j.contains("total") && j["total"].is_number()) {
			CString new_path = path + _T(".total");
			traverse_and_update(j["total"], new_path);
			// 继续处理其他字段（如 details）但不重复处理 total
			for (auto&[key, value] : j.items()) {
				CString strKey(key.c_str());
				if (strKey != _T("total") && strKey != _T("_comment") && strKey != _T("_field_comments")) {
					CString new_path2 = path + _T(".") + strKey;
					traverse_and_update(value, new_path2);
				}
			}
			return;
		}
		for (auto&[key, value] : j.items()) {
			CString strKey(key.c_str());
			if (strKey == _T("_comment") || strKey == _T("_field_comments")) continue;
			CString new_path = path.IsEmpty() ? strKey : path + _T(".") + strKey;
			traverse_and_update(value, new_path);
		}
	}
	else if (j.is_number()) {
		long long val_k = get_safe_value(j);
		double val_yi = val_k / 1e8;

		// 解析路径最后一段字段名
		int lastDot = path.ReverseFind('.');
		CString fieldName = (lastDot == -1) ? path : path.Mid(lastDot + 1);
		// 处理数组索引 [n] 后缀
		int bracket = fieldName.Find('[');
		if (bracket != -1)
			fieldName = fieldName.Left(bracket);

		// 判断属于资产还是负债（根据路径前缀）
		if (path.Find("balance_sheet.current_assets") == 0 ||
			path.Find("balance_sheet.non_current_assets") == 0)
		{
			auto it = m_assetFieldToCategory.find(fieldName);
			if (it != m_assetFieldToCategory.end())
				m_mpVal[it->second][fieldName] = val_yi;
		}
		else if (path.Find("balance_sheet.current_liabilities") == 0 ||
			path.Find("balance_sheet.non_current_liabilities") == 0)
		{
			auto it = m_liabilityFieldToCategory.find(fieldName);
			if (it != m_liabilityFieldToCategory.end())
				m_mpDebt[it->second][fieldName] = val_yi;
		}
	}
	else if (j.is_array()) {
		for (size_t i = 0; i < j.size(); ++i) {
			CString array_path;
			array_path.Format(_T("%s[%zu]"), (LPCTSTR)path, i);
			traverse_and_update(j[i], array_path);
		}
	}
	// 其他类型（字符串、布尔等）忽略
}

void CValueInverstingDlg::CalWindowSize()
{
	int iWidth = 1024;
	int iDrawPaillrWidth = g_AllFields.size()*m_iBold;// 柱子需要宽度

	iWidth = m_iLeftStartX + iDrawPaillrWidth ;
	CRect rcClient;
	GetWindowRect(rcClient);
	rcClient.right = rcClient.left + iWidth;
	MoveWindow(rcClient);
}

void CValueInverstingDlg::CalBold()
{
	int iDrawRealPailrCounts = m_config.assetCategories.size() + m_config.liabilityCategories.size();// 2个区间放三个
	CRect rcClient;
	GetWindowRect(rcClient);
	int iDrawWidth = rcClient.Width() - m_iLeftStartX;
	m_iBold = iDrawWidth/ (iDrawRealPailrCounts)-m_iRealWidthPillar;//10 为柱子宽度
	if (m_iBold < 15)
		m_iBold = 15;
}

void CValueInverstingDlg::DefalutConfig()
{
	m_config.assetCategories.clear();
	m_config.liabilityCategories.clear();
	// ==================== 资产类 ====================
	// 现金
	m_config.assetCategories.push_back({ "现金", {"cash", "settlement_reserve", "trading_financial_assets"} });
	// 应收款
	m_config.assetCategories.push_back({ "应收款", {"notes_receivable", "accounts_receivable", "receivables_financing", "other_receivables", "reverse_repurchase_agreements"} });
	// 预付款
	m_config.assetCategories.push_back({ "预付款", {"prepayments"} });
	// 存货
	m_config.assetCategories.push_back({ "存货", {"inventories"} });
	// 其他流动性资产
	m_config.assetCategories.push_back({ "其他流动性资产", {"non_current_assets_due_within_one_year", "other_current_assets"} });
	// 长期投资
	m_config.assetCategories.push_back({ "长期投资", {"debt_investments", "long_term_receivables", "long_term_equity_investments"} });
	// 固定资产
	m_config.assetCategories.push_back({ "固定资产", {"fixed_assets", "construction_in_progress"} });
	// 无形资产 & 商誉
	m_config.assetCategories.push_back({ "无形资产&商誉", {"biological_assets", "right_of_use_assets", "intangible_assets", "goodwill"} });
	// 其他非流动性资产
	m_config.assetCategories.push_back({ "其他非流动性资产", {"long_term_deferred_expenses", "deferred_tax_assets", "other_non_current_assets"} });

	// ==================== 负债类 ====================
	// 短期借款
	m_config.liabilityCategories.push_back({ "短期借款", {"short_term_borrowings", "non_current_liabilities_due_within_one_year"} });
	// 应付款
	m_config.liabilityCategories.push_back({ "应付款", {"notes_payable", "accounts_payable", "other_payables"} });
	// 预收款
	m_config.liabilityCategories.push_back({ "预收款", {"advance_from_customers", "contract_liabilities"} });
	// 薪酬 & 税
	m_config.liabilityCategories.push_back({ "薪酬&税", {"employee_benefits_payable", "taxes_payable"} });
	// 其他流动性负债
	m_config.liabilityCategories.push_back({ "其他流动性负债", {"non_current_liabilities_due_within_one_year", "other"} });
	// 长期借款
	m_config.liabilityCategories.push_back({ "长期借款", {"long_term_borrowings", "bonds_payable"} });
	// 其他非流动性负债
	m_config.liabilityCategories.push_back({ "其他非流动性负债", {"lease_liabilities", "long_term_accounts_payable", "deferred_tax_liabilities", "other_non_current_liabilities"} });
	RebuildFieldMappings();
}

void CValueInverstingDlg::OnSettings()
{
	CSettingBalanceSheetDlg dlg;
	dlg.SetConfig(&m_config);   // 传入配置引用
	if (dlg.DoModal() == IDOK) {
		m_config = dlg.SaveConfig();
		CalBold();// 重新计算config 距离
		RebuildFieldMappings();
		ApplyConfigToMaps();
		InitDebtVal();
		InitMaxMin();
		Invalidate();
	}
}

// 修改后的 ReadJsonFile 函数
int CValueInverstingDlg::ReadJsonFile() {
	CString strfile = m_strTitle + ".json";
	ifstream file(strfile);
	if (!file.is_open()) {
		cerr << "错误：无法打开文件 BydBalanceSheet.json" << endl;
		return 0;
	}

	json data;
	try {
		file >> data;
	}
	catch (const json::parse_error& e) {
		cerr << "JSON 解析错误：" << e.what() << endl;
		return 0;
	}

	try {
		if (!data.contains("balance_sheet")) {
			cerr << "错误：JSON 中缺少 'balance_sheet' 节点" << endl;
			return 0;
		}
		// 从 balance_sheet 开始递归遍历并更新成员变量 m_mpVal / m_mpDebt
		traverse_and_update(data["balance_sheet"], "balance_sheet");
	}
	catch (const json::type_error& e) {
		cerr << "数据类型错误：" << e.what() << endl;
		return 0;
	}
	catch (const json::out_of_range& e) {
		cerr << "键不存在：" << e.what() << endl;
		return 0;
	}
	return 1;
}

void CValueInverstingDlg::InitDebtValVis()
{
	// 后续将m_mpxx 的值合并到m_vNameValue中
	m_mpValVis.clear();
	m_mpDebtVis.clear();
	for (auto&[key1, value1] : m_mpVal)
	{
		for (auto&[key2, value2] : value1)
		{
			if (m_mpValVis.find(key1) == m_mpValVis.end())
			{
				m_mpValVis.insert(std::pair<CString, double>{ key1, value2 });
			}
			else
			{
				m_mpValVis[key1] += value2;
			}
		}
	}
	for (auto&[key1, value1] : m_mpDebt)
	{
		for (auto&[key2, value2] : value1)
		{
			if (m_mpDebtVis.find(key1) == m_mpDebtVis.end())
			{
				m_mpDebtVis.insert(std::pair<CString, double>{ key1, value2 });
			}
			else
			{
				m_mpDebtVis[key1] += value2;
			}
		}
	}
	m_vNameValue.clear();
	// 绘制下标文字和tips保持一致，
	for (auto&[key, value] : m_mpValVis)
	{
		m_vNameValue.push_back({ key,value });
	}

	for (auto&[key, value] : m_mpDebtVis)
	{
		m_vNameValue.push_back({ key,value });
	}
}

// 在 InitDebtVal 中调用
void CValueInverstingDlg::InitDebtVal() {
	ReadJsonFile();
	// 资产和负债的可视化初始化
	InitDebtValVis();
}
// 修改了m_ibold 补
double CValueInverstingDlg::GetValueByPoint(CPoint pt)
{
	int len = m_vNameValue.size();
	int iIndex = 0;
	int iStartX = m_iLeftStartX - m_iBold / 2;
	iIndex = (pt.x - iStartX) / (m_iBold + m_iRealWidthPillar);
	if (pt.x < m_iLeftStartX)
		return m_vNameValue[0].second;

	if (iIndex > len - 1)
		return m_vNameValue[len - 1].second;

	return m_vNameValue[iIndex].second;
}

CString CValueInverstingDlg::GetNameByPoint(CPoint pt)
{
	int len = m_vNameValue.size();
	int iIndex = 0;
	int iStartX = m_iLeftStartX-m_iBold/2;
	iIndex = (pt.x - iStartX) / (m_iBold+m_iRealWidthPillar);
	if (pt.x < m_iLeftStartX)
		return m_vNameValue[0].first;

	if (iIndex > len - 1)
		return m_vNameValue[len - 1].first;

	return m_vNameValue[iIndex].first;
}

void CValueInverstingDlg::InitMaxMin()
{
	m_dbMax = 1.0f;
	m_dbMin = 0.0f;
	for (const auto&pair: m_vNameValue)
	{
		if (pair.second > m_dbMax)
			m_dbMax = pair.second;
		if (pair.second < m_dbMin)
			m_dbMin = pair.second;
	}
}

void CValueInverstingDlg::InitFont(LOGFONT*lpfont)
{
	lpfont->lfHeight = 16;               // 字体高度（逻辑单位）
	lpfont->lfWidth = 0;                  // 宽度（0 表示根据纵横比自动匹配）
	lpfont->lfEscapement = 600;             // 文本行与 x 轴的角度（十分之一度），0 为水平
	lpfont->lfOrientation = 600;            // 字符基线角度（通常与 Escapement 相同）
	lpfont->lfWeight = FW_NORMAL;         // 字体粗细（FW_BOLD 为粗体）
	lpfont->lfItalic = FALSE;              // ★ 关键：设置为 TRUE 表示斜体
	lpfont->lfUnderline = FALSE;
	lpfont->lfStrikeOut = FALSE;
	lpfont->lfCharSet = DEFAULT_CHARSET;
	lpfont->lfOutPrecision = OUT_DEFAULT_PRECIS;
	lpfont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lpfont->lfQuality = DEFAULT_QUALITY;
	lpfont->lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy_s(lpfont->lfFaceName, LF_FACESIZE, _T("宋体")); // 字体名称
}
void CValueInverstingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_comboxDate);
	DDX_Control(pDX, IDC_BUTTON1, m_Query);
	DDX_Control(pDX, IDC_SettingOwner, m_btnCustomSetting);
}

BEGIN_MESSAGE_MAP(CValueInverstingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSELEAVE()
	ON_COMMAND(ID_FILE_OPEN, &CValueInverstingDlg::OnFileOpen)
	ON_BN_CLICKED(IDC_SettingOwner, &CValueInverstingDlg::OnBnClickedSettingowner)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CValueInverstingDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CValueInverstingDlg 消息处理程序

BOOL CValueInverstingDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;  // 禁止默认背景擦除
}

void CValueInverstingDlg::LoadConfig()
{
	std::ifstream file("BalanceSheetConfig.json");
	if (!file.is_open())
	{
		// 使用默认配置（即你代码中硬编码的结构）
		// 此处可调用一个生成默认配置的函数
		return;
	}

	json j;
	file >> j;
	m_config.assetCategories.clear();
	m_config.liabilityCategories.clear();

	auto loadVec = [](const json& arr, std::vector<CategoryItem>& vec) {
		for (auto& item : arr) {
			CategoryItem cat;
			cat.name = item["name"].get<std::string>().c_str();
			for (auto& f : item["fields"])
				cat.fields.push_back(f.get<std::string>().c_str());
			vec.push_back(cat);
		}
	};
	if (j.contains("assetCategories"))
		loadVec(j["assetCategories"], m_config.assetCategories);
	if (j.contains("liabilityCategories"))
		loadVec(j["liabilityCategories"], m_config.liabilityCategories);
}

void CValueInverstingDlg::ApplyConfigToMaps()
{
	m_mpVal.clear();
	m_mpDebt.clear();

	for (auto& cat : m_config.assetCategories) {
		std::map<CString, double> inner;
		for (auto& f : cat.fields)
			inner[f] = 0.0;
		m_mpVal[cat.name] = inner;
	}
	for (auto& cat : m_config.liabilityCategories) {
		std::map<CString, double> inner;
		for (auto& f : cat.fields)
			inner[f] = 0.0;
		m_mpDebt[cat.name] = inner;
	}

	// 如果没有配置（文件缺失），使用默认配置
	if (m_mpVal.empty() && m_mpDebt.empty()) {
		// 此处填充你代码中的默认结构，并同步到 m_config
		// 略...
	}
}

void CValueInverstingDlg::RebuildFieldMappings()
{
    m_assetFieldToCategory.clear();
    m_liabilityFieldToCategory.clear();

    for (auto& cat : m_config.assetCategories) {
        for (auto& f : cat.fields)
            m_assetFieldToCategory[f] = cat.name;
    }
    for (auto& cat : m_config.liabilityCategories) {
        for (auto& f : cat.fields)
            m_liabilityFieldToCategory[f] = cat.name;
    }
}

void CValueInverstingDlg::SaveConfig()
{
	json j;
	auto saveVec = [](json& arr, const std::vector<CategoryItem>& vec) {
		arr = json::array();
		for (auto& cat : vec) {
			json item;
			item["name"] = CT2A(cat.name);
			json fields = json::array();
			for (auto& f : cat.fields)
				fields.push_back(CT2A(f));
			item["fields"] = fields;
			arr.push_back(item);
		}
	};
	saveVec(j["assetCategories"], m_config.assetCategories);
	saveVec(j["liabilityCategories"], m_config.liabilityCategories);

	std::ofstream file("BalanceSheetConfig.json");
	file << j.dump(4);
}

void CValueInverstingDlg::LoadData(CString strFilePath)
{
	// 1. 将所有数值重置为 0.0
	for (auto& outer : m_mpVal)
	{
		for (auto& inner : outer.second)
		{
			inner.second = 0.0;
		}
	}
	for (auto& outer : m_mpDebt)
	{
		for (auto& inner : outer.second)
		{
			inner.second = 0.0;
		}
	}

	// 2. 从 JSON 文件读取并更新数值
	ifstream file(strFilePath);
	if (!file.is_open())
	{
		AfxMessageBox(_T("无法打开文件: ") + strFilePath);
		return;
	}

	json data;
	try
	{
		file >> data;
	}
	catch (const json::parse_error& e)
	{
		CString msg;
		msg.Format(_T("JSON 解析错误: %s"), e.what());
		AfxMessageBox(msg);
		return;
	}

	if (!data.contains("balance_sheet"))
	{
		AfxMessageBox(_T("JSON 中缺少 'balance_sheet' 节点"));
		return;
	}

	traverse_and_update(data["balance_sheet"], _T("balance_sheet"));

	// 3. 重新生成可视化数据并更新最大值/最小值
	// 对m_mpValVis、m_mpDebtVis置零
	for (auto& outer : m_mpValVis)
	{
		outer.second = 0.0;
	}
	for (auto& outer : m_mpDebtVis)
	{
		outer.second = 0.0;
	}

	InitDebtValVis();   // 填充 m_mpValVis, m_mpDebtVis, m_vNameValue
	InitMaxMin();       // 重新计算 m_dbMax, m_dbMin
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetFont());
	CString strCursorContext;
	strCursorContext = "9999.99亿";
	m_iLeftLinePosx = dc.GetTextExtent(strCursorContext).cx;
	strCursorContext.Format("%.2f亿", m_dbMax);
	if(m_iLeftLinePosx< dc.GetTextExtent(strCursorContext).cx)
		m_iLeftLinePosx = dc.GetTextExtent(strCursorContext).cx;
	m_iLeftStartX = m_iLeftLinePosx + 40;
	dc.SelectObject(pOldFont);
	// 4. 刷新窗口
	Invalidate();
	UpdateWindow();
}

void CValueInverstingDlg::OnFileOpen()
{
	CFileDialog dlg(TRUE, _T("json"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("JSON Files (*.json)|*.json|All Files (*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK)
	{
		CString strFilePath = dlg.GetPathName();
		CString strTitle = dlg.GetFileTitle();
		SetWindowTitle(strTitle);
		LoadData(strFilePath);
		// 强制重绘标题栏和菜单栏
		SendMessage(WM_NCACTIVATE, TRUE);
	}
}

BOOL CValueInverstingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();   // 这一行必须存在
	// TODO: 在此添加额外的初始化代码
	if (m_menu.LoadMenu(IDR_MENU1))
	{
		SetMenu(&m_menu);
	}
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetFont());
	CString strCursorContext;
	strCursorContext = "9999.99亿";
	m_iLeftLinePosx = dc.GetTextExtent(strCursorContext).cx;
	strCursorContext.Format("%.2f亿", m_dbMax);
	if (m_iLeftLinePosx < dc.GetTextExtent(strCursorContext).cx)
		m_iLeftLinePosx = dc.GetTextExtent(strCursorContext).cx;
	m_iLeftStartX = m_iLeftLinePosx + 40;
	dc.SelectObject(pOldFont);
	// 初始化combox
	m_comboxDate.AddString("20260331");
	m_comboxDate.AddString("20251231");
	m_comboxDate.SetCurSel(0);
	// 取消编辑框中的文本选中
	m_comboxDate.SetFocus();
	m_comboxDate.SetEditSel(-1, -1);   // 或者 SetEditSel(0, 0)

	if (m_editCode.Create(WS_CHILD | WS_VISIBLE, CRect{ 0,0,1,1 }, this, 100))
	{
		// 限制股票代码编辑框只能输入数字
		m_editCode.ModifyStyle(0, ES_NUMBER);
		m_editCode.SetPlaceholder("请输入股票代码000001");
	}

	SetWindowTitle("BydBalanceSheet2025");
	DefalutConfig();
	CalWindowSize();
	CalBold();
	Relayout();
	// 清空并初始化结构（所有值为 0.0）
	ApplyConfigToMaps();
	InitDebtVal();
	InitMaxMin();

	LOGFONT lf = { 0 };
	InitFont(&lf);
	m_font.CreateFontIndirect(&lf);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CValueInverstingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CValueInverstingDlg::CalTipPos(CDC&dc, CPoint pt)
{
	CString strText;
	double dbVal = GetValueByPoint(pt);
	CString strName = GetNameByPoint(pt);
	strText.Format("%s:%.2f亿", strName, dbVal);
	CSize textSize = dc.GetTextExtent(strText);
	m_strTips = strText;
	int nWidth = textSize.cx;
	int nHeight = textSize.cy;
	CPoint tipPos = pt; // 计算位置
	m_rcCurToolTip.left = pt.x + 5;
	m_rcCurToolTip.top = pt.y + 2;
	m_rcCurToolTip.right = m_rcCurToolTip.left + nWidth + 16;
	m_rcCurToolTip.bottom = m_rcCurToolTip.top + nHeight + 4;
	if (m_rcCurToolTip.bottom > m_rcDraw.bottom)
	{
		m_rcCurToolTip.bottom = pt.y - 2;
		m_rcCurToolTip.top = m_rcCurToolTip.bottom - nHeight - 4;
	}
	if (m_rcCurToolTip.top < m_rcDraw.top)// 不会触发，暂时不处理逻辑
	{

	}
	if (m_rcCurToolTip.right > m_rcDraw.right)
	{
		m_rcCurToolTip.right = pt.x - 5;
		m_rcCurToolTip.left = m_rcCurToolTip.right - nWidth - 16;
	}
	if (m_rcCurToolTip.left < m_rcDraw.left)// 不会触发，暂时不处理逻辑
	{

	}
	CRect rcDirty = m_rcCurToolTip;
	rcDirty.UnionRect(rcDirty, m_rcOldToolTip);
	m_rcOldToolTip = m_rcCurToolTip;
	InvalidateRect(rcDirty, TRUE);
}


void CValueInverstingDlg::CalLeftCursorPos(CDC&dc,CPoint pt)
{
	//填充区域计算
	int iHeightText = dc.GetTextExtent("亿").cy;
	m_rcCurLeftCursor.left = 0;
	m_rcCurLeftCursor.right = m_iLeftLinePosx;
	m_rcCurLeftCursor.top = pt.y - iHeightText / 2 - 1;
	m_rcCurLeftCursor.bottom = pt.y + iHeightText / 2 + 1;
	if (m_rcCurLeftCursor.top < m_rcDraw.top)
	{
		m_rcCurLeftCursor.top = m_rcDraw.top;
		m_rcCurLeftCursor.bottom = m_rcCurLeftCursor.top + iHeightText + 1;
	}
	if (m_rcCurLeftCursor.bottom > m_rcDraw.bottom)
	{
		m_rcCurLeftCursor.bottom = m_rcDraw.bottom;
		m_rcCurLeftCursor.top = m_rcCurLeftCursor.bottom - iHeightText - 1;
	}
	CRect rcDirty = m_rcCurLeftCursor;
	rcDirty.UnionRect(rcDirty, m_rcOldLeftCursor);
	m_rcOldLeftCursor = m_rcCurLeftCursor;

	// 游标内容计算
	double dbPrice = 0.0f;
	double dbMaxPrice = m_dbMax / 8.0f*9.0f;
	int iBottom = m_rcDraw.bottom;
	double dbHeightDraw = m_rcDraw.Height()*1.0f;
	if (dbHeightDraw!=0)
	{
		dbPrice = (iBottom - pt.y) / dbHeightDraw * dbMaxPrice;
	}
	m_strLeftCursor.Format("%.2f亿", dbPrice);
	InvalidateRect(rcDirty, TRUE);
}

void CValueInverstingDlg::GetClrBrush(CPoint pt)
{
	int len1 = m_mpValVis.size();
	int len2 = m_mpDebtVis.size();
	int len =len1+len2;
	int iIndex = 0;
	int iStartX = m_iLeftStartX - m_iBold / 2;
	iIndex = (pt.x - iStartX) / (m_iBold + m_iRealWidthPillar);
	if (pt.x < m_iLeftStartX)
		m_brushCircleClr = RGB(52, 192, 219);
	else if (iIndex > len - 1)
		m_brushCircleClr = RGB(219, 192, 52);
	else
	{
		if(iIndex<len1)
			m_brushCircleClr = RGB(52, 192, 219);
		else
			m_brushCircleClr = RGB(219, 192, 52);
	}
}

void CValueInverstingDlg::DrawLeftCursor(CDC&dc)
{
	int iOldMode = dc.SetBkMode(TRANSPARENT);
	CFont* pOldFont = dc.SelectObject(GetFont());// 对话框字体
	COLORREF clrOld = dc.SetTextColor(RGB(0, 0, 0));
	dc.FillSolidRect(m_rcCurLeftCursor, RGB(255, 0, 0));
	dc.DrawText(m_strLeftCursor, m_rcCurLeftCursor, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	dc.SetTextColor(clrOld);
	dc.SelectObject(pOldFont);
	dc.SetBkMode(iOldMode);
}

void CValueInverstingDlg::DrawLineDot(CDC&dc)
{
	CPen pen(PS_DOT, 1, RGB(110, 112, 121));
	CPen penSolid(PS_SOLID, 1, RGB(110, 112, 121));
	int iOldMode = dc.SetBkMode(TRANSPARENT);
	CFont* pOldFont = dc.SelectObject(GetFont());// 对话框字体
	CPen* pOldPen = dc.SelectObject(&penSolid);
	COLORREF clrOld = dc.SetTextColor(RGB(32, 32, 32));
	int iHeightText = dc.GetTextExtent("亿").cy;
	double dbValPer = m_dbMax / 8.0f;
	int iHeightPer = m_rcDraw.Height() /9;
	int iPoy = m_rcDraw.top;
	int iCountLine = 8;//8根虚线+最上面的一根实线
	double dbLeftLabelPrice = 0.0f;

	CString strLeftPrice;

	CRect rcLeftPrice;
	rcLeftPrice.left = 0;
	rcLeftPrice.right = m_iLeftLinePosx;
	dc.MoveTo(m_iLeftLinePosx, iPoy);
	dc.LineTo(m_rcDraw.right, iPoy);
	dbLeftLabelPrice = dbValPer * 9.0f;
	rcLeftPrice.top = iPoy - iHeightText / 2 - 1;
	rcLeftPrice.bottom = iPoy + iHeightText / 2 + 1;
	strLeftPrice.Format("%.2f亿", dbLeftLabelPrice);
	dc.DrawText(strLeftPrice, rcLeftPrice, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	dc.SelectObject(&pen);
	for (int i = 0; i < iCountLine; i++)
	{
		iPoy += iHeightPer;
		dc.MoveTo(m_iLeftLinePosx, iPoy);
		dc.LineTo(m_rcDraw.right, iPoy);
		rcLeftPrice.top = iPoy - iHeightText / 2 - 1;
		rcLeftPrice.bottom = iPoy + iHeightText / 2 + 1;
		dbLeftLabelPrice = dbValPer * (iCountLine - i);
		strLeftPrice.Format("%.2f亿", dbLeftLabelPrice);
		dc.DrawText(strLeftPrice, rcLeftPrice, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}
	iPoy = m_rcDraw.bottom;
	rcLeftPrice.top = iPoy - iHeightText / 2 - 1;
	rcLeftPrice.bottom = iPoy + iHeightText / 2 + 1;
	strLeftPrice="0.00亿";
	dc.DrawText(strLeftPrice, rcLeftPrice, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	dc.SelectObject(pOldPen);
	dc.SetTextColor(clrOld);
	dc.SelectObject(pOldFont);
	dc.SetBkMode(iOldMode);
}

void CValueInverstingDlg::OnPaint()
{
	CRect rcClient;
	GetClientRect(rcClient);
	CPaintDC dcOrigin(this);
	CDC dc;
	dc.CreateCompatibleDC(&dcOrigin);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dcOrigin, rcClient.Width(),rcClient.Height());
	CBitmap* pOldBmp = dc.SelectObject(&bmp);
	dc.FillSolidRect(rcClient, RGB(255,255,255));
	int iOldMode = dc.SetBkMode(TRANSPARENT);
	CFont* pOldFont = dc.SelectObject(&m_font);
	int iMaxHeightFont = static_cast<int>(dc.GetTextExtent("其他非流动性资产").cx * 1.8/2);
	int iBottom = rcClient.bottom - iMaxHeightFont;
	m_rcDraw = rcClient;
	m_rcDraw.top = m_rcTitle.bottom+16;
	m_rcDraw.left = m_iLeftLinePosx;
	m_rcDraw.bottom = iBottom;

	DrawLineDot(dc);
	DrawLeftCursor(dc);
	CPen pen(PS_SOLID,1,RGB(110,112,121));
	CPen* pOldPen = dc.SelectObject(&pen);
	dc.MoveTo(m_iLeftLinePosx, m_rcDraw.top);
	dc.LineTo(m_iLeftLinePosx, iBottom);
	dc.LineTo(m_rcDraw.right, iBottom);
	dc.LineTo(m_rcDraw.right, m_rcDraw.top);
	int iBold = m_iBold;
	int iLeft = m_iLeftStartX;
	int iMaxHeightPillar = static_cast<int>((iBottom- m_rcDraw.top) * 0.9f);
	int iRealHeightPillar = iMaxHeightPillar;
	int iRealWidthPillar = m_iRealWidthPillar;
	int iWidthX = iBold;
	for (auto it = m_mpValVis.begin(); ;)
	{
		int iHeightY = static_cast<int>(dc.GetTextExtent(it->first).cx *1.73 / 2);
		int iTextWidthX = static_cast<int>(dc.GetTextExtent(it->first).cx / 2);
		iRealHeightPillar = static_cast<int>(iMaxHeightPillar * (it->second / (m_dbMax - m_dbMin)));
		CRect rcPillar{ iLeft,iBottom - iRealHeightPillar,iLeft + iRealWidthPillar,iBottom };
		dc.FillSolidRect(rcPillar,RGB(52, 152, 219));
		dc.TextOut(iLeft - iTextWidthX, iBottom + iHeightY, it->first);
		auto beforeit = it;
		++it;
		if (it == m_mpValVis.end())
		{
			iLeft = rcPillar.right + iWidthX;
			break;
		}
		iLeft = rcPillar.right + iWidthX;
	}
	for (auto it = m_mpDebtVis.begin(); ;)
	{
		int iHeightY = static_cast<int>(dc.GetTextExtent(it->first).cx *1.73 / 2);
		int iTextWidthX = static_cast<int>(dc.GetTextExtent(it->first).cx / 2);
		iRealHeightPillar = static_cast<int>(iMaxHeightPillar * (it->second / (m_dbMax - m_dbMin)));
		CRect rcPillar{ iLeft,iBottom - iRealHeightPillar,iLeft + iRealWidthPillar,iBottom };
		dc.FillSolidRect(rcPillar, RGB(219, 152, 52));
		dc.TextOut(iLeft - iTextWidthX, iBottom + iHeightY, it->first);
		auto beforeit = it;
		++it;
		if (it == m_mpDebtVis.end())
		{
			iLeft = rcPillar.right + iWidthX;
			break;
		}
		iLeft = rcPillar.right + iWidthX;
	}

	// 2. 创建内存 DC 和位图
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp1;
	bmp1.CreateCompatibleBitmap(&dc, m_rcCurToolTip.Width(), m_rcCurToolTip.Height());
	CBitmap* pOldBmp1 = memDC.SelectObject(&bmp1);

	// 3. 在内存 DC 上绘制不透明内容
	memDC.FillSolidRect(0, 0, m_rcCurToolTip.Width(), m_rcCurToolTip.Height(), RGB(255, 255, 255)); // 白色背景
	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(RGB(0, 0, 0));
	CFont* pOldFont2 = memDC.SelectObject(GetFont()); // 使用对话框字体
// 在 memDC 上绘制圆形时使用 GDI+
	{
		Graphics graphics(memDC.GetSafeHdc());
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		if (graphics.GetLastStatus() != Ok)
		{
			TRACE("GDI+ Graphics creation failed\n");
		}
		// 定义圆形区域（在提示框内）
		float fCircleSize = 12.0f;
		float fLeft = 2.0f;
		float fTop = (m_rcCurToolTip.Height() - fCircleSize) / 2.0f;
		RectF rcCircle(fLeft, fTop, fCircleSize, fCircleSize);

		// 填充黑色圆形
		SolidBrush blackBrush(Color(192, GetRValue(m_brushCircleClr), GetGValue(m_brushCircleClr), GetBValue(m_brushCircleClr)));
		graphics.FillEllipse(&blackBrush, rcCircle);
		graphics.Flush();
	}
	memDC.DrawText(m_strTips, CRect(16, 0, m_rcCurToolTip.Width(), m_rcCurToolTip.Height()), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	memDC.SelectObject(pOldFont2);

	// 4. 使用 AlphaBlend 混合到目标 DC
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 192; // 透明度 (0-255), 180 表示约 70% 不透明
	bf.AlphaFormat = 0;            // 不使用每个像素的 Alpha 通道，使用 SourceConstantAlpha
	dc.AlphaBlend(m_rcCurToolTip.left, m_rcCurToolTip.top, m_rcCurToolTip.Width(), m_rcCurToolTip.Height(),
		&memDC, 0, 0, m_rcCurToolTip.Width(), m_rcCurToolTip.Height(), bf);

	// 5. 清理
	memDC.SelectObject(pOldBmp1);

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);
	dc.SetBkMode(iOldMode);
	dcOrigin.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dc, 0, 0, SRCCOPY);
	dc.SelectObject(pOldBmp);
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CValueInverstingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CValueInverstingDlg::OnMouseLeave()
{
	// 重置追踪标志，以便下次鼠标重新进入时再次注册
	m_bTrackingMouse = FALSE;

	// 如果存在提示框，则清除它
	if (!m_rcCurToolTip.IsRectEmpty())
	{
		// 使提示框区域无效，以便重绘时不再显示
		m_rcCurToolTip.SetRectEmpty();
		m_rcOldToolTip.SetRectEmpty();
		m_strTips.Empty();
		// 可选：立即更新窗口
		Invalidate();
	}
	if (!m_rcOldLeftCursor.IsRectEmpty())
	{
		m_rcOldLeftCursor.SetRectEmpty();
		m_rcCurLeftCursor.SetRectEmpty();
		m_strLeftCursor.Empty();
		Invalidate();
	}
	// 可以在此添加其他需要鼠标离开时的处理（如恢复默认状态）

	CDialogEx::OnMouseLeave();
}

void CValueInverstingDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!m_bTrackingMouse)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;  // 通常为 0 或 HOVER_DEFAULT
		::TrackMouseEvent(&tme);
		m_bTrackingMouse = TRUE;
	}
	if (m_rcDraw.PtInRect(point))
	{
		GetClrBrush(point);
		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(GetFont());
		CalLeftCursorPos(dc,point);
		CalTipPos(dc, point);
		dc.SelectObject(pOldFont);
	}
	else
	{
		if (!m_rcOldToolTip.IsRectEmpty())
		{
			m_rcOldToolTip.SetRectEmpty();
			m_rcCurToolTip.SetRectEmpty();
			Invalidate();
		}
		if (!m_rcOldLeftCursor.IsRectEmpty())
		{
			m_rcOldLeftCursor.SetRectEmpty();
			m_rcCurLeftCursor.SetRectEmpty();
			Invalidate();
		}
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CValueInverstingDlg::OnBnClickedSettingowner()
{
	// TODO: 在此添加控件通知处理程序代码
	OnSettings();
}

void CValueInverstingDlg::Relayout()
{
	CRect rcClient;
	GetClientRect(rcClient);
	m_rcTitle = rcClient;
	m_rcTitle.bottom = m_rcTitle.top + 32;
	int iLeft = 0;
	int iTop = 0;
	int iControlHeight = 26;
	if (m_comboxDate.GetSafeHwnd())
	{
		iTop = (m_rcTitle.Height() - iControlHeight) / 2;
		m_comboxDate.MoveWindow(iLeft, iTop, 150, iControlHeight);
		iLeft += 150+ 8;
	}
	if (m_editCode.GetSafeHwnd())
	{
		iTop = (m_rcTitle.Height() - 24) / 2;
		m_editCode.MoveWindow(iLeft, iTop, 150 , 24);
		iLeft += 150 + 8;
	}
	if (m_Query.GetSafeHwnd())
	{
		iTop = (m_rcTitle.Height() - iControlHeight) / 2;
		m_Query.MoveWindow(iLeft, iTop,  60, iControlHeight);
		iLeft += 60 + 8;
	}
	if (m_btnCustomSetting.GetSafeHwnd())
	{
		iTop = (m_rcTitle.Height() - iControlHeight) / 2;
		m_btnCustomSetting.MoveWindow(m_rcTitle.right-158, iTop, 150, iControlHeight);
	}
}

void CValueInverstingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	Relayout();
	Invalidate();
	// TODO: 在此处添加消息处理程序代码
}


void CValueInverstingDlg::OnBnClickedButton1()
{
	m_comboxDate.EnableWindow(FALSE);
	m_editCode.EnableWindow(FALSE);
	m_Query.EnableWindow(FALSE);

	CString strCode;
	m_editCode.GetWindowText(strCode);
	CString strDate;
	m_comboxDate.GetLBText(m_comboxDate.GetCurSel(), strDate);
	if (strCode.GetLength() != 6)
	{
		MessageBox("请重新输入正确股票代码,平安银行代码格式:000001", "提示", MB_OK);
		m_comboxDate.EnableWindow(TRUE);
		m_editCode.EnableWindow(TRUE);
		m_Query.EnableWindow(TRUE);
		return;
	}
	json j = m_conn.Connect(strCode.GetBuffer(), strDate.GetBuffer());
	if (j["balance_sheet"] == nullptr)
	{
		CString strErrInfo;
		strErrInfo.Format("%s时期%s的数据暂时没有,可以点击下载按钮,下载对应格式的json文件。\n将财报投喂给任意Ai模型,生成对应json文件,本地选择文件打开新生成的文件即可", strDate,strCode);
		MessageBox(strErrInfo, "提示", MB_OK);
		m_comboxDate.EnableWindow(TRUE);
		m_editCode.EnableWindow(TRUE);
		m_Query.EnableWindow(TRUE);
		return;
	}
	traverse_and_update(j["balance_sheet"], _T("balance_sheet"));

	// 3. 重新生成可视化数据并更新最大值/最小值
	// 对m_mpValVis、m_mpDebtVis置零
	for (auto& outer : m_mpValVis)
	{
		outer.second = 0.0;
	}
	for (auto& outer : m_mpDebtVis)
	{
		outer.second = 0.0;
	}

	InitDebtValVis();   // 填充 m_mpValVis, m_mpDebtVis, m_vNameValue
	InitMaxMin();       // 重新计算 m_dbMax, m_dbMin
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetFont());
	CString strCursorContext;
	strCursorContext = "9999.99亿";
	m_iLeftLinePosx = dc.GetTextExtent(strCursorContext).cx;
	strCursorContext.Format("%.2f亿", m_dbMax);
	if (m_iLeftLinePosx < dc.GetTextExtent(strCursorContext).cx)
		m_iLeftLinePosx = dc.GetTextExtent(strCursorContext).cx;
	m_iLeftStartX = m_iLeftLinePosx + 40;
	dc.SelectObject(pOldFont);
	// 4. 刷新窗口
	Invalidate();
	UpdateWindow();
	m_comboxDate.EnableWindow(TRUE);
	m_editCode.EnableWindow(TRUE);
	m_Query.EnableWindow(TRUE);

}
