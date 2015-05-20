// ipcToolDlg.h : 头文件
//
#pragma once
#include <iostream>
#include <CString>
#include <Map>
#include "./Utility/xml/XMLMethod.h"

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
#define LIST_COL_PORT			11

#define	MAX_SERVER_COUNTS		100
#define MULTICAST_IP 			"245.255.255.250"	//多播组地址
// 网络参数
#define PC_TOOL_PORT 			60011
#define	SCAN_SERVER_PORT		60012


typedef struct DeviceParams_
{
	CString m_deviceId;
	CString m_version;
	CString m_product;
	CString m_deviceType;
	CString m_deviceMode;
	CString m_ipAddress;
	CString m_subNetMask;
	CString m_gateway;
	CString m_server;
	CString m_port;
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
	void SearchDevice(int v_iNum,char *scanrequestbuf);
	void InitNetWorkSearch();
	int  reloveDeviceParamsXml(S_Data &sData,CString &mac);
	void InsertReocrd();
	

private:
	CListCtrl		m_RecordListCtrl;
	SOCKET			m_UdpScanSocket[10];
	sockaddr_in		m_ServerScanAddr;
	sockaddr_in		m_ZuboScanAddr;
	map<CString,DeviceParams> m_deviceParams;  //macAddress
	int				m_iNetCardNum;

public:
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonModify();
};
