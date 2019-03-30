#ifndef BSP_AT_H_
#define BSP_AT_H_

#include <stdint.h>
#include <stdbool.h>


#define MY_SEM_TYPE uint8_t
#define AT_ACK_BUF_SIZE  256

typedef struct
{
    void (*send_interface)(const char * buf, uint32_t len);//���ͺ���
    void (*delay_ms_interface)(uint32_t ms);//��ʱ����    
    void (*at_clear_ack_buff)(void);//��ջظ���buf
    void (*module_reboot)(void);//ģ����������
}at_func_t;


typedef struct
{
    uint32_t   resp_time1ms;//���ͺ��ѯ������Ϣ����ʱ��1msΪ��λ������Ϊָ�������Ӧʱ�䡣
    uint32_t   try_delay1ms; //����ʧ�ܺ��ٴη���ʱ����ʱ��1msΪ��λ
    uint8_t    max_try_times; //������Դ���
    uint8_t    max_reset_times; //�����������
    uint8_t    wait_ack;  //0-��Ҫ�ȴ����� 1-����ȴ�����
} at_config_t;

void at_init(void);
void at_register_handler(at_func_t func, char* recv_buf, MY_SEM_TYPE* recv_sem);
uint8_t at_send_cmd(const char *send_str,const char *recv_str,at_config_t *p_at_config);
uint8_t at_send_data(const char *send_buf,const uint16_t buf_len,const char *recv_str,at_config_t *p_at_config);


#endif /* BSP_AT_H_ */
