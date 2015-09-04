#pragma once

#include "../Utility/httpClient/SyTcpClient.h"
// COSSConfigParamSet 对话框

class COSSConfigParamSet : public CDialog
{
	DECLARE_DYNAMIC(COSSConfigParamSet)

public:
	COSSConfigParamSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COSSConfigParamSet();
	
	void init(CString strServer,int port)
	{
		m_objTcpClient.InitTcpSocket(strServer, port);
	}
// 对话框数据
	enum { IDD = IDD_DIALOG_OSSCONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CSyTcpClient m_objTcpClient;

	CString		m_szBuctetName;
	CString 	m_szOssEndPoint;
	CString 	m_szAccessKeyId;
	CString 	m_szAccessKeySecret;

	DECLARE_MESSAGE_MAP()
};
