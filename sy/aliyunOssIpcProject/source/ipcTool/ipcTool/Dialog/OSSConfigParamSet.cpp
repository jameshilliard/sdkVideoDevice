// OSSConfigParamSet.cpp : 实现文件
//

#include "stdafx.h"
#include "../ipcTool.h"
#include "OSSConfigParamSet.h"


// COSSConfigParamSet 对话框

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

	// TODO:  在此添加额外的初始化
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
	// 异常: OCX 属性页应返回 FALSE
}
// COSSConfigParamSet 消息处理程序
// CAlgorithmParamSet 消息处理程序
void COSSConfigParamSet::OnBnClickedOk()
{
	UpdateData(TRUE);
	// TODO:  在此添加额外的初始化
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
		AfxMessageBox("更新成功");

	}
	else
	{
		AfxMessageBox("更新失败");
	}
}