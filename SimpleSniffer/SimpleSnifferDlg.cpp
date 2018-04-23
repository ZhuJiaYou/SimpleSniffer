#include "stdafx.h"
#include "SimpleSniffer.h"
#include "SimpleSnifferDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI capThread(LPVOID lpParameter);

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSimpleSnifferDlg �Ի���



CSimpleSnifferDlg::CSimpleSnifferDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SIMPLESNIFFER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleSnifferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NETWORK_COMBO, m_comboNetwork);
	DDX_Control(pDX, IDC_RULE_COMBO, m_comboRule);
	DDX_Control(pDX, IDC_LIST1, m_listPackage);
	DDX_Control(pDX, ID_START_BUTTON, m_buttonStart);
	DDX_Control(pDX, ID_STOP_BUTTON, m_buttonStop);
	DDX_Control(pDX, IDC_PACKAGE_EDIT, m_editPackage);
	DDX_Control(pDX, IDC_PACKAGE_TREE, m_treePackage);
	DDX_Control(pDX, IDC_SAVE_BUTTON, m_buttonSave);
}

BEGIN_MESSAGE_MAP(CSimpleSnifferDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CSimpleSnifferDlg::OnLvnItemchangedList1)
	ON_BN_CLICKED(ID_START_BUTTON, &CSimpleSnifferDlg::OnBnClickedStartButton)
	ON_BN_CLICKED(ID_STOP_BUTTON, &CSimpleSnifferDlg::OnBnClickedStopButton)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CSimpleSnifferDlg::OnNMCustomdrawList1)
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &CSimpleSnifferDlg::OnBnClickedSaveButton)
	ON_CBN_SELCHANGE(IDC_NETWORK_COMBO, &CSimpleSnifferDlg::OnCbnSelchangeNetworkCombo)
END_MESSAGE_MAP()


// CSimpleSnifferDlg ��Ϣ�������

BOOL CSimpleSnifferDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//TODO: �ڴ���Ӷ���ĳ�ʼ������

	m_listPackage.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_listPackage.InsertColumn(0, _T("���"), 3, 40);  //1��ʾ�ң�2��ʾ�У�3��ʾ��
	m_listPackage.InsertColumn(1, _T("ʱ��"), 3, 130);
	m_listPackage.InsertColumn(2, _T("����"), 3, 72);
	m_listPackage.InsertColumn(3, _T("ԴMAC��ַ"), 3, 140);
	m_listPackage.InsertColumn(4, _T("Ŀ��MAC��ַ"), 3, 140);
	m_listPackage.InsertColumn(5, _T("Э��"), 3, 70);
	m_listPackage.InsertColumn(6, _T("ԴIP��ַ"), 3, 145);
	m_listPackage.InsertColumn(7, _T("Ŀ��IP��ַ"), 3, 145);

	m_comboNetwork.AddString(_T("ѡ������������ӿ�"));
	m_comboRule.AddString(_T("ѡ����˹���(��ѡ)"));

	//��ʼ���ӿ��б�
	if(initWincap()<0)
		return FALSE;
	for(dev = alldev; dev; dev = dev->next)
	{
		if(dev->description)
			m_comboNetwork.AddString(CString(dev->description));
	}

	//��ʼ�����˹����б�
	m_comboRule.AddString(_T("tcp"));
	m_comboRule.AddString(_T("udp"));
	m_comboRule.AddString(_T("ip"));
	m_comboRule.AddString(_T("icmp"));
	m_comboRule.AddString(_T("arp"));

	m_comboNetwork.SetCurSel(0);
	m_comboRule.SetCurSel(0);
	
	m_buttonStop.EnableWindow(FALSE);
	m_buttonSave.EnableWindow(FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

//��ʼ��winpcap
int CSimpleSnifferDlg::initWincap()
{
	devCount = 0;
	if(pcap_findalldevs(&alldev, errbuf) == -1)
		return -1;
	for(dev = alldev; dev; dev = dev->next)
		devCount++;
	return 0;
}

//��ʼ����
int CSimpleSnifferDlg::startCap()
{
	int if_index, filter_index, count;
	u_int netmask;
	struct bpf_program fcode;
	CString Ncard;
	initWincap();

	//��ýӿں͹���������
	if_index = this->m_comboNetwork.GetCurSel();
	filter_index = this->m_comboRule.GetCurSel();

	if(0 == if_index || CB_ERR == if_index)
	{
		MessageBox(_T("��ѡ��һ�����ʵ������ӿڣ�"));
		return -1;
	}
	if(CB_ERR == filter_index)
	{
		MessageBox(_T("������ѡ�����"));
		return -1;
	}

	//���ѡ�е������ӿ�
	dev = alldev;
	for(count = 0; count<if_index - 1; count++)
		dev = dev->next;
	if((adhandle = pcap_open_live(dev->name,  // �豸��
		 65536,  //�������ݰ�����																					
		 1,  // ���û���ģʽ(��0��ζ���ǻ���ģʽ)
		 1000,  // ����ʱ����
		 errbuf  // ������Ϣ
	   )) == NULL)
	{
		MessageBox(_T("�޷��򿪽ӿڣ�" + CString(dev->description)));
		pcap_freealldevs(alldev);
		return -1;
	}

	//����Ƿ�Ϊ��̫��
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		MessageBox(_T("�����ڷ���̫����!"));
		pcap_freealldevs(alldev);
		return -1;
	}

	if (dev->addresses != NULL)
		netmask = ((struct sockaddr_in *)(dev->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		netmask = 0xffffff;

	//���������
	if (0 == filter_index)
	{
		char filter[] = "";
		if (pcap_compile(adhandle, &fcode, filter, 1, netmask) <0)
		{
			MessageBox(_T("�﷨�����޷������������"));
			pcap_freealldevs(alldev);
			return -1;
		}
	}
	else {
		CString str;
		char *filter;
		int len, x;
		this->m_comboRule.GetLBText(filter_index, str);
		len = str.GetLength() + 1;
		filter = (char*)malloc(len);
		for (x = 0; x<len; x++)
		{
			filter[x] = str.GetAt(x);
		}
		if (pcap_compile(adhandle, &fcode, filter, 1, netmask) <0)
		{
			MessageBox(_T("�﷨�����޷������������"));
			pcap_freealldevs(alldev);
			return -1;
		}
	}

	//���ù�����
	if (pcap_setfilter(adhandle, &fcode)<0)
	{
		MessageBox(_T("���ù���������"));
		pcap_freealldevs(alldev);
		return -1;
	}

	//�������ݰ��洢·��
	CFileFind file;
	char thistime[30];
	struct tm *ltime;
	memset(filepath, 0, 512);
	memset(filename, 0, 64);

	if (!file.FindFile(_T("PackagesSaved")))
	{
		CreateDirectory(_T("PackagesSaved"), NULL);
	}

	time_t nowtime;
	time(&nowtime);
	ltime = localtime(&nowtime);
	strftime(thistime, sizeof(thistime), "%Y%m%d-%H%M%S", ltime);
	strcpy(filepath, "PackagesSaved\\");
	strcat(filename, thistime);
	strcat(filename, ".txt");

	strcat(filepath, filename);
	dumpfile = pcap_dump_open(adhandle, filepath);
	if (dumpfile == NULL)
	{
		MessageBox(_T("�����ļ�����"));
		return -1;
	}

	pcap_freealldevs(alldev);

	//�������ݣ��½��̴߳���
	LPDWORD threadCap = NULL;
	m_ThreadHandle = CreateThread(NULL, 0, capThread, this, 0, threadCap);
	if (m_ThreadHandle == NULL)
	{
		int code = GetLastError();
		CString str;
		str.Format(_T("�����̴߳��󣬴���Ϊ%d��"), code);
		MessageBox(str);
		return -1;
	}

	return 1;
}

DWORD WINAPI capThread(LPVOID lpParameter)
{
	int res, nItem;
	struct tm *ltime;
	CString timestr, buf, srcMac, destMac;
	time_t local_tv_sec;
	struct pcap_pkthdr *header;  //���ݰ�ͷ
	const u_char *pkt_data = NULL, *pData = NULL;  //�������յ����ֽ�������
	u_char *ppkt_data;

	CSimpleSnifferDlg *pthis = (CSimpleSnifferDlg *)lpParameter;
	if(NULL == pthis->m_ThreadHandle)
	{
		MessageBox(NULL, _T("�߳̾������"), _T("��ʾ"), MB_OK);
		return -1;
	}

	while((res = pcap_next_ex(pthis->adhandle, &header, &pkt_data)) >= 0)
	{
		if (res == 0)  //��ʱ
			continue;

		struct datapkt *data = (struct datapkt *)malloc(sizeof(struct datapkt));
		memset(data, 0, sizeof(struct datapkt));

		if (NULL == data)
		{
			MessageBox(NULL, _T("�ռ��������޷������µ����ݰ�!"), _T("Error"), MB_OK);
			return -1;
		}

		//������������������ݰ����ڴ���Χ��
		if(analyze_frame(pkt_data, data, &(pthis->npacket)) < 0)
			continue;

		//�����ݰ����浽�򿪵��ļ���
		if (pthis->dumpfile != NULL)
		{
			pcap_dump((unsigned char *)pthis->dumpfile, header, pkt_data);
		}

		//���¸������ݰ�����
		//pthis->sniffer_updateNPacket();

		//�����ػ��������װ��һ�������У��Ա����ʹ��		
		ppkt_data = (u_char *)malloc(header->len);
		memcpy(ppkt_data, pkt_data, header->len);

		pthis->m_localDataList.AddTail(data);
		pthis->m_netDataList.AddTail(ppkt_data);

		//Ԥ�������ʱ�䡢����
		data->len = header->len;  //��·���յ������ݳ���
		local_tv_sec = header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		data->time[0] = ltime->tm_year + 1900;
		data->time[1] = ltime->tm_mon + 1;
		data->time[2] = ltime->tm_mday;
		data->time[3] = ltime->tm_hour;
		data->time[4] = ltime->tm_min;
		data->time[5] = ltime->tm_sec;

		//Ϊ�½��յ������ݰ���listControl���½�һ��item
		buf.Format(_T("%d"), pthis->npkt);
		nItem = pthis->m_listPackage.InsertItem(pthis->npkt, buf);

		//��ʾʱ���
		timestr.Format(_T("%d/%d/%d  %d:%d:%d"), data->time[0], data->time[1], data->time[2], 
			                                     data->time[3], data->time[4], data->time[5]);
		pthis->m_listPackage.SetItemText(nItem, 1, timestr);
		//pthis->m_listPackage.setitem

		//��ʾ����
		buf.Empty();
		buf.Format(_T("%d"), data->len);
		pthis->m_listPackage.SetItemText(nItem, 2, buf);

		//��ʾԴMAC
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->src[0], data->ethh->src[1], data->ethh->src[2], 
			                                            data->ethh->src[3], data->ethh->src[4], data->ethh->src[5]);
		pthis->m_listPackage.SetItemText(nItem, 3, buf);

		//��ʾĿ��MAC
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->dest[0], data->ethh->dest[1], data->ethh->dest[2], 
			                                            data->ethh->dest[3], data->ethh->dest[4], data->ethh->dest[5]);
		pthis->m_listPackage.SetItemText(nItem, 4, buf);

		//���Э��
		pthis->m_listPackage.SetItemText(nItem, 5, CString(data->pktType));

		//���ԴIP
		buf.Empty();
		if(0x0806 == data->ethh->type)
		{
			buf.Format(_T("%d.%d.%d.%d"), data->arph->ar_srcip[0], data->arph->ar_srcip[1], data->arph->ar_srcip[2], 
				                          data->arph->ar_srcip[3]);
		}
		else if(0x0800 == data->ethh->type) 
		{
			struct in_addr in;
			in.S_un.S_addr = data->iph->saddr;
			buf = CString(inet_ntoa(in));
		}
		/*
		else if(0x86dd == data->ethh->type) 
		{
			int n;
			for(n = 0; n<8; n++)
			{
				if(n <= 6)
					buf.AppendFormat(_T("%02x:"), data->iph6->saddr[n]);
				else
					buf.AppendFormat(_T("%02x"), data->iph6->saddr[n]);
			}
		}*/
		pthis->m_listPackage.SetItemText(nItem, 6, buf);

		//���Ŀ��IP
		buf.Empty();
		if(0x0806 == data->ethh->type)
		{
			buf.Format(_T("%d.%d.%d.%d"), data->arph->ar_destip[0], data->arph->ar_destip[1], 
				                          data->arph->ar_destip[2], data->arph->ar_destip[3]);
		}
		else if(0x0800 == data->ethh->type) 
		{
			struct  in_addr in;
			in.S_un.S_addr = data->iph->daddr;
			buf = CString(inet_ntoa(in));
		}
		/*
		else if(0x86dd == data->ethh->type) 
		{
			int n;
			for(n = 0; n<8; n++)
			{
				if (n <= 6)
					buf.AppendFormat(_T("%02x:"), data->iph6->daddr[n]);
				else
					buf.AppendFormat(_T("%02x"), data->iph6->daddr[n]);
			}
		}*/
		pthis->m_listPackage.SetItemText(nItem, 7, buf);

		//�԰�����
		pthis->npkt++;

	}
	return 1;
}

//���б���Ŀ�仯��Ϣ������
void CSimpleSnifferDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int index;
	index = this->m_listPackage.GetHotItem();

	if(index > this->m_localDataList.GetCount() - 1)
		return;

	this->updateEdit(index);
	this->updateTree(index);
	*pResult = 0;
}

//������Ϣ
int CSimpleSnifferDlg::updateEdit(int index)
{
	POSITION localpos, netpos;
	localpos = this->m_localDataList.FindIndex(index);
	netpos = this->m_netDataList.FindIndex(index);

	struct datapkt *local_data = (struct datapkt *)(this->m_localDataList.GetAt(localpos));
	u_char *net_data = (u_char *)(this->m_netDataList.GetAt(netpos));

	CString buf;
	print_packet_hex(net_data, local_data->len, &buf);
	this->m_editPackage.SetWindowText(buf);

	return 1;
}

void CSimpleSnifferDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSimpleSnifferDlg::OnPaint()
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSimpleSnifferDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSimpleSnifferDlg::OnBnClickedStartButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(this->m_localDataList.IsEmpty() == FALSE)
	{
		if(MessageBox(_T("                 ��ȷ�������汾�μ���������\n������ǡ�����������μ������ݣ�������񡱽��ᱣ�����ݡ�"),
		   _T("����"), MB_YESNO | MB_ICONWARNING) == IDNO)
		{
			this->saveFile();
		}
	}

	this->npkt = 1;  //���¼���
	this->m_localDataList.RemoveAll();  //ÿ��һ��ʼ�ͽ���ǰ���������յ�
	this->m_netDataList.RemoveAll();
	memset(&(this->npacket), 0, sizeof(struct pktcount));
	//this->sniffer_updateNPacket();

	if (this->startCap() < 0)
		return;

	this->m_listPackage.DeleteAllItems();
	this->m_treePackage.DeleteAllItems();
	this->m_editPackage.SetWindowTextW(_T(""));
	this->m_buttonStart.EnableWindow(FALSE);
	this->m_buttonStop.EnableWindow(TRUE);
    this->m_buttonSave.EnableWindow(FALSE);
}


void CSimpleSnifferDlg::OnBnClickedStopButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == this->m_ThreadHandle)
		return;
	if(TerminateThread(this->m_ThreadHandle, -1) == 0)
	{
		MessageBox(_T("�ر��̴߳������Ժ����ԣ�"));
		return;
	}
	this->m_ThreadHandle = NULL;
	this->m_buttonStart.EnableWindow(TRUE);
	this->m_buttonStop.EnableWindow(FALSE);
	this->m_buttonSave.EnableWindow(TRUE);
}


void CSimpleSnifferDlg::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pNMCD = (LPNMLVCUSTOMDRAW)pNMHDR;
	*pResult = 0;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (CDDS_PREPAINT == pNMCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage) {
		COLORREF crText;
		char buf[10];
		memset(buf, 0, 10);
		POSITION pos = this->m_localDataList.FindIndex(pNMCD->nmcd.dwItemSpec);
		struct datapkt * local_data = (struct datapkt *)this->m_localDataList.GetAt(pos);
		strcpy(buf, local_data->pktType);

		//if (strcmp(buf, "IPV6") == 0)
			//crText = RGB(111, 224, 254);
		if(strcmp(buf, "UDP") == 0)
			crText = RGB(146, 248, 172);
		else if(strcmp(buf, "TCP") == 0)
			crText = RGB(230, 230, 230);
		else if(strcmp(buf, "ARP") == 0)
			crText = RGB(149, 216, 225);
		else if(strcmp(buf, "ICMP") == 0)
			crText = RGB(250, 154, 221);
		else if(strcmp(buf, "HTTP") == 0)
			crText = RGB(167, 156, 209);
		//else if (strcmp(buf, "ICMPv6") == 0)
			//crText = RGB(189, 254, 76);

		pNMCD->clrTextBk = crText;
		*pResult = CDRF_DODEFAULT;
	}
}

int CSimpleSnifferDlg::saveFile()
{
	CFileFind find;
	if (NULL == find.FindFile(CString(filepath)))
	{
		MessageBox(_T("�����ļ�����δ֪����"));
		return -1;
	}

	//���ļ��Ի���
	CFileDialog   FileDlg(FALSE, _T(".txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	FileDlg.m_ofn.lpstrInitialDir = _T("c:\\");
	if (FileDlg.DoModal() == IDOK)
	{
		CopyFile(CString(filepath), FileDlg.GetPathName(), TRUE);
	}
	return 1;
}

void CSimpleSnifferDlg::OnBnClickedSaveButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (this->saveFile()<0)
		return;
}

int CSimpleSnifferDlg::updateTree(int index)
{
	POSITION localpos;
	CString str;
	int i;

	this->m_treePackage.DeleteAllItems();

	localpos = this->m_localDataList.FindIndex(index);
	struct datapkt* local_data = (struct datapkt *)(this->m_localDataList.GetAt(localpos));

	HTREEITEM root = this->m_treePackage.GetRootItem();
	str.Format(_T("��%d�����ݰ�"), index + 1);
	HTREEITEM data = this->m_treePackage.InsertItem(str, root);

	//����֡����
	HTREEITEM frame = this->m_treePackage.InsertItem(_T("��·������"), data);
	//ԴMAC
	str.Format(_T("ԴMAC��"));
	for(i = 0; i<6; i++)
	{
		if(i <= 4)
			str.AppendFormat(_T("%02x-"), local_data->ethh->src[i]);
		else
			str.AppendFormat(_T("%02x"), local_data->ethh->src[i]);
	}
	this->m_treePackage.InsertItem(str, frame);
	//Ŀ��MAC
	str.Format(_T("Ŀ��MAC��"));
	for(i = 0; i<6; i++)
	{
		if(i <= 4)
			str.AppendFormat(_T("%02x-"), local_data->ethh->dest[i]);
		else
			str.AppendFormat(_T("%02x"), local_data->ethh->dest[i]);
	}
	this->m_treePackage.InsertItem(str, frame);
	//����
	str.Format(_T("���ͣ�0x%02x"), local_data->ethh->type);
	this->m_treePackage.InsertItem(str, frame);

	//����IP��ARP��IPv6���ݰ�
	if(0x0806 == local_data->ethh->type)  //ARP
	{
		HTREEITEM arp = this->m_treePackage.InsertItem(_T("ARPЭ��ͷ"), data);
		str.Format(_T("Ӳ�����ͣ�%d"), local_data->arph->ar_hrd);
		this->m_treePackage.InsertItem(str, arp);
		str.Format(_T("Э�����ͣ�0x%02x"), local_data->arph->ar_pro);
		this->m_treePackage.InsertItem(str, arp);
		str.Format(_T("Ӳ����ַ���ȣ�%d"), local_data->arph->ar_hln);
		this->m_treePackage.InsertItem(str, arp);
		str.Format(_T("Э���ַ���ȣ�%d"), local_data->arph->ar_pln);
		this->m_treePackage.InsertItem(str, arp);
		str.Format(_T("�����룺%d"), local_data->arph->ar_op);
		this->m_treePackage.InsertItem(str, arp);

		str.Format(_T("���ͷ�MAC��"));
		for(i = 0; i<6; i++)
		{
			if(i <= 4)
				str.AppendFormat(_T("%02x-"), local_data->arph->ar_srcmac[i]);
			else
				str.AppendFormat(_T("%02x"), local_data->arph->ar_srcmac[i]);
		}
		this->m_treePackage.InsertItem(str, arp);

		str.Format(_T("���ͷ�IP��"), local_data->arph->ar_hln);
		for(i = 0; i<4; i++)
		{
			if(i <= 2)
				str.AppendFormat(_T("%d."), local_data->arph->ar_srcip[i]);
			else
				str.AppendFormat(_T("%d"), local_data->arph->ar_srcip[i]);
		}
		this->m_treePackage.InsertItem(str, arp);

		str.Format(_T("���շ�MAC��"), local_data->arph->ar_hln);
		for(i = 0; i<6; i++)
		{
			if(i <= 4)
				str.AppendFormat(_T("%02x-"), local_data->arph->ar_destmac[i]);
			else
				str.AppendFormat(_T("%02x"), local_data->arph->ar_destmac[i]);
		}
		this->m_treePackage.InsertItem(str, arp);

		str.Format(_T("���շ�IP��"), local_data->arph->ar_hln);
		for(i = 0; i<4; i++)
		{
			if(i <= 2)
				str.AppendFormat(_T("%d."), local_data->arph->ar_destip[i]);
			else
				str.AppendFormat(_T("%d"), local_data->arph->ar_destip[i]);
		}
		this->m_treePackage.InsertItem(str, arp);

	}
	else if(0x0800 == local_data->ethh->type)  //IP
	{

		HTREEITEM ip = this->m_treePackage.InsertItem(_T("IPЭ��ͷ"), data);

		str.Format(_T("�汾��%d"), local_data->iph->version);
		this->m_treePackage.InsertItem(str, ip);
		str.Format(_T("IPͷ����%d"), local_data->iph->ihl);
		this->m_treePackage.InsertItem(str, ip);
		str.Format(_T("�������ͣ�%d"), local_data->iph->tos);
		this->m_treePackage.InsertItem(str, ip);
		str.Format(_T("�ܳ��ȣ�%d"), local_data->iph->tlen);
		this->m_treePackage.InsertItem(str, ip);
		str.Format(_T("��ʶ��0x%02x"), local_data->iph->id);
		this->m_treePackage.InsertItem(str, ip);
		str.Format(_T("��ƫ�ƣ�%d"), local_data->iph->frag_off);
		this->m_treePackage.InsertItem(str, ip);
		str.Format(_T("�����ڣ�%d"), local_data->iph->ttl);
		this->m_treePackage.InsertItem(str, ip);
		str.Format(_T("Э�飺%d"), local_data->iph->proto);
		this->m_treePackage.InsertItem(str, ip);
		str.Format(_T("ͷ��У��ͣ�0x%02x"), local_data->iph->check);
		this->m_treePackage.InsertItem(str, ip);

		str.Format(_T("ԴIP��"));
		struct in_addr in;
		in.S_un.S_addr = local_data->iph->saddr;
		str.AppendFormat(CString(inet_ntoa(in)));
		this->m_treePackage.InsertItem(str, ip);

		str.Format(_T("Ŀ��IP��"));
		in.S_un.S_addr = local_data->iph->daddr;
		str.AppendFormat(CString(inet_ntoa(in)));
		this->m_treePackage.InsertItem(str, ip);

		//�������ICMP��UDP��TCP
		if(1 == local_data->iph->proto)  //ICMP
		{
			HTREEITEM icmp = this->m_treePackage.InsertItem(_T("ICMPЭ��ͷ"), data);

			str.Format(_T("���ͣ�%d"), local_data->icmph->type);
			this->m_treePackage.InsertItem(str, icmp);
			str.Format(_T("���룺%d"), local_data->icmph->code);
			this->m_treePackage.InsertItem(str, icmp);
			str.Format(_T("��ţ�%d"), local_data->icmph->seq);
			this->m_treePackage.InsertItem(str, icmp);
			str.Format(_T("У��ͣ�%d"), local_data->icmph->chksum);
			this->m_treePackage.InsertItem(str, icmp);

		}
		else if(6 == local_data->iph->proto)  //TCP
		{

			HTREEITEM tcp = this->m_treePackage.InsertItem(_T("TCPЭ��ͷ"), data);

			str.Format(_T("  Դ�˿ڣ�%d"), local_data->tcph->sport);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  Ŀ�Ķ˿ڣ�%d"), local_data->tcph->dport);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  ���кţ�0x%02x"), local_data->tcph->seq);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  ȷ�Ϻţ�%d"), local_data->tcph->ack_seq);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  ͷ�����ȣ�%d"), local_data->tcph->doff);

			HTREEITEM flag = this->m_treePackage.InsertItem(_T(" +��־λ"), tcp);

			str.Format(_T("cwr %d"), local_data->tcph->cwr);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("ece %d"), local_data->tcph->ece);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("urg %d"), local_data->tcph->urg);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("ack %d"), local_data->tcph->ack);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("psh %d"), local_data->tcph->psh);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("rst %d"), local_data->tcph->rst);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("syn %d"), local_data->tcph->syn);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("fin %d"), local_data->tcph->fin);
			this->m_treePackage.InsertItem(str, flag);

			str.Format(_T("  ����ָ�룺%d"), local_data->tcph->urg_ptr);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  У��ͣ�0x%02x"), local_data->tcph->check);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  ѡ�%d"), local_data->tcph->opt);
			this->m_treePackage.InsertItem(str, tcp);
		}
		else if(17 == local_data->iph->proto)  //UDP
		{
			HTREEITEM udp = this->m_treePackage.InsertItem(_T("UDPЭ��ͷ"), data);

			str.Format(_T("Դ�˿ڣ�%d"), local_data->udph->sport);
			this->m_treePackage.InsertItem(str, udp);
			str.Format(_T("Ŀ�Ķ˿ڣ�%d"), local_data->udph->dport);
			this->m_treePackage.InsertItem(str, udp);
			str.Format(_T("�ܳ��ȣ�%d"), local_data->udph->len);
			this->m_treePackage.InsertItem(str, udp);
			str.Format(_T("У��ͣ�0x%02x"), local_data->udph->check);
			this->m_treePackage.InsertItem(str, udp);
		}
	}/*
	else if(0x86dd == local_data->ethh->type)  //IPv6
	{
		HTREEITEM ip6 = this->m_treePackage.InsertItem(_T("IPv6Э��ͷ"), data);

		str.Format(_T("�汾��%d"), local_data->iph6->flowtype);
		this->m_treePackage.InsertItem(str, ip6);
		str.Format(_T("�����ͣ�%d"), local_data->iph6->version);
		this->m_treePackage.InsertItem(str, ip6);

		str.Format(_T("����ǩ��%d"), local_data->iph6->flowid);
		this->m_treePackage.InsertItem(str, ip6);
		str.Format(_T("��Ч�غɳ��ȣ�%d"), local_data->iph6->plen);
		this->m_treePackage.InsertItem(str, ip6);
		str.Format(_T("��һ���ײ���0x%02x"), local_data->iph6->nh);
		this->m_treePackage.InsertItem(str, ip6);
		str.Format(_T("�����ƣ�%d"), local_data->iph6->hlim);
		this->m_treePackage.InsertItem(str, ip6);

		str.Format(_T("Դ��ַ��"));
		int n;
		for(n = 0; n<8; n++)
		{
			if(n <= 6)
				str.AppendFormat(_T("%02x:"), local_data->iph6->saddr[n]);
			else
				str.AppendFormat(_T("%02x"), local_data->iph6->saddr[n]);
		}
		this->m_treePackage.InsertItem(str, ip6);

		str.Format(_T("Ŀ�ĵ�ַ��"));
		for (n = 0; n<8; n++)
		{
			if (n <= 6)
				str.AppendFormat(_T("%02x:"), local_data->iph6->saddr[n]);
			else
				str.AppendFormat(_T("%02x"), local_data->iph6->saddr[n]);
		}
		this->m_treePackage.InsertItem(str, ip6);

		//�������ICMPv6��UDP��TCP
		if (0x3a == local_data->iph6->nh)  //ICMPv6
		{
			HTREEITEM icmp6 = this->m_treePackage.InsertItem(_T("ICMPv6Э��ͷ"), data);

			str.Format(_T("���ͣ�%d"), local_data->icmph6->type);
			this->m_treePackage.InsertItem(str, icmp6);
			str.Format(_T("���룺%d"), local_data->icmph6->code);
			this->m_treePackage.InsertItem(str, icmp6);
			str.Format(_T("��ţ�%d"), local_data->icmph6->seq);
			this->m_treePackage.InsertItem(str, icmp6);
			str.Format(_T("У��ͣ�%d"), local_data->icmph6->chksum);
			this->m_treePackage.InsertItem(str, icmp6);
			str.Format(_T("ѡ��-���ͣ�%d"), local_data->icmph6->op_type);
			this->m_treePackage.InsertItem(str, icmp6);
			str.Format(_T("ѡ��-����%d"), local_data->icmph6->op_len);
			this->m_treePackage.InsertItem(str, icmp6);
			str.Format(_T("ѡ��-��·���ַ��"));
			int i;
			for(i = 0; i<6; i++)
			{
				if(i <= 4)
					str.AppendFormat(_T("%02x-"), local_data->icmph6->op_ethaddr[i]);
				else
					str.AppendFormat(_T("%02x"), local_data->icmph6->op_ethaddr[i]);
			}
			this->m_treePackage.InsertItem(str, icmp6);

		}
		else if(0x06 == local_data->iph6->nh)  //TCP
		{

			HTREEITEM tcp = this->m_treePackage.InsertItem(_T("TCPЭ��ͷ"), data);

			str.Format(_T("  Դ�˿ڣ�%d"), local_data->tcph->sport);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  Ŀ�Ķ˿ڣ�%d"), local_data->tcph->dport);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  ���кţ�0x%02x"), local_data->tcph->seq);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  ȷ�Ϻţ�%d"), local_data->tcph->ack_seq);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  ͷ�����ȣ�%d"), local_data->tcph->doff);

			HTREEITEM flag = this->m_treePackage.InsertItem(_T("��־λ"), tcp);

			str.Format(_T("cwr %d"), local_data->tcph->cwr);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("ece %d"), local_data->tcph->ece);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("urg %d"), local_data->tcph->urg);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("ack %d"), local_data->tcph->ack);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("psh %d"), local_data->tcph->psh);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("rst %d"), local_data->tcph->rst);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("syn %d"), local_data->tcph->syn);
			this->m_treePackage.InsertItem(str, flag);
			str.Format(_T("fin %d"), local_data->tcph->fin);
			this->m_treePackage.InsertItem(str, flag);

			str.Format(_T("  ����ָ�룺%d"), local_data->tcph->urg_ptr);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  У��ͣ�0x%02x"), local_data->tcph->check);
			this->m_treePackage.InsertItem(str, tcp);
			str.Format(_T("  ѡ�%d"), local_data->tcph->opt);
			this->m_treePackage.InsertItem(str, tcp);
		}
		else if(0x11 == local_data->iph6->nh)  //UDP
		{
			HTREEITEM udp = this->m_treePackage.InsertItem(_T("UDPЭ��ͷ"), data);

			str.Format(_T("Դ�˿ڣ�%d"), local_data->udph->sport);
			this->m_treePackage.InsertItem(str, udp);
			str.Format(_T("Ŀ�Ķ˿ڣ�%d"), local_data->udph->dport);
			this->m_treePackage.InsertItem(str, udp);
			str.Format(_T("�ܳ��ȣ�%d"), local_data->udph->len);
			this->m_treePackage.InsertItem(str, udp);
			str.Format(_T("У��ͣ�0x%02x"), local_data->udph->check);
			this->m_treePackage.InsertItem(str, udp);
		}
	}*/

	return 1;
}


//ȷ��������ѡ���ѡ��
void CSimpleSnifferDlg::OnCbnSelchangeNetworkCombo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	int m_LastSel = 1;

	int index = m_comboNetwork.GetCurSel();
	afxDump << index << "\n";
	if (index == 0)
	{
		m_comboNetwork.SetCurSel(1);
	}
	else
	{
		m_LastSel = index;
	}
}
