// AlgorithmParamSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../ipcTool.h"
#include "AlgorithmParamSet.h"


// CAlgorithmParamSet �Ի���

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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;
	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4000);
	if(bRet)
	{
		m_csBefRecLastTime=mapofResult["BefRecLastTime"];				
		m_csBefRecTimes=mapofResult["BefRecTimes"];		
		m_csConRecLastTime=mapofResult["ConRecLastTime"];
		m_csConRecTimes=mapofResult["ConRecTimes"];
		m_csEndRecTime=mapofResult["EndRecTime"];
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

// CAlgorithmParamSet ��Ϣ�������
void CAlgorithmParamSet::OnBnClickedOk()
{
	UpdateData(TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	map<CString,CString> mapofparam;
	map<CString,CString> mapofResult;

	mapofparam["BefRecLastTime"]=m_csBefRecLastTime;				
	mapofparam["BefRecTimes"]=m_csBefRecTimes;		
	mapofparam["ConRecLastTime"]=m_csConRecLastTime;
	mapofparam["ConRecTimes"]=m_csConRecTimes;
	mapofparam["EndRecTime"]=m_csEndRecTime;
	bool bRet=m_objTcpClient.TcpPost(mapofparam, mapofResult, 4001);
	if(bRet)
	{

	}
}