// VideoParam.cpp : 实现文件
//

#include "stdafx.h"
#include "ipcTool.h"
#include "VideoParam.h"


// CVideoParam 对话框

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


// CVideoParam 消息处理程序
