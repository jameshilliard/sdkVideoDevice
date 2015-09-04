// ipcToolDlg.h : ͷ�ļ�
//
#pragma once
#include <iostream>
#include <CString>
#include <Map>
#include "./Utility/xml/tinyxml/xmlparser.h"
#include "./Utility/httpClient/httpClient.h"




using namespace std;
//��ʾ�豸��Ϣ�б����к�

#define LIST_COL_NO				0
#define LIST_COL_DEVICEID		1
#define LIST_COL_VERSION		2
#define LIST_COL_PRODUCT		3
#define LIST_COL_DEVICETYPE		4
#define LIST_COL_DEVICETMODE	5
#define LIST_COL_IPADDRESS		6
#define LIST_COL_SUBNETMASK		7
#define LIST_COL_GATEWAY		8
#define LIST_COL_MACADDRESS		9
#define LIST_COL_SERVER			10
#define LIST_COL_PORT			11
#define LIST_COL_SERVERSTATUS	12
#define LIST_COL_HTTPPORT		13
#define LIST_COL_SECRET			14

#define	MAX_SERVER_COUNTS		100
#define MULTICAST_IP 			"245.255.255.250"	//�ಥ���ַ
// �������
#define PC_TOOL_PORT 			60011
#define	SCAN_SERVER_PORT		60012


typedef struct DeviceParams_
{
	std::string m_deviceId;
	std::string m_version;
	std::string m_product;
	std::string m_deviceType;
	std::string m_deviceMode;
	std::string m_ipAddress;
	std::string m_subNetMask;
	std::string m_gateway;
	std::string m_server;
	std::string m_port;
	std::string m_serverStatus;
	std::string m_httpPort;
	std::string m_secret;
	DeviceParams_()
	{
		m_deviceId="";
		m_version="";
		m_product="";
		m_deviceType="";
		m_deviceMode="";
		m_ipAddress="";
		m_subNetMask="";
		m_gateway="";
		m_server="";
		m_port="";
		m_serverStatus="";
		m_httpPort="";
		 m_secret="";
	}
}DeviceParams;



// CipcToolDlg �Ի���
class CipcToolDlg : public CDialog
{
// ����
public:
	CipcToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

	void OnNMClickListRecord(NMHDR *pNMHDR, LRESULT *pResult);

// �Ի�������
	enum { IDD = IDD_IPCTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	void SearchDevice(int v_iNum,const char *scanrequestbuf);
	void InitNetWorkSearch();
	int  reloveDeviceParamsXml(S_Data &sData,std::string &mac);
	void InsertReocrd();
	int  GetFirstSelect(void);

private:
	CListCtrl		m_RecordListCtrl;
	SOCKET			m_UdpScanSocket[10];
	sockaddr_in		m_ServerScanAddr;
	sockaddr_in		m_ZuboScanAddr;
	map<std::string,DeviceParams> m_deviceParams;  //macAddress
	int				m_iNetCardNum;

public:
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonVideoset();
	afx_msg void OnBnClickedButtonAlgorithmset();
	afx_msg void OnBnClickedButtonOssset();
};
