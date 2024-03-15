/*****************Header files inclusion*****************************/

#include "sha256.h" //Hashing algorithm which is used to preserve the integrity of data that are transported between the sender and the receiver.
#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/uip.h" //Header file for the uIP TCP/IP stack.The uIP TCP/IP stack is intended to make it possible to communicate using the TCP/IP protocol suite even on small 8-bit micro-controllers.
                        //The uIP TCP/IP stack header file contains definitions for a number of C macros that are used by uIP programs as well as internal uIP structures, TCP/IP header structures 
                         // and function declarations.
#include "net/rpl/rpl.h" //This header file contains the routing related information which is used to build DAG(Directed Acyclic Graph) 

#include "net/netstack.h" //Include file for the Contiki low-layer network stack (NETSTACK)
#include "dev/button-sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/*****************Header files inclusion*****************************/

/*****************MACRO definitions*****************************/

#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h" //A set of debugging macros for the IP stack

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN]) //*Defines the upper layer length ie IP fragment length. This length must be compatible with the lower layer link header length. Also used to  Check validity of the IP header.For more details, navigate to contiki/core/net/ipv6/uip6.c */

#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

/*****************MACRO definitions*****************************/


/*****************Variable declarations*****************************/
char *appdata;

const char text1[100]={"client1"}; // the client's identity
const char hash[32]; // used to store hash
int key = 5; // key for Caesar cipher algorithm

SHA256_CTX ctx;
int reply=0;
int flag=0;

static char cipher[100]; // encrypted message sent from the sender
static char cipher_decrypt[100]; // decrypted message

static struct uip_udp_conn *server_conn;  //Representation of a uIP UDP connection. The current UDP connection.

/*****************Variable declarations*****************************/

//Process declaration
PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);


/*****************Function definitions*****************************/


/////////////////////////////////////////////////////////////////////

void calculate_hash()
{

sha256_init(&ctx);
sha256_update(&ctx,text1,strlen(text1));
sha256_final(&ctx,hash);
char hash_final[32];
int idx;
for (idx=0; idx < 32; idx++){
        hash_final[idx] = hash[idx];}
compare_hash(hash_final);
}
//////////////////////////////////////////////////////////////////

void compare_hash(unsigned char hash[])
{
   int idx;
   char string[80];
   for (idx=0; idx < 32; idx++)
	string[idx]=hash[idx];
   if(strncmp(string,appdata,8)==0)
	//printf("\nAuthenticated!!!\n");
	reply = 1;
   else
	//printf("\nNot Authenticated!!!");
	reply = 0;

}

////////////////////////////////////////////////////////////////////

void receive_hash()
{
 printf("receiving...\n");
 PRINTF("Received hash from client.\n");

 printf("Processing...\n");
 calculate_hash();
  
 PRINTF("Server sending reply....\n");
 uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);

 if(reply==1){
    uip_udp_packet_send(server_conn,"ACK",sizeof("ACK")); //send ACK message to the client
    flag++;} //increase flag to receive the encrypted message
 else
    uip_udp_packet_send(server_conn,"NACK",sizeof("NACK")); // send NACK message to the client

 uip_create_unspecified(&server_conn->ripaddr);
}

/////////////////////////////////////////////////////////////////////////

void receive_msg() // receive the encrypted message
{	
char *data = appdata;
int len = strlen(data);

printf("Receiving...\n");
int i = 0;
while(i<=len)
{
	cipher[i] = data[i];
	i++;
}
cipher[i] = '\0'; // '\0' is zero character. In C it is mostly used to indicate the termination of a character string
printf("Message Received : %s\n",cipher);

printf("Decrypting.......\n");
decryptd();

printf("Decrypted message : %s \n",cipher_decrypt);

PRINTF("Server sending reply....\n");
    uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr); //Copying source ip address to remote ip address
    uip_udp_packet_send(server_conn,"Msg Received",sizeof("Msg Received")); //sending UDP packets through uIP
    uip_create_unspecified(&server_conn->ripaddr); //set IP address a to unspecified 
flag++;
}

///////////////////////////////////////////////////////////////////

void decryptd()
{
   
    char ch;
    int i;

    for(i = 0; cipher[i] != '\0'; i++){
        ch = cipher[i];
	// space
        if (ch == 32){
        cipher_decrypt[i] = 32;
        }
	// lowercase character
        if(ch >= 'a' && ch <= 'z'){
            ch = ch - key;
            
            if(ch < 'a'){
                ch = ch + 'z' - 'a' + 1;
            }
            cipher_decrypt[i] = ch;
        }
	// uppercase character
        else if(ch >= 'A' && ch <= 'Z'){
            ch = ch - key;
            
            if(ch < 'A'){
                ch = ch + 'Z' - 'A' + 1;
            }
            
            cipher_decrypt[i] = ch;
	    
        }
    }
}
/////////////////////////////////////////////////////////////////////


static void tcpip_handler(void)
{
  if(uip_newdata()) { //Is new incoming data available?
    appdata = (char *)uip_appdata; // new data for the application present at the uip_appdata pointer. The size of the data is available through the uip_len variable.
    appdata[uip_datalen()] = 0;
	 }
  if(flag%2==0) // % is the modulus operator. It means the remainder of flag devided by 2. We use this operator to check whether the received message is the hash value or the encrypted message.
	receive_hash();
  else
	receive_msg();
}
/////////////////////////////////////////////////////////////////////////////////
//refer contiki/core/net/ipv6/uip-ds6.h, uip-ds6.c files
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Server IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) { //UIP_DS6_ADDR_NB--unicast address list
    state = uip_ds6_if.addr_list[i].state; //Used by Cooja to enable extraction of addresses from memory
    if(state == ADDR_TENTATIVE || state == ADDR_PREFERRED) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
      /* hack to make address "final" */
      if (state == ADDR_TENTATIVE) {
	uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////


PROCESS_THREAD(udp_server_process, ev, data) //Define the body of a process.The process is called whenever an event occurs in the system.The auxiliary data to be sent with the event
{
  uip_ipaddr_t ipaddr; //specifying IP address that uIP handles internally
  struct uip_ds6_addr *root_if; //Unicast address structure.A unicast address is an address that identifies a unique node on a network. Unicast addressing is available in IPv4 and IPv6 and typically refers to a single sender or a single receiver, although it can be used in both sending and receiving

  PROCESS_BEGIN();

  PROCESS_PAUSE(); //This macro yields the currently running process for a short while, thus letting other processes run before the process continues.

  SENSORS_ACTIVATE(button_sensor);

  PRINTF("UDP server started\n");

#if UIP_CONF_ROUTER
/* The choice of server address determines its 6LoPAN header compression.
 * Obviously the choice made here must also be selected in udp-client.c.
 *
 * For correct Wireshark decoding using a sniffer, add the /64 prefix to the 6LowPAN protocol preferences,
 * e.g. set Context 0 to aaaa::.  At present Wireshark copies Context/128 and then overwrites it.
 * (Setting Context 0 to aaaa::1111:2222:3333:4444 will report a 16 bit compressed address of aaaa::1111:22ff:fe33:xxxx)
 * Note Wireshark's IPCMV6 checksum verification depends on the correct uncompressed addresses.
 */
 
#if 0
/* Mode 1 - 64 bits inline */
   uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
#elif 1
/* Mode 2 - 16 bits inline */
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0x00ff, 0xfe00, 1); //Construct an IPv6 address from eight 16-bit words. 
#else
/* Mode 3 - derived from link local (MAC) address */
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr); //set the last 64 bits of an IP address based on the MAC address
#endif

// check whether the ADDR_MANUAL was set succefuly or not
  uip_ds6_addr_add(&ipaddr, 0, ADDR_MANUAL);
  root_if = uip_ds6_addr_lookup(&ipaddr);
  if(root_if != NULL) {
    rpl_dag_t *dag;
    dag = rpl_set_root(RPL_DEFAULT_INSTANCE,(uip_ip6addr_t *)&ipaddr); //set the ip adress of server as the root of initial DAG 
    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0); //Construct an IPv6 address from eight 16-bit words. 
    rpl_set_prefix(dag, &ipaddr, 64);
    PRINTF("created a new RPL dag\n");
  } else {
    PRINTF("failed to create a new RPL DAG\n");
  }
#endif /* UIP_CONF_ROUTER */
  
  print_local_addresses();

  /* The data sink runs with a 100% duty cycle in order to ensure high 
     packet reception rates. */
  NETSTACK_MAC.off(1);

  server_conn = udp_new(NULL, UIP_HTONS(UDP_CLIENT_PORT), NULL);//new connection with remote host
  if(server_conn == NULL) {
    PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(server_conn, UIP_HTONS(UDP_SERVER_PORT));//UIP_HTONS--Convert a 16-bit quantity from host byte order to network byte order. 

  PRINTF("Created a server connection with remote address ");
  PRINT6ADDR(&server_conn->ripaddr);
  PRINTF(" local/remote port %u/%u\n", UIP_HTONS(server_conn->lport),
         UIP_HTONS(server_conn->rport));


  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    } else if (ev == sensors_event && data == &button_sensor) {
      PRINTF("Initiaing global repair\n");
      rpl_repair_root(RPL_DEFAULT_INSTANCE);
    }
  }

  PROCESS_END();
}


/*****************Function definitions*****************************/

