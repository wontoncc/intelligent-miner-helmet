#include "2401.h"
#include "delay.h"

#define NRF2401_Interval_Long 25

uint8_t  const TX_ADDRESS[TX_ADR_WIDTH]= {0x0d,0x43,0x10,0x10,0xff};	
uint8_t  const RX_ADDRESS[RX_ADR_WIDTH]= {0x0d,0x43,0x10,0x10,0xff};


#define SET_CE	GPIO_SetBits(GPIOC,NRF_CE);
#define CLE_CE	GPIO_ResetBits(GPIOC,NRF_CE);

#define SET_CLK	GPIO_SetBits(GPIOC , CLK);
#define CLE_CLK	GPIO_ResetBits(GPIOC , CLK);

#define GET_MISO	GPIO_ReadInputDataBit(GPIOC , MISO)

#define SET_MOSI	GPIO_SetBits(GPIOC , MOSI);
#define CLE_MOSI	GPIO_ResetBits(GPIOC , MOSI);

void Nrf_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE );
	
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT ;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP ;
//	GPIO_InitStruct.GPIO_Pin = CS |CLK |MOSI;
	GPIO_InitStruct.GPIO_Pin = CLK |MOSI;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT ;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStruct.GPIO_Pin = CS;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN ;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStruct.GPIO_Pin = MISO;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz ;
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	/*
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz ;
	
	GPIO_Init(GPIOx,&GPIO_InitStruct);
	*/
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT ;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStruct.GPIO_Pin = NRF_CE ;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz ;
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);
}


uint8_t SPI_RW(uint8_t uint8_ts)
{
	uint8_t bit_ctr;
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++) 
   	{
		//MOSI =(uint8_ts & 0x80); 
		if(uint8_ts & 0x80)	SET_MOSI
			else	CLE_MOSI
		
		uint8_ts = (uint8_ts << 1);          
		SET_CLK;                      
		uint8_ts |= GET_MISO;       		
		CLE_CLK            		 
   	}
    return(uint8_ts);           		 
}

uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;
	CLE_CS                  
	status = SPI_RW(reg);     
	SPI_RW(value);            
	SET_CS                   
	return(status);          
}

uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts)
{
	uint8_t status,uint8_t_ctr;	
	CLE_CS                 
	status = SPI_RW(reg);   
	for(uint8_t_ctr=0; uint8_t_ctr<uint8_ts; uint8_t_ctr++){
		SPI_RW(*pBuf++);
	}
	SET_CS          
	return(status);     
}

uint8_t SPI_Read(uint8_t reg){
	uint8_t reg_val;
	CLE_CS;                
	SPI_RW(reg);           
	reg_val = SPI_RW(0);  
	SET_CS;                
	
	return(reg_val);     
}

int NRF24L01_CheckAck(){
	if(SPI_Read(STATUS)&(1<<5)){
		return 1;
	} else{
		return 0;
	}
}

void NRF24L01_TxPacket(unsigned char * tx_buf)
{
	int j = 0;
	CLE_CE;		
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);
	SET_CE;	
	DelayMS(NRF2401_Interval_Long);
	
	while(!NRF24L01_CheckAck() && j < 7){
		DelayMS(5);
		j++;
	}
	
	SPI_RW_Reg(WRITE_REG+STATUS,0XFF);//清除发送标志位
	CLE_CS
	SPI_RW(FLUSH_TX);
	SET_CS
	
	DelayUS(10);
}

void Init_NRF24L01_Tx(void)
{
	
    DelayUS(100);
 	CLE_CE;    
 	SET_CS ;   
 	SET_CLK ;  
	SPI_Write_Buf(WRITE_REG + TX_ADDR, (uint8_t *)TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
	
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      //  频道0自动	ACK应答允许	
//	SPI_RW_Reg(WRITE_REG + EN_AA, 0x00);      //  频道0自动	ACK应答不允许	
	
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  允许接收地址只有频道0，如果需要多频道可以参考Page21  
	SPI_RW_Reg(WRITE_REG + RF_CH, 0);        //   设置信道工作为2.4GHZ，收发必须一致
	
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //设置接收数据长度，本次设置为32字节
	
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   		//设置发射速率为1MHZ，发射功率为最大值0dB	
//	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x0f);   		//设置发射速率为1MHZ，发射功率为最大值0dB	
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ收发完成中断响应，16位CRC，主发送	
	
	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0xff);	//重试15次，每次间隔4ms
}



