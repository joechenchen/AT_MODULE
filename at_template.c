
#include "nrf_delay.h"
#include "at.h"
#include "simple_uart.h"
#include "boards.h"
#include "nrf_nvmc.h"
#include "at_template.h"
#include "string.h"

#define FACTORY_ADDR 0x19000
#define CMD_LEN 50
const char rm08_send_cmd[][CMD_LEN] = 
{
    "at+Netmode=1\r\n",//����ת��̫��ģʽ
    "at+LANIp=192.168.16.254\r\n",//LAN��IP
    "at+LANIpMask=255.255.255.0\r\n",//LAN��IP����
    "at+Dhcpd=0\r\n",//DHCP��������ʹ��
    "at+Dhcpc=0\r\n",//��̬IP
    "at+WANIp=192.168.1.108\r\n",//WAN��IP
    "at+WANIpMask=255.255.255.0\r\n",//WAN������
    "at+SGw=192.168.1.1\r\n",//��̬����
    "at+SDnsF=192.168.1.1\r\n",//��̬��dns  NProType0
    //����ָ��Ϊ����ģ������·��������Ĳ��������Ϊ�������
    "at+NProType0=2\r\n",//͸�� Socket Э������ TCPClient
    "at+NLPort0=8080\r\n",//Socket ���ض˿�
    "at+NTcpTo0=0\r\n",//Socket tcp ��ʱʱ��
    "at+SBaud0=115200\r\n",
    "at+SWidth0=8\r\n",
    "at+SStop0=1\r\n",
    "at+SPari0=0\r\n",
    //����ָ��Ϊ���� 0 ����ָ��
    "\r\nat+Dhcpd=0\r\n",//DHCP��������ʹ��
    "at+Dhcpc=0\r\n",//��̬IP
    "at+NDomain0=192.168.1.245\r\n",//Socket Զ�� IP
    "at+NRPort0=8235\r\n",//Socket Զ�˶˿�
    "at+Save=1\r\n",
    "at+Apply=1\r\n",
    "at+SAtMode=0\r\n"//����͸��
};

const char rm08_ack_cmd[][CMD_LEN] =
{
    "at+RNetmode=1\r\n",
    "at+RLANIp=192.168.16.254\r\n",
    "at+RLANIpMask=255.255.255.0\r\n",
    "at+RDhcpd=0\r\n",
    "at+RDhcpc=0\r\n",
    "at+RWANIp=192.168.1.108\r\n",
    "at+RWANIpMask=255.255.255.0\r\n",
    "at+RSGw=192.168.1.1\r\n",
    "at+RSDnsF=192.168.1.1\r\n",
    "at+RNProType0=2\r\n",
    "at+RNLPort0=8080\r\n",
    "at+RNTcpTo0=0\r\n",
    "at+RSBaud0=115200\r\n",
    "at+RSWidth0=8\r\n",
    "at+RSStop0=1\r\n",
    "at+RSPari0=0\r\n",
    "at+RDhcpd=0\r\n",
    "at+RDhcpc=0\r\n",
    "at+RNDomain0=192.168.1.245\r\n",
    "at+RNRPort0=8235\r\n",
    "at+RSave=1\r\n",
    "at+RApply=1\r\n"
};

MY_SEM_TYPE gt_recv_flag;//������ɱ�־λ�����տ���ʱ��1
char g_at_ack_buf[AT_ACK_BUF_SIZE];
uint32_t at_buf_idx = 0;
uint32_t at_recv_overtime_cnt = 0;//��ʱ������
at_config_t gt_at_config;
static void xx_rx_buf_clear(void);
static bool get_factory_state(void);

const char (*p_send)[CMD_LEN] = rm08_send_cmd;
const char (*p_recv)[CMD_LEN] = rm08_ack_cmd;

/*************************************************
@Description:rm08_init
@Input:rm08ģ���ʼ��
@Output:
@Return:
*************************************************/

void rm08_init(void)
{
    uint8_t cmd_num;//�������
    uint8_t i = 0;
    uint8_t ret = 0;
    
    RM08_POWEROFF_DLY();
    RM08_POWERON_DLY();
//	if(get_factory_state())//���Ǹ����صĳ��򣬲���Ҫ��ʼ��
//		return;
    xx_at_init();
    RM08_ES0_ENTER_AT();//����AT
    cmd_num = sizeof(rm08_send_cmd)/sizeof(rm08_send_cmd[0]);
    for( i = 0; i < cmd_num - 1; i++)
    {
        ret = at_send_cmd(*(p_send+i),*(p_recv+i),&gt_at_config);
        if( ret > 0 )
        {
            break;
        }
    }
    gt_at_config.wait_ack = 1;//���ȴ�����
    at_send_cmd(*(p_send+i),*(p_recv+i),&gt_at_config);
    factory_set(1);
    nrf_delay_ms(6000);//��ʱ6s
    
}

/***********************************************************
* Name    : rm08_reboot
* Brief   : 
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void rm08_reboot(void)
{
    RM08_POWEROFF_DLY();
    RM08_POWERON_DLY(); 
    RM08_ES0_ENTER_AT();//����AT
}

/***********************************************************
* Name    : get_factory_state
* Brief   : �ж��Ƿ���Ҫ��������rm08
* Params  : 
* Return  : true-���Ǹ����صĳ���
* Note    : 
************************************************************/

static bool get_factory_state(void)
{
    bool state = true;
    uint8_t flag = 0;

    flag = (*(uint8_t *)FACTORY_ADDR);
    if(flag == 1)
    {
        state = true;
    }
    else
    {
        state = false;
    }

    return state;
}


/***********************************************************
* Name    : factory_set
* Brief   : 1-���ñ�־λ��0-�����־λ
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void factory_set(uint8_t op)
{

    if(1 == op)
    {
        nrf_nvmc_page_erase(FACTORY_ADDR);
        nrf_nvmc_write_byte(FACTORY_ADDR,1);
    }
    else
    {
        nrf_nvmc_page_erase(FACTORY_ADDR);
    }

}

/***********************************************************
* Name    : xx_at_init
* Brief   : 
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void xx_at_init(void)
{
    at_func_t t_at_func;
    
    at_init();//at��ʼ��
    t_at_func.send_interface = my_uart_send;
    t_at_func.delay_ms_interface = nrf_delay_ms;
    t_at_func.module_reboot = rm08_reboot;
    t_at_func.at_clear_ack_buff = xx_rx_buf_clear;
    at_register_handler(t_at_func,g_at_ack_buf,&gt_recv_flag);
    at_recv_overtime_cnt = 0;
    gt_at_config.max_reset_times = 2;
    gt_at_config.max_try_times = 2;
    gt_at_config.resp_time1ms = 200;//���յȴ�
    gt_at_config.try_delay1ms = 1;//�ط����
    gt_at_config.wait_ack = 0;//���ȴ�����
}

/***********************************************************
* Name    : xx_at_recv_buff
* Brief   : ��Ҫ�ŵ����ڽ����ж���
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void xx_at_recv_buff(uint8_t rx_data)
{
    if(at_buf_idx < AT_ACK_BUF_SIZE)
    {
        g_at_ack_buf[at_buf_idx++] = rx_data;
    }
    at_recv_overtime_cnt = 0;
}

/***********************************************************
* Name    : xx_at_recv_buff
* Brief   : ��ʱ��⣬��Ҫ�ŵ���ʱ���ж��У�����20ms��ʱʱ��
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void at_recv_ot_check(void)
{
    //�����׼ʱ��Ϊ10ms
    at_recv_overtime_cnt++;
    if(at_recv_overtime_cnt > 1 )
    {
        gt_recv_flag = 1;
    }
}


/***********************************************************
* Name    : xx_rx_buf_clear
* Brief   : ��ջ�����
* Params  : 
* Return  : 
* Note    : 
************************************************************/

static void xx_rx_buf_clear(void)
{
    memset(g_at_ack_buf,0,AT_ACK_BUF_SIZE);
    at_buf_idx = 0;
}



