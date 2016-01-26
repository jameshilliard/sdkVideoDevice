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
	DDX_Control(pDX, IDC_COMBO_SOUNDSERVERSUCCESS,m_cbSoundServerSuccessEnable);
	DDX_Control(pDX, IDC_COMBO_SOUNDENBALE,m_cbSoundServerFailureEnable);
	DDX_Control(pDX, IDC_COMBO_SOUNDENBALE,m_cbSoundUpgradeEnable);
	DDX_Control(pDX, IDC_COMBO_SOUNDENBALE,m_cbSoundLoginInEnable);
	DDX_Control(pDX, IDC_COMBO_SOUNDENBALE,m_cbSoundUrgencyStartEnable);
	DDX_Control(pDX, IDC_COMBO_SOUNDENBALE,m_cbSoundUrgencyOverEnable);
	DDX_Control(pDX, IDC_COMBO_SOUNDENBALE,m_cbSoundEnable);
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

	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4006);
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
void CSoundEableSet::OnBnClickedOk()
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