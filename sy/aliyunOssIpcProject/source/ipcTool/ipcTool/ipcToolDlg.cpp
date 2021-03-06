// ipcToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ipcTool.h"
#include "ipcToolDlg.h"

#include "./Dialog/VideoParamSet.h"
#include "./Dialog/AlgorithmParamSet.h"
#include "./Dialog/OSSConfigParamSet.h"
#include "./Dialog/SoundEableSet.h"
#include "./Dialog/UrgencyMotionSet.h"
#include "./Utility/httpClient/SyTcpClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CipcToolDlg 对话框




CipcToolDlg::CipcToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CipcToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_lastTickCount=0;
}

void CipcToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_RECORD,m_RecordListCtrl);
	DDX_Control(pDX,IDC_COMBO_RESET,m_cbReset);
}

BEGIN_MESSAGE_MAP(CipcToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CipcToolDlg::OnBnClickedButtonSearch)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RECORD, &CipcToolDlg::OnNMClickListRecord)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CipcToolDlg::OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_VIDEOSET, &CipcToolDlg::OnBnClickedButtonVideoset)
	ON_BN_CLICKED(IDC_BUTTON_ALGORITHMSET, &CipcToolDlg::OnBnClickedButtonAlgorithmset)
	ON_BN_CLICKED(IDC_BUTTON_OSSSET, &CipcToolDlg::OnBnClickedButtonOssset)
	ON_BN_CLICKED(IDC_BUTTON_SOUNDSET, &CipcToolDlg::OnBnClickedButtonSoundset)
	ON_BN_CLICKED(IDC_BUTTON_URGENCYMOTIONSET, &CipcToolDlg::OnBnClickedButtonUrgencymotionset)
END_MESSAGE_MAP()


// CipcToolDlg 消息处理程序

BOOL CipcToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CComboBox* pLan =(CComboBox* )GetDlgItem(IDC_COMBO_LANGUAGE);
	pLan->ResetContent();
	pLan->InsertString(0,(LPCTSTR)"简体中文");
	pLan->InsertString(1,(LPCTSTR)"English");
	pLan->SetCurSel(1);

	m_RecordListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_RecordListCtrl.InsertColumn( LIST_COL_NO	, (LPCTSTR)"No", LVCFMT_LEFT, 40 );//插入列
	m_RecordListCtrl.InsertColumn( LIST_COL_DEVICEID, (LPCTSTR)"DeviceId", LVCFMT_LEFT, 100 );
	m_RecordListCtrl.InsertColumn( LIST_COL_VERSION, (LPCTSTR)"Version", LVCFMT_LEFT, 100 );
	m_RecordListCtrl.InsertColumn( LIST_COL_PRODUCT, (LPCTSTR)"Product", LVCFMT_LEFT, 100 );
	m_RecordListCtrl.InsertColumn( LIST_COL_DEVICETYPE, (LPCTSTR)"ProductType", LVCFMT_LEFT, 100 );
	m_RecordListCtrl.InsertColumn( LIST_COL_DEVICETMODE,(LPCTSTR) "ProductMode", LVCFMT_LEFT, 100 );
	m_RecordListCtrl.InsertColumn( LIST_COL_IPADDRESS, (LPCTSTR)"IPAddress", LVCFMT_LEFT, 100 );//插入列
	m_RecordListCtrl.InsertColumn( LIST_COL_SUBNETMASK, (LPCTSTR)"Mask", LVCFMT_LEFT, 100 );//插入列
	m_RecordListCtrl.InsertColumn( LIST_COL_GATEWAY, (LPCTSTR)"Gateway", LVCFMT_LEFT, 100 );
	m_RecordListCtrl.InsertColumn( LIST_COL_MACADDRESS, (LPCTSTR)"Mac", LVCFMT_LEFT, 100 );
	m_RecordListCtrl.InsertColumn( LIST_COL_SERVER, (LPCTSTR)"Server", LVCFMT_LEFT, 100 );
	//m_RecordListCtrl.InsertColumn( LIST_COL_PORT, (LPCTSTR)"Port", LVCFMT_LEFT, 100 );
	m_RecordListCtrl.InsertColumn( LIST_COL_SERVERSTATUS, (LPCTSTR)"Status", LVCFMT_LEFT, 100 );
	m_RecordListCtrl.InsertColumn( LIST_COL_HTTPPORT, (LPCTSTR)"HttpPort", LVCFMT_LEFT, 100 );
	m_RecordListCtrl.InsertColumn( LIST_COL_SECRET, (LPCTSTR)"secret", LVCFMT_LEFT, 100 );

	WSADATA Ws;
	if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
		return FALSE;
	}
	m_iNetCardNum = 0;
	InitNetWorkSearch();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

int CipcToolDlg::GetFirstSelect(void)
{
	int nItem;
	int m_nitem=-1;
	POSITION pos = m_RecordListCtrl.GetFirstSelectedItemPosition();
	// 查看是否有设备被选中
	if (pos == NULL)
	{
		AfxMessageBox("请在类表中单击选择要配置的设备");
		return m_nitem;
	}

	m_nitem = m_RecordListCtrl.GetNextSelectedItem(pos);
	while(pos)
	{
		nItem = m_RecordListCtrl.GetNextSelectedItem(pos);
		m_RecordListCtrl.SetItemState(nItem, 0, 0xFFFFFFFF);
	}

	return m_nitem;
}

void CipcToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CipcToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CipcToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CipcToolDlg::InitNetWorkSearch()
{
	char szHostName[128];
	if( gethostname(szHostName, 128) == 0 )
	{
		// Get host adresses
		struct hostent * pHost;	
		int i; 	
		pHost = gethostbyname(szHostName); 
		for( i = 0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++ ) 	
		{
			LPCSTR psz = inet_ntoa(*(struct in_addr *)pHost->h_addr_list[i]);
			//AfxMessageBox(psz);
			//初始化扫描用的UDP客户端	 
			if(m_UdpScanSocket[i] != NULL)
			{
				//如果原来打开这套接字先关闭
				closesocket(m_UdpScanSocket[i]);
				m_UdpScanSocket[i] = NULL;
			}

			if(m_UdpScanSocket[i] == NULL)
			{
				//创建新的流套接字
				m_UdpScanSocket[i] = socket(AF_INET, SOCK_DGRAM,0);
				ASSERT(m_UdpScanSocket[i] != NULL);
			}
			//本地绑定的地址端口
			struct sockaddr_in local_addr;
			local_addr.sin_family = AF_INET;
			local_addr.sin_addr.s_addr = inet_addr(psz);
			local_addr.sin_port = htons(PC_TOOL_PORT);
			int addr_len = sizeof(local_addr);
			//绑定端口
			bind(m_UdpScanSocket[i], (struct sockaddr *)&local_addr,
				addr_len);

			int nOn = 1;
			int nRet = setsockopt(m_UdpScanSocket[i],SOL_SOCKET,SO_BROADCAST,(char*)&nOn,sizeof(nOn));//允许发送广播
			if ( nRet!=0 )
			{
				m_UdpScanSocket[i] = NULL;
				continue;
			}

			m_iNetCardNum ++;

		}

	}

	// 这里发送了组播地址和广播地址，因为在特定的网络环境下，广播包无法被发送出去
	//准备服务器的信息，这里需要指定服务器的地址
	m_ServerScanAddr.sin_family = AF_INET;
	m_ServerScanAddr.sin_addr.S_un.S_addr = inet_addr("255.255.255.255");
	m_ServerScanAddr.sin_port = htons(SCAN_SERVER_PORT);

	//组播地址
	m_ZuboScanAddr.sin_family = AF_INET;
	m_ZuboScanAddr.sin_addr.S_un.S_addr = inet_addr(MULTICAST_IP);
	m_ZuboScanAddr.sin_port = htons(SCAN_SERVER_PORT);
}
int CipcToolDlg::reloveDeviceParamsXml(S_Data &sData,std::string &mac)
{
	mac="";
	DeviceParams deviceParams;
	std::string result="";
	if(sData.commandName=="1000")
	{
		map<string,string>::iterator it;
		for (it = sData.params.begin(); it != sData.params.end(); it ++)
		{
			if(it->first=="mac")
				mac=it->second;
			if(it->first=="version")
				deviceParams.m_version=it->second;
			if(it->first=="ip")
				deviceParams.m_ipAddress=it->second;
			if(it->first=="gateway")
				deviceParams.m_gateway=it->second;
			if(it->first=="ipmask")
				deviceParams.m_subNetMask=it->second;
			if(it->first=="devicetype") 
				deviceParams.m_deviceType=it->second;
			if(it->first=="deviceproduct")
				deviceParams.m_product=it->second;
			if(it->first=="devicemode")
				deviceParams.m_deviceMode=it->second;
			if(it->first=="deviceno")
				deviceParams.m_deviceId=it->second;
			if(it->first=="server")
				deviceParams.m_server=it->second;
			//if(it->first=="serverport")
			//	deviceParams.m_port=it->second;
			if(it->first=="httpport")
				deviceParams.m_httpPort=it->second;
			if(it->first=="serverstatus")
				deviceParams.m_serverStatus=it->second;
			if(it->first=="secret")
				deviceParams.m_secret=it->second;
		}
		m_deviceParams[mac]=deviceParams;
		return 0;
	}
	else if(sData.commandName=="1002")
	{
		map<string,string>::iterator it;
		for (it = sData.params.begin(); it != sData.params.end(); it ++)
		{
			if(it->first=="mac")
				mac=it->second;
			if(it->first=="result")
				result=it->second;
		}
		DWORD nowTickCount=GetTickCount();
		if(nowTickCount-m_lastTickCount>3*1000)
		{
			m_lastTickCount=nowTickCount;
			if(result=="10000")
			{
				AfxMessageBox("更新成功");
			}
			else
			{
				AfxMessageBox("更新失败");
			}	
		}
		return 0;
	}
	else
		return -1;
}

void CipcToolDlg::SearchDevice(int v_iNum,const char *scanrequestbuf)
{
	fd_set m_fds;
	struct timeval tv;
	char	data[1500];
	int recvnum = 0;
	int	addrlen = sizeof(sockaddr);
	sockaddr_in fromaddr;
	std::string strMac = "";

	FD_ZERO(&m_fds);
	FD_SET(m_UdpScanSocket[v_iNum],&m_fds);
	tv.tv_sec = 0; // 1秒超时
	tv.tv_usec = 500000;
	sendto(m_UdpScanSocket[v_iNum], scanrequestbuf, strlen(scanrequestbuf)+1, 
		0, (struct sockaddr *)&m_ServerScanAddr, addrlen);
	sendto(m_UdpScanSocket[v_iNum], scanrequestbuf, strlen(scanrequestbuf)+1,
		0, (struct sockaddr *)&m_ZuboScanAddr, addrlen);
	
	while(1)
	{
		FD_ZERO(&m_fds);
		FD_SET(m_UdpScanSocket[v_iNum],&m_fds);
		if(select(m_UdpScanSocket[v_iNum]+1,&m_fds,NULL,NULL,&tv))
		{
			if(FD_ISSET(m_UdpScanSocket[v_iNum],&m_fds))
			{
				memset(data,0,sizeof(data));
				recvnum = recvfrom(m_UdpScanSocket[v_iNum],(char *)data,1500,0,(struct sockaddr *)&fromaddr,&addrlen);
				//得判断收到的是不是正确的数据
				if (recvnum < 12)
				{
					continue;
				}
				S_Data sData;
				std::string mac;
				xmlparser::Decode((char *)data,&sData);
				reloveDeviceParamsXml(sData,mac);
				//得判断收到的是不是正确的数据
			}
		}
		else	//set servers information to list
		{
			break;
		}
	}
}

void CipcToolDlg::InsertReocrd()
{
	int i=0;
	std::string recordNo="";
	std::string sindex="";
	char buffer[100]="";
	map<std::string,DeviceParams>::iterator it;
	m_RecordListCtrl.DeleteAllItems();
	for (it = m_deviceParams.begin(); it != m_deviceParams.end(); it ++)
	{
		memset(buffer,0,sizeof(buffer));
		sprintf_s(buffer,"%d",(i+1));
		sindex=buffer;		 
		m_RecordListCtrl.InsertItem(i,sindex.c_str());
		recordNo=sindex;
		std::string mac=it->first;
		DeviceParams deviceParams=it->second;

		m_RecordListCtrl.SetItemText(i,LIST_COL_NO,recordNo.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_DEVICEID,deviceParams.m_deviceId.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_VERSION,deviceParams.m_version.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_PRODUCT,deviceParams.m_product.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_DEVICETYPE,deviceParams.m_deviceType.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_DEVICETMODE,deviceParams.m_deviceMode.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_SUBNETMASK,deviceParams.m_subNetMask.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_IPADDRESS,deviceParams.m_ipAddress.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_GATEWAY,deviceParams.m_gateway.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_SERVER,deviceParams.m_server.c_str());
		//m_RecordListCtrl.SetItemText(i,LIST_COL_PORT,deviceParams.m_port.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_HTTPPORT,deviceParams.m_httpPort.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_SECRET,deviceParams.m_secret.c_str());
		m_RecordListCtrl.SetItemText(i,LIST_COL_MACADDRESS,it->first.c_str());
		//0:登录失败 1：登录成功，2：uid不存在，3:pwd错误，4：该uid已经登陆，不能重复登陆。
		if(deviceParams.m_serverStatus=="0")
			m_RecordListCtrl.SetItemText(i,LIST_COL_SERVERSTATUS,"服务器连接不正常");
		else if(deviceParams.m_serverStatus=="1")
			m_RecordListCtrl.SetItemText(i,LIST_COL_SERVERSTATUS,"路由服务器登录成功");
		else if(deviceParams.m_serverStatus=="2")
			m_RecordListCtrl.SetItemText(i,LIST_COL_SERVERSTATUS,"设备uid不存在");
		else if(deviceParams.m_serverStatus=="3")
			m_RecordListCtrl.SetItemText(i,LIST_COL_SERVERSTATUS,"设备pwd错误");
		else if(deviceParams.m_serverStatus=="4")
			m_RecordListCtrl.SetItemText(i,LIST_COL_SERVERSTATUS,"该uid已经登陆，不能重复登陆");
		i++;
	}
}
void CipcToolDlg::OnBnClickedButtonSearch()
{
	char scanrequestbuf[] = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><devicecmd><cmdid>99</cmdid><command>1000</command></devicecmd>";
	m_deviceParams.clear();
	for(int j = 0; j< m_iNetCardNum; j++)
	{
		if(m_UdpScanSocket[j] != NULL)
		{
			for(int k = 0;k< 2; k++)
			{	
				SearchDevice(j,scanrequestbuf);// 搜索SEARCHTM次
			}
		}
	}
	InsertReocrd();     
	UpdateData(FALSE);
	AfxMessageBox("搜索完成……");
	// TODO: 在此添加控件通知处理程序代码
}

void CipcToolDlg::OnNMClickListRecord(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR; 

	int nItem = temp-> iItem;//行 
	int nSubItem = temp-> iSubItem;//列 
	if(nSubItem == 0 || nSubItem == -1 || nItem == -1) 
		return   ;
	GetDlgItem(IDC_IPADDRESS_IP)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS_MASK)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS_GW)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_DEVID)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_DEVICEID));
	GetDlgItem(IDC_EDIT_MAC)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_MACADDRESS));
	GetDlgItem(IDC_EDIT_SERVER)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_SERVER));
	//GetDlgItem(IDC_EDIT_PORT)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_PORT));
	GetDlgItem(IDC_EDIT_SECRET)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_SECRET));
	GetDlgItem(IDC_IPADDRESS_IP)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_IPADDRESS));
	GetDlgItem(IDC_IPADDRESS_MASK)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_SUBNETMASK));
	GetDlgItem(IDC_IPADDRESS_GW)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_GATEWAY));
	m_cbReset.AddString("否");
	m_cbReset.AddString("是");

	UpdateData(FALSE);
	*pResult = 0;
}
void CipcToolDlg::OnBnClickedButtonModify()
{
	S_Data sData;
	// TODO: 在此添加控件通知处理程序代码
	
	CString deviceId="";
	CString secret="";
	CString serverIp="";
	CString serverPort="";
	CString mac="";

	GetDlgItem(IDC_EDIT_MAC)->GetWindowText(mac);
	GetDlgItem(IDC_EDIT_DEVID)->GetWindowText(deviceId);
	GetDlgItem(IDC_EDIT_SECRET)->GetWindowText(secret);
	GetDlgItem(IDC_EDIT_SERVER)->GetWindowText(serverIp);
	//GetDlgItem(IDC_EDIT_PORT)->GetWindowText(serverPort);

	sData.commandId=100;
	sData.commandName="1002";
	sData.type=CONNETTYPE;
	sData.params["mac"]=mac.GetBuffer();
	sData.params["server"]=serverIp.GetBuffer();
	//sData.params["port"]=serverPort.GetBuffer();
	sData.params["secret"]=secret.GetBuffer();
	sData.params["id"]=deviceId.GetBuffer();
	CString mReset;
	m_cbReset.GetWindowText(mReset);
	sData.params["reset"]=(mReset=="是"?"1":"0");

	std::string xmlString="";

	xmlparser::Encode(&sData,xmlString);
	for(int j = 0; j< m_iNetCardNum; j++)
	{
		if(m_UdpScanSocket[j] != NULL)
		{
			for(int k = 0;k< 2; k++)
			{	
				if(xmlString!="")
					SearchDevice(j,xmlString.c_str());// 搜索SEARCHTM次
			}
		}
	}
}

void CipcToolDlg::OnBnClickedButtonVideoset()
{
	// TODO: 在此添加控件通知处理程序代码
	int nItem=GetFirstSelect();
	if(-1==nItem)
	{
		return;
	}
	CString ip = m_RecordListCtrl.GetItemText(nItem,LIST_COL_IPADDRESS);
	int port =  SERVER_PORT;
	CVideoParamSet *mVideoParamSet=new CVideoParamSet();
	mVideoParamSet->init(ip,port);
	mVideoParamSet->DoModal();
	delete(mVideoParamSet);
}

void CipcToolDlg::OnBnClickedButtonAlgorithmset()
{
	// TODO: 在此添加控件通知处理程序代码
	int nItem=GetFirstSelect();
	if(-1==nItem)
	{
		return;
	}
	CString ip = m_RecordListCtrl.GetItemText(nItem,LIST_COL_IPADDRESS);
	int port =  SERVER_PORT;
	CAlgorithmParamSet *mAlgorithmParamSet=new CAlgorithmParamSet();
	mAlgorithmParamSet->init(ip,port);
	mAlgorithmParamSet->DoModal();
	delete(mAlgorithmParamSet);
}

void CipcToolDlg::OnBnClickedButtonOssset()
{
	
	// TODO: 在此添加控件通知处理程序代码
	int nItem=GetFirstSelect();
	if(-1==nItem)
	{
		return;
	}
	CString ip = m_RecordListCtrl.GetItemText(nItem,LIST_COL_IPADDRESS);
	int port =  SERVER_PORT;
	COSSConfigParamSet *mOSSConfigParamSet=new COSSConfigParamSet();
	mOSSConfigParamSet->init(ip,port);
	mOSSConfigParamSet->DoModal();
	delete(mOSSConfigParamSet);
}

void CipcToolDlg::OnBnClickedButtonSoundset()
{
	// TODO: 在此添加控件通知处理程序代码
	int nItem=GetFirstSelect();
	if(-1==nItem)
	{
		return;
	}
	CString ip = m_RecordListCtrl.GetItemText(nItem,LIST_COL_IPADDRESS);
	int port =  SERVER_PORT;
	CSoundEableSet *mCSoundEableSet=new CSoundEableSet();
	mCSoundEableSet->init(ip,port);
	mCSoundEableSet->DoModal();
	delete(mCSoundEableSet);
}

void CipcToolDlg::OnBnClickedButtonUrgencymotionset()
{
	// TODO: 在此添加控件通知处理程序代码
	int nItem=GetFirstSelect();
	if(-1==nItem)
	{
		return;
	}
	CString ip = m_RecordListCtrl.GetItemText(nItem,LIST_COL_IPADDRESS);
	int port =  SERVER_PORT;
	CUrgencyMotionSet *mUrgencyMotionSet=new CUrgencyMotionSet();
	mUrgencyMotionSet->init(ip,port);
	mUrgencyMotionSet->DoModal();
	delete(mUrgencyMotionSet);
}
