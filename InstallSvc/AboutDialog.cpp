// AboutDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "InstallSvc.h"
#include "AboutDialog.h"
#include "afxdialogex.h"


// CAboutDialog �Ի���

IMPLEMENT_DYNAMIC(CAboutDialog, CDialogEx)

CAboutDialog::CAboutDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAboutDialog::IDD, pParent)
{
	this->m_brush.CreateSolidBrush(RGB(200, 245, 142)); 
}

CAboutDialog::~CAboutDialog()
{

}

void CAboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDialog, CDialogEx)
	ON_BN_CLICKED(IDC_LOG_BTN, &CAboutDialog::OnBnClickedLogBtn)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON4, &CAboutDialog::OnBnClickedButton4)
END_MESSAGE_MAP()


BOOL CAboutDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//���� Log ����
	CFont * pMajorLogFont = new CFont;
	pMajorLogFont->CreateFont(33, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, _T("΢���ź�"));
	((CStatic *)GetDlgItem(IDC_MAJORLOG_STATIC))->SetFont(pMajorLogFont, FALSE);

	//���� Button ��С������������ͼƬ�����ҽ�ͼƬ�󶨵� Button �ؼ���
	CWnd * pTmpCwnd = GetDlgItem(IDC_LOG_BTN);
	pTmpCwnd->SetWindowPos( NULL, 0, 0, 100, 100, SWP_NOZORDER | SWP_NOMOVE);
	SetResourceImageToCtrl(TEXT("PNG"), IDC_LOG_BTN, IDB_PNG1);

	//���ô���͸���ȣ�120 ��͸���ȣ���Χ�� 0��255
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);  
	::SetLayeredWindowAttributes(m_hWnd, 0, 215, LWA_ALPHA);

	//���ô����ϵĴ���ͼ��Ϊ IDI_ICON1
	HICON hIcon=AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(hIcon, FALSE);

	return TRUE;
}


//����Դ�ļ��ж�ȡ�� PNG ��ʽ��ͼƬ�����ҽ���ͼƬת��Ϊ Bitmap��Ȼ����ʾ��ָ�� ID �Ŀؼ���
void CAboutDialog::SetResourceImageToCtrl(LPCTSTR lpszImgType, int nCtrlCode, int nImgResourceID)
{
	CImage cImg;
	HRSRC hRsrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nImgResourceID), lpszImgType);
	if(NULL != hRsrc)
	{
		HGLOBAL hImgData = LoadResource(AfxGetResourceHandle(), hRsrc);
		if(NULL != hImgData)
		{
			LPSTREAM lpStream = NULL;
			LPVOID lpVoid = LockResource(hImgData);
			DWORD dwSize = SizeofResource(AfxGetResourceHandle(), hRsrc);

			HGLOBAL hAllocate = GlobalAlloc(GHND, dwSize);
			LPBYTE lpByte = (LPBYTE)GlobalLock(hAllocate);
			memcpy(lpByte, lpVoid, dwSize);
			GlobalUnlock(hAllocate);

			HRESULT hResult = CreateStreamOnHGlobal(hAllocate, TRUE, &lpStream);
			if(S_OK == hResult)
			{
				cImg.Load(lpStream);

				HBITMAP hBitmap = cImg.Detach();

				((CButton *)GetDlgItem(nCtrlCode))->SetBitmap(hBitmap);				
			}

			GlobalFree(hAllocate);
			FreeResource(hImgData);
		}
	}
}


void CAboutDialog::OnBnClickedLogBtn()
{
	//��������д��ҵ���ҳ
	ShellExecute(this->m_hWnd, NULL, TEXT("www.cnblogs.com/boyxiao"), NULL, NULL, SW_SHOWNORMAL);
}


HBRUSH CAboutDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	//ֻ�е��ǶԻ�����ʱ���Ž���ˢ����Ϊ m_brush
	//����һЩ�����Ŀؼ�֮����򲻲�������ʹ��Ԥ���屳��ɫ
	if(nCtlColor == CTLCOLOR_DLG)
	{  
		//return this->m_brush;
	} 

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


void CAboutDialog::OnBnClickedButton4()
{
	SendMessage(WM_CLOSE, NULL, NULL);
}
