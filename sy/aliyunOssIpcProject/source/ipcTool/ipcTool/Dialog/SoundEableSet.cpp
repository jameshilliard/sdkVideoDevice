// SoundEableSet.cpp : 实现文件
//

#include "stdafx.h"
#include "../ipcTool.h"
#include "SoundEableSet.h"


// CSoundEableSet 对话框

IMPLEMENT_DYNAMIC(CSoundEableSet, CDialog)

CSoundEableSet::CSoundEableSet(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundEableSet::IDD, pParent)
{

}

CSoundEableSet::~CSoundEableSet()
{
}

void CSoundEableSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_SOUNDSTARTUP,m_cbSoundStartUpEnable);
	//DDX_Control(pDX, IDC_COMBO_SOUNDSERVERSUCCESS,m_cbSoundServerSuccessEnable);
	//DDX_Control(pDX, IDC_COMBO_SOUNDSERVERFAILURE,m_cbSoundServerFailureEnable);
	//DDX_Control(pDX, IDC_COMBO_SOUNDUPGRADE,m_cbSoundUpgradeEnable);
	DDX_Control(pDX, IDC_COMBO_SOUNDLOGININ,m_cbSoundLoginInEnable);
	DDX_Control(pDX, IDC_COMBO_SOUNDURGENCYSTART,m_cbSoundUrgencyStartEnable);
	DDX_Control(pDX, IDC_COMBO_SOUNDURGENCYOVER,m_cbSoundUrgencyOverEnable);
	DDX_Control(pDX, IDC_COMBO_SOUNDENABLE,m_cbSoundEnable);
}


BEGIN_MESSAGE_MAP(CSoundEableSet, CDialog)
	ON_BN_CLICKED(IDOK, &CSoundEableSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CSoundEableSet 消息处理程序


BOOL CSoundEableSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	/* 码流模式、HI_TRUE为固定码流，HI_FALSE为可变码流 */

	// TODO:  在此添加额外的初始化
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;

	m_cbSoundStartUpEnable.AddString("是");
	m_cbSoundLoginInEnable.AddString("是");
	m_cbSoundUrgencyStartEnable.AddString("是");
	m_cbSoundUrgencyOverEnable.AddString("是");
	m_cbSoundEnable.AddString("是");

	m_cbSoundStartUpEnable.AddString("否");
	m_cbSoundLoginInEnable.AddString("否");
	m_cbSoundUrgencyStartEnable.AddString("否");
	m_cbSoundUrgencyOverEnable.AddString("否");
	m_cbSoundEnable.AddString("否");


	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4008);
	//if(bRet)
	{
		int iRet=0;
		iRet=m_cbSoundStartUpEnable.SelectString(0,(mapofResult["StartUpEnable"]=="0")?"否":"是");
		iRet=m_cbSoundLoginInEnable.SelectString(0,(mapofResult["LoginInEnable"]=="0")?"否":"是");
		iRet=m_cbSoundUrgencyStartEnable.SelectString(0,(mapofResult["UrgencyStartEnable"]=="0")?"否":"是");
		iRet=m_cbSoundUrgencyOverEnable.SelectString(0,(mapofResult["UrgencyOverEnable"]=="0")?"否":"是");
		iRet=m_cbSoundEnable.SelectString(0,(mapofResult["Enable"]=="0")?"否":"是");
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
// CAlgorithmParamSet 消息处理程序
void CSoundEableSet::OnBnClickedOk()
{
	UpdateData(TRUE);
	CString codeType;
	// TODO:  在此添加额外的初始化
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;

	CString enable="";

	m_cbSoundStartUpEnable.GetWindowText(enable);mapofparam["StartUpEnable"]=(enable=="是"?"1":"0");
	m_cbSoundLoginInEnable.GetWindowText(enable);mapofparam["LoginInEnable"]=(enable=="是"?"1":"0");
	m_cbSoundUrgencyStartEnable.GetWindowText(enable);mapofparam["UrgencyStartEnable"]=(enable=="是"?"1":"0");
	m_cbSoundUrgencyOverEnable.GetWindowText(enable);mapofparam["UrgencyOverEnable"]=(enable=="是"?"1":"0");
	m_cbSoundEnable.GetWindowText(enable);mapofparam["Enable"]=(enable=="是"?"1":"0");

	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4009);
	if(bRet)
	{
		AfxMessageBox("更新成功");

	}
	else
	{
		AfxMessageBox("更新失败");
	}
}