


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
* Brief   : 注册AT指令运行环境：发送和延时函数注册，接收缓冲区注册，接收完成信号注册
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void at_register_handler(at_func_t func, char* recv_buf, MY_SEM_TYPE* recv_sem)
{
    gt_at_func.send_interface = func.send_interface;//串口发送数据的函数
    gt_at_func.delay_ms_interface = func.delay_ms_interface;//串口发送延时函数
    gt_at_func.at_clear_ack_buff = func.at_clear_ack_buff;//清空接收buf函数
    gt_at_func.module_reboot = func.module_reboot;
    p_recv_ack_sem = recv_sem;  //串口收到AT指令回令的信号量，可在串口接收空闲后发出
    at_ack_buf = (char*)recv_buf;       //串口接收缓冲区
    return;
}


/***********************************************************
* Name    : at_send_cmd
* Brief   : 发送AT指令
* Params  : send_buf:发送的字符串
            recv_str：期待回令中包含的子字符串
            p_at_config：AT配置
* Return  : 2-指针空，1-模块未响应 0-正常
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
        *p_recv_ack_sem = 0;//发送前信号清空
        gt_at_func.send_interface(send_str,strlen(send_str));

        _p_msg = at_search_recv_buf(recv_str,p_at_config->resp_time1ms);
        //如果是中断发送，所以放在接收超时这
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
* Brief   : 功能：发送数组数据
* Params  : send_buf:发送的数组
            buf_len：数组长度
            recv_str：期待回令中包含的子字符串
            p_at_config：AT配置
* Return  : 2-指针空，1-重启未生效  0-正常
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
        //如果是中断发送，所以放在接收超时这
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
* Brief   : 查询AT指令的回令中是否有需要的字符串
* Params  : recv_str:期待输出字符串中需要含有的子字符串，如"OK\r\n"
            max_resp_time：指令最大响应时间，单位1ms
* Return  : 查找到的子字符串指针
* Note    : 
************************************************************/

char* at_search_recv_buf(const char* recv_str,uint32_t max_resp_time)
{
    char *_p_msg = NULL;
    uint32_t resp_cnt = 0;

    do
    {
        gt_at_func.delay_ms_interface(1);//1ms基准
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





