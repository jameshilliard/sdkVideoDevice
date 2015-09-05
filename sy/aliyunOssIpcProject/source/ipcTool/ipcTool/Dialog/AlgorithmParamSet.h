#pragma once

#include "../Utility/httpClient/SyTcpClient.h"

// CAlgorithmParamSet �Ի���

class CAlgorithmParamSet : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithmParamSet)

public:
	CAlgorithmParamSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAlgorithmParamSet();

	void init(CString strServer,int port)
	{
		m_objTcpClient.InitTcpSocket(strServer, port);
	}
// �Ի�������
	enum { IDD = IDD_DIALOG_ALGORITHMSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
