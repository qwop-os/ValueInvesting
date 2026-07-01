// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#include <gdiplus.h>
#include <afxcontrolbars.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
#include <vector>
#include <afxcontrolbars.h>
// 单个一级类别配置
struct CategoryItem {
	CString name;                   // 一级显示名称（用户自定义）
	std::vector<CString> fields;    // 包含的二级字段键名（如 "cash", "accounts_receivable"）
};

// 整体配置
struct BalanceSheetConfig {
	std::vector<CategoryItem> assetCategories;      // 资产类（流动资产+非流动资产）
	std::vector<CategoryItem> liabilityCategories;  // 负债类（流动负债+非流动负债）
};

struct FieldMeta {
	CString key;            // JSON 中的字段名
	CString display;        // 中文显示名称
	CString section;        // 所属节："current_assets", "non_current_assets", "current_liabilities", "non_current_liabilities"
	bool isTotalObject;     // 是否为嵌套对象（需要取 total）
};
#endif //PCH_H
