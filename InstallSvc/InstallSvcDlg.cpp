#include "stdafx.h"
#include "InstallSvc.h"
#include "InstallSvcDlg.h"
#include "afxdialogex.h"
#include "AboutDialog.h"

#include <strsafe.h>
#include <winsvc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CInstallSvcDlg::CInstallSvcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInstallSvcDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CInstallSvcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SVCPATH_EDIT, m_ctrlSvcPath);
	DDX_Control(pDX, IDC_SVCNAME_EDIT, m_ctrlSvcName);
	DDX_Control(pDX, IDC_SVCNAMETITLE_EDIT, m_ctrlSvcNameTitle);
	DDX_Control(pDX, IDC_MSGTITLE_EDIT, m_ctrlMsgTitle);
	DDX_Control(pDX, IDC_MSG_EDIT, m_ctrlMsg);
}


BEGIN_MESSAGE_MAP(CInstallSvcDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE_BTN, &CInstallSvcDlg::OnBnClickedBrowseBtn)
	ON_BN_CLICKED(IDC_INSTALL_BTN, &CInstallSvcDlg::OnBnClickedInstallBtn)
	ON_BN_CLICKED(IDC_START_BTN, &CInstallSvcDlg::OnBnClickedStartBtn)
	ON_BN_CLICKED(IDC_STOP_BTN, &CInstallSvcDlg::OnBnClickedStopBtn)
	ON_BN_CLICKED(IDC_UNINSTALL_BTN, &CInstallSvcDlg::OnBnClickedUninstallBtn)
	ON_BN_CLICKED(IDC_ABOUT_BTN, &CInstallSvcDlg::OnBnClickedAboutBtn)
	ON_BN_CLICKED(IDC_EXIT_BTN, &CInstallSvcDlg::OnBnClickedExitBtn)
	ON_BN_CLICKED(IDC_MANUALLY_CHECK, &CInstallSvcDlg::OnBnClickedManuallyCheck)
END_MESSAGE_MAP()


BOOL CInstallSvcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//���ô����ϵĴ���ͼ��Ϊ IDI_ICON1
	HICON hIcon=AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(hIcon, FALSE);	// ����Сͼ��
	SetIcon(hIcon, TRUE);	// ���ô�ͼ��		

	//���ô���͸���ȣ�120 ��͸���ȣ���Χ�� 0��255
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);  
	::SetLayeredWindowAttributes(m_hWnd, 0, 215, LWA_ALPHA);

	InitControl();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


void CInstallSvcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


HCURSOR CInstallSvcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//�����ť
void CInstallSvcDlg::OnBnClickedBrowseBtn()
{
	InitControl();

	LPTSTR lpszFilter = TEXT("��ִ���ļ�(*.exe)|*.exe|ϵͳ�ں��ļ�(*.sys)|*.sys|�����ļ�(*.*)|*.*||");
	CFileDialog cFileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, lpszFilter, NULL);
	if(IDOK == cFileDialog.DoModal())
	{
		CString cStrExeExt("exe");
		CString cStrSysExt("sys");
		CString cStrPath = cFileDialog.GetPathName();
		CString cStrNameAndExt = cFileDialog.GetFileName();

		this->m_strFileExt = cFileDialog.GetFileExt().MakeLower();

		int nIndex = cStrNameAndExt.ReverseFind('.');
		CString cStrName = cStrNameAndExt.Left(nIndex);

		if(this->m_strFileExt == cStrSysExt)
		{
			CButton *pTmpBtn = (CButton *)GetDlgItem(IDC_DEMAND_RADIO);
			pTmpBtn->EnableWindow(TRUE);

			pTmpBtn = (CButton *)GetDlgItem(IDC_BOOT_RADIO);
			pTmpBtn->EnableWindow(TRUE);

			pTmpBtn = (CButton *)GetDlgItem(IDC_SYSTEM_RADIO);
			pTmpBtn->EnableWindow(TRUE);

			pTmpBtn = (CButton *)GetDlgItem(IDC_AUTO_RADIO);
			pTmpBtn->EnableWindow(TRUE);
		}
		if(this->m_strFileExt == cStrExeExt)
		{
			CButton *pTmpBtn = (CButton *)GetDlgItem(IDC_DEMAND_RADIO);
			pTmpBtn->EnableWindow(TRUE);

			pTmpBtn = (CButton *)GetDlgItem(IDC_AUTO_RADIO);
			pTmpBtn->EnableWindow(TRUE);
		}

		m_ctrlSvcPath.SetWindowTextW(cStrPath);
		m_ctrlSvcName.SetWindowTextW(cStrName);
	}
}


//��װ������
bool CInstallSvcDlg::InstallServiceIsOK()
{
	DWORD dwStartType = 0;

	CString cStrSvcPath;
	CString cStrSvcName;
	CString cStrExeExt("exe");
	CString cStrSysExt("sys");

	CButton * pTmpBoot = (CButton *)GetDlgItem(IDC_BOOT_RADIO);
	CButton * pTmpSystem = (CButton *)GetDlgItem(IDC_SYSTEM_RADIO);
	CButton * pTmpAuto = (CButton *)GetDlgItem(IDC_AUTO_RADIO);
	CButton * pTmpDemand = (CButton *)GetDlgItem(IDC_DEMAND_RADIO);

	if(pTmpBoot->GetCheck() == TRUE)
	{
		dwStartType = SERVICE_BOOT_START;
	}
	else if(pTmpSystem->GetCheck() == TRUE)
	{
		dwStartType = SERVICE_SYSTEM_START;
	}
	else if(pTmpAuto->GetCheck() == TRUE)
	{
		dwStartType = SERVICE_AUTO_START;
	}
	else if(pTmpDemand->GetCheck() == TRUE)
	{
		dwStartType = SERVICE_DEMAND_START;
	}
	else
	{
		return FALSE;
	}

	this->m_ctrlSvcPath.GetWindowTextW(cStrSvcPath);
	this->m_ctrlSvcName.GetWindowTextW(cStrSvcName);
	if(cStrSvcName.IsEmpty() || cStrSvcPath.IsEmpty() || this->m_strFileExt.IsEmpty())
	{
		return FALSE;
	}

	if(cStrExeExt == this->m_strFileExt)
	{
		return InstallSvc(((LPTSTR)(LPCTSTR)cStrSvcName), ((LPTSTR)(LPCTSTR)cStrSvcName), ((LPTSTR)(LPCTSTR)cStrSvcPath), SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, dwStartType);
	}
	else if(cStrSysExt == this->m_strFileExt)
	{
		return InstallSvc(((LPTSTR)(LPCTSTR)cStrSvcName), ((LPTSTR)(LPCTSTR)cStrSvcName), ((LPTSTR)(LPCTSTR)cStrSvcPath), SERVICE_KERNEL_DRIVER, dwStartType);
	}
	else
	{
		return FALSE;
	}
}


//��װ��ť
void CInstallSvcDlg::OnBnClickedInstallBtn()
{
	if(InstallServiceIsOK() == TRUE)
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("Install Service Success !"));
	}
	else
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("Install Service Failed !"));
	}
}


//������ť
void CInstallSvcDlg::OnBnClickedStartBtn()
{
	CString cStrSvcName;

	this->m_ctrlSvcName.GetWindowTextW(cStrSvcName);
	if(cStrSvcName.IsEmpty())
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("Start Service Failed !"));
		return;
	}

	if(StartSvc((LPTSTR)((LPCTSTR)cStrSvcName)) == TRUE)
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("Start Service Success !"));
	}
	else
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("Start Service Failed !"));
	}
}


//ֹͣ��ť
void CInstallSvcDlg::OnBnClickedStopBtn()
{
	CString cStrSvcName;

	this->m_ctrlSvcName.GetWindowTextW(cStrSvcName);
	if(cStrSvcName.IsEmpty())
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("Stop Service Failed !"));
		return;
	}

	if(StopSvc((LPTSTR)(LPCTSTR)cStrSvcName) == TRUE)
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("Stop Service Success !"));
	}
	else
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("Stop Service Failed !"));
	}
}


//ж�ذ�ť
void CInstallSvcDlg::OnBnClickedUninstallBtn()
{
	CString cStrSvcName;

	this->m_ctrlSvcName.GetWindowTextW(cStrSvcName);
	if(cStrSvcName.IsEmpty())
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("UnInstall Service Failed !"));
		return;
	}

	if(UnInstallSvc((LPTSTR)(LPCTSTR)cStrSvcName) == TRUE)
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("UnInstall Service Success !"));
	}
	else
	{
		this->m_ctrlMsg.SetWindowTextW(TEXT("UnInstall Service Failed !"));
	}
}


//���ڰ�ť
void CInstallSvcDlg::OnBnClickedAboutBtn()
{
	CAboutDialog cAboutDialog;
	cAboutDialog.DoModal();
}


//�˳���ť
void CInstallSvcDlg::OnBnClickedExitBtn()
{
	this->SendMessage(WM_CLOSE, NULL, NULL);
}


//�ֶ���ѡ��
void CInstallSvcDlg::OnBnClickedManuallyCheck()
{
	CButton *pCkBox = (CButton *)GetDlgItem(IDC_MANUALLY_CHECK);
	m_ctrlSvcName.SetReadOnly(!pCkBox->GetCheck());
}


//��ʼ���ؼ�
void CInstallSvcDlg::InitControl()
{
	this->m_ctrlSvcNameTitle.SetWindowTextW(TEXT("Service Name:"));
	this->m_ctrlMsgTitle.SetWindowTextW(TEXT("Service Message:"));

	this->m_ctrlMsg.SetWindowTextW(NULL);
	this->m_ctrlSvcPath.SetWindowTextW(NULL);

	CButton *pTmpBtn = (CButton *)GetDlgItem(IDC_DEMAND_RADIO);
	pTmpBtn->EnableWindow(FALSE);
	pTmpBtn = (CButton *)GetDlgItem(IDC_BOOT_RADIO);
	pTmpBtn->EnableWindow(FALSE);
	pTmpBtn = (CButton *)GetDlgItem(IDC_SYSTEM_RADIO);
	pTmpBtn->EnableWindow(FALSE);
	pTmpBtn = (CButton *)GetDlgItem(IDC_AUTO_RADIO);
	pTmpBtn->EnableWindow(FALSE);

	pTmpBtn = (CButton *)GetDlgItem(IDC_DEMAND_RADIO);
	pTmpBtn->SetCheck(TRUE);

	pTmpBtn = (CButton *)GetDlgItem(IDC_MANUALLY_CHECK);
	pTmpBtn->SetCheck(FALSE);

	m_ctrlSvcName.SetReadOnly(TRUE);

	this->m_strFileExt.Empty();
}


//=====================================================================================//
//Name: bool AdjustProcessTokenPrivilege()                                             //
//                                                                                     //
//Descripion: ������ǰ����Ȩ��										  		               //
//=====================================================================================//
bool CInstallSvcDlg::AdjustProcessTokenPrivilege()
{
	LUID luidTmp;
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		OutputDebugString(TEXT("AdjustProcessTokenPrivilege OpenProcessToken Failed ! \n"));

		return false;
	}

	if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidTmp))
	{
		OutputDebugString(TEXT("AdjustProcessTokenPrivilege LookupPrivilegeValue Failed ! \n"));

		CloseHandle(hToken);

		return FALSE;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luidTmp;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if(!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		OutputDebugString(TEXT("AdjustProcessTokenPrivilege AdjustTokenPrivileges Failed ! \n"));

		CloseHandle(hToken);

		return FALSE;
	}
	return true;
}


//=====================================================================================//
//Name: bool OutputErrorMessage()                                                      //
//                                                                                     //
//Descripion: ��ӡ����ǰ��������� GetLastError ������Ĵ��󣬱�ʾʧ��  		               //
//=====================================================================================//
bool CInstallSvcDlg::OutputErrorMessage(LPTSTR lpszMsg)
{
	LPVOID lpszBufMsg;
	LPVOID lpszBufErrorMsg;
	DWORD dwError = GetLastError();

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpszBufErrorMsg, 0, NULL);

	lpszBufMsg = (LPVOID)LocalAlloc(LMEM_ZEROINIT, sizeof(TCHAR) * 256);
	StringCchPrintf((LPTSTR)lpszBufMsg, LocalSize(lpszBufMsg), (lpszMsg), dwError, lpszBufErrorMsg);

	OutputDebugString((LPTSTR)lpszBufMsg);

	LocalFree(lpszBufMsg);

	return FALSE;
}


//=====================================================================================//
//Name: bool OutputSuccessMessage()                                                    //
//                                                                                     //
//Descripion: ʵ�ִ�ӡһ����Ϣ��������ʾ�����ɹ�	    							     	   //
//=====================================================================================//
bool CInstallSvcDlg::OutputSuccessMessage(LPTSTR lpszMsg)
{
	OutputDebugString(lpszMsg);

	return TRUE;
}


//=====================================================================================//
//Name: bool InstallSvc()                                                              //
//                                                                                     //
//Descripion: ��װ����																   //
//            lpszSvcName Ϊ�������ƣ�												   //
//            lpszDisplay Ϊ��ʾ�ڷ�����ƹ������е����ƣ�								   //
//            lpszSvcBinaryPath Ϊ����ӳ���ļ�����·����								   //
//            dwSvcType Ϊ��������													   //
//            dwStartType Ϊ������������			 							     	   //
//=====================================================================================//
bool CInstallSvcDlg::InstallSvc(LPTSTR lpszSvcName, LPTSTR lpszDisplayName, LPTSTR lpszSvcBinaryPath, DWORD dwSvcType, DWORD dwStartType)
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hSvc = NULL;

	AdjustProcessTokenPrivilege();

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(NULL == hSCM)
	{
		OutputErrorMessage(TEXT("InstallSvc - OpenSCManager Failed , Error Code Is %d , Error Message Is %s !"));

		return FALSE;
	}

	for(int i = 0; i < 10 && (NULL == hSvc); i++)
	{
		//SERVICE_WIN32_OWN_PROCESS  | SERVICE_INTERACTIVE_PROCESS
		hSvc = CreateService(hSCM, lpszSvcName, lpszDisplayName, SERVICE_ALL_ACCESS, 
			dwSvcType, dwStartType, SERVICE_ERROR_NORMAL, 
			lpszSvcBinaryPath, NULL, NULL, NULL, NULL, NULL);
		if(NULL != hSvc || (NULL == hSvc && ERROR_SERVICE_EXISTS == GetLastError()))
		{
			if(NULL != hSvc)
			{
				CloseServiceHandle(hSvc);
			}
			CloseServiceHandle(hSCM);
			return TRUE;
		}
	}

	OutputErrorMessage(TEXT("InstallSvc - CreateService Failed , Error Code Is %d , Error Message Is %s !"));

	CloseServiceHandle(hSCM);

	return FALSE;
}


//=====================================================================================//
//Name: bool UnInstallSvc()							                                   //
//                                                                                     //
//Descripion: ʵ��ж�ط���					    							     	   //
//=====================================================================================//
bool CInstallSvcDlg::UnInstallSvc(LPTSTR lpszSvcName)
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hSvc = NULL;
	bool rtResult = FALSE;

	AdjustProcessTokenPrivilege();

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(NULL == hSCM)
	{
		OutputErrorMessage(TEXT("UnInstallSvc - OpenSCManager Failed , Error Code Is %d , Error Message Is %s !"));

		return FALSE;
	}

	hSvc = OpenService(hSCM, lpszSvcName, SERVICE_ALL_ACCESS);
	if(NULL == hSvc)
	{
		OutputErrorMessage(TEXT("UnInstallSvc - OpenService Failed , Error Code Is %d , Error Message Is %s !"));

		CloseServiceHandle(hSCM);

		return FALSE;
	}

	rtResult = DeleteService(hSvc);

	CloseServiceHandle(hSvc);
	CloseServiceHandle(hSCM);

	return rtResult;
}


//=====================================================================================//
//Name: bool StartSvc()								                                   //
//                                                                                     //
//Descripion: ʵ����������					    							     	   //
//=====================================================================================//
bool CInstallSvcDlg::StartSvc(LPTSTR lpszSvcName)
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hSvc = NULL;
	bool rtResult = FALSE;
	
	AdjustProcessTokenPrivilege();

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(NULL == hSCM)
	{
		OutputErrorMessage(TEXT("StartSvc - OpenSCManager Failed , Error Code Is %d , Error Message Is %s !"));

		return FALSE;
	}

	hSvc = OpenService(hSCM, lpszSvcName, SERVICE_ALL_ACCESS);
	if(NULL == hSvc)
	{
		OutputErrorMessage(TEXT("StartSvc - OpenService Failed , Error Code Is %d , Error Message Is %s !"));

		CloseServiceHandle(hSCM);

		return FALSE;
	}

	rtResult = StartService(hSvc, NULL, NULL);

	CloseServiceHandle(hSvc);
	CloseServiceHandle(hSCM);

	if(FALSE == rtResult)
	{
		if(ERROR_SERVICE_ALREADY_RUNNING == GetLastError())
		{
			return TRUE;
		}
		else
		{
			OutputErrorMessage(TEXT("StartSvc - StartService Failed , Error Code Is %d , Error Message Is %s !"));

			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}
}


//=====================================================================================//
//Name: bool StopSvc()								                                   //
//                                                                                     //
//Descripion: ʵ��ֹͣ����					    							     	   //
//=====================================================================================//
bool CInstallSvcDlg::StopSvc(LPTSTR lpszSvcName)
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hSvc = NULL;
	bool rtResult = FALSE;

	SERVICE_STATUS svcStatus;

	AdjustProcessTokenPrivilege();

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(NULL == hSCM)
	{
		OutputErrorMessage(TEXT("StopSvc - OpenSCManager Failed , Error Code Is %d , Error Message Is %s !"));

		return FALSE;
	}

	hSvc = OpenService(hSCM, lpszSvcName, SERVICE_ALL_ACCESS);
	if(NULL == hSvc)
	{
		OutputErrorMessage(TEXT("StopSvc - OpenService Failed , Error Code Is %d , Error Message Is %s !"));

		CloseServiceHandle(hSCM);

		return FALSE;
	}

	rtResult = ControlService(hSvc, SERVICE_CONTROL_STOP, &svcStatus);
	if(rtResult == FALSE)
	{
		OutputErrorMessage(TEXT("StopSvc - ControlService Failed , Error Code Is %d , Error Message Is %s !"));
	}
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hSCM);

	return rtResult;
}
