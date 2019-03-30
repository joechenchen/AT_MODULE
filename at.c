


#include "at.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

MY_SEM_TYPE *p_recv_ack_sem=NULL;
char *at_ack_buf;
static at_func_t gt_at_func;

static char* at_search_recv_buf(const char* recv_str,uint32_t max_resp_time);



/***********************************************************
* Name    : at_init
* Brief   : 
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void at_init(void)
{
    gt_at_func.delay_ms_interface = NULL;
    gt_at_func.send_interface = NULL;
    gt_at_func.at_clear_ack_buff = NULL;
    gt_at_func.module_reboot = NULL;
}

/***********************************************************
* Name    : at_register_handler
* Brief   : ע��ATָ�����л��������ͺ���ʱ����ע�ᣬ���ջ�����ע�ᣬ��������ź�ע��
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void at_register_handler(at_func_t func, char* recv_buf, MY_SEM_TYPE* recv_sem)
{
    gt_at_func.send_interface = func.send_interface;//���ڷ������ݵĺ���
    gt_at_func.delay_ms_interface = func.delay_ms_interface;//���ڷ�����ʱ����
    gt_at_func.at_clear_ack_buff = func.at_clear_ack_buff;//��ս���buf����
    gt_at_func.module_reboot = func.module_reboot;
    p_recv_ack_sem = recv_sem;  //�����յ�ATָ�������ź��������ڴ��ڽ��տ��к󷢳�
    at_ack_buf = (char*)recv_buf;       //���ڽ��ջ�����
    return;
}


/***********************************************************
* Name    : at_send_cmd
* Brief   : ����ATָ��
* Params  : send_buf:���͵��ַ���
            recv_str���ڴ������а��������ַ���
            p_at_config��AT����
* Return  : 2-ָ��գ�1-ģ��δ��Ӧ 0-����
* Note    : 
************************************************************/

uint8_t at_send_cmd(const char *send_str,const char *recv_str,at_config_t *p_at_config)
{
    uint8_t _ret = 0;
    uint8_t _try_times = 0;
    uint8_t _reset_times = 0;
    char * _p_msg = NULL;    

    if(gt_at_func.send_interface ==NULL || at_ack_buf==NULL)
    {
        return _ret = 2;
    }

    do
    {
        gt_at_func.at_clear_ack_buff();
        *p_recv_ack_sem = 0;//����ǰ�ź����
        gt_at_func.send_interface(send_str,strlen(send_str));

        _p_msg = at_search_recv_buf(recv_str,p_at_config->resp_time1ms);
        //������жϷ��ͣ����Է��ڽ��ճ�ʱ��
        if(p_at_config->wait_ack)
        {
            return _ret = 0;
        }
        if ( _p_msg != NULL )
        {
            return _ret = 0;
        }
        else
        {
			_try_times ++;
			if ( _try_times >= p_at_config->max_try_times )
			{
				_reset_times ++;

				if ( _reset_times >= p_at_config->max_reset_times + 1 )
				{
					return _ret = 1;
				}
				else
				{
					gt_at_func.module_reboot();
				}
			}
            else
            {
                gt_at_func.delay_ms_interface(p_at_config->try_delay1ms);
            }
        }
    }
    while(1);

    return _ret;

}


/***********************************************************
* Name    : at_send_data
* Brief   : ���ܣ�������������
* Params  : send_buf:���͵�����
            buf_len�����鳤��
            recv_str���ڴ������а��������ַ���
            p_at_config��AT����
* Return  : 2-ָ��գ�1-����δ��Ч  0-����
* Note    : 
************************************************************/

uint8_t at_send_data(const char *send_buf,const uint16_t buf_len,const char *recv_str,at_config_t *p_at_config)
{
    uint8_t _ret = 0;
    uint8_t _try_times = 0;
    uint8_t _reset_times = 0;
    char * _p_msg = NULL;    


    if(gt_at_func.send_interface == NULL || at_ack_buf==NULL || p_recv_ack_sem == NULL)
    {
        return _ret = 2;
    }

    do
    {
        gt_at_func.at_clear_ack_buff();
        *p_recv_ack_sem = 0;
        gt_at_func.send_interface(send_buf,buf_len);
        _p_msg = at_search_recv_buf(recv_str,p_at_config->resp_time1ms);
        //������жϷ��ͣ����Է��ڽ��ճ�ʱ��
        if(p_at_config->wait_ack)
        {
            return _ret = 0;
        }
        if ( _p_msg != NULL )
        {
            return _ret = 0;
        }
        else
        {
			_try_times ++;
			if ( _try_times >= p_at_config->max_try_times )
			{
				_reset_times ++;

				if ( _reset_times >= p_at_config->max_reset_times + 1 )
				{
					return _ret = 1;
				}
				else
				{
					gt_at_func.module_reboot();
				}
			}
            else
            {
                gt_at_func.delay_ms_interface(p_at_config->try_delay1ms);                

            }
        }
    }
    while(1);

    return _ret;
}


/***********************************************************
* Name    : at_search_recv_buf
* Brief   : ��ѯATָ��Ļ������Ƿ�����Ҫ���ַ���
* Params  : recv_str:�ڴ�����ַ�������Ҫ���е����ַ�������"OK\r\n"
            max_resp_time��ָ�������Ӧʱ�䣬��λ1ms
* Return  : ���ҵ������ַ���ָ��
* Note    : 
************************************************************/

char* at_search_recv_buf(const char* recv_str,uint32_t max_resp_time)
{
    char *_p_msg = NULL;
    uint32_t resp_cnt = 0;

    do
    {
        gt_at_func.delay_ms_interface(1);//1ms��׼
        if( *p_recv_ack_sem )
        {
            *p_recv_ack_sem = 0;
            _p_msg = strstr( at_ack_buf, recv_str );
            if(_p_msg!=NULL)
            {
                break;
            }
        }
        resp_cnt++;
    }
    while( resp_cnt < max_resp_time);

    return _p_msg;
}





