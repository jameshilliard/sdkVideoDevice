// VideoParam.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ipcTool.h"
#include "VideoParam.h"


// CVideoParam �Ի���

IMPLEMENT_DYNAMIC(CVideoParam, CDialog)

CVideoParam::CVideoParam(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoParam::IDD, pParent)
{

}

CVideoParam::~CVideoParam()
{
}

void CVideoParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
void CVideoParam::OnInitDialog()
{
	
}

BEGIN_MESSAGE_MAP(CVideoParam, CDialog)
END_MESSAGE_MAP()


// CVideoParam ��Ϣ�������
