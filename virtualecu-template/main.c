/*
 *	Project Owner	: TU Chemnitz
 *	Project Name	: ASE Tutorial Unit-2
 *	File Name		: main.c
 *	Author			: ASE Admin
 *  Created on		: 25 Feb 2024
 */

/* Includes ******************************************************************/
#include "init.h"
#include <string.h>
#include "stdio.h"
#include <stdint.h>
#include "siu.h"
#include "can.h"
/* Global variables **********************************************************/
int speed = 0;
int restart_flag = 0;
int rpm = 8000;					  // Start at 8000 RPM
int rpm_direction = -250;		  // Initial direction to decrease RPM by 250
int increase_decrease_toggle = 1; // for decrement

int can_message_11_flag = 0; // Tracks if 0x11 was received
int led_u1_state = 0;		 // Tracks the current state of LED_U1

void can_send_engine_on(void)
{

	// Use buffer index X (replace X with the specific index)
	int buffer_index = 8; // For example, using buffer 8

	// Set the message ID to 0xDA0 (standard frame format)
	CAN_0.BUF[8].MSG_ID.B.STD_ID = 0xDA0; // Standard ID
	CAN_0.BUF[8].MSG_ID.B.EXT_ID = 0;	  // Ensure extended ID is 0

	// Set payload length to 8 bytes
	// CAN_0.BUF[8].CS.B.LENGTH = 0;
	CAN_0.BUF[buffer_index].CS.B.LENGTH = 8;
	// Example message to send
	const char *message = "ENGINEON";

	// Initialize payload data - replace with specific data if needed
	for (int i = 0; i < 8; i++)
	{
		// CAN_0.BUF[8].DATA.B[i] = 0;  // Default data, set to 0
		if (i < strlen(message))
		{
			CAN_0.BUF[buffer_index].DATA.B[i] = message[i]; // Copy message character
		}
		else
		{
			CAN_0.BUF[buffer_index].DATA.B[i] = 0; // Fill remaining bytes with 0
		}
	}

	// Set CODE to 0xC to instruct CAN controller to transmit
	// CAN_0.BUF[8].CS.B.CODE = 0xC;
	CAN_0.BUF[buffer_index].CS.B.CODE = 0xC; // Transmit request

	// Toggle LED_Tx to indicate message transmission
	LED_Tx = !LED_Tx;
}
void can_send_string(const char *str)
{
	int i;
	int len = strlen(str);

	if (len > 8)
		len = 8;

	for (i = 0; i < len; i++)
	{
		CAN_0.BUF[5].DATA.B[i] = str[i];
	}

	for (i = len; i < 8; i++)
	{
		CAN_0.BUF[5].DATA.B[i] = 0;
	}

	CAN_0.BUF[5].MSG_ID.B.EXT_ID = 0x7299; // Extended ID (if needed)

	// Set the length of the payload (up to 8 bytes)
	CAN_0.BUF[5].CS.B.LENGTH = len;

	// Set the frame type to extended (1 for extended frame, 0 for standard)
	CAN_0.BUF[5].MSG_CS.B.IDE = 1; // 1 means Extended Frame

	// Request the CAN controller to transmit the message
	CAN_0.BUF[5].CS.B.CODE = 0xC; // Transmit request
}

void can_send_speed(int value)
{
	/* TO-DO: your task implementations **************************************/
	// Map potentiometer value (0 to 4095) to speed (0 to 240 km/h)
	int speed = (value * 240) / 4095;
	// Prepare a message buffer for "Speed" and value
	char message[8];
	sprintf(message, "SPD:%03d", speed); // Format as "SPD:XXX" (e.g., "SPD:120")

	// Prepare the CAN message for speed
	CAN_0.BUF[1].DATA.B[0] = speed; // Here the speed is display in hexadecimal
	CAN_0.BUF[1].MSG_ID.B.STD_ID = 0x5A0;
	CAN_0.BUF[1].CS.B.LENGTH = 1;
	CAN_0.BUF[1].CS.B.CODE = 0xC; // Transmit code

	can_send_string(message);
}

void can_send_rpm(int value)
{
	/* TO-DO: your task implementations **************************************/
	// Update RPM based on direction
	// Split the RPM value into two bytes in little-endian format
	printf("value of rpm: %d\n", value);
	CAN_0.BUF[2].DATA.B[0] = (value >> 8) & 0xFF; // Lower byte of RPM
	CAN_0.BUF[2].DATA.B[1] = value & 0xFF;		  // Higher byte of RPM

	// Set the message ID to 0x280 for RPM
	CAN_0.BUF[2].MSG_ID.B.STD_ID = 0x280;
	CAN_0.BUF[2].CS.B.LENGTH = 2; // Data length of 2 bytes
	CAN_0.BUF[2].CS.B.CODE = 0xC; // Transmit code

	// Optional: Toggle LED to indicate transmission
	// LED_Tx = !LED_Tx;
}
void update_rpm(void)
{
	// Update RPM by the current direction
	rpm += rpm_direction;

	// Reverse direction if RPM reaches the limits (0 or 8000)
	if (rpm == 0)
	{
		increase_decrease_toggle = 0; // For increment
	}
	if (rpm == 8000)
	{
		increase_decrease_toggle = 1;
	}
	if (increase_decrease_toggle)
	{
		rpm = rpm;
		rpm_direction = -250; // Change direction to decrease
	}
	else
	{
		rpm = rpm;
		rpm_direction = 250; // Change direction to increase
	}
	// Send the updated RPM value over CAN
	can_send_rpm(rpm);
}

void can_send_lights_status(int left_indicator, int right_indicator)
{
	// Determine the indicator status based on inputs
	uint8_t indicator_status = 0x00; // Default: No indicator on

	if (left_indicator && right_indicator)
	{
		indicator_status = 0x03; // Hazard indicator on (both indicators)
	}
	else if (right_indicator)
	{
		indicator_status = 0x01; // Left indicator on
	}
	else if (left_indicator)
	{
		indicator_status = 0x02; // Right indicator on
	}

	// Prepare and send the CAN message
	CAN_0.BUF[3].DATA.B[0] = indicator_status; // Set the indicator status byte
	CAN_0.BUF[3].MSG_ID.B.STD_ID = 0x470;	   // Set the message ID for lights
	CAN_0.BUF[3].CS.B.LENGTH = 1;			   // Data length of 1 byte
	CAN_0.BUF[3].CS.B.CODE = 0xC;			   // Transmit code

	// Toggle LED indicators for visual feedback (optional, if you want LEDs to blink)
	if (indicator_status == 0x01 && can_message_11_flag)
	{
		const char *message = "Left On";
		CAN_0.BUF[7].CS.B.LENGTH = 8;
		// Initialize payload data - replace with specific data if needed
		for (int i = 0; i < 8; i++)
		{
			// CAN_0.BUF[8].DATA.B[i] = 0;  // Default data, set to 0
			if (i < strlen(message))
			{
				CAN_0.BUF[7].DATA.B[i] = message[i]; // Copy message character
			}
			else
			{
				CAN_0.BUF[7].DATA.B[i] = 0; // Fill remaining bytes with 0
			}
		}

		// Set CODE to 0xC to instruct CAN controller to transmit
		// CAN_0.BUF[8].CS.B.CODE = 0xC;
		CAN_0.BUF[7].CS.B.CODE = 0xC; // Transmit request
		LED_U1 = !LED_U1;			  // Blink Left LED
	}
	else if (indicator_status == 0x02)
	{
		const char *message = "Right On";
		CAN_0.BUF[7].CS.B.LENGTH = 8;
		// Initialize payload data - replace with specific data if needed
		for (int i = 0; i < 8; i++)
		{
			// CAN_0.BUF[8].DATA.B[i] = 0;  // Default data, set to 0
			if (i < strlen(message))
			{
				CAN_0.BUF[7].DATA.B[i] = message[i]; // Copy message character
			}
			else
			{
				CAN_0.BUF[7].DATA.B[i] = 0; // Fill remaining bytes with 0
			}
		}

		// Set CODE to 0xC to instruct CAN controller to transmit
		// CAN_0.BUF[8].CS.B.CODE = 0xC;
		CAN_0.BUF[7].CS.B.CODE = 0xC; // Transmit request
		LED_U3 = !LED_U3;			  // Blink Right LED
	}
	else if (indicator_status == 0x03)
	{
		const char *message = "Hazard";
		CAN_0.BUF[7].CS.B.LENGTH = 8;
		// Initialize payload data - replace with specific data if needed
		for (int i = 0; i < 8; i++)
		{
			// CAN_0.BUF[8].DATA.B[i] = 0;  // Default data, set to 0
			if (i < strlen(message))
			{
				CAN_0.BUF[7].DATA.B[i] = message[i]; // Copy message character
			}
			else
			{
				CAN_0.BUF[7].DATA.B[i] = 0; // Fill remaining bytes with 0
			}
		}

		// Set CODE to 0xC to instruct CAN controller to transmit
		// CAN_0.BUF[8].CS.B.CODE = 0xC;
		CAN_0.BUF[7].CS.B.CODE = 0xC; // Transmit request
		LED_U1 = !LED_U1;			  // Blink Left LED for Hazard
		LED_U3 = !LED_U3;			  // Blink Right LED for Hazard
	}
	else
	{
		// LED_U1 = 0;        // Turn off Left LED
		LED_U3 = 0; // Turn off Right LED
		LED_U1 = led_u1_state;
	}
}

void can_send_lights(void)
{
	/* TO-DO: your task implementations **************************************/
	can_send_lights_status(SW1, SW2);
}

void can_send(void)
{
	/* TO-DO: your task implementations **************************************/
	int pot_value = (ADC0.CDR[5].R & 0x00000FFF);
	can_send_engine_on();
	can_send_speed(pot_value);
	update_rpm();
	can_send_lights();
	// can_send_string("hello");
	/*************************************************************************/
}

/*
 * @brief	Main program
 *
 * @param	void
 * @retval	int
 */
int main(void)
{
	/* peripherals initialization */
	peripheralsInit();

	/* TO-DO: your task implementations **************************************/

	/*************************************************************************/

	/* main program */
	while (1)
	{
		/* System function */
		systemFunction();
		/* TO-DO: your task implementations **********************************/
		if (restart_flag)
		{
			can_send();
		}
		/*********************************************************************/
		/* 10 ms OS delay */
		osalThreadDelayMilliseconds(10UL);
	}
}

/*
 * @brief	CAN receive interrupt
 *
 * @param   CANDriver   canp	The CAN controller which received a message
 * @param	CANRxFrame	crfp	The message which was received
 * @retval	void
 */
void can_receive(CANRxFrame crfp)
{
	if (crfp.IDE == CAN_IDE_STD) // a standard message frame has been received
	{
		/* TO-DO: your task implementations ******************************/
		// Task 1
		LED_Rx = !LED_Rx;	  // toggle Rx when any msg is recieved
		if (crfp.SID == 0x10) // check the Message ID
		{
			// LED_Rx = !LED_Rx; // toggle Rx when msg is recieved
			LED_P = 0;	// Turn off LED_P
			LED_Tx = 0; ////Turn off LED_Tx
			restart_flag = 0;
			can_message_11_flag = 0;
			led_u1_state = 0;
			LED_U1 = 0;
		}
		else if (crfp.SID == 0x11)
		{
			LED_U1 = !LED_U1; // only toggle U1 when SID 0x11 is recieved
			LED_P = 1;		  // Turn on LED_P. Engine ON
			restart_flag = 1;
			led_u1_state = LED_U1;	 // Update LED_U1 state
			can_message_11_flag = 1; // Set flag for 0x11
		}
	}
}

/*
 * @brief	PIT timer channel 1 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_1(void)
{
	/* TO-DO: your task implementations **************************************/
	// can_send_engine_on();
}

/*
 * @brief	PIT timer channel 2 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_2(void)
{
	/* TO-DO: your task implementations **************************************/
}

/*
 * @brief	PIT timer channel 3 IRQ callback
 *
 * @param	void
 * @retval	void
 */
void PIT_Channel_3(void)
{
	/* TO-DO: your task implementations **************************************/
}
