#ifndef BSP_AT_H_
#define BSP_AT_H_

#include <stdint.h>
#include <stdbool.h>


#define MY_SEM_TYPE uint8_t
#define AT_ACK_BUF_SIZE  256

typedef struct
{
    void (*send_interface)(const char * buf, uint32_t len);//发送函数
    void (*delay_ms_interface)(uint32_t ms);//延时函数    
    void (*at_clear_ack_buff)(void);//清空回复的buf
    void (*module_reboot)(void);//模块重启函数
}at_func_t;


typedef struct
{
    uint32_t   resp_time1ms;//发送后查询返回信息的延时，1ms为单位。可设为指令最大响应时间。
    uint32_t   try_delay1ms; //发送失败后再次发送时的延时，1ms为单位
    uint8_t    max_try_times; //最大重试次数
    uint8_t    max_reset_times; //最大重启次数
    uint8_t    wait_ack;  //0-需要等待接收 1-无需等待接收
} at_config_t;

void at_init(void);
void at_register_handler(at_func_t func, char* recv_buf, MY_SEM_TYPE* recv_sem);
uint8_t at_send_cmd(const char *send_str,const char *recv_str,at_config_t *p_at_config);
uint8_t at_send_data(const char *send_buf,const uint16_t buf_len,const char *recv_str,at_config_t *p_at_config);


#endif /* BSP_AT_H_ */
