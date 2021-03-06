// ipcToolDlg.h : 头文件
//
#pragma once
#include <iostream>
#include <CString>
#include <Map>
#include "./Utility/xml/tinyxml/xmlparser.h"



using namespace std;
//显示设备信息列表序列号

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
//#define LIST_COL_PORT			11
#define LIST_COL_SERVERSTATUS	11
#define LIST_COL_HTTPPORT		12
#define LIST_COL_SECRET			13

#define	MAX_SERVER_COUNTS		100
#define MULTICAST_IP 			"245.255.255.250"	//多播组地址
// 网络参数
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



// CipcToolDlg 对话框
class CipcToolDlg : public CDialog
{
// 构造
public:
	CipcToolDlg(CWnd* pParent = NULL);	// 标准构造函数

	void OnNMClickListRecord(NMHDR *pNMHDR, LRESULT *pResult);

// 对话框数据
	enum { IDD = IDD_IPCTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	CComboBox	    m_cbReset;
	DWORD			m_lastTickCount;
	map<std::string,DeviceParams> m_deviceParams;  //macAddress
	int				m_iNetCardNum;

public:
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonVideoset();
	afx_msg void OnBnClickedButtonAlgorithmset();
	afx_msg void OnBnClickedButtonOssset();
	afx_msg void OnBnClickedButtonWeb();
	afx_msg void OnBnClickedButtonSoundset();
	afx_msg void OnBnClickedButtonUrgencymotionset();
};
