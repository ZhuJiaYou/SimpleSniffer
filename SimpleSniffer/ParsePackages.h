#include "afxcmn.h"
#include "afxwin.h"
#include <malloc.h> 
#include <Winsock2.h>
#include "DataStructures.h"

#ifndef UTILITIES_H
#define UTILITIES_H

/*链路层包*/
int analyze_frame(const u_char *pkt, struct datapkt *data, struct pktcount *npakcet);

/*网络层包*/
int analyze_ip(const u_char *pkt, struct datapkt *data, struct pktcount *npakcet);
//int analyze_ip6(const u_char *pkt, struct datapkt *data, struct pktcount *npakcet);
int analyze_arp(const u_char *pkt, struct datapkt *data, struct pktcount *npakcet);

/*传输层包*/
//通常认为icmp是ip的一部份，但它像tcp和udp一样作为ip包的有效载荷的形式在网络中传输，所以这里当作传输层处理
int analyze_icmp(const u_char *pkt, struct datapkt *data, struct pktcount *npakcet);
//int analyze_icmp6(const u_char *pkt, struct datapkt *data, struct pktcount *npakcet);
int analyze_tcp(const u_char *pkt, struct datapkt *data, struct pktcount *npakcet);
int analyze_udp(const u_char *pkt, struct datapkt *data, struct pktcount *npakcet);

/*应用层包*/
//int analyze_http(const u_char *pkt, struct datapkt *data, struct pktcount *npakcet);

/*将数据包以十六进制的方式打印*/
void print_packet_hex(const u_char *pkt, int size_pkt, CString *buf);

#endif