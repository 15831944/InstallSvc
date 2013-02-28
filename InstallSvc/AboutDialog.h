#pragma once


// CAboutDialog �Ի���

class CAboutDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAboutDialog)

public:
	CAboutDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAboutDialog();

// �Ի�������
	enum { IDD = IDD_ABOUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	void SetResourceImageToCtrl(LPCTSTR lpszImgType, int nCtrlCode, int nImgResourceID);
private:
	CBrush m_brush;
public:
	afx_msg void OnBnClickedLogBtn();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButton4();
};
