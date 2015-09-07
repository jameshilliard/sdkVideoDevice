// VideoParamSet.cpp : ʵ���ļ�
//

#include "../stdafx.h"
#include "../ipcTool.h"
#include "VideoParamSet.h"


// CVideoParamSet �Ի���

IMPLEMENT_DYNAMIC(CVideoParamSet, CDialog)

CVideoParamSet::CVideoParamSet(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoParamSet::IDD, pParent)
{

}

CVideoParamSet::~CVideoParamSet()
{
}

void CVideoParamSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CODE,m_csCode);
	DDX_Text(pDX, IDC_EDIT_KEYFRAME,m_csKeyFrame);
	DDX_Control(pDX, IDC_COMBO_RESOLUTION,m_cbResolution);
	DDX_Control(pDX, IDC_COMBO_CODETYPE,m_cbCodeType);
	DDX_Control(pDX, IDC_COMBO_QUALITY,m_cbQuality);
	DDX_Control(pDX, IDC_COMBO_FRAME,m_cbFrame);
}


BEGIN_MESSAGE_MAP(CVideoParamSet, CDialog)
	ON_BN_CLICKED(IDOK, &CVideoParamSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CVideoParamSet ��Ϣ�������


BOOL CVideoParamSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	/* ����ģʽ��HI_TRUEΪ�̶�������HI_FALSEΪ�ɱ����� */
	m_cbCodeType.AddString("�ɱ�����");
	m_cbCodeType.AddString("�̶�����");
	int i=1;
	char buffer[16]={0};
	for(i=1;i<26;i++)
	{
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"%d",i);
		m_cbFrame.AddString(buffer);
	}
	m_cbResolution.AddString("1280X960");
	m_cbResolution.AddString("1280X720");
	//m_cbResolution.AddString("640X480");
	//m_cbResolution.AddString("640X352");

	for(i=1;i<7;i++)
	{
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"%d",i);
		m_cbQuality.AddString(buffer);
	}
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;

	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4004);
	//if(bRet)
	{
		int iRet=m_cbResolution.SelectString(0,mapofResult["Resolution"]);
		iRet=m_cbQuality.SelectString(0,mapofResult["Quality"]);
		iRet=m_cbCodeType.SelectString(0,((mapofResult["CodeType"]=="0")?"�ɱ�����":"�̶�����"));
		iRet=m_cbFrame.SelectString(0,mapofResult["Frame"]);
		m_csCode=mapofResult["Code"];
		m_csKeyFrame=mapofResult["KeyFrame"];
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
// CAlgorithmParamSet ��Ϣ�������
void CVideoParamSet::OnBnClickedOk()
{
	UpdateData(TRUE);
	CString codeType;
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;
	/* ����ģʽ��HI_TRUEΪ�̶�������HI_FALSEΪ�ɱ����� */
	m_cbResolution.GetWindowText(mapofparam["Resolution"]);
	m_cbQuality.GetWindowText(mapofparam["Quality"]);
	m_cbCodeType.GetWindowText(codeType);
	mapofparam["CodeType"]=(codeType=="�̶�����"?"1":"0");
	m_cbFrame.GetWindowText(mapofparam["Frame"]);
	mapofparam["Code"]=m_csCode;
	mapofparam["KeyFrame"]=m_csKeyFrame;

	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4005);
	if(bRet)
	{
		AfxMessageBox("���³ɹ�");

	}
	else
	{
		AfxMessageBox("����ʧ��");
	}
}