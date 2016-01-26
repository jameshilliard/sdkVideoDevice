#pragma once

#include "../Utility/httpClient/SyTcpClient.h"

// CSoundEableSet �Ի���

class CSoundEableSet : public CDialog
{
	DECLARE_DYNAMIC(CSoundEableSet)

public:
	CSoundEableSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSoundEableSet();
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

	CComboBox	 m_cbSoundStartUpEnable;
	CComboBox	 m_cbSoundServerSuccessEnable;
	CComboBox	 m_cbSoundServerFailureEnable;
	CComboBox	 m_cbSoundUpgradeEnable;
	CComboBox	 m_cbSoundLoginInEnable;
	CComboBox	 m_cbSoundUrgencyStartEnable;
	CComboBox	 m_cbSoundUrgencyOverEnable;
	CComboBox	 m_cbSoundEnable;

	DECLARE_MESSAGE_MAP()
};
