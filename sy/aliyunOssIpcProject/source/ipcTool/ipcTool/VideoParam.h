#pragma once


// CVideoParam �Ի���

class CVideoParam : public CDialog
{
	DECLARE_DYNAMIC(CVideoParam)

public:
	CVideoParam(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CVideoParam();

// �Ի�������
	enum { IDD = IDD_VIDEOSET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
