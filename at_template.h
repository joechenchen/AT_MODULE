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
* Brief   : 需要放到串口接收中断中
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void xx_at_recv_buff(uint8_t rx_data);

/***********************************************************
* Name    : xx_at_recv_buff
* Brief   : 超时检测，需要放到定时器中断中，设置20ms超时时间
* Params  : 
* Return  : 
* Note    : 
************************************************************/

void at_recv_ot_check(void);

/*************************************************
@Description:factory_set
@Input:1-设置标志位，0-清除标志位
@Output:
@Return:
*************************************************/
void factory_set(uint8_t op);


/*************************************************
@Description:rm08_init
@Input:rm08模块初始化
@Output:
@Return:
*************************************************/

void rm08_init(void);

#endif /* BSP_AT_H_ */
