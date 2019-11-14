#ifndef _UDEMOD_H__
#define _UDEMOD_H__

#include <syslog.h>
#include <mosquitto.h>


/************************调试信息相关*****************************/
#define  LLOG_DEBUG      "debug"
#define  LLOG_INFO       "info"
#define  LLOG_WARNING    "waring"
#define  LLOG_ERR        "error"
#define  LLOG_FATAL      "fatal" 

#define  TEST_DEBUG
#ifdef   TEST_DEBUG
#define DEBUG_LOG(level, fmt, ...) \
   do \
   { \
   	(void)syslog(LOG_DEBUG,"[%s][%s:%d] " fmt "\r\n", \
   			level, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
   } while (0)
#else
#define DEBUG_LOG(level, fmt, ...)
#endif



/************************串口设置相关*****************************/
struct tty_context {
	int fd;
	char state;         // 0：未连接   1：连接
	char *port;			// 串口 /dev/ttyUSBxxx 
	int speed;          // 串口波特率   
	int flow_ctrl;      // 数据流控 
	int databits;       // 数据位
	int stopbits;       // 停止位
	int parity;         // 校验位
	char rx_buf[1024];
	int rx_len;
	int (*rx_callback)(char *data, int len);
	
	char tx_buf[1024];
	int tx_len;
};
extern struct tty_context tty_ctx; 

int UART0_Open(int fd,char* port);    
int UART0_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity); 
int UART0_Send(int fd, char *send_buf,int data_len); 
int UART0_Recv(int fd, char *rcv_buf,int data_len); 
void UART0_Close(int fd); 
void UART0_Packet(struct tty_context *tty, char *data, int len);
int uart_init(); 


/************************MQTT相关函数**************************/
struct mqtt_context {
	int connected;        //0未连接 1连接
	struct mosquitto *mosq;
	
	char *mqtt_name;      //mqtt用户名
	char *mqtt_password;  //mqtt密码
	char *mqtt_host;      //服务器地址
	int  mqtt_port;       //端口号
	int  mqtt_keepalive;  //keepalive
	char *mqtt_clientId;  //客户端id
	bool mqtt_clean_session; 
	char *mqtt_message_header; //消息头
	
	char *publish_topic_upload;   //透传串口数据主题
	char publish_topic_upload_qos;
	bool publish_topic_upload_retain;
	
	char *subscribe_topic[16];  //保存需要订阅的主题
	char subscribe_topic_qos[16]; 
};  
extern struct mqtt_context *mqtt_ctx; 

int mqtt_init();
void mosquitto_message_process(const struct mosquitto_message *message); 
int do_tty_msg_publish(char *data, int len);


/************************其他**************************/
char *get_mac();



#endif
