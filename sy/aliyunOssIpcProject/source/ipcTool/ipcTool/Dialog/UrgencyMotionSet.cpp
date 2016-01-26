// VideoParamSet.cpp : 实现文件
//
#include "stdafx.h"
#include "../ipcTool.h"
#include "UrgencyMotionSet.h"


// CUrgencyMotionSet 对话框

IMPLEMENT_DYNAMIC(CUrgencyMotionSet, CDialog)

CUrgencyMotionSet::CUrgencyMotionSet(CWnd* pParent /*=NULL*/)
	: CDialog(CUrgencyMotionSet::IDD, pParent)
{

}

CUrgencyMotionSet::~CUrgencyMotionSet()
{
}

void CUrgencyMotionSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_URGSTARTPERIOD,m_csUrgStartPeriod);
	DDX_Text(pDX, IDC_EDIT_URGSTARTSUMDETECT,m_csUrgStartSumDetect);
	DDX_Text(pDX, IDC_EDIT_URGSTARTSUMAREA,m_csUrgStartSumArea);
	DDX_Text(pDX, IDC_EDIT_URGSTARTSOUNDSIZE,m_csUrgStartSoundSize);

	DDX_Text(pDX, IDC_EDIT_URGOVERPERIOD,m_csUrgOverPeriod);
	DDX_Text(pDX, IDC_EDIT_URGOVERSUMDETECT,m_csUrgOverSumDetect);
	DDX_Text(pDX, IDC_EDIT_URGOVERSUMAREA,m_csUrgOverSumArea);
	DDX_Text(pDX, IDC_EDIT_URGOVERSOUNDSIZE,m_csUrgOverSoundSize);

	DDX_Text(pDX, IDC_EDIT_URGENDRECTIME,m_csUrgEndRecTime);

	DDX_Control(pDX, IDC_COMBO_URGEBABLE,m_cbUrgEnable);
}


BEGIN_MESSAGE_MAP(CUrgencyMotionSet, CDialog)
	ON_BN_CLICKED(IDOK, &CUrgencyMotionSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CUrgencyMotionSet 消息处理程序


BOOL CUrgencyMotionSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;
	m_cbUrgEnable.AddString("是");
	m_cbUrgEnable.AddString("否");

	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4006);
	//if(bRet)
	{
		int iRet=m_cbUrgEnable.SelectString(0,(mapofResult["Enable"]=="0")?"否":"是");
		m_csUrgStartPeriod		=	mapofResult["StartPeriod"];			
		m_csUrgStartSumDetect	=	mapofResult["StartSumDetect"];	
		m_csUrgStartSumArea		=	mapofResult["StartSumArea"];	
		m_csUrgStartSoundSize	=	mapofResult["StartSoundSize"];
		m_csUrgOverPeriod		=	mapofResult["OverPeriod"];			
		m_csUrgOverSumDetect	=	mapofResult["OverSumDetect"];	
		m_csUrgOverSumArea		=	mapofResult["OverSumArea"];	
		m_csUrgOverSoundSize	=	mapofResult["OverSoundSize"];
		m_csUrgEndRecTime		=	mapofResult["EndRecTime"];	
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
// CAlgorithmParamSet 消息处理程序
void CUrgencyMotionSet::OnBnClickedOk()
{
	UpdateData(TRUE);
	CString codeType;
	// TODO:  在此添加额外的初始化
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;

	CString enable="";
	m_cbUrgEnable.GetWindowText(enable);
	mapofparam["Enable"]=(enable=="是"?"1":"0");
	mapofparam["StartPeriod"]		=	m_csUrgStartPeriod;
	mapofparam["StartSumDetect"]	=	m_csUrgStartSumDetect;
	mapofparam["StartSumArea"]		=	m_csUrgStartSumArea;
	mapofparam["StartSoundSize"]	=	m_csUrgStartSoundSize;
	mapofparam["OverPeriod"]		=	m_csUrgOverPeriod;
	mapofparam["OverSumDetect"]		=	m_csUrgOverSumDetect;
	mapofparam["OverSumArea"]		=	m_csUrgOverSumArea;
	mapofparam["OverSoundSize"]		=	m_csUrgOverSoundSize;
	mapofparam["EndRecTime"]		=	m_csUrgEndRecTime;
	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4007);
	if(bRet)
	{
		AfxMessageBox("更新成功");

	}
	else
	{
		AfxMessageBox("更新失败");
	}
}