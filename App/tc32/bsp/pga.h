#ifndef		pga_H
#define		pga_H


//set ANA_C<3> and ANA_C<2> as positive and minus input of the PGA
#define		setPGAchannel_ONE			(*(volatile unsigned char  *)0x800028 &= 0xFE)	

//set ANA_C<5> and ANA_C<4> as positive and minus input of the PGA
#define		setPGAchannel_TWO			(*(volatile unsigned char  *)0x800028 |= 0x01)

enum PREAMPValue{
	DBP0,
	DB20,
};

enum POSTAMPValue{
	DB0,
	DB3,
	DB6,
	DB9,
};


#endif
