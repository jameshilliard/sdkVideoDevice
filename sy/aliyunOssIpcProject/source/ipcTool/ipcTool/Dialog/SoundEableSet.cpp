// SoundEableSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../ipcTool.h"
#include "SoundEableSet.h"


// CSoundEableSet �Ի���

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


// CSoundEableSet ��Ϣ�������


BOOL CSoundEableSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	/* ����ģʽ��HI_TRUEΪ�̶�������HI_FALSEΪ�ɱ����� */

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;

	m_cbSoundStartUpEnable.AddString("��");
	m_cbSoundLoginInEnable.AddString("��");
	m_cbSoundUrgencyStartEnable.AddString("��");
	m_cbSoundUrgencyOverEnable.AddString("��");
	m_cbSoundEnable.AddString("��");

	m_cbSoundStartUpEnable.AddString("��");
	m_cbSoundLoginInEnable.AddString("��");
	m_cbSoundUrgencyStartEnable.AddString("��");
	m_cbSoundUrgencyOverEnable.AddString("��");
	m_cbSoundEnable.AddString("��");


	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4008);
	//if(bRet)
	{
		int iRet=0;
		iRet=m_cbSoundStartUpEnable.SelectString(0,(mapofResult["StartUpEnable"]=="0")?"��":"��");
		iRet=m_cbSoundLoginInEnable.SelectString(0,(mapofResult["LoginInEnable"]=="0")?"��":"��");
		iRet=m_cbSoundUrgencyStartEnable.SelectString(0,(mapofResult["UrgencyStartEnable"]=="0")?"��":"��");
		iRet=m_cbSoundUrgencyOverEnable.SelectString(0,(mapofResult["UrgencyOverEnable"]=="0")?"��":"��");
		iRet=m_cbSoundEnable.SelectString(0,(mapofResult["Enable"]=="0")?"��":"��");
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
// CAlgorithmParamSet ��Ϣ�������
void CSoundEableSet::OnBnClickedOk()
{
	UpdateData(TRUE);
	CString codeType;
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;

	CString enable="";

	m_cbSoundStartUpEnable.GetWindowText(enable);mapofparam["StartUpEnable"]=(enable=="��"?"1":"0");
	m_cbSoundLoginInEnable.GetWindowText(enable);mapofparam["LoginInEnable"]=(enable=="��"?"1":"0");
	m_cbSoundUrgencyStartEnable.GetWindowText(enable);mapofparam["UrgencyStartEnable"]=(enable=="��"?"1":"0");
	m_cbSoundUrgencyOverEnable.GetWindowText(enable);mapofparam["UrgencyOverEnable"]=(enable=="��"?"1":"0");
	m_cbSoundEnable.GetWindowText(enable);mapofparam["Enable"]=(enable=="��"?"1":"0");

	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4009);
	if(bRet)
	{
		AfxMessageBox("���³ɹ�");

	}
	else
	{
		AfxMessageBox("����ʧ��");
	}
}