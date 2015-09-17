#pragma once


// CVideoParam 对话框

class CVideoParam : public CDialog
{
	DECLARE_DYNAMIC(CVideoParam)

public:
	CVideoParam(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVideoParam();

// 对话框数据
	enum { IDD = IDD_VIDEOSET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
