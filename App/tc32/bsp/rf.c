
/**********************************************************************************************************************
* 文件名称： rf.c
* 内容摘要：8267_bleplus
* 当前版本： V1.0
* 作    者：陈凯鑫
* 完成日期： 2015.06.25
*
* 修改记录1：
**********************************************************************************************************************/
#include "bsp.h"
#include "rf.h"
/**********************************************************************************************************************
 *                                                 内部常量                                                           *
 *********************************************************************************************************************/
/* 内部常量是为了不让别的文件使用并减少重名可能性，如果有需要就在这里定义 */
#define     NULL             0
#define	   	FRE_OFFSET    			0
#define 	MAX_RF_CHANNEL			16

#define		RX_RPTR		0x800528
#define		RX_WPTR		0x800529
#define		TX_RPTR		0x80052a
#define		TX_WPTR		0x80052b
#define		TX_RPTR_PIPE	0x800538
#define		TX_WPTR_PIPE	0x800530
#define		SS		0x800f24

#define STATE0		0x1234
#define STATE1		0x5678
#define STATE2		0xabcd
#define STATE3		0xef01
/**********************************************************************************************************************
 *                                                 内部宏                                                             *
 *********************************************************************************************************************/
/* 内部宏是为了不让别的文件使用并减少重名可能性，如果有需要就在这里定义 */
//#define		gain_base		TP_GAIN0[g_rf_mode]
//#define		gain_g			((TP_GAIN0[g_rf_mode] - TP_GAIN1[g_rf_mode])*256/80)


/**********************************************************************************************************************
 *                                                 内部数据类型                                                       *
 *********************************************************************************************************************/



/**********************************************************************************************************************
 *                                                 全局变量                                                           *
 *********************************************************************************************************************/



/**********************************************************************************************************************
 *                                                 本地变量                                                          *
 *********************************************************************************************************************/
/* 本地变量也就是全局静态变量，只在本文件范围内有效，如果有需要就在这里定义 */

/*const unsigned char rf_chn[MAX_RF_CHANNEL] = {
	FRE_OFFSET+ 5, FRE_OFFSET+ 9, FRE_OFFSET+13, FRE_OFFSET+17,
	FRE_OFFSET+22, FRE_OFFSET+26, FRE_OFFSET+30, FRE_OFFSET+35,
	FRE_OFFSET+40, FRE_OFFSET+45, FRE_OFFSET+50, FRE_OFFSET+55,
	FRE_OFFSET+60, FRE_OFFSET+65, FRE_OFFSET+70, FRE_OFFSET+76,
};*/

const unsigned char tbl_agc[] = {
	0x31,0x32,0x33,0x30,0x38,0x3c,0x2c,0x18 ,0x1c,0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,
	0x0a,0x0f,0x15,0x1b,0x21,0x27,0x2e,0x32 ,0x38,0x3e
};

const TBLCMDSET  tbl_rf_ini[] = {

	0x06, 0x00,  TCMD_UNDER_BOTH | TCMD_WAREG, //power down control.
	0x8f, 0x38,  TCMD_UNDER_BOTH | TCMD_WAREG, //boot rx vco current, temporary fix 				(fix)
	0xa2, 0x2c,  TCMD_UNDER_BOTH | TCMD_WAREG, //pa_ramp_target ****0-5bit

	0xa0, 0x28,  TCMD_UNDER_BOTH | TCMD_WAREG, //dac datapath delay ******change  remington 0x26								(fix)

	0x04, 0x7c,  TCMD_UNDER_BOTH | TCMD_WAREG,
	0xa7, 0x61,  TCMD_UNDER_BOTH | TCMD_WAREG,
	0x8d, 0x67,  TCMD_UNDER_BOTH | TCMD_WAREG,
	0xac, 0xaa,  TCMD_UNDER_BOTH | TCMD_WAREG,

	0x04ca, 0x88,	TCMD_UNDER_BOTH | TCMD_WRITE,	// ********enable DC distance bit 3
	0x04cb, 0x04,	TCMD_UNDER_BOTH | TCMD_WRITE,	// ********eset distance
	0x042d, 0x33,	TCMD_UNDER_BOTH | TCMD_WRITE,	// ********DC alpha=1/8, bit[6:4]
	
	//AGC_table
	0x430,0x12, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x43d,0x71, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x438,0xb7, TCMD_UNDER_BOTH | TCMD_WRITE,
};

//RxMaxGain
const TBLCMDSET  tbl_rf_ini_manual_agc[] = {               // 16M crystal                         (fix)
	0x0433, 0x00,	TCMD_UNDER_BOTH | TCMD_WRITE,	// set mgain disable 01 -> 00                 (fix)
	0x0434, 0x01,	TCMD_UNDER_BOTH | TCMD_WRITE,	// pel0: 21 -> 01                             (fix)
	0x043a, 0x77,	TCMD_UNDER_BOTH | TCMD_WRITE,	// Rx signal power change threshold: 22 -> 77 (fix)
	0x043e, 0xc9,	TCMD_UNDER_BOTH | TCMD_WRITE,	// set rx peak detect manual: 20 -> c9        (fix)
	0x04cd, 0x06,	TCMD_UNDER_BOTH | TCMD_WRITE,	// fix rst_pga=0: len = 0 enable              (fix)
//	0x04c0, 0x81,	TCMD_UNDER_BOTH | TCMD_WRITE,	// lowpow agc/sync: 87                        (fix)
};


const TBLCMDSET  tbl_rf_ini_16M[] = {               // 16M crystal
	//0x04eb, 0x60,	TCMD_UNDER_BOTH | TCMD_WRITE,   // auto            								(fix)
	//0x99, 	0x31,  TCMD_UNDER_BOTH | TCMD_WAREG,	// gauss filter sel: 16M  						(fix)
	//0x82,	0x14,  TCMD_UNDER_BOTH | TCMD_WAREG,	// gauss filter sel: 16M   						(fix)
	
};

const TBLCMDSET  tbl_rf_ini_12M[] = {               // 12M crystal             						(fix)
	//////0x04eb, 0xe0,  TCMD_UNDER_BOTH | TCMD_WRITE,	// RX freq regisgter: 4d4  						(fix)
	//////0x99, 	0xb1,  TCMD_UNDER_BOTH | TCMD_WAREG,	// gauss filter sel: 16M   						(fix)
	////////0x82,	0x00,  TCMD_UNDER_BOTH | TCMD_WAREG,	//enable rxadc clock       						(fix)
	//0x9e, 	0x82,  TCMD_UNDER_BOTH | TCMD_WAREG, 	//reg_dc_mod (500K)        					(fix)
};

// rf mode
////////////////////////////////////////////////////////////////////////////////////////////////

const TBLCMDSET  tbl_rf_zigbee_250k[] = {
	  0x9e, 0xad,  TCMD_UNDER_BOTH | TCMD_WAREG,   //reg_dc_mod (500K); ble: 250k
	  0xa3, 0x10,  TCMD_UNDER_BOTH | TCMD_WAREG,   //pa_ramp_en = 1, pa ramp table max
	  0xaa, 0x2a,  TCMD_UNDER_BOTH | TCMD_WAREG,  //filter iq_swap, 2M bandwidth
	  0x93, 0x50,  TCMD_UNDER_BOTH | TCMD_WAREG,

	  0x0400, 0x03, TCMD_UNDER_BOTH | TCMD_WRITE, // 250K mode
	  0x0401, 0x40, TCMD_UNDER_BOTH | TCMD_WRITE, // pn enable
	  0x0402, 0x26, TCMD_UNDER_BOTH | TCMD_WRITE, // 8-byte pre-amble
	  0x0404, 0xc0, TCMD_UNDER_BOTH | TCMD_WRITE, // head_mode/crc_mode: normal c0
	  0x0405, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE, // access code length 4
	  0x0408, 0xc9, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte3
	  0x0409, 0x8a, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte2
	  0x040a, 0x11, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte1
	  0x040b, 0xf8, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte0

	  0x0420, 0x90, TCMD_UNDER_BOTH | TCMD_WRITE, // sync threshold: 1e (4); 26 (5)
	  0x0421, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE, // no avg
	  0x0422, 0x1a, TCMD_UNDER_BOTH | TCMD_WRITE, // threshold
	  0x0424, 0x52, TCMD_UNDER_BOTH | TCMD_WRITE, // number for sync: bit[6:4]
	  0x0428, 0xe0, TCMD_UNDER_BOTH | TCMD_WRITE,
	  0x042b, 0xf3, TCMD_UNDER_BOTH | TCMD_WRITE, // access code: 1
	  0x042c, 0x88, TCMD_UNDER_BOTH | TCMD_WRITE, // maxiumum length 48-byte
	  0x043b, 0x0c, TCMD_UNDER_BOTH | TCMD_WRITE,
	  0x0464, 0x06, TCMD_UNDER_BOTH | TCMD_WRITE,
	  0x04cd, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE,
	  0x0f03, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE, // bit3: crc2_en; normal 1e

	  //AGC_table
	  0x043a, 0x33, TCMD_UNDER_BOTH | TCMD_WRITE,
	  0x04c0, 0x81, TCMD_UNDER_BOTH | TCMD_WRITE,
	  
	  0x04eb, 0xa0, TCMD_UNDER_BOTH | TCMD_WRITE,//[6]lowswap for rx integer/frac auto calculation(for zigbee +4M problem)


};
/*const TBLCMDSET  tbl_rf_zigbee_250k[] = {
  0x9e, 0xad,  TCMD_UNDER_BOTH | TCMD_WAREG,   //reg_dc_mod (500K); ble: 250k
  0xa3, 0x10,  TCMD_UNDER_BOTH | TCMD_WAREG,   //pa_ramp_en = 1, pa ramp table max
  0xaa, 0x2e,  TCMD_UNDER_BOTH | TCMD_WAREG,  //filter iq_swap, 2M bandwidth
  0x93, 0x50,  TCMD_UNDER_BOTH | TCMD_WAREG,

  0x0400, 0x03, TCMD_UNDER_BOTH | TCMD_WRITE, // 250K mode
  0x0401, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE, // pn enable
  0x0402, 0x23, TCMD_UNDER_BOTH | TCMD_WRITE, // 8-byte pre-amble
  0x0404, 0xc0, TCMD_UNDER_BOTH | TCMD_WRITE, // head_mode/crc_mode: normal c0
  0x0405, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE, // access code length 4
  0x0408, 0xc9, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte3
  0x0409, 0x8a, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte2
  0x040a, 0x11, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte1
  0x040b, 0xf8, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte0
  0x0420, 0x90, TCMD_UNDER_BOTH | TCMD_WRITE, // sync threshold: 1e (4); 26 (5)
  0x0421, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE, // no avg
  0x0422, 0x1a, TCMD_UNDER_BOTH | TCMD_WRITE, // threshold
  0x0424, 0x52, TCMD_UNDER_BOTH | TCMD_WRITE, // number for sync: bit[6:4]
  0x0428, 0xe0, TCMD_UNDER_BOTH | TCMD_WRITE,
  0x042b, 0xf3, TCMD_UNDER_BOTH | TCMD_WRITE, // access code: 1
  0x042c, 0x80, TCMD_UNDER_BOTH | TCMD_WRITE, // maxiumum length 48-byte
  0x043b, 0x0c, TCMD_UNDER_BOTH | TCMD_WRITE,
  0x0464, 0x06, TCMD_UNDER_BOTH | TCMD_WRITE,
  0x04cd, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE,
  0x0f03, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE, // bit3: crc2_en; normal 1e

};*/
const TBLCMDSET  tbl_rf_ble_2m[] = {

	0x9e, 0xad,  TCMD_UNDER_BOTH | TCMD_WAREG, 	//*****reg_dc_mod (500KHz*128/126)****remington  	reg_dc_mod (500K)	0x82				(fix)
	0xa3, 0xd0,  TCMD_UNDER_BOTH | TCMD_WAREG,//********[7:6] disable gauflt [5] LUT 2M or 1M
	0x93, 0x50,  TCMD_UNDER_BOTH | TCMD_WAREG,//*****dac gain settting
	0xaa, 0x2e,  TCMD_UNDER_BOTH | TCMD_WAREG,//*******filter iq_swap, adjust the bandwidth*****remington 0x2e

	0x0400, 0x0f, TCMD_UNDER_BOTH | TCMD_WRITE, // new 2M mode
	0x0401, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x0402, 0x2b, TCMD_UNDER_BOTH | TCMD_WRITE, // 8-byte pre-amble
	0x0404, 0xc5, TCMD_UNDER_BOTH | TCMD_WRITE, // head_mode/crc_mode: normal c5

	0x0405, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE, // access code length 4
	0x0408, 0xf8, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte3
	0x0409, 0x11, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte2
	0x040a, 0x8a, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte1
	0x040b, 0xc9, TCMD_UNDER_BOTH | TCMD_WRITE, // access code byte0

	0x0420, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE, // sync threshold: 1e (4); 26 (5)
	0x0421, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE, // no avg
	0x0422, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE, // threshold
	0x0424, 0x12, TCMD_UNDER_BOTH | TCMD_WRITE, // number fo sync: bit[6:4]
	0x0428, 0x80, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x042b, 0xf1, TCMD_UNDER_BOTH | TCMD_WRITE, // access code: 1
	0x042c, 0x80, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x043b, 0x8c, TCMD_UNDER_BOTH | TCMD_WRITE, //enable timer stamp & dc output ( need move to Rx Setting)
	0x0464, 0x07, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x04cd, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE, // enable packet lenght = 0
	0x0f03, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE,

	0x043a, 0x22, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x04c0, 0x87, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x04eb, 0xe0, TCMD_UNDER_BOTH | TCMD_WRITE,//[6]zigbee
};


// ble 1m mode
const TBLCMDSET  tbl_rf_ble_1m[] = {

	0x9e, 0x56,  TCMD_UNDER_BOTH | TCMD_WAREG, 	//*****reg_dc_mod (500KHz*128/126)****remington  	reg_dc_mod (500K)	0x82				(fix)
	0xa3, 0xf0,  TCMD_UNDER_BOTH | TCMD_WAREG,//********[7:6] disable gauflt [5] LUT 2M or 1M
	0x93, 0x28,  TCMD_UNDER_BOTH | TCMD_WAREG,//*****dac gain settting
	0xaa, 0x26,  TCMD_UNDER_BOTH | TCMD_WAREG,//*******filter iq_swap, adjust the bandwidth*****remington 0x2e

	0x0400, 0x0f, TCMD_UNDER_BOTH | TCMD_WRITE, // new 2M mode
	0x0401, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x0402, 0x2b, TCMD_UNDER_BOTH | TCMD_WRITE, // 8-byte pre-amble
	0x0404, 0xd5, TCMD_UNDER_BOTH | TCMD_WRITE, // head_mode/crc_mode: normal c5
	0x0405, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x0408, 0xc9, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x0409, 0x8a, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x040a, 0x11, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x040b, 0xf8, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x0420, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE, // sync threshold: 1e (4); 26 (5)
	0x0421, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE, // no avg
	0x0422, 0x00, TCMD_UNDER_BOTH | TCMD_WRITE, // threshold
	0x0424, 0x12, TCMD_UNDER_BOTH | TCMD_WRITE, // number fo sync: bit[6:4]
	0x0428, 0x80, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x042b, 0xf1, TCMD_UNDER_BOTH | TCMD_WRITE, // access code: 1
	0x042c, 0x80, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x043b, 0x8c, TCMD_UNDER_BOTH | TCMD_WRITE, //enable timer stamp & dc output ( need move to Rx Setting)
	0x0464, 0x07, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x04cd, 0x04, TCMD_UNDER_BOTH | TCMD_WRITE, // enable packet lenght = 0
	0x0f03, 0x1e, TCMD_UNDER_BOTH | TCMD_WRITE,

	0x043a, 0x22, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x04c0, 0x87, TCMD_UNDER_BOTH | TCMD_WRITE,
	0x04eb, 0xe0, TCMD_UNDER_BOTH | TCMD_WRITE,//[6]
};


const char tbl_rf_power[] = {
  //a2    04    a7    8d
	0x25, 0x7c, 0x67, 0x67,		// 7 dBm
	0x0a, 0x7c, 0x67, 0x67,		// 5 dBm
	0x06, 0x74, 0x43, 0x61,		// -0.6
	0x06, 0x64, 0xc2, 0x61,		// -4.3
	0x06, 0x64, 0xc1, 0x61,		// -9.5
	0x05, 0x7c, 0x67, 0x67,		// -13.6
	0x03, 0x7c, 0x67, 0x67,		// -18.8
	0x02, 0x7c, 0x67, 0x67,		// -23.3
	0x01, 0x7c, 0x67, 0x67,		// -27.5
	0x00, 0x7c, 0x67, 0x67,		// -30
	0x00, 0x64, 0x43, 0x61,		// -37
	0x00, 0x64, 0xcb, 0x61,		// -max	power down PA & PAD
};
//const unsigned  char  TP_GAIN0[3] = {0x5d,0x5d,0x5d};
//const unsigned  char  TP_GAIN1[3] = {0x57,0x57,0x57};
enum   M_RF_MODE   g_rf_mode;
unsigned char rx_fifo_num=0;
unsigned char tx_fifo_num=0;
unsigned char  *  g_rf_stx_buffer_ptr;
unsigned char  *  g_rf_srx_buffer_ptr;
unsigned char  * g_rf_rx_buffer_ptr;
int				 g_rf_rx_buffer_size;
unsigned char    g_rf_rx_pingpong_en;
unsigned short g_rf_tx_single_fifo_length;
unsigned short g_rf_rx_single_fifo_length;
unsigned char 	empty__packet[6] __attribute__ ((aligned (4))) = {0x02,0x00,0x00,0x00,0x00,0x00};
unsigned char  packet[128] __attribute__ ((aligned (4))) = {0x22,0x00,0x00,0x00,0x00,0x20,0x00,0xaa,0xaa,0xaa,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};


/**********************************************************************************************************************
 *                                                 局部函数原型                                                       *
 *********************************************************************************************************************/
unsigned int Rf_FsmIsIdle(void);
unsigned int Rf_BrxFifoIsEmpty(void);
void Rf_BrxIncRptr(void);
unsigned int Rf_GetBtxWptr(void);
unsigned int Rf_GetBtxRptr(void);
void Rf_BtxIncWptr(int ba);

/*void Rf_Init_setting(void)
{
	LoadTblCmdSet (tbl_rf_ini, sizeof (tbl_rf_ini)/sizeof (TBLCMDSET));
}*/
/**********************************************************************************************************************
 *                                                 全局函数实现                                                       *
 *********************************************************************************************************************/
/**********************************************************************************************************************
* 函数名称：int	 Rf_Init( enum  M_OSC_SEL  osc_sel,enum M_RF_MODE m)
* 功能描述：对RF功能进行初始化
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
int	 Rf_Init( enum  M_OSC_SEL  osc_sel,enum M_RF_MODE m)
{
	unsigned  char   i;
	LoadTblCmdSet (tbl_rf_ini, sizeof (tbl_rf_ini)/sizeof (TBLCMDSET));
	if(osc_sel == RF_OSC_12M)
	{
		LoadTblCmdSet (tbl_rf_ini_12M, sizeof (tbl_rf_ini_12M)/sizeof (TBLCMDSET));
	}
	else
	{
		LoadTblCmdSet (tbl_rf_ini_16M, sizeof (tbl_rf_ini_16M)/sizeof (TBLCMDSET));
	}
	for (i=0; i<26; i++)
		write_reg8 (0x800480+i, tbl_agc[i]);	//set AGC table

	//LoadTblCmdSet (tbl_rf_ini_manual_agc, sizeof (tbl_rf_ini_manual_agc)/sizeof (TBLCMDSET));

	if(m == RF_MODE_BLE_2M)
	{
		LoadTblCmdSet (tbl_rf_ble_2m, sizeof (tbl_rf_ble_2m)/sizeof (TBLCMDSET));
	}
	else if(m == RF_MODE_BLE_1M)
	{
		LoadTblCmdSet (tbl_rf_ble_1m, sizeof (tbl_rf_ble_1m)/sizeof (TBLCMDSET));
	}
	else if(m == RF_MODE_ZIGBEE_250K)
	{
	   LoadTblCmdSet (tbl_rf_zigbee_250k, sizeof (tbl_rf_zigbee_250k)/sizeof (TBLCMDSET));

	}
	else
	{
		return  -1;
	}

	//g_rf_mode = m;
	return  0;
}
/**********************************************************************************************************************
* 函数名称：void Rf_ModeSet(enum M_RF_MODE m)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_ModeSet(enum M_RF_MODE m)
{
	if(m == RF_MODE_BLE_2M)
	{
		LoadTblCmdSet (tbl_rf_ble_2m, sizeof (tbl_rf_ble_2m)/sizeof (TBLCMDSET));
	}
	else if(m == RF_MODE_BLE_1M)
	{

		LoadTblCmdSet (tbl_rf_ble_1m, sizeof (tbl_rf_ble_1m)/sizeof (TBLCMDSET));
	}
	else if(m == RF_MODE_ZIGBEE_250K)
	{
	   LoadTblCmdSet (tbl_rf_zigbee_250k, sizeof (tbl_rf_zigbee_250k)/sizeof (TBLCMDSET));
	}
	///g_rf_mode = m;
}


/**********************************************************************************************************************
* 函数名称：void Rf_BaseBandReset (void)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_BaseBandReset (void)
{
	write_reg8(0x800060,0x80);
	write_reg8(0x800060,0x00);
}

/**********************************************************************************************************************
* 函数名称：unsigned char  Rf_TrxStateSet(enum M_RF_STATE  state ,signed char channel)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
unsigned char  Rf_TrxStateSet(enum M_RF_STATE  state ,signed char channel)
{
    unsigned char  err = 0;
	unsigned  long  fre;

	if(state == RF_MODE_TX)
	{
		write_reg8  (0x800f02, 0x45);
		fre = 2400 + channel;
		write_reg16 (0x8004d6, fre);
		write_reg8  (0x800f02, 0x55);
		//write_reg8  (0x80050f, 0x80);
	}
	else if(state == RF_MODE_RX)
	{
		write_reg8  (0x800f02, 0x45);
		fre = 2400 + channel;
		write_reg16 (0x8004d6, fre);
		write_reg8  (0x800f02, 0x65);
		write_reg8 (0x800428, read_reg8(0x800428)| BIT(0));	// rx enable
	}
	else if(state == RF_MODE_AUTO)
	{
		fre = 2400 + channel;
		write_reg16 (0x8004d6, fre);
		//write_reg16(0x800f1c, 0x03); //tx_irq and rx_irq
		//write_reg8(0x800641, 0x20);
		//write_reg8(0x800643, 0x01);
	}
	else if(state == RF_MODE_OFF)//和channel的关系？
    {
	    WriteAnalogReg (0x06,0xff); 		//  power down analog module 
		WriteAnalogReg (0x05,0xe2); 		//  power down analog module
	}
	else
	{
	    err = 1;
	}
	return  err;

}
/**********************************************************************************************************************
* 函数名称：void Rf_TxPkt (unsigned char* addr)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_TxPkt (unsigned char* addr)
{
		write_reg16 (0x80050c, (unsigned short)(addr));
		//write_reg8(0x80050e,0x10);
		//write_reg8  (0x80050f, 0x80);
		write_reg8 (0x800524, 0x08);
}
/**********************************************************************************************************************
* 函数名称：void Rf_StartStx  (unsigned char* tx_addr,unsigned int tick)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_StartStx  (unsigned char* tx_addr,unsigned int tick)
{
	 write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
	write_reg32(0x800f18, tick);						// Setting schedule trigger time
	write_reg16 (0x80050c, (unsigned short)(tx_addr));
	write_reg8 (0x800f00, 0x85);						// single TX

	//write_reg8(0x80050e,0x10);
	//write_reg8  (0x80050f, 0x80);
}

/**********************************************************************************************************************
* 函数名称：void Rf_StartStxToRx  ( unsigned char* tx_addr ,unsigned int tick,unsigned short timeout_us)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_StartStxToRx  ( unsigned char* tx_addr ,unsigned int tick,unsigned short timeout_us)
{
	//write_reg8 (0x800f03, 0x1f);
	//write_reg32 (0x800f2c, 0x0fffffff);
	write_reg16(0x800f0a, timeout_us-1);
	write_reg32(0x800f18, tick);						// Setting schedule trigger time
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
    write_reg16 (0x80050c, (unsigned short)(tx_addr));
	write_reg8  (0x800f00, 0x87);						// single tx2rx
}
/**********************************************************************************************************************
* 函数名称：void Rf_StartSrxToTx  (unsigned char* tx_addr  ,unsigned int tick,unsigned int timeout_us)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_StartSrxToTx  (unsigned char* tx_addr  ,unsigned int tick,unsigned int timeout_us)
{
	write_reg32 (0x800f28, timeout_us-1);					// first timeout
	write_reg32(0x800f18, tick);						// Setting schedule trigger time
    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
    write_reg16 (0x80050c, (unsigned short)(tx_addr));
	write_reg16 (0x800f00, 0x88);						// single rx2tx
}

/**********************************************************************************************************************
* 函数名称：void  Rf_RxBufferSet(unsigned char *  addr, int size, unsigned char  pingpong_en)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void  Rf_RxBufferSet(unsigned char *  addr, int size, unsigned char  pingpong_en)
{
    unsigned char mode;
	mode = pingpong_en ? 0x03 : 0x01;
    write_reg16 (0x800508, (unsigned short)(addr));
    write_reg8(0x80050a, size>>4);
	write_reg8(0x80050b,mode);
	g_rf_rx_buffer_ptr = (unsigned char *) addr;
	g_rf_rx_buffer_size = size;
	g_rf_rx_pingpong_en = pingpong_en;
}
/**********************************************************************************************************************
* 函数名称：unsigned char Rf_RxBufferRequest(void)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
unsigned char Rf_RxBufferRequest(void)
{
    int i;
	int loop;
	unsigned char * ptr;
	loop = g_rf_rx_pingpong_en ? 2 : 1;
	ptr = NULL;
	for(i=0;i<loop;i++)
	{
		if (read_reg8(0x800526+i) & 0x04)  // is not empty
		{
			 break;
		}
	}
	return i;   // 0  &  1  

}
/**********************************************************************************************************************
* 函数名称：void  Rf_RxBufferClearFlag(unsigned char  idx)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void  Rf_RxBufferClearFlag(unsigned char  idx)
{
	write_reg8(0x800526+idx, 0x04);
}
/**********************************************************************************************************************
* 函数名称：unsigned char  RF_TxFinish(void)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
unsigned char  RF_TxFinish(void)
{
	return  (read_reg8(0x800f20) & 0x02);
	//return  read_reg8(0x800524);
}
/**********************************************************************************************************************
* 函数名称：void Rf_TxFinishClearFlag (void)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_TxFinishClearFlag (void)
{
	write_reg8 (0x800f20, read_reg8(0x800f20)|0x02);
}


/**********************************************************************************************************************
* 函数名称： void RF_TxAccessCodeSelect (unsigned char  idx)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_TxAccessCodeSelect (unsigned char  idx)
{
	write_reg8 (0x800f15, read_reg8(0x800f15) & 0xf8 | idx);//Tx_Channel_man[2:0]
}
void Rf_AccessCodeLengthSetting (unsigned char length)
{
	write_reg8 (0x800405,read_reg8(0x800405)&0xf8|length);//access_byte_num[2:0]
	if( 5 == length )
		{

			write_reg8 (0x800420,0x28);//access_bit_threshold
		}
		else if( 4 == length )
		{

			write_reg8 (0x800420,0x20);//access_bit_threshold
		}
		else if( 3 == length )
		{

			write_reg8 (0x800420,0x18);//access_bit_threshold
		}
}
/**********************************************************************************************************************
* 函数名称：void RF_AccessCodeSetting01 (unsigned char  idx,unsigned long long  access_code)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_AccessCodeSetting01 (unsigned char  idx,unsigned long long  access_code)
{
	unsigned char length=0;
	length = read_reg8 (0x800405);
	if( 5 == length )
	{
		if(0 == idx)
		{
			write_reg32 (0x800408,(unsigned int)access_code);//access_code 0
			write_reg8 (0x80040c,(unsigned char)(access_code>>32));
		}
		else if(1 == idx)
		{
			write_reg32 (0x800410,(unsigned int)access_code);//access_code 1
			write_reg8 (0x800414,(unsigned char)(access_code>>32));
		}
	}
	else if( 4 == length )
	{
		if(0 == idx)
		{
			write_reg32 (0x800408,(unsigned int)access_code);//access_code 0
		}
		else if(1 == idx)
		{
			write_reg32 (0x800410,(unsigned int)access_code);//access_code 1
		}
	}
	else if( 3 == length )
	{
		if(0 == idx)
		{
			write_reg16 (0x800408,(unsigned short)access_code);//access_code 0
			write_reg8 (0x80040a,(unsigned char)(access_code>>16));//access_code 0
		}
		else if(1 == idx)
		{
			write_reg16 (0x800410,(unsigned short)access_code);//access_code 1
			write_reg8 (0x800412,(unsigned char)(access_code>>16));
		}

	}
}
/**********************************************************************************************************************
* 函数名称：void RF_AccessCodeSetting2345 (unsigned char  idx,unsigned char  prefix)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_AccessCodeSetting2345 (unsigned char  idx,unsigned char  prefix)
{
	write_reg8 (0x800418+idx-2,prefix);//access_code 2~5
}
/**********************************************************************************************************************
* 函数名称： void RF_RxAccessCodeEnable (unsigned char enable)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_RxAccessCodeEnable (unsigned char enable)
{
	write_reg8 (0x800407,read_reg8(0x800407) & 0xc0 | enable);//rx_access_code_chn_en
}
/**********************************************************************************************************************
* 函数名称：void Rf_PowerLevelSet(enum M_RF_POWER level)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_PowerLevelSet(enum M_RF_POWER level)
{
	unsigned char *p;
	if (level + 1 > (sizeof (tbl_rf_power)>>2)) {
		level = (sizeof (tbl_rf_power)>>2) - 1;
	}
	p = tbl_rf_power + level * 4;
	WriteAnalogReg (0xa2, *p ++);
	WriteAnalogReg (0x04, *p ++);
	WriteAnalogReg (0xa7, *p ++);
	WriteAnalogReg (0x8d, *p ++);
}


/**********************************************************************************************************************
* 函数名称： void Rf_EmiCarrierOnlyTest(enum M_RF_POWER power_level,signed char rf_chn)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_EmiCarrierOnlyTest(enum M_RF_POWER power_level,signed char rf_chn)
{
	Rf_TrxStateSet(RF_MODE_TX,rf_chn);
	Rf_PowerLevelSet(power_level);
	WriteAnalogReg(0xa5,0x44);   // for carrier  mode
	write_reg8 (0x8004e8, 0x04); // for  carrier mode
	//return 1;
}
/**********************************************************************************************************************
* 函数名称： void Rf_EmiCarrierDataTest(enum M_RF_POWER power_level,signed char rf_chn)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/

void Rf_EmiCarrierDataTest(enum M_RF_POWER power_level,signed char rf_chn)
{

	int i;
	int state0,state1,state2,state3, feed;
	unsigned char cmd_now = read_reg8(0x808007) & 0x7f;
	Rf_TrxStateSet(RF_MODE_TX,rf_chn);
	Rf_PowerLevelSet(power_level);

	write_reg8(0x80050e,0x2); // this size must small than the beacon_packet dma send length
	state0 = STATE0;
	state1 = STATE1;
	state2 = STATE2;
	state3 = STATE3;
	write_reg32((packet+28),(state0<<16)+state1); // the last value
	write_reg8(0x80050f, 0x80);  // must fix to 0x80
	write_reg8(0x800402, 0x21);	//preamble length=1
    Rf_TxPkt(packet);
    while(1)
	{
		write_reg32((packet+28),(state0<<16)+state1); // the last value
		//advance PN generator
		state0 = pnGen(state0);
		state1 = pnGen(state1);
	}
	//return 1;
}


/**********************************************************************************************************************
 *                      局部函数实现                                      *
 *********************************************************************************************************************/
unsigned int Rf_FsmIsIdle(void)
{
    return (read_reg8(SS)==0);
}
unsigned int Rf_BrxFifoIsEmpty(void)
{
    return (read_reg8 (RX_RPTR) == read_reg8 (RX_WPTR));
}
void Rf_BrxIncRptr(void)
{
	 write_reg8 (RX_RPTR, 0x20);
}
unsigned int Rf_GetBtxWptr(void)
{
	return read_reg8 (TX_WPTR);
}

unsigned int Rf_GetBtxRptr(void)
{
	return read_reg8 (TX_RPTR);
}
void Rf_BtxIncWptr(int ba)
{
	write_reg16(0x80052c, ba);

}

int pnGen(int state)
{
	int feed = 0;
	feed = (state&0x4000) >> 1;
	state ^= feed;
	state <<= 1;
	state = (state&0xfffe) + ((state&0x8000)>>15);
	return state;
}
/**********************************************************************************************************************
 *                      保留函数                                      *
 *********************************************************************************************************************/

// just for debug

// set max receive length  // need check
//0x042c
void  Rf_SetMaxRcvLen(unsigned char len)
{
   write_reg8(0x80042c,len);
}

/**********************************************************************************************************************
* 函数名称：void Rf_StxrxFifoSetting(void* tx_addr,void* rx_addr,int tx_single_fifo_length,int rx_single_fifo_length,unsigned char tx_fifo_size,unsigned char rx_fifo_size)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
void Rf_StxrxFifoSetting(void* tx_addr,void* rx_addr,unsigned short tx_single_fifo_length,unsigned short rx_single_fifo_length,unsigned char tx_fifo_size,unsigned char rx_fifo_size)
{
	write_reg16 (0x80050c, (unsigned short)((unsigned int)tx_addr));
	g_rf_stx_buffer_ptr = (unsigned char *) tx_addr;
	g_rf_tx_single_fifo_length=tx_single_fifo_length;
	write_reg8(0x80050e, tx_single_fifo_length>>4);
	//	write_reg8(0x80050e,0x08);
		//write_reg8  (0x80050f, 0x80);
	if(tx_fifo_size==1)
	{
		write_reg8(0x80050f,0x00);
		tx_fifo_num=1;
	}
	else if(tx_fifo_size==2)
	{
		write_reg8(0x80050f,0x40);
		tx_fifo_num=2;
	}
	else if(tx_fifo_size==4)
	{
		write_reg8(0x80050f,0x80);
		tx_fifo_num=4;
	}
	else if(tx_fifo_size==8)
	{
		write_reg8(0x80050f,0xc0);
		tx_fifo_num=8;
	}
	write_reg16 (0x800508, (unsigned short)(rx_addr));
	g_rf_srx_buffer_ptr = (unsigned char *) rx_addr;
//	write_reg8(0x80050a, 0x08);
	g_rf_rx_single_fifo_length=rx_single_fifo_length;
	write_reg8(0x80050a, rx_single_fifo_length>>4);

	if(rx_fifo_size==1)
	{
		write_reg8(0x80050b,0x01);
		rx_fifo_num=1;
	}
	else if(rx_fifo_size==2)
	{
		write_reg8(0x80050b,0x43);
		rx_fifo_num=2;
	}
	else if(rx_fifo_size==4)
	{
		write_reg8(0x80050b,0x83);
		rx_fifo_num=4;
	}
	else if(rx_fifo_size==8)
	{
		write_reg8(0x80050b,0xc3);
		rx_fifo_num=8;
	}
}

/**********************************************************************************************************************
* 函数名称：unsigned char *Rf_RxReadDataAddr(void)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
unsigned char *Rf_RxReadDataAddr(void)
{
	//static unsigned long packet_num=0;
	unsigned int rx_addr=0;
	unsigned char ptr=0;
	unsigned char * data_addr_ptr=NULL;
	  if(!Rf_BrxFifoIsEmpty())
	  {
		 ptr = read_reg8 (RX_RPTR);
		 rx_addr = g_rf_srx_buffer_ptr + (ptr % rx_fifo_num) * g_rf_rx_single_fifo_length;
		 data_addr_ptr=rx_addr;
		 Rf_BrxIncRptr();
		 return data_addr_ptr;
	  }
	  else
	  {
		  //my_printf("no data\n");
		  return NULL;
	  }
}
/**********************************************************************************************************************
* 函数名称：unsigned char *Rf_TxPushPktAddr(void)
* 功能描述：
* 输入参数：
* 输出参数：
* 返 回 值：
**********************************************************************************************************************/
unsigned char *Rf_TxPushPktAddr(void)
{
	//static unsigned long packet_num=0;
	unsigned int tx_addr=0;
	unsigned char ptr=0;
	unsigned char * data_addr_ptr=NULL;

	 ptr = Rf_GetBtxWptr();
	 tx_addr = g_rf_stx_buffer_ptr + (ptr % tx_fifo_num) * g_rf_tx_single_fifo_length;
	 data_addr_ptr=tx_addr;
	 Rf_BtxIncWptr(tx_addr);
	 return data_addr_ptr;

}
