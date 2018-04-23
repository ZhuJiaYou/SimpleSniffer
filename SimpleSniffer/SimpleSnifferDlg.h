#pragma once
#include "afxwin.h"
#include "pcap.h"
#include "ParsePackages.h"
#include "afxcmn.h"

// CSimpleSnifferDlg 对话框
class CSimpleSnifferDlg : public CDialogEx
{
// 构造
public:
	CSimpleSnifferDlg(CWnd* pParent = NULL);  //标准构造函数
	int initWincap();
	int startCap();
	int updateEdit(int);
	int saveFile();
	int updateTree(int);

	int devCount;
	struct pktcount npacket;  //各类数据包计数
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *alldev;
	pcap_if_t *dev;
	pcap_t *adhandle;
	pcap_dumper_t *dumpfile;
	int npkt;
	char filepath[512];  //文件保存路径
	char filename[64];  //文件名称
	HANDLE m_ThreadHandle;  //线程

//对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLESNIFFER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


//实现
protected:
	HICON m_hIcon;

	//生成的消息映射函数
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
	CPtrList m_localDataList;  //保存被本地化后的数据包
	CPtrList m_netDataList;  //保存从网络中直接获取的数据包
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
