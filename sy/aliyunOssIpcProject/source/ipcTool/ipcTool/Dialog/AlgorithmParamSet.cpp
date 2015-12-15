// AlgorithmParamSet.cpp : 实现文件
//

#include "stdafx.h"
#include "../ipcTool.h"
#include "AlgorithmParamSet.h"


// CAlgorithmParamSet 对话框

IMPLEMENT_DYNAMIC(CAlgorithmParamSet, CDialog)

CAlgorithmParamSet::CAlgorithmParamSet(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmParamSet::IDD, pParent)
{

}

CAlgorithmParamSet::~CAlgorithmParamSet()
{
}

void CAlgorithmParamSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ENBALE,m_cbEnbale);
	DDX_Text(pDX, IDC_EDIT_BefRecLastTime,m_csBefRecLastTime);
	DDX_Text(pDX, IDC_EDIT_BefRecTimes,m_csBefRecTimes);
	DDX_Text(pDX, IDC_EDIT_ConRecLastTime,m_csConRecLastTime);
	DDX_Text(pDX, IDC_EDIT_ConRecTimes,m_csConRecTimes);
	DDX_Text(pDX, IDC_EDIT_EndRecTime,m_csEndRecTime);
}


BEGIN_MESSAGE_MAP(CAlgorithmParamSet, CDialog)
	ON_BN_CLICKED(IDOK, &CAlgorithmParamSet::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CAlgorithmParamSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;
	m_cbEnbale.AddString("是");
	m_cbEnbale.AddString("否");
	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4000);
	if(bRet)
	{
		m_csBefRecLastTime=mapofResult["BefRecLastTime"];				
		m_csBefRecTimes=mapofResult["BefRecTimes"];		
		m_csConRecLastTime=mapofResult["ConRecLastTime"];
		m_csConRecTimes=mapofResult["ConRecTimes"];
		m_csEndRecTime=mapofResult["EndRecTime"];
		int iRet=m_cbEnbale.SelectString(0,(mapofResult["Enable"]=="0")?"否":"是");

	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// CAlgorithmParamSet 消息处理程序
void CAlgorithmParamSet::OnBnClickedOk()
{
	UpdateData(TRUE);
	// TODO:  在此添加额外的初始化
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;
	CString enbale="";

	mapofparam["BefRecLastTime"]=m_csBefRecLastTime;				
	mapofparam["BefRecTimes"]=m_csBefRecTimes;		
	mapofparam["ConRecLastTime"]=m_csConRecLastTime;
	mapofparam["ConRecTimes"]=m_csConRecTimes;
	mapofparam["EndRecTime"]=m_csEndRecTime;
	m_cbEnbale.GetWindowText(enbale);
	mapofparam["Enable"]=(enbale=="是"?"1":"0");
	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4001);
	if(bRet)
	{
		AfxMessageBox("更新成功");

	}
	else
	{
		AfxMessageBox("更新失败");
	}
}