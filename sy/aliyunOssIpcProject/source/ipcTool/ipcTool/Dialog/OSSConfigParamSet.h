#pragma once

#include "../Utility/httpClient/SyTcpClient.h"
// COSSConfigParamSet �Ի���

class COSSConfigParamSet : public CDialog
{
	DECLARE_DYNAMIC(COSSConfigParamSet)

public:
	COSSConfigParamSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COSSConfigParamSet();
	
	void init(CString strServer,int port)
	{
		m_objTcpClient.InitTcpSocket(strServer, port);
	}
// �Ի�������
	enum { IDD = IDD_DIALOG_OSSCONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
