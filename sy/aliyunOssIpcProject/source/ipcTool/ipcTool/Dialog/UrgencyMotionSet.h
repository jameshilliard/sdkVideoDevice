#pragma once

#include "../Utility/httpClient/SyTcpClient.h"

// CUrgencyMotionSet 对话框

class CUrgencyMotionSet : public CDialog
{
	DECLARE_DYNAMIC(CUrgencyMotionSet)

public:
	CUrgencyMotionSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUrgencyMotionSet();
	void init(CString strServer,int port)
	{
		m_objTcpClient.InitTcpSocket(strServer, port);
	}
// 对话框数据
	enum { IDD = IDD_DIALOG_URGENCYMOTIONSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CSyTcpClient m_objTcpClient;

	CComboBox	m_cbUrgEnable;
	CString		m_csUrgStartPeriod;				
	CString		m_csUrgStartSumDetect;		
	CString		m_csUrgStartSumArea;		
	CString		m_csUrgStartSoundSize;

	CString		m_csUrgOverPeriod;				
	CString		m_csUrgOverSumDetect;		
	CString		m_csUrgOverSumArea;		
	CString		m_csUrgOverSoundSize;
	CString		m_csUrgEndRecTime;

	DECLARE_MESSAGE_MAP()
};
