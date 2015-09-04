#pragma once

#include "../Utility/httpClient/SyTcpClient.h"

// CVideoParamSet �Ի���

class CVideoParamSet : public CDialog
{
	DECLARE_DYNAMIC(CVideoParamSet)

public:
	CVideoParamSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CVideoParamSet();
	void init(CString strServer,int port)
	{
		m_objTcpClient.InitTcpSocket(strServer, port);
	}
// �Ի�������
	enum { IDD = IDD_DIALOG_VIDEOSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CSyTcpClient m_objTcpClient;

	CComboBox m_cbResolution;
	CComboBox m_cbFrame;
	CComboBox m_cbCodeType;
	CComboBox m_cbQuality;
	CString		m_csCode;
	CString 	m_csKeyFrame;

	DECLARE_MESSAGE_MAP()
};
