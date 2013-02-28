
// InstallSvcDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"



// CInstallSvcDlg �Ի���
class CInstallSvcDlg : public CDialogEx
{
// ����
public:
	CInstallSvcDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_INSTALLSVC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctrlSvcPath;
	CEdit m_ctrlSvcName;
	CEdit m_ctrlSvcNameTitle;
	CEdit m_ctrlMsgTitle;
	CEdit m_ctrlMsg;
	afx_msg void OnBnClickedBrowseBtn();
	afx_msg void OnBnClickedInstallBtn();
	afx_msg void OnBnClickedStartBtn();
	afx_msg void OnBnClickedStopBtn();
	afx_msg void OnBnClickedUninstallBtn();
	afx_msg void OnBnClickedAboutBtn();
	afx_msg void OnBnClickedExitBtn();
	afx_msg void OnBnClickedManuallyCheck();
private:
	void InitControl();
	bool OutputErrorMessage(LPTSTR lpszMsg);
	bool OutputSuccessMessage(LPTSTR lpszMsg);

	bool AdjustProcessTokenPrivilege();

	bool InstallSvc(LPTSTR lpszSvcName, LPTSTR lpszDisplayName, LPTSTR lpszSvcBinaryPath, DWORD dwSvcType, DWORD dwStartType);
	bool UnInstallSvc(LPTSTR lpszSvcName);
	bool StartSvc(LPTSTR lpszSvcName);
	bool StopSvc(LPTSTR lpszSvcName);
	bool InstallServiceIsOK();

private:
	CString m_strFileExt;
};
