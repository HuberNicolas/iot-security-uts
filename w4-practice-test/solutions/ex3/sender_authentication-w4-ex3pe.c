/*****************Header files inclusion*****************************/
#include "sha256.h"  //Hashing algorithm which is used to preserve the integrity of data that are transported between the sender and the receiver.
#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "net/uip.h" //Header file for the uIP TCP/IP stack.The uIP TCP/IP stack is intended to make it possible to communicate using the TCP/IP protocol suite even on small 8-bit micro-controllers.
                        //The uIP TCP/IP stack header file contains definitions for a number of C macros that are used by uIP programs as well as internal uIP structures, TCP/IP header structures 
                         // and function declarations.
#include "net/uip-ds6.h"
#include "net/rpl/rpl.h" //This header file contains the routing related information which is used to build DAG(Directed Acyclic Graph) 

#include "net/netstack.h" //Include file for the Contiki low-layer network stack (NETSTACK)
#include "net/uip-udp-packet.h"
#include "sys/ctimer.h"
#include <stdio.h>
#include <string.h>

/*****************Header files inclusion*****************************/



/*****************MACRO definitions*****************************/
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define UDP_EXAMPLE_ID  190

#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"  //A set of debugging macros for the IP stack


#ifndef PERIOD
#define PERIOD 60
#endif

#define START_INTERVAL		(15 * CLOCK_SECOND)
#define SEND_INTERVAL		(PERIOD * CLOCK_SECOND)
#define SEND_TIME		(random_rand() % (SEND_INTERVAL))
#define MAX_PAYLOAD_LEN		1000

/*****************MACRO definitions*****************************/



/*****************Variable declarations*****************************/
char message[100] = "Secret"; // original message was changed from confidential to Secret
char cipher[100]; // used to store the encrypted message
int key = 6; // this was changed from 5 to 6

static struct uip_udp_conn *client_conn;
static uip_ipaddr_t server_ipaddr;

const char text1[100]={"IoTLab"}; // the client's identity was changed from client1 to IoTLab
const char hash[32]; // used to store hash
SHA256_CTX ctx;

/*****************Variable declarations*****************************/


/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client process");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/


/*****************Function definitions*****************************/

static void
send_packet()
{
  static int seq_id;
  char buf[MAX_PAYLOAD_LEN];
  int idx;

   sha256_init(&ctx);
   sha256_update(&ctx,text1,strlen(text1));
   sha256_final(&ctx,hash);

  printf("Sending hash...\n");
  printf("Hash sent : ");
  char hash_final[32];
  for (idx=0; idx < 32; idx++){
        hash_final[idx] = hash[idx];
	PRINTF("%x",hash[idx]);}
        
  printf("\n");

  sprintf(buf,"%s",hash_final); // stores the output in the specified char array

  uip_udp_packet_sendto(client_conn, buf, strlen(buf),
                        &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
}

static void
tcpip_handler()
{
  char *str;

  if(uip_newdata()) {
    str = uip_appdata;
    str[uip_datalen()] = '\0';
    }
    printf("Server Respone received is '%s'\n", str);

  if(strcmp(str,"ACK")==0){
	send_epacket();
	}
  else if (strcmp(str,"NACK")==0){
	printf("Trying to resend");
        send_packet();
	}

}
//////////////////////////////////////////////////////////////////

void send_epacket() // this function is used to send the encrypted message
{
  static int seq_id;
  char buf[MAX_PAYLOAD_LEN];
  printf("Encrypting.......\n");
  encryptc();
  printf("Sending encrypted messsage....\n");
  sprintf(buf, "%s",cipher);
  uip_udp_packet_sendto(client_conn, buf, strlen(buf),
                        &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));

  printf("Message sent \n");
}
///////////////////////////////////////////////////////////////////

void encryptc()
{
    char ch;
    int i;
    
    for(i = 0; message[i] != '\0'; ++i){
        ch = message[i];
	// space
	if (ch == 32){
            cipher[i] = 32;
        }
        
        // lowercase character
        if(ch >= 'a' && ch <= 'z'){
            ch = ch + key;
            
            if(ch > 'z'){
                ch = ch - 'z' + 'a' - 1;
            }
            cipher[i] = ch;
        }
        // uppercase character
        else if(ch >= 'A' && ch <= 'Z'){
            ch = ch + key;
            
            if(ch > 'Z'){
                ch = ch - 'Z' + 'A' - 1;
            }
	    cipher[i] = ch;
        }
    }
 
}
/////////////////////////////////////////////////////////////////////

//refer contiki/core/net/ipv6/uip-ds6.h, uip-ds6.c files
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Client IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) { //UIP_DS6_ADDR_NB--unicast address list
    state = uip_ds6_if.addr_list[i].state;  //Used by Cooja to enable extraction of addresses from memory
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
      /* hack to make address "final" */
      if (state == ADDR_TENTATIVE) {
	uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
      }
    }
  }
}
//////////////////////////////////////////////////////////////////

static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;

  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0); //Construct an IPv6 address from eight 16-bit words. 
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr); //set the last 64 bits of an IP address based on the MAC address
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

/* The choice of server address determines its 6LoPAN header compression.
 * (Our address will be compressed Mode 3 since it is derived from our link-local address)
 * Obviously the choice made here must also be selected in udp-server.c.
 *
 * For correct Wireshark decoding using a sniffer, add the /64 prefix to the 6LowPAN protocol preferences,
 * e.g. set Context 0 to aaaa::.  At present Wireshark copies Context/128 and then overwrites it.
 * (Setting Context 0 to aaaa::1111:2222:3333:4444 will report a 16 bit compressed address of aaaa::1111:22ff:fe33:xxxx)
 *
 * Note the IPCMV6 checksum verification depends on the correct uncompressed addresses.
 */
 
#if 0
/* Mode 1 - 64 bits inline */
   uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
#elif 1
/* Mode 2 - 16 bits inline */
  uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0, 0x00ff, 0xfe00, 1);
#else
/* Mode 3 - derived from server link-local (MAC) address */
  uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0x0250, 0xc2ff, 0xfea8, 0xcd1a); //redbee-econotag
#endif
}

//////////////////////////////////////////////////////////////////////////

PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic;
  static struct ctimer backoff_timer;

  PROCESS_BEGIN();

  PROCESS_PAUSE();  //This macro yields the currently running process for a short while, thus letting other processes run before the process continues.

  set_global_address();
  
  PRINTF("UDP client process started\n");

  print_local_addresses();

  /* new connection with remote host */
  client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_PORT), NULL); 
  if(client_conn == NULL) {
    PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(client_conn, UIP_HTONS(UDP_CLIENT_PORT)); 

  PRINTF("Created a connection with the server ");
  PRINT6ADDR(&client_conn->ripaddr);
  PRINTF(" local/remote port %u/%u\n",
	UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));//UIP_HTONS--Convert a 16-bit quantity from host byte order to network byte order. 


  etimer_set(&periodic, SEND_INTERVAL);
  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    }
    
    if(etimer_expired(&periodic)) {
      etimer_reset(&periodic);
      ctimer_set(&backoff_timer, SEND_TIME, send_packet, NULL);

    }
  }

  PROCESS_END();
}

/*****************Function definitions*****************************/


