// ipcToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ipcTool.h"
#include "ipcToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CipcToolDlg �Ի���




CipcToolDlg::CipcToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CipcToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CipcToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_RECORD,m_RecordListCtrl);
}

BEGIN_MESSAGE_MAP(CipcToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CipcToolDlg::OnBnClickedButtonSearch)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RECORD, &CipcToolDlg::OnNMClickListRecord)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CipcToolDlg::OnBnClickedButtonModify)
END_MESSAGE_MAP()


// CipcToolDlg ��Ϣ�������

BOOL CipcToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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


	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CComboBox* pLan =(CComboBox* )GetDlgItem(IDC_COMBO_LANGUAGE);
	pLan->ResetContent();
	pLan->InsertString(0,(LPCTSTR)"��������");
	pLan->InsertString(1,(LPCTSTR)"English");
	pLan->SetCurSel(1);

	m_RecordListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_RecordListCtrl.InsertColumn( LIST_COL_NO	, (LPCTSTR)"No", LVCFMT_LEFT, 40 );//������
	m_RecordListCtrl.InsertColumn( LIST_COL_DEVICEID, (LPCTSTR)"DeviceId", LVCFMT_LEFT, 130 );
	m_RecordListCtrl.InsertColumn( LIST_COL_VERSION, (LPCTSTR)"Version", LVCFMT_LEFT, 200 );
	m_RecordListCtrl.InsertColumn( LIST_COL_PRODUCT, (LPCTSTR)"Product", LVCFMT_LEFT, 120 );
	m_RecordListCtrl.InsertColumn( LIST_COL_DEVICETYPE, (LPCTSTR)"ProductType", LVCFMT_LEFT, 120 );
	m_RecordListCtrl.InsertColumn( LIST_COL_DEVICETMODE,(LPCTSTR) "ProductMode", LVCFMT_LEFT, 120 );
	m_RecordListCtrl.InsertColumn( LIST_COL_IPADDRESS, (LPCTSTR)"IPAddress", LVCFMT_LEFT, 120 );//������
	m_RecordListCtrl.InsertColumn( LIST_COL_SUBNETMASK, (LPCTSTR)"Mask", LVCFMT_LEFT, 120 );//������
	m_RecordListCtrl.InsertColumn( LIST_COL_GATEWAY, (LPCTSTR)"Gateway", LVCFMT_LEFT, 120 );
	m_RecordListCtrl.InsertColumn( LIST_COL_MACADDRESS, (LPCTSTR)"Mac", LVCFMT_LEFT, 120 );
	m_RecordListCtrl.InsertColumn( LIST_COL_SERVER, (LPCTSTR)"Server", LVCFMT_LEFT, 120 );
	m_RecordListCtrl.InsertColumn( LIST_COL_PORT, (LPCTSTR)"Port", LVCFMT_LEFT, 120 );
	m_RecordListCtrl.InsertColumn( LIST_COL_SERVERSTATUS, (LPCTSTR)"Status", LVCFMT_LEFT, 120 );
	
	
	GetDlgItem(IDC_EDIT_USER)->SetWindowText("admin");
	GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowText("******");

	WSADATA Ws;
	if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
		return FALSE;
	}
	m_iNetCardNum = 0;
	InitNetWorkSearch();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CipcToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
			//��ʼ��ɨ���õ�UDP�ͻ���	 
			if(m_UdpScanSocket[i] != NULL)
			{
				//���ԭ�������׽����ȹر�
				closesocket(m_UdpScanSocket[i]);
				m_UdpScanSocket[i] = NULL;
			}

			if(m_UdpScanSocket[i] == NULL)
			{
				//�����µ����׽���
				m_UdpScanSocket[i] = socket(AF_INET, SOCK_DGRAM,0);
				ASSERT(m_UdpScanSocket[i] != NULL);
			}
			//���ذ󶨵ĵ�ַ�˿�
			struct sockaddr_in local_addr;
			local_addr.sin_family = AF_INET;
			local_addr.sin_addr.s_addr = inet_addr(psz);
			local_addr.sin_port = htons(PC_TOOL_PORT);
			int addr_len = sizeof(local_addr);
			//�󶨶˿�
			bind(m_UdpScanSocket[i], (struct sockaddr *)&local_addr,
				addr_len);

			int nOn = 1;
			int nRet = setsockopt(m_UdpScanSocket[i],SOL_SOCKET,SO_BROADCAST,(char*)&nOn,sizeof(nOn));//�����͹㲥
			if ( nRet!=0 )
			{
				m_UdpScanSocket[i] = NULL;
				continue;
			}

			m_iNetCardNum ++;

		}

	}

	// ���﷢�����鲥��ַ�͹㲥��ַ����Ϊ���ض������绷���£��㲥���޷������ͳ�ȥ
	//׼������������Ϣ��������Ҫָ���������ĵ�ַ
	m_ServerScanAddr.sin_family = AF_INET;
	m_ServerScanAddr.sin_addr.S_un.S_addr = inet_addr("255.255.255.255");
	m_ServerScanAddr.sin_port = htons(SCAN_SERVER_PORT);

	//�鲥��ַ
	m_ZuboScanAddr.sin_family = AF_INET;
	m_ZuboScanAddr.sin_addr.S_un.S_addr = inet_addr(MULTICAST_IP);
	m_ZuboScanAddr.sin_port = htons(SCAN_SERVER_PORT);
}
int CipcToolDlg::reloveDeviceParamsXml(S_Data &sData,CString &mac)
{
	mac="";
	DeviceParams deviceParams;
	CString result="";
	if(sData.commandName=="1000")
	{
		map<CString,CString>::iterator it;
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
			if(it->first=="port")
				deviceParams.m_port=it->second;
			if(it->first=="serverstatus")
				deviceParams.m_serverStatus=it->second;
		}
		m_deviceParams[mac]=deviceParams;
		return 0;
	}
	else if(sData.commandName=="1002")
	{
		map<CString,CString>::iterator it;
		for (it = sData.params.begin(); it != sData.params.end(); it ++)
		{
			if(it->first=="mac")
				mac=it->second;
			if(it->first=="result")
				result=it->second;
		}
		return 0;
	}
	else
		return -1;
}

void CipcToolDlg::SearchDevice(int v_iNum,char *scanrequestbuf)
{
	fd_set m_fds;
	struct timeval tv;
	char	data[1500];
	int recvnum = 0;
	int	addrlen = sizeof(sockaddr);
	sockaddr_in fromaddr;
	CString strMac = "";

	FD_ZERO(&m_fds);
	FD_SET(m_UdpScanSocket[v_iNum],&m_fds);
	tv.tv_sec = 0; // 1�볬ʱ
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
				//���ж��յ����ǲ�����ȷ������
				if (recvnum < 12)
				{
					continue;
				}
				S_Data sData;
				CString mac;
				CXMLMethod::GetInstance()->Decode((char *)data,&sData);
				reloveDeviceParamsXml(sData,mac);
				//���ж��յ����ǲ�����ȷ������
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
	CString recordNo="";
	CString sindex="";
	map<CString,DeviceParams>::iterator it;
	m_RecordListCtrl.DeleteAllItems();
	for (it = m_deviceParams.begin(); it != m_deviceParams.end(); it ++)
	{
		sindex.Format("%d",i+1);		 
		m_RecordListCtrl.InsertItem(i,sindex);
		recordNo.Format("%d",i);
		CString mac=it->first;
		DeviceParams deviceParams=it->second;

		m_RecordListCtrl.SetItemText(i,LIST_COL_NO,recordNo);
		m_RecordListCtrl.SetItemText(i,LIST_COL_DEVICEID,deviceParams.m_deviceId);
		m_RecordListCtrl.SetItemText(i,LIST_COL_VERSION,deviceParams.m_version);
		m_RecordListCtrl.SetItemText(i,LIST_COL_PRODUCT,deviceParams.m_product);
		m_RecordListCtrl.SetItemText(i,LIST_COL_DEVICETYPE,deviceParams.m_deviceType);
		m_RecordListCtrl.SetItemText(i,LIST_COL_DEVICETMODE,deviceParams.m_deviceMode);
		m_RecordListCtrl.SetItemText(i,LIST_COL_SUBNETMASK,deviceParams.m_subNetMask);
		m_RecordListCtrl.SetItemText(i,LIST_COL_IPADDRESS,deviceParams.m_ipAddress);
		m_RecordListCtrl.SetItemText(i,LIST_COL_GATEWAY,deviceParams.m_gateway);
		m_RecordListCtrl.SetItemText(i,LIST_COL_SERVER,deviceParams.m_server);
		m_RecordListCtrl.SetItemText(i,LIST_COL_PORT,deviceParams.m_port);
		m_RecordListCtrl.SetItemText(i,LIST_COL_MACADDRESS,it->first);
		if(deviceParams.m_serverStatus=="0")
			m_RecordListCtrl.SetItemText(i,LIST_COL_SERVERSTATUS,"���������Ӳ�����");
		else if(deviceParams.m_serverStatus=="1")
			m_RecordListCtrl.SetItemText(i,LIST_COL_SERVERSTATUS,"·�ɷ�������������");
		else if(deviceParams.m_serverStatus=="2")
			m_RecordListCtrl.SetItemText(i,LIST_COL_SERVERSTATUS,"�豸δע��");
		else if(deviceParams.m_serverStatus=="3")
			m_RecordListCtrl.SetItemText(i,LIST_COL_SERVERSTATUS,"�豸��������");
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
				SearchDevice(j,scanrequestbuf);// ����SEARCHTM��
			}
		}
	}
	InsertReocrd();
	UpdateData(FALSE);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CipcToolDlg::OnNMClickListRecord(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)pNMHDR; 

	int nItem = temp-> iItem;//�� 
	int nSubItem = temp-> iSubItem;//�� 
	if(nSubItem == 0 || nSubItem == -1 || nItem == -1) 
		return   ;
	GetDlgItem(IDC_EDIT_DEVID)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_DEVICEID));
	GetDlgItem(IDC_EDIT_MAC)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_MACADDRESS));
	GetDlgItem(IDC_EDIT_SERVER)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_SERVER));
	GetDlgItem(IDC_EDIT_PORT)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_PORT));
	GetDlgItem(IDC_EDIT_VERSION)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_VERSION));
	GetDlgItem(IDC_IPADDRESS_IP)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_IPADDRESS));
	GetDlgItem(IDC_IPADDRESS_MASK)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_SUBNETMASK));
	GetDlgItem(IDC_IPADDRESS_GW)->SetWindowText(m_RecordListCtrl.GetItemText(nItem,LIST_COL_GATEWAY));
	
	UpdateData(FALSE);
	*pResult = 0;
}
void CipcToolDlg::OnBnClickedButtonModify()
{
	S_Data sData;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString mac="";
	CString deviceId="";
	CString server="";
	CString port="";
	GetDlgItem(IDC_EDIT_MAC)->GetWindowText(mac);
	GetDlgItem(IDC_EDIT_DEVID)->GetWindowText(deviceId);
	GetDlgItem(IDC_EDIT_SERVER)->GetWindowText(server);
	GetDlgItem(IDC_EDIT_PORT)->GetWindowText(port);
	
	sData.commandId=100;
	sData.commandName="1002";
	sData.params["mac"]=mac;
	sData.params["deviceId"]=deviceId;
	sData.params["server"]=server;
	sData.params["port"]=port;
	CString scanrequestbuf="";
	CXMLMethod::GetInstance()->Encode(&sData,scanrequestbuf);
	
	m_deviceParams.clear();
	for(int j = 0; j< m_iNetCardNum; j++)
	{
		if(m_UdpScanSocket[j] != NULL)
		{
			for(int k = 0;k< 2; k++)
			{	
				SearchDevice(j,scanrequestbuf.GetBuffer());// ����SEARCHTM��
			}
		}
	}

}
