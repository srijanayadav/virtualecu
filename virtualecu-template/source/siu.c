/*
 *	Project Owner	: TU Chemnitz
 *	Project Name	: ASE Tutorial Unit-2
 *	File Name		: siu.c
 *	Author			: ASE Admin
 *  Created on		: 25 Feb 2024
 */

/* Includes ******************************************************************/
#include "siu.h"

/*
 * @brief	SIU pin configuration function
 *
 * @param	void
 * @retval	void
 */
void SIU_Init(void){

	/* Input pin configurations */
	SIU.PCR[60].R = 0x0100; // BT1 - D[12]
	SIU.PCR[62].R = 0x0100; // BT2 - D[14]
	SIU.PCR[52].R = 0x0100; // SW1 - D[4]
	SIU.PCR[53].R = 0x0100; // SW2 - D[5]
	SIU.PCR[54].R = 0x0100; // SW3 - D[6]
	SIU.PCR[55].R = 0x0100; // SW4 - D[7]


	/* Output pin configurations */
	SIU.PCR[56].R = 0x0200; // P - D[8]
	SIU.PCR[57].R = 0x0200; // Rx - D[9]
	SIU.PCR[58].R = 0x0200; // Tx - D[10]
	SIU.PCR[59].R = 0x0200; // U1 - D[11]
	SIU.PCR[43].R = 0x0200; // U2 - C[11]
	SIU.PCR[6].R = 0x0200; // U3 - A[6]


	/* Analog pin configurations */
	SIU.PCR[66].R = 0x2500; // POT - E[2]
	SIU.PCR[32].R = 0x2500; // LDR- C[0]



	/* TO-DO: your task implementations **************************************/

	/* Setup CAN controller pins */

	/* TX - transmitting */
	SIU.PCR[16].B.PA = 1; // ALT1 for TXD
	SIU.PCR[16].B.OBE = 1; //Output Buffer Enable = 1 
	SIU.PCR[16].B.IBE = 0; //Input Buffer Enable = 0 

	/* RX - receiving */
	SIU.PCR[17].B.PA = 1; //ALT1 for RXD
	SIU.PCR[17].B.OBE = 0; //Output Buffer Enable = 0
	SIU.PCR[17].B.IBE = 1; //Input Buffer Enable = 0

	SIU.PSMI[0].B.PADSEL = 0x1;
	SIU.PSMI[33].B.PADSEL = 0x1;

	/*************************************************************************/
}

