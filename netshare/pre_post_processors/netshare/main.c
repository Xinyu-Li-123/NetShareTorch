// https://www.binarytides.com/packet-sniffer-code-c-libpcap-linux-sockets/
// http://tonylukasavage.com/blog/2010/12/19/offline-packet-capture-analysis-with-c-c----amp--libpcap/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <tgmath.h>
#include <x86intrin.h>

/*
pcap and network related
*/
#include <pcap/pcap.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include "packet.h"

#define ETHER_HDR_TRUNCATE 0

int tcp = 0, udp = 0, icmp = 0, others = 0, total = 0;

void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    struct ether_header *ep;
    unsigned short ether_type;

    // read Ethernet header if not truncated
    if (!ETHER_HDR_TRUNCATE)
    {
        ep = (struct ether_header *)packet;

        // protocol type
        ether_type = ntohs(ep->ether_type);

        // IPv4
        if (ether_type == ETHERTYPE_IP)
        {
            packet += ETHER_HDR_LEN;
        }

        // 802.1Q
        else if (ether_type == ETHERTYPE_VLAN)
        {
            ether_type = ntohs(*(uint16_t *)(packet + 16));

            // only process IP packet
            if (ether_type == ETHERTYPE_IP)
            {
                packet += ETHER_HDR_LEN;
                packet += 4;
            }

            else
                return;
        }
    }

    Packet p;

    /* Timestamp */
    unsigned long time_in_micros = pkthdr->ts.tv_sec * 1000000 + pkthdr->ts.tv_usec;
    p.timestamp = time_in_micros;
    // printf("timestamp: %lu\n", time_in_micros);

    /* IP header */
    // Note: caida traces does not include ethernet headers
    // data_center (IMC 2010): enternet headers, YES
    // MACCDC_2012
    const struct ip *ipHeader;
    char sourceIp[INET_ADDRSTRLEN];
    char destIp[INET_ADDRSTRLEN];

    ipHeader = (struct ip *)(packet);

    inet_ntop(AF_INET, &(ipHeader->ip_src), sourceIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->ip_dst), destIp, INET_ADDRSTRLEN);

    struct in_addr tmpPkt1, tmpPkt2;
    inet_aton(sourceIp, &tmpPkt1);
    inet_aton(destIp, &tmpPkt2);

    p.srcip = ntohl(tmpPkt1.s_addr);
    p.dstip = ntohl(tmpPkt2.s_addr);

    p.ip_hl = (unsigned int)ipHeader->ip_hl;
    p.ip_v = (unsigned int)ipHeader->ip_v;
    p.ip_tos = (uint8_t)ipHeader->ip_tos;
    p.ip_len = ntohs(ipHeader->ip_len);
    p.ip_id = ntohs(ipHeader->ip_id);
    p.ip_off = ntohs(ipHeader->ip_off);
    p.ip_ttl = (uint8_t)ipHeader->ip_ttl;
    p.ip_p = (uint8_t)ipHeader->ip_p;
    p.ip_sum = ntohs(ipHeader->ip_sum);

    // TCP/UDP
    total++;
    switch (ipHeader->ip_p)
    {
    // ICMP Protocol
    case 1:
        icmp++;
        break;

    // TCP Protocol
    case 6:
        tcp++;

        struct tcphdr *tcpHeader = (struct tcphdr *)(packet + p.ip_hl * 4);
        p.srcport = ntohs(tcpHeader->th_sport);
        p.dstport = ntohs(tcpHeader->th_dport);
        // printf("%hu, %hu\n", p.srcport, p.dstport);
        break;

    // UDP Protocol
    case 17:
        udp++;
        struct udphdr *udpHeader = (struct udphdr *)(packet + p.ip_hl * 4);
        p.srcport = ntohs(udpHeader->uh_sport);
        p.dstport = ntohs(udpHeader->uh_dport);
        // printf("%hu, %hu\n", p.srcport, p.dstport);
        break;

    default:
        others++;
        break;
    }

    // printf("%u, %u, %hu, %hu, %u, %lu, %hu, %u, %u, %u, %hu, %u, %hu\n", p.srcip, p.dstip, p.srcport, p.dstport, p.ip_p, p.timestamp, p.ip_len, p.ip_v, p.ip_hl, p.ip_tos, p.ip_id, p.ip_ttl, p.ip_sum);

    trace_pkts = (Packet *)realloc(trace_pkts, (trace_count + 1) * sizeof(Packet));
    trace_pkts[trace_count] = p;

    trace_count++;
}

void pcapParser(char *fileName)
{
    pcap_t *descr;
    char errbuf[PCAP_ERRBUF_SIZE];

    // open trace file for offline processing
    printf("Pre-process pcap file %s\n", fileName);
    trace_count = 0;

    descr = pcap_open_offline(fileName, errbuf);

    if (descr == NULL)
    {
        printf("[FILE ERROR] pcap_open_live() failed: \n");
    }

    // start packet processing loop, just like live capture
    if (pcap_loop(descr, 0, packetHandler, NULL) < 0)
    {
        printf("pcap_loop() failed: %s\n", pcap_geterr(descr));
    }

    printf("This pcap chunk reading is done... total %d packets \n", trace_count);
}

int pcap2csv(char *pcapFile, char *csvFile)
{
    printf("pcap file: %s\n", pcapFile);
    printf("csv file: %s\n", csvFile);

    pcapParser(pcapFile);
    printf("TCP: %d, UDP: %d, ICMP: %d, Others: %d, total: %d\n", tcp, udp, icmp, others, total);

    FILE *fp;
    fp = fopen(csvFile, "w+");

    fprintf(fp, "srcip,dstip,srcport,dstport,proto,time,pkt_len,version,ihl,tos,id,flag,off,ttl,chksum\n");

    for (int i = 0; i < trace_count; i++)
    {
        Packet p = trace_pkts[i];

        unsigned short int ip_flag = p.ip_off >> 13;
        unsigned short int ip_off = p.ip_off & IP_OFFMASK;

        char proto[128];
        if (p.ip_p == 6)
        {
            strcpy(proto, "TCP");
            printf("TCP packet\n");
        }
        else if (p.ip_p == 17)
        {
            strcpy(proto, "UDP");
            printf("UDP packet\n");
        }
        else
        {
            // skip this packet and print the p.ip_p
            printf("p.ip_p = %u, not pcap or udp, skipped\n", p.ip_p);
            // printf("Not TCP/UDP packet, skipped!\n");
            continue;
        }

        // switch (ip_p) {
        //     case 0: strcpy(proto, "HOPOPT"); break;
        //     case 1: strcpy(proto, "ICMP"); break;
        //     case 2: strcpy(proto, "IGMP"); break;
        //     case 3: strcpy(proto, "GGP"); break;
        //     case 4: strcpy(proto, "IPv4"); break;
        //     case 5: strcpy(proto, "ST"); break;
        //     case 6: strcpy(proto, "TCP"); break;
        //     case 7: strcpy(proto, "CBT"); break;
        //     case 8: strcpy(proto, "EGP"); break;
        //     case 9: strcpy(proto, "IGP"); break;
        //     case 10: strcpy(proto, "BBN-RCC-MON"); break;
        //     case 11: strcpy(proto, "NVP-II"); break;
        //     case 12: strcpy(proto, "PUP"); break;
        //     case 13: strcpy(proto, "ARGUS (deprecated)"); break;
        //     case 14: strcpy(proto, "EMCON"); break;
        //     case 15: strcpy(proto, "XNET"); break;
        //     case 16: strcpy(proto, "CHAOS"); break;
        //     case 17: strcpy(proto, "UDP"); break;
        //     case 18: strcpy(proto, "MUX"); break;
        //     case 19: strcpy(proto, "DCN-MEAS"); break;
        //     case 20: strcpy(proto, "HMP"); break;
        //     case 21: strcpy(proto, "PRM"); break;
        //     case 22: strcpy(proto, "XNS-IDP"); break;
        //     case 23: strcpy(proto, "TRUNK-1"); break;
        //     case 24: strcpy(proto, "TRUNK-2"); break;
        //     case 25: strcpy(proto, "LEAF-1"); break;
        //     case 26: strcpy(proto, "LEAF-2"); break;
        //     case 27: strcpy(proto, "RDP"); break;
        //     case 28: strcpy(proto, "IRTP"); break;
        //     case 29: strcpy(proto, "ISO-TP4"); break;
        //     case 30: strcpy(proto, "NETBLT"); break;
        //     case 31: strcpy(proto, "MFE-NSP"); break;
        //     // ... (and so on for the rest of the cases)
        //     case 123: strcpy(proto, "PTP"); break;
        //     case 124: strcpy(proto, "ISIS over IPv4"); break;
        //     case 125: strcpy(proto, "FIRE"); break;
        //     case 126: strcpy(proto, "CRTP"); break;
        //     case 127: strcpy(proto, "CRUDP"); break;
        //     case 128: strcpy(proto, "SSCOPMCE"); break;
        //     case 129: strcpy(proto, "IPLT"); break;
        //     case 130: strcpy(proto, "SPS"); break;
        //     // ... (and so on for the rest of the cases)
        //     case 255: strcpy(proto, "Reserved"); break;
        //     default: strcpy(proto, "Unknown"); break;
        // }

        fprintf(fp, "%u,%u,%hu,%hu,%s,%lu,%hu,%u,%u,%u,%hu,%hu,%hu,%u,%hu\n", p.srcip, p.dstip, p.srcport, p.dstport, proto, p.timestamp, p.ip_len, p.ip_v, p.ip_hl, p.ip_tos, p.ip_id, ip_flag, ip_off, p.ip_ttl, p.ip_sum);

    }

    fclose(fp);

    return 0;
}