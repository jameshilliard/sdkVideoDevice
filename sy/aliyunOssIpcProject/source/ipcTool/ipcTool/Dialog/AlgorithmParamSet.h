#pragma once

#include "../Utility/httpClient/SyTcpClient.h"

// CAlgorithmParamSet 对话框

class CAlgorithmParamSet : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmParamSet)

public:
	CAlgorithmParamSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAlgorithmParamSet();

	void init(CString strServer,int port)
	{
		m_objTcpClient.InitTcpSocket(strServer, port);
	}
// 对话框数据
	enum { IDD = IDD_DIALOG_ALGORITHMSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CSyTcpClient m_objTcpClient;
	CComboBox	 m_cbEnbale;
	CString		m_csBefRecLastTime;				
	CString		m_csBefRecTimes;		
	CString		m_csConRecLastTime;		
	CString		m_csConRecTimes;	
	CString		m_csEndRecTime;	
	DECLARE_MESSAGE_MAP()
};
