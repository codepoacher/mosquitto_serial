#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "udemo.h"

struct mqtt_context mqtt_back; 
struct mqtt_context *mqtt_ctx = &mqtt_back; 
const char *devid = "0001812290002355";

void mosquitto_message_process(const struct mosquitto_message *message) 
{
	int i = 0;
	char printbuf[1024] = {0};
	
	for(i =0; i<message->payloadlen; i++)
	{
		snprintf(&printbuf[i*3], 1024, "%02x ", *(unsigned char *)(message->payload+i));
	}
	DEBUG_LOG(LLOG_DEBUG,"send data(len=%d) is:%s\n", message->payloadlen, printbuf);   
	
	UART0_Packet(&tty_ctx, message->payload, message->payloadlen);
}

//透传串口数据
int do_tty_msg_publish(char *data, int len)
{
	int ret = 0;
	char tmp[1024] = {0};
	char printbuf[2048] = {0};
	int i = 0;
	int header_len = strlen(mqtt_ctx->mqtt_message_header);
	
	memcpy( tmp, mqtt_ctx->mqtt_message_header, header_len);
	memcpy( &tmp[header_len], data, len);
	
	for(i=0; i<header_len+len; i++)
	{
		snprintf(&printbuf[i*3], 1024, "%02x ", (unsigned char)tmp[i]);
	}
	DEBUG_LOG(LLOG_DEBUG,"publish topic:%s", mqtt_ctx->publish_topic_upload);
	DEBUG_LOG(LLOG_DEBUG,"publish data(len=%d) is:%s", len+header_len,  printbuf);

	if( mqtt_ctx->connected ) 
	{
		ret = mosquitto_publish(mqtt_ctx->mosq, NULL, mqtt_ctx->publish_topic_upload, \
				len + header_len, tmp, mqtt_ctx->publish_topic_upload_qos, mqtt_ctx->publish_topic_upload_retain);
		if (ret != MOSQ_ERR_SUCCESS)
		{
			DEBUG_LOG(LLOG_ERR,"\n");
			return ret;
		}
		return len;
	} else {
		DEBUG_LOG(LLOG_ERR, "mosquitto connected = %d\n", mqtt_ctx->connected);	
		return -1;
	}
}

static void connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
	int ret = 0;
	int i = 0; 

	DEBUG_LOG(LLOG_DEBUG,"rc=%d\n",rc);
	
	if(rc){
		exit(1);
	}else{
		mqtt_ctx->connected = 1;
	
		for(i=0; i<sizeof(mqtt_ctx->subscribe_topic_qos); i++)
		{
			if(mqtt_ctx->subscribe_topic[i] != NULL){
				ret = mosquitto_subscribe(mosq, NULL, mqtt_ctx->subscribe_topic[i], mqtt_ctx->subscribe_topic_qos[i]);
				if(ret != MOSQ_ERR_SUCCESS)
				{
					DEBUG_LOG(LLOG_ERR,"subscribe error\n");
				}
			}
		}
	}
}


static void disconnect_callback(struct mosquitto *mosq, void *obj, int rc)
{
	DEBUG_LOG(LLOG_DEBUG, "mqtt_disconnect rc=%d\n",rc);
	mqtt_ctx->connected = 0;
}


static void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)      
{
	if(message->payloadlen){
		DEBUG_LOG(LLOG_DEBUG, "topic:%s, payload:%s\n", (char *)message->topic, (char *)message->payload);
		mosquitto_message_process(message);
	} else {
		DEBUG_LOG(LLOG_ERR,"%s (null)\n", message->topic);
	}
}


int mqtt_init()
{
	//char tmp[256] = {0};
	
	/**************配置感兴趣的主题*****************/	
	/*snprintf(tmp, 256, "/iot_dtu/devid_%s/plate/set/unlock", mqtt_ctx->mqtt_message_header);
	mqtt_ctx->subscribe_topic_set = strdup(tmp);
	snprintf(tmp, 256, "/iot_dtu/devid_%s/plate/lock/state", mqtt_ctx->mqtt_message_header);
	mqtt_ctx->subscribe_topic_state = strdup(tmp);*/


	/***************配置需要发布的主题***************/
	/*snprintf(tmp,256,"/iot_dtu/devid_%s/state/upload",mqtt_ctx->mqtt_message_header);
	mqtt_ctx->publish_topic_upload = strdup(tmp);*/	
	
	
	mosquitto_lib_init();
	mqtt_ctx->mosq = mosquitto_new(mqtt_ctx->mqtt_clientId, mqtt_ctx->mqtt_clean_session, NULL);
	if(!mqtt_ctx->mosq) {
		DEBUG_LOG(LLOG_ERR,"Out of memory.\n");
		return -1;
	}
	if(mqtt_ctx->mqtt_name && mqtt_ctx->mqtt_password){
		mosquitto_username_pw_set(mqtt_ctx->mosq, mqtt_ctx->mqtt_name, mqtt_ctx->mqtt_password);
	}


	mosquitto_connect_callback_set(mqtt_ctx->mosq, connect_callback);        
	mosquitto_disconnect_callback_set(mqtt_ctx->mosq, disconnect_callback);  
	mosquitto_message_callback_set(mqtt_ctx->mosq, message_callback);        
	//mosquitto_connect_async(mqtt_ctx->mosq, "192.168.10.236", 1883, 60);             
	mosquitto_connect_async(mqtt_ctx->mosq, mqtt_ctx->mqtt_host, mqtt_ctx->mqtt_port, mqtt_ctx->mqtt_keepalive);             

	//开启一个线程，在线程里不停的调用 mosquitto_loop() 来处理网络信息
	int loop = mosquitto_loop_start(mqtt_ctx->mosq);
	if(loop != MOSQ_ERR_SUCCESS)
	{
		DEBUG_LOG(LLOG_ERR,"mosquitto loop err!!\n");
		return -1;
	}
	
	return 1;
}

/*
void stop_mqtt_worker() {
	if(mqtt_ctx->mosq){
		mosquitto_loop_stop(mqtt_ctx->mosq, false);
		mosquitto_disconnect(mqtt_ctx->mosq);
		mosquitto_destroy(mqtt_ctx->mosq);
	}
	mosquitto_lib_cleanup();
} 
*/
