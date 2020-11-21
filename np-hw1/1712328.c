// /*
//   Usage:
//     1. ./studenID <target hostname/IP>/<Number bit of netID>
// */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netpacket/packet.h>

/* Ethernet frame header length */
#define ETHER_HEADER_LEN sizeof(struct ether_header)
/*The length of the entire arp structure */
#define ETHER_ARP_LEN sizeof(struct ether_arp)
/*Ethernet + entire arp structure length */
#define ETHER_ARP_PACKET_LEN ETHER_HEADER_LEN + ETHER_ARP_LEN
/*IP address length */
#define IP_ADDR_LEN 4
/*Broadcast address */
#define BROADCAST_ADDR {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

char* local_ip;

void err_exit(const char *err_msg)
{
  perror(err_msg);
  exit(1);
}

int power(int base, int power)
{
    int res = 1;
    for (int i = 0; i < power; i++)
        res *= base;
    return res;
}

int parse_calc_rangehost(char* agrument) 
{
   char * token = strtok(agrument, "/");
   token = strtok(NULL, " ");
   int range_host = 0;
   for (int i = 0; i < (32-atoi(token)); i++)
	   range_host += power(2,i);
   return range_host;
}

char* parse_dest_host_name(char* agrument)
{
   char * token = strtok(agrument, "/");
   token[strlen(token)-1] = '\0';
   return token;	
}

// ====================================================================================
/*  Fill the arp package */
struct ether_arp *fill_arp_packet(const unsigned char *src_mac_addr, const char *src_ip, const char *dst_ip)
{
  struct ether_arp *arp_packet;
  struct in_addr src_in_addr, dst_in_addr;
  unsigned char dst_mac_addr[ETH_ALEN] = BROADCAST_ADDR;
  
  /*  IP address translation */
  inet_pton(AF_INET, src_ip, &src_in_addr);
  inet_pton(AF_INET, dst_ip, &dst_in_addr);
  
  /*  The entire arp package */
  arp_packet = (struct ether_arp *)malloc(ETHER_ARP_LEN);
  arp_packet->arp_hrd = htons(ARPHRD_ETHER);
  arp_packet->arp_pro = htons(ETHERTYPE_IP);
  arp_packet->arp_hln = ETH_ALEN;
  arp_packet->arp_pln = IP_ADDR_LEN;
  arp_packet->arp_op = htons(ARPOP_REQUEST);
  memcpy(arp_packet->arp_sha, src_mac_addr, ETH_ALEN);
  memcpy(arp_packet->arp_tha, dst_mac_addr, ETH_ALEN);
  memcpy(arp_packet->arp_spa, &src_in_addr, IP_ADDR_LEN);
  memcpy(arp_packet->arp_tpa, &dst_in_addr, IP_ADDR_LEN);
  
  return arp_packet;
}

void arp_request(const char *dst_ip, FILE* res_txt)
{
  struct sockaddr_ll saddr_ll;
  struct ether_header *eth_header;
  struct ether_arp *arp_packet;
  struct ifreq ifr;
  char buf[ETHER_ARP_PACKET_LEN];
  unsigned char src_mac_addr[ETH_ALEN];
  unsigned char dst_mac_addr[ETH_ALEN] = BROADCAST_ADDR;
  char *src_ip;
  int sock_raw_fd, ret_len;
  
  char* if_name = (char *) malloc (40 * sizeof (char));
  memset(if_name,0, 40 * sizeof (char));
  strcpy (if_name, "ens33");
  
  if ((sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1)
    err_exit("[#] socket() failed");
    
  bzero(&saddr_ll, sizeof(struct sockaddr_ll));
  bzero(&ifr, sizeof(struct ifreq));
  
  /*NIC interface name */
  memcpy(ifr.ifr_name, if_name, strlen(if_name));

  /*Get NIC interface index */
  if (ioctl(sock_raw_fd, SIOCGIFINDEX, &ifr) == -1)
    err_exit("[#] ioctl() get ifindex failed");
  
  saddr_ll.sll_ifindex = ifr.ifr_ifindex;
  saddr_ll.sll_family = PF_PACKET;

  /*Get NIC interface IP */
  if (ioctl(sock_raw_fd, SIOCGIFADDR, &ifr) == -1)
    err_exit("[#] ioctl() get ip failed");
  
  src_ip = inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr);
  strcpy(local_ip, src_ip);

  /*Get the MAC address of the NIC interface */
  if (ioctl(sock_raw_fd, SIOCGIFHWADDR, &ifr))
  err_exit("[#] octl() get mac failed");
  memcpy(src_mac_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

  bzero(buf, ETHER_ARP_PACKET_LEN);

  /*Fill the ether header */
  eth_header = (struct ether_header *)buf;
  memcpy(eth_header->ether_shost, src_mac_addr, ETH_ALEN);
  memcpy(eth_header->ether_dhost, dst_mac_addr, ETH_ALEN);
  eth_header->ether_type = htons(ETHERTYPE_ARP);
  
  /*arp package */
  arp_packet = fill_arp_packet(src_mac_addr, src_ip, dst_ip);
  memcpy(buf + ETHER_HEADER_LEN, arp_packet, ETHER_ARP_LEN);
  
  /*send request*/
  ret_len = sendto(sock_raw_fd, buf, ETHER_ARP_PACKET_LEN, 0, (struct sockaddr *)&saddr_ll, sizeof(struct sockaddr_ll));
  if ( ret_len > 0)
    printf("[*] Send %d\n", ret_len);
  
  free(if_name);
  close(sock_raw_fd);
}

void arp_reply(FILE* res_txt){
  struct ether_arp *arp_packet;
  char buf[ETHER_ARP_PACKET_LEN];
  int sock_raw_fd, ret_len, i;

  struct timeval start, end;
  long mtime, seconds, useconds;
  
  if ((sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1)
      err_exit("[#] socket() failed");

  while (1)
  {
    bzero(buf, ETHER_ARP_PACKET_LEN);
    ret_len = recv(sock_raw_fd, buf, ETHER_ARP_PACKET_LEN, 0);

    if (ret_len > 0)
    {
      arp_packet = (struct ether_arp *)(buf + ETHER_HEADER_LEN);
      if (ntohs(arp_packet->arp_op) == 2)
      {
        printf("[*] Detected: ", ret_len);
        for (i = 0; i < IP_ADDR_LEN; i++){
          printf(".%u", arp_packet->arp_spa[i]);
          if (i == 3)
            fprintf(res_txt, "%u",arp_packet->arp_spa[i]);
          else
            fprintf(res_txt, "%u.",arp_packet->arp_spa[i]);
        }
        fprintf(res_txt,"\n");
        printf("\n");
        break;
      }
    }
    else continue;
    break;
  }
  close(sock_raw_fd);
}

int main(int argc, const char *argv[]) {
  if (argc != 2)
  {
    printf("Usage: %s <Address ID>/<Number of NetID>\n", argv[0]);
    exit(1);
  }

  // Open file
  FILE* res_txt = fopen("1712328.txt", "w");
  if(!res_txt){
    err_exit("[#] Unable to open 1712328.txt\n");
    exit (EXIT_FAILURE);
	}

  local_ip = (char*) malloc(16*sizeof(char));

  // Pharse agrument
  char* temp = (char*) malloc(15*sizeof(char));
  strcpy(temp, argv[1]);
  char* dest_host_name = parse_dest_host_name(argv[1]);

  int host_range = parse_calc_rangehost(temp);

  int in_len = strlen(dest_host_name)+1;
	for (int i = 1; i <= host_range; i++)
	{
		char* buff = (char*) malloc (4* sizeof(char));
		sprintf(buff, "%d", i);
		dest_host_name = strcat(dest_host_name, buff);
		printf("%s\n", dest_host_name);

    arp_request(dest_host_name, res_txt);
    arp_reply(res_txt);

    if (strlen(dest_host_name) == in_len)
		{
			int len = strlen(dest_host_name)-1;
			memmove(&dest_host_name[len], &dest_host_name[len + 1], strlen(dest_host_name) - len);
		} else if (strlen(dest_host_name) - 1 == in_len)
		{
			int len = strlen(dest_host_name)-1;
			memmove(&dest_host_name[len], &dest_host_name[len + 1], strlen(dest_host_name) - len);
			len = strlen(dest_host_name)-1;
			memmove(&dest_host_name[len], &dest_host_name[len + 1], strlen(dest_host_name) - len);
		} else
		{
			int len = strlen(dest_host_name)-1;
			memmove(&dest_host_name[len], &dest_host_name[len + 1], strlen(dest_host_name) - len);
			len = strlen(dest_host_name)-1;
			memmove(&dest_host_name[len], &dest_host_name[len + 1], strlen(dest_host_name) - len);
			len = strlen(dest_host_name)-1;
			memmove(&dest_host_name[len], &dest_host_name[len + 1], strlen(dest_host_name) - len);
		}
		free(buff);
	}
  fprintf(res_txt,"%s\n", local_ip);
  free(local_ip);
  fclose(res_txt);
  return 0;
}