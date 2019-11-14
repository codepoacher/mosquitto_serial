/*
 * main.c
 *
 *  Created on: Oct 18, 2018
 *      Author: majian
 */
#include <sys/types.h>  
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "udemo.h"

int NoDetach = 0;
int main (int argc , char *argv[])
{
	int opt;
	char tmp[8];
	int cpid;
	//./udemod -d /dev/ttysWK0 -o 8N1 -b 115200 -u user -P admin -h 192.168.10.236 -p 1883 -t /iot_pub -I123456 -S TRUE -Q 0 -k 60 -R FALSE -T 1000 -m 1234 -q /iot_sub -a 0 -N
	while ((opt = getopt(argc, argv, "Nd:o:b:u:P:h:p:t:S:Q:k:R:T:m:a:s:f:g:I::q::w::e::r::")) != -1) 
	{                                                                  
		switch (opt)                                                    
		{                                                               
			case 'd':   //-d "/dev/ttyUSB485" 或"/dev/ttyUSB232"
				tty_ctx.port = strdup(optarg);
				break;
			
			case 'o':   //-o ${databit}${parity}${stopbit} 8N1
				strcpy(tmp, optarg);
				tty_ctx.databits = tmp[0] - 0x30;
				tty_ctx.parity  = tmp[1]; 
				tty_ctx.stopbits = tmp[2] - 0x30;
				break;
			
			case 'b':	 //-b 115200
				tty_ctx.speed = atoi(optarg);
				break;

			case 'u':   //-u user                                      
				mqtt_ctx->mqtt_name = strdup(optarg);
				break;                                                    

			case 'P':	//-P admin
				mqtt_ctx->mqtt_password = strdup(optarg); 
				break;                                                    

			case 'h':	//-h 192.168.10.236
				mqtt_ctx->mqtt_host = strdup(optarg);
				break;                                                    

			case 'p':   //-p 1883
				mqtt_ctx->mqtt_port = atoi(optarg); 
				break;                                                    

			case 't':	//-t /iot_dtu/devid_dtu/state/upload
				mqtt_ctx->publish_topic_upload = strdup(optarg);
				break;   

			case 'I':   //-I 123456789  没有配置则使用mac地址
				if(optarg == NULL)
				{
					mqtt_ctx->mqtt_clientId = get_mac();
				} else {
					mqtt_ctx->mqtt_clientId = strdup(optarg);
				}
				break;                       

			case 'S':	//-S TRUE   clean_session
				if( strncmp(optarg, "TRUE", strlen("TRUE") == 0 ))
				{
					mqtt_ctx->mqtt_clean_session = true;
				} else {
					mqtt_ctx->mqtt_clean_session = false;
				}
				break;           
			
			case 'Q':	//-Q 0     
				mqtt_ctx->publish_topic_upload_qos = atoi(optarg);
				break;                       

			case 'k':   //-k 60
				mqtt_ctx->mqtt_keepalive =atoi(optarg);
				break;                       

			case 'R':	//-R FALSE
				if( strncmp(optarg, "TRUE", strlen("TRUE") == 0 ))
				{
					mqtt_ctx->publish_topic_upload_retain = true;
				} else {
					mqtt_ctx->publish_topic_upload_retain = false;
				}
				break;                       

			case 'T':	//-T 1000
				//timeout = atoi(optarg);
				break;

			case 'm':	//-m 0001812290002355
				mqtt_ctx->mqtt_message_header = strdup(optarg);
				break;
			
			case 'q':
				if(optarg != NULL) {
					mqtt_ctx->subscribe_topic[0] = strdup(optarg);
				}
				break;
			case 'a':
					mqtt_ctx->subscribe_topic_qos[0] = atoi(optarg);
				break;
		
			case 'w':
				if(optarg != NULL) {
					mqtt_ctx->subscribe_topic[1] = strdup(optarg);
				}
				break;
			case 's':
					mqtt_ctx->subscribe_topic_qos[1] = atoi(optarg);
				break;
			
			case 'e':
				if(optarg != NULL) {
					mqtt_ctx->subscribe_topic[2] = strdup(optarg);
				}
				break;
			case 'f':
					mqtt_ctx->subscribe_topic_qos[2] = atoi(optarg);
				break;
			
			case 'r':
				if(optarg != NULL) {
					mqtt_ctx->subscribe_topic[3] = strdup(optarg);
				}
				break;
			case 'g':
					mqtt_ctx->subscribe_topic_qos[3] = atoi(optarg);
				break;

			case 'N':
				NoDetach = 1;
				break;
		} 
	}
	
	if (NoDetach == 0)                                                  
	{
		if ((cpid = fork()) > 0)
			exit(0);
		else if (cpid < 0)
		{
			exit(1);
		}
		else
		{
			chdir("/etc");
			close(0);
			close(1);
			close(2);
		}
	}
	
	DEBUG_LOG(LLOG_DEBUG,"%s",mqtt_ctx->mqtt_clientId);

	mqtt_init();
	uart_init();

	while(1)
	{
		;
	}
	return 0;
}

char *get_mac()
{
	char *cmd = "mac=`setmac | awk -F'mac:' '{print $2}'`;sn=`echo $mac | sed 's/://g'`;echo $sn";
	char uci_cmd[64] = { 0 };
	FILE *stream = NULL;
	char *mac = malloc(13);

	memset(mac, 0, 13);
	stream = popen(cmd, "r");
	if(!stream)
	{
		DEBUG_LOG(LLOG_ERR,"get mac error"); 
		return NULL;
	}
	fread( mac, sizeof(char), 12,  stream);
	DEBUG_LOG(LLOG_DEBUG,"mac:%s",mac);
	
	//将mac 地址保存在dtu文件
	snprintf(uci_cmd, 64, "uci set dtu.dtu.mqttid=%s;uci commit", mac);
	stream = popen(uci_cmd,"r");

	pclose( stream );
	

	return mac;
}





