
// ValueInverstingDlg.h: 头文件
//

#pragma once

#include<map>
#include<vector>
#include<set>
#include <fstream>
#include <string>
#include "json.hpp"


using namespace std;
using json = nlohmann::json;
using std::set;
using std::map;
using std::vector;
// CValueInverstingDlg 对话框
class CValueInverstingDlg : public CDialogEx
{
// 构造
public:
	CValueInverstingDlg(CWnd* pParent = nullptr);	// 标准构造函数
	void InitDebtVal();
	void InitDebtValVis();
	void InitFont(LOGFONT*lpfont);
	void InitMaxMin();
	double GetValueByPoint(CPoint pt);
	CString GetNameByPoint(CPoint pt);
	int ReadJsonFile();
	void traverse_and_update(const json& j, const CString& path);
	void SetWindowTitle(CString strTitle);
	void LoadData(CString strFilePath); // 从指定 JSON 文件加载数据并刷新界面
	void DrawLineDot(CDC&dc);
	void DrawLeftCursor(CDC&dc);
	void GetClrBrush(CPoint pt);
	void CalLeftCursorPos(CDC&dc,CPoint pt);
	void CalTipPos(CDC&dc, CPoint pt);
	// ValueInverstingDlg.h 类内添加
	void LoadConfig();          // 从文件加载配置，若不存在则生成默认配置
	void SaveConfig();          // 保存配置到文件
	void ApplyConfigToMaps();   // 根据配置重建 m_mpVal / m_mpDebt 结构
	void RebuildFieldMappings();
	void OnSettings();
	void DefalutConfig();		// 初始化m_config默认值
	void Relayout();
	void CalWindowSize();
	void CalBold();
public:
	BalanceSheetConfig m_config;
	std::map<CString, CString> m_assetFieldToCategory;
	std::map<CString, CString> m_liabilityFieldToCategory;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VALUEINVERSTING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFileOpen();          // 响应“打开文件”命令
	DECLARE_MESSAGE_MAP()
private:
	map<CString,map<CString,double>>m_mpDebt;
	map<CString, map<CString, double>>m_mpVal;
	map<CString, double>m_mpValVis;// 显示mapval
	map<CString, double>m_mpDebtVis;//
	vector<std::pair<CString, double>>m_vNameValue;
	CFont m_font;
	double m_dbMax{ 1.0f };
	double m_dbMin{0.0f};
	CRect m_rcDraw;
	CRect m_rcCurToolTip;   // 当前提示框区域
	CRect m_rcOldToolTip;   // 上一帧提示框区域（用于计算脏矩形）
	CRect m_rcCurLeftCursor;// 左侧当前cursor区域。
	CRect m_rcOldLeftCursor;
	int m_iRealWidthPillar{ 10 };
	int m_iLeftStartX{ 40 };
	int m_iBold{ 25 };
	CString m_strTitle;
	CString m_strTips;
	CString m_strLeftCursor;
	BOOL m_bTrackingMouse{FALSE};
	CMenu m_menu;                       // 菜单对象
	int m_iLeftLinePosx;				//左侧竖线位置
	COLORREF m_brushCircleClr;			//tip 圆圈
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnBnClickedSettingowner();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
