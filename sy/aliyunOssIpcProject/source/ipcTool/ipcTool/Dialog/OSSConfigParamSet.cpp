// OSSConfigParamSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../ipcTool.h"
#include "OSSConfigParamSet.h"


// COSSConfigParamSet �Ի���

IMPLEMENT_DYNAMIC(COSSConfigParamSet, CDialog)

COSSConfigParamSet::COSSConfigParamSet(CWnd* pParent /*=NULL*/)
	: CDialog(COSSConfigParamSet::IDD, pParent)
{

}

COSSConfigParamSet::~COSSConfigParamSet()
{
}

void COSSConfigParamSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OSSEndPoint,m_szOssEndPoint);
	DDX_Text(pDX, IDC_EDIT_BucketName,m_szBuctetName);
	DDX_Text(pDX, IDC_EDIT_AccessKeyId,m_szAccessKeyId);
	DDX_Text(pDX, IDC_EDIT_AccessKeySecret,m_szAccessKeySecret);
	DDX_Text(pDX, IDC_EDIT_VIDEOPATH,m_szVideoPath);
	DDX_Text(pDX, IDC_EDIT_JPGPATH,m_szJpgPath);
}


BEGIN_MESSAGE_MAP(COSSConfigParamSet, CDialog)
	ON_BN_CLICKED(IDOK, &COSSConfigParamSet::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL COSSConfigParamSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;
	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4002);
	if(bRet)
	{
		m_szBuctetName=mapofResult["BuctetName"];				
		m_szOssEndPoint=mapofResult["OssEndPoint"];		
		m_szAccessKeyId=mapofResult["AccessKeyId"];
		m_szAccessKeySecret=mapofResult["AccessKeySecret"];
		m_szVideoPath=mapofResult["VideoPath"];
		m_szJpgPath=mapofResult["JpgPath"];
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
// COSSConfigParamSet ��Ϣ�������
// CAlgorithmParamSet ��Ϣ�������
void COSSConfigParamSet::OnBnClickedOk()
{
	UpdateData(TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;

	mapofparam["BuctetName"]=m_szBuctetName;				
	mapofparam["OssEndPoint"]=m_szOssEndPoint;		
	mapofparam["AccessKeyId"]=m_szAccessKeyId;
	mapofparam["AccessKeySecret"]=m_szAccessKeySecret;
	mapofparam["VideoPath"]=m_szVideoPath;
	mapofparam["JpgPath"]=m_szJpgPath;

	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4003);
	if(bRet)
	{
		AfxMessageBox("���³ɹ�");

	}
	else
	{
		AfxMessageBox("����ʧ��");
	}
}