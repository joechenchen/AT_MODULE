#ifndef __AT_TEMPLATE_H_
#define __AT_TEMPLATE_H_

#include <stdint.h>
#include <stdbool.h>

/***********************************************************
* Name    : xx_at_init
* Brief   : 
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void xx_at_init(void);

/***********************************************************
* Name    : xx_at_recv_buff
* Brief   : ��Ҫ�ŵ����ڽ����ж���
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void xx_at_recv_buff(uint8_t rx_data);

/***********************************************************
* Name    : xx_at_recv_buff
* Brief   : ��ʱ��⣬��Ҫ�ŵ���ʱ���ж��У�����20ms��ʱʱ��
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void at_recv_ot_check(void);

/*************************************************
@Description:factory_set
@Input:1-���ñ�־λ��0-�����־λ
@Output:
@Return:
*************************************************/
void factory_set(uint8_t op);


/*************************************************
@Description:rm08_init
@Input:rm08ģ���ʼ��
@Output:
@Return:
*************************************************/

void rm08_init(void);

#endif /* BSP_AT_H_ */
