#pragma once
#include "afxwin.h"
#include "pcap.h"
#include "ParsePackages.h"
#include "afxcmn.h"

// CSimpleSnifferDlg �Ի���
class CSimpleSnifferDlg : public CDialogEx
{
// ����
public:
	CSimpleSnifferDlg(CWnd* pParent = NULL);  //��׼���캯��
	int initWincap();
	int startCap();
	int updateEdit(int);
	int saveFile();
	int updateTree(int);

	int devCount;
	struct pktcount npacket;  //�������ݰ�����
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *alldev;
	pcap_if_t *dev;
	pcap_t *adhandle;
	pcap_dumper_t *dumpfile;
	int npkt;
	char filepath[512];  //�ļ�����·��
	char filename[64];  //�ļ�����
	HANDLE m_ThreadHandle;  //�߳�

//�Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLESNIFFER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


//ʵ��
protected:
	HICON m_hIcon;

	//���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	void OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	CComboBox m_comboNetwork;
	CComboBox m_comboRule;
	CListCtrl m_listPackage;
	CPtrList m_localDataList;  //���汻���ػ�������ݰ�
	CPtrList m_netDataList;  //�����������ֱ�ӻ�ȡ�����ݰ�
	CButton m_buttonStart;
	CButton m_buttonStop;
	afx_msg void OnBnClickedStartButton();
	afx_msg void OnBnClickedStopButton();
	CEdit m_editPackage;
	afx_msg void OnBnClickedSaveButton();
	CTreeCtrl m_treePackage;
	CButton m_buttonSave;
	afx_msg void OnCbnSelchangeNetworkCombo();
};
