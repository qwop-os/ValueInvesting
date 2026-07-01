// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"


 extern const std::vector<FieldMeta> g_AllFields = {
	// 流动资产
	{ "cash", "货币资金", "current_assets", false },
	{ "settlement_reserve", "结算备付金", "current_assets", false },
	{ "trading_financial_assets", "交易性金融资产", "current_assets", false },
	{ "notes_receivable", "应收票据", "current_assets", false },
	{ "accounts_receivable", "应收账款", "current_assets", false },
	{ "receivables_financing", "应收款项融资", "current_assets", false },
	{ "prepayments", "预付款项", "current_assets", false },
	{ "other_receivables", "其他应收款", "current_assets", true },   // total
	{ "inventories", "存货", "current_assets", true },               // total
	{ "non_current_assets_due_within_one_year", "一年内到期的非流动资产", "current_assets", false },
	{ "other_current_assets", "其他流动资产", "current_assets", false },
	// 非流动资产
	{ "debt_investments", "债权投资", "non_current_assets", false },
	{ "long_term_receivables", "长期应收款", "non_current_assets", false },
	{ "long_term_equity_investments", "长期股权投资", "non_current_assets", false },
	{ "fixed_assets", "固定资产", "non_current_assets", false },
	{ "construction_in_progress", "在建工程", "non_current_assets", false },
	{ "biological_assets", "生产性生物资产", "non_current_assets", false },
	{ "right_of_use_assets", "使用权资产", "non_current_assets", false },
	{ "intangible_assets", "无形资产", "non_current_assets", true },
	{ "goodwill", "商誉", "non_current_assets", false },
	{ "long_term_deferred_expenses", "长期待摊费用", "non_current_assets", false },
	{ "deferred_tax_assets", "递延所得税资产", "non_current_assets", false },
	{ "other_non_current_assets", "其他非流动资产", "non_current_assets", false },
	// 流动负债
	{ "short_term_borrowings", "短期借款", "current_liabilities", false },
	{ "notes_payable", "应付票据", "current_liabilities", false },
	{ "accounts_payable", "应付账款", "current_liabilities", false },
	{ "advance_from_customers", "预收款项", "current_liabilities", false },
	{ "contract_liabilities", "合同负债", "current_liabilities", false },
	{ "employee_benefits_payable", "应付职工薪酬", "current_liabilities", false },
	{ "taxes_payable", "应交税费", "current_liabilities", false },
	{ "other_payables", "其他应付款", "current_liabilities", true },
	{ "non_current_liabilities_due_within_one_year", "一年内到期的非流动负债", "current_liabilities", false },
	{ "other_current_liabilities", "其他流动负债", "current_liabilities", false },
	// 非流动负债
	{ "long_term_borrowings", "长期借款", "non_current_liabilities", false },
	{ "bonds_payable", "应付债券", "non_current_liabilities", true },
	{ "lease_liabilities", "租赁负债", "non_current_liabilities", false },
	{ "long_term_accounts_payable", "长期应付款", "non_current_liabilities", false },
	{ "deferred_tax_liabilities", "递延所得税负债", "non_current_liabilities", false },
	{ "other_non_current_liabilities", "其他非流动负债", "non_current_liabilities", false },
};
// 当使用预编译的头时，需要使用此源文件，编译才能成功。
