/************************************************************************************************************************
*                                                                                                                       *
*                                                   COPYRIGHTS                                                          *
*                                                                                                                       *
/************************************************************************************************************************
RFM69OOK.h - RFM69OOK library

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later
version.
This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
Warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the
Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This library makes reference to RFM69(H)W_OOK_Library_Vx.y.pdf
/***********************************************************************************************************************/
/************************************************************************************************************************
*                                                                                                                       *
*                                                      DESCRIPTION                                                      *
*                                                                                                                       *
/************************************************************************************************************************
* Version:      1.4
* Date:         15/11/2017
* Author:       Robert
* Description:  Library for OOK Kaku alike transmission using RFM69 transceivers
* Compiler:     - Arduino Compiler 1.83
* Hardware:     Tested with:
*				- Moteino R4 and Moteino Mega with RFM69 DIO2 connection
*				- Moteino Mega with RFM69 DIO2 connection
*				- Anarduino MiniWireless69 with RFM69 DIO2 connection
*               - Arduino UNO R3 + RFM69(H)W Board
*               - Arduino Ethernet R3 + RFM69(H)W Board
*               - Arduino ATMEGA2560 + RFM69(H)W Shield
*               - Arduino ATMEGA2560 + Ethernet Shield RFM69(H)W Shield
*				- ESP8266
* Software:     Tested with RFM69 Library version 9 May 2015 (see https://github.com/LowPowerLab/RFM69)
* Documentation:
				RFM69(H)W_OOK_Library_Vx.y.pdf (see https://github.com/rrobinet/RFM69OOK )
* Notes:        1. 	This library is currently limited to OOK TRANSMISSION using a RFM69(H)W transceivers
*					(RFM69(H)CW and OOK RECEPTION are not scoped)
*				2.	Functions are limited to a set of well known OOK protocols aka KAKU protocols
*					(Klik aan Klik uit) more specifically to KAKU OLD, KAKU NEW and KAKU GOGEX
*					(see https://github.com/rrobinet/SAW_Devices_and_OOK/blob/master/OOK_Protocols_decription_V0.0.pdf)
* 				3.  This is a companion of the RFM69 library, by adding OOK modulation functions
*        		4.  OOK modulation is obtained by offering data to the DIO2 pin of the RFM69 transceiver.
*                   Therefore a modified version of the RFM69(H)W on current platform (Moteino and MiniWireless69) is
*                   necessary. Alternatively the RFM69(H)W Board or RFM69(H)W Shield may be used.
*					see (https://github.com/rrobinet/RFM69/blob/master/RFM69(H)W_Board_and_Shield_V0.0.pdf )
* 				5. 	The library functions when activated, temporarily change the Modulation and Operation mode of
*					the RFM69 FSK settings for OOK modulation, settings are restored after the function is terminated
*				6.	Default parameters are:
*					- RFM69 DI02 connected to:
*												- Moteino R4					Pin 3
*												- MiniWireless69				Pin 3
*												- Moteino Mega					Pin 11
*												- Arduino Uno					Pin 3 	using the RFM69(H)W Board
*												- Arduino Ethernet 				Pin 3 	using the RFM69(H)W Board
*												- Arduino Leonardo				Pin 2	using the RFM69(H)W Board
*												- ATMEGA 2560					Pin 18	using the RFM69(H)W Shield
*												- Expressive ESP8266 (ESP-12)	Pin 16	using the RFM69(H)W Board
*												- WeMos D1 mini and Pro 		Pin D2  using the RFM Shield Board
*					- OOK Symbol time is 300 us (good compromise between Kaku New, Old and Gogex)
*					- Datagram Repeat delay is 20ms (looks to be an acceptable value for Gogex)
*					- Number of repeated datagrams is 10
*					These default parameters may be changed on a case by case basis  (see setOokPin and setOokParams)
*				7.	The functions are tested with the following devices
*		  				- CHACON Remote command Handset       	Type: LYCT-505 (New Kaku)
*						- CHACON Wireless Timer Control Handset	Type: TMT-502 (Old Kaku)
*						- CHACON Remote switch 					Type: LYCR-3500 (non-dimmable)
*						- CHACON Remote switch 					Type: LYCR-300 (dimmable)
*		  				- COGEX Remote command Handset       	Type: 91789
*						- COGEX Remote switch 					Type: 91219B (non-dimmable)
*				8.	The function where ONLY tested using 433MHz RFM69(H)W transceivers
*				9.	The functions are made to clone (replay) commands to configured devices, it is therefore important
*					the know before using these functions:
*						- One House address of one handset (New or Old) Ex: New: 1332798 or Old: D or Cogex:  12
*						- The Unit address of each device to manage
*						- The dimmable capability of the unit
*                   The not so easy part is to find the House address, if not mentioned on the handset we refer to a
*					decoding session using RX433 receiver
*					see:
*						https://github.com/rrobinet/SAW_Devices_and_OOK/blob/master/OOK_Poor_man's_monitoring_tool_V0.0.pdf
*						https://bitbucket.org/fuzzillogic/433mhzforarduino/wiki/Home)
/***********************************************************************************************************************/
/**************************************** Revision History **************************************************************
* 1.0 - First release
* 1.1 - Correct ATMEGA2560 DIO2 pin from 19 to 18
* 1.2 - Correction is made to the ookOldKakuRfmPulse and ookCogexRfmPulse function to cope with no 0 integer
*	    value calcutation crash with the ESP8266
* 1.3 - Save and restore of Bit rate MSB and LSB register while calling a OOK function
* 1.4 - Add pre and post sending procedure for OOK frame sending
************************************************************************************************************************/
#include "RFM69OOK.h"
/****************************************************** RFM69OOK *******************************************************
* Function:  	Define a RFM69OOK Class with default parameters
* Parameters:	None
/***********************************************************************************************************************/
RFM69OOK::RFM69OOK ()
{
	_ookDataPin=LORA_D2;		// Processor OOK data pin according to the processor type
   	_periodusec=300;				// Processor OOK pulse time for OOK SAW devices is 300 us
   	_repeats=10;					// Number of time the OOK datagram is repeated is by default 10
   	_repDly=20;						// Default delay between datagrams is 20 ms
   	pinMode(_ookDataPin, OUTPUT);	// Set OOK pin to output
   	digitalWrite(_ookDataPin,LOW);	// with default low value
}
/****************************************************** RFM69OOK *******************************************************
* Function:		Define a RFM69OOK Class with individual parameters
* Parameters:
*				Processor pin to use to connect RFM69 DIO2 pin
*				OOK pulse period for OOK SAW devices
*				Number of time a datagram is to be repeated
*   			Delay between repeated datagrams
/***********************************************************************************************************************/
RFM69OOK::RFM69OOK(byte dataPin, unsigned int periodusec, byte repeats, byte repDly)
{
   _ookDataPin=dataPin;
   _periodusec=periodusec;
   _repeats=repeats;
   _repDly = repDly;
   pinMode(_ookDataPin, OUTPUT);
   digitalWrite(_ookDataPin,LOW);
}

/*************************************************** setOokPin *********************************************************
* Function:  	Change the OOK Data output pin
* Parameters:
*				Processor pin to use to connect RFM69 DIO2 pin
/***********************************************************************************************************************/
void RFM69OOK::setOokPin(uint8_t newOokDataPin)
{
	_ookDataPin = newOokDataPin;
	pinMode(_ookDataPin, OUTPUT);
	digitalWrite(_ookDataPin,LOW);

}
/************************************************** setOokParams *******************************************************
* Function:  	Change the OOK Transmitter timing parameters
* Parameters:
*				OOK pulse period for OOK SAW devices
*				Number of time a datagram is to be repeated
*   			Delay between repeated datagrams
/***********************************************************************************************************************/
 void RFM69OOK::setOokParams(unsigned int periodusec, byte repeats, byte repDly)
 {
   _periodusec=periodusec;
   _repeats=repeats;
   _repDly = repDly;
 }

/**************************************************** sendKakuNew *******************************************************
* Function:    Send message datagram according to the NEW Kaku protocol
* Parameters:
*              	RFM69 radio instance
*				Handset address
*              	Remote switch unit address
*              	Remote switch state level
*              	Remote switch group command option
*              	Remote switch dim level
/***********************************************************************************************************************/
void RFM69OOK::sendKakuNew(RFM69 &radio, unsigned long int addr, byte unit, boolean on, boolean group, byte dimLevel)
{
    ookPreSend (radio);									// Prepare RFM69 registers and media for OOK sending
   	/* Form  the Command datagram:
   	Unit = (-1) From 0...15
   	Address = From 0...﻿67.108.864 shift left 6 positions
   	DimLevel = 1 to 15
   	*/
   	if(unit !=0) unit--; 	          	        		// Set Unit address to logical
   	if (group) unit = 0x20 ;       						// Ensure that Unit is set to 0 in case of group
    unsigned long int cmd = 0 | addr << 6 | unit;  		// Build the command datagram with the address and unit info
    if (on) cmd |= 0x10;            		      		// Set the Level bit to ON in the command datagram
    else  dimLevel = 0;                 				// Avoid DIM to be active while setting the level to OFF
    if(RFM69OOK_DEBUG) printOokInfos(addr, unit, on, group, dimLevel,cmd);	// Print info for debugging

  	for (int i = 0; i< _repeats; i++)					// Output the data to the RFM69
  	{
    	ookNewKakuRfmPulse(0, _periodusec*10);        	// Send Start  bit
    for (byte bit = 0; bit <27  ; bit++)            	// Send Address and Group bits MSB first
    {
      	unsigned long int level = bitRead(cmd, 31-bit) ? _periodusec*5 : _periodusec; // bit 1: 5,1: bit 0: 1,5
      	ookNewKakuRfmPulse(level, _periodusec*6-level);
    }
    if (dimLevel)                                    	// Case Dim Level option
    {
      ookNewKakuRfmPulse(_periodusec, _periodusec);    	// Send Dimmer pulse
      for (byte bit = 28; bit <32  ; bit++)           	// Send Unit MSB bits first
      {
      	unsigned long int level = bitRead(cmd, 31-bit) ? _periodusec*5 : _periodusec; // bit 1: 5,1: bit 0: 1,5
        ookNewKakuRfmPulse(level, _periodusec*6-level);
      }
      for (byte bit = 0; bit < 4  ; bit++)            	// Send Dimmer Level MSB bits first
      {
        unsigned long int level = bitRead(dimLevel,3-bit) ? _periodusec*5 : _periodusec;// bit 1: 5,1: bit 0: 1,5
        ookNewKakuRfmPulse(level, _periodusec*6-level);
      }
    }
    else                    							// Case ON / OFF option
 	{
      for (byte bit = 27; bit <32  ; bit++)           	// Send Level and Unit MSB bits first
      {
        unsigned long int level = bitRead(cmd, 31-bit) ? _periodusec*5 : _periodusec; // bit 1: 5,1: bit 0: 1,5
        ookNewKakuRfmPulse(level, _periodusec*6-level);
      }
     }
     ookNewKakuRfmPulse(0, _periodusec*10);           	// Stop bit
     delay (_repDly);                                 	// Wait some delay between reties
	}
    ookPostSend (radio);								// Restore RFM69 registers aftrer OOK sending
}

/*************************************************** ookNewKakuRfmPulse ************************************************
* Function:  	KAKU New pulse switching function
* Parametres: 	Duration of the symbol levels
/***********************************************************************************************************************/
void RFM69OOK::ookNewKakuRfmPulse(int l1, int l2)
{
 	int corrFactor = 150;                 					// Instruction delay correction factor
 	if (l1 != 0)       										// Skipped for Start/Stop SYNC
 	{
 	 	digitalWrite(_ookDataPin,HIGH);
		delayMicroseconds (_periodusec-corrFactor);      	// Adjusted for processing delay
	  	digitalWrite(_ookDataPin,LOW);
  		delayMicroseconds (l1+corrFactor);          		// Adjusted for processing delay
 	}
  		digitalWrite(_ookDataPin,HIGH);
  		delayMicroseconds (_periodusec-corrFactor);  		// Adjusted for processing delay
  		digitalWrite(_ookDataPin,LOW);
  		delayMicroseconds (l2+corrFactor);     			 	// Adjusted for processing delay
}
/**************************************************** sendKakuOld *******************************************************
* Function:    	Send DATAGRAM command according to the OLD Kaku protocol
* Parameters:
*              	RFM69 radio instance
*				House address
*              	Remote switch unit address
*              	Remote switch state level
*              	Remote switch group command option
*              	Remote switch dim level
/***********************************************************************************************************************/
 void RFM69OOK::sendKakuOld(RFM69 &radio, char addr, byte unit, byte on)
 {
    ookPreSend (radio);												// Prepare RFM69 registers and media for OOK sending
  	/* Form  the Command datagram:
   	Fixed float Bits = x11xxxxxxxxx (0x600)
   	Unit = (-1) From 0...15 shift 4 bits left
   	Address = ASCII -1 to Binary (ex: 'D' = 68-65 = 3)
   	Level = if ON, value is 1xxxxxxxxxxx (0x800)
   	*/
 	int cmd = 0 | 0x600 | ((unit - 1) << 4) | (addr - 65);
  	if (on) cmd |= 0x800;
  	if(RFM69OOK_DEBUG)	printOokInfos(addr, unit-1, on, 0, 0,cmd);	// Print info for debugging
  	for(int i = 0; i< _repeats; i++)
  	{
    	ookOldKakuRfmPulse(0, _periodusec*3);						// Start bit
    for (byte bit = 0; bit <12 ; ++bit)
    {
     	int on = bitRead(cmd, bit) ? _periodusec*3 : _periodusec; 	// bit 1: 3,1(,1,3): bit 0: 1,3(,1,3)
      	ookOldKakuRfmPulse(on, _periodusec*4-on);
    }
  	delay (_repDly);
  }
  ookPostSend (radio);												// Restore RFM69 registers aftrer OOK sending

}
/************************************************ ookOldKakuRfmPulse ***************************************************
* Function:  	KAKU Old pulse switching function with RFM69
* Parametres: 	Duration of the symbol levels
/***********************************************************************************************************************/
void RFM69OOK::ookOldKakuRfmPulse(int on, int off)
{
 	int corrFactor = 150;                 							// Instruction delay correction factor
  if(on !=0) {
  	digitalWrite(_ookDataPin,HIGH);  		    // Skip the transition for the Start bit
    delayMicroseconds (on-corrFactor);			// Adjusted for processing delay
  }
  	digitalWrite(_ookDataPin,LOW);
  	delayMicroseconds (off+corrFactor);       						// Adjusted for processing delay
  	digitalWrite(_ookDataPin,HIGH);              					// Common part of any bit (1,3)
  	delayMicroseconds ((_periodusec)-corrFactor);
  	digitalWrite(_ookDataPin,LOW);
  	delayMicroseconds (corrFactor+(_periodusec*3));
}
/****************************************************** sendKakuCogex **************************************************
* Function:    	Send DATAGRAM command according to the COGEX Kaku protocol
* Parametres:
*              	RFM69 radio instance
*				House address
*              	Remote switch unit address
*              	Remote switch state level
*              	Remote switch group command option
*              	Remote switch dim level
/***********************************************************************************************************************/
 void RFM69OOK::sendKakuCogex(RFM69 &radio, byte addr, byte unit, byte on)
 {
    ookPreSend (radio);												// Prepare RFM69 registers and media for OOK sending
    /* Form  the Command datagram:
    Fixed float Bits = x11xxxxxxxxx (0x600)
    Unit = From 1 to 15; shift 5 bits left
    Address = From 1 to 15; shift 1 bit left
    Level = if ON, value is 1xxxxxxxxxx1 (0x801)
    Note: bit 1 is encoded as Float
    */
   int cmd = 0 | 0x600 | unit << 5 | addr << 1;
   if (on) cmd |= 0x801;
   if(RFM69OOK_DEBUG) printOokInfos(addr, unit, on, 0, 0,cmd); 		// Print Debug infos
   for(int i = 0; i< _repeats; i++)
   {
     ookCogexRfmPulse(0,_periodusec*3);                               // Start bit
     for (byte bit = 0; bit <12 ; ++bit)
     {
       int on = bitRead(cmd, bit) ? _periodusec*3 : _periodusec*1; // Float: 3,1(,1,3): bit 0: 1,3(,1,3)
       ookCogexRfmPulse(on, _periodusec*4-on);
    }
  delay (_repDly);
  }
  ookPostSend (radio);												// Restore RFM69 registers aftrer OOK sending
 }
/************************************************ ookCogexRfmPulse *****************************************************
* Function:  	KAKU Cogex pulse switching function with RFM69
* Parameters: 	Duration of the symbol levels
/***********************************************************************************************************************/
 void RFM69OOK::ookCogexRfmPulse(int on, int off)
{
  int corrFactor = 150;                 			// Instruction delay correction factor
  if(on !=0) {
  	digitalWrite(_ookDataPin,HIGH);  		    // Skip the transition for the Start bit
    delayMicroseconds (on-corrFactor);			// Adjusted for processing delay
  }
  digitalWrite(_ookDataPin,LOW);
  delayMicroseconds (off+corrFactor);       		// Adjusted for processing delay
  digitalWrite(_ookDataPin,HIGH);              		// Common part of any bit (1,3)
  delayMicroseconds ((_periodusec)-corrFactor);
  digitalWrite(_ookDataPin,LOW);
  delayMicroseconds (corrFactor+(_periodusec*3));	// Adjusted for processing delay
}
/***********************************************************************************************************************/

/*************************************************** printOokInfos *****************************************************
* Function:  	Debugging information
* Parameters: 	None
/***********************************************************************************************************************/
 void RFM69OOK::printOokInfos (unsigned long int addr, unsigned long int unit, boolean on, boolean group, byte dimLevel,int cmd)
{
    	Serial.print ("RF69OOK version: "), Serial.print(MAJOR), Serial.print("."), Serial.println (MINOR);
    	Serial.print ("RF69OOKPIN: "), Serial.println(RF69_OOK_PIN);
    	Serial.print ("House Code: ");
    	if (addr > 64 & addr <81) Serial.print((char) addr);		// Convert address to ASCII for old Kaku address
    	else Serial.print(addr);
    	Serial.print (" Set Logical Unit: "),
        Serial.print (unit), Serial.print (" to: "), Serial.println(on);
        if(group) Serial.print("Group: "), Serial.print(group);
        if(dimLevel) Serial.print(" DIM level: "), Serial.println (dimLevel);
        Serial.print("Data pin: "), Serial.print (_ookDataPin); Serial.print ("; Period: " ),
        Serial.print( _periodusec), Serial.print ("; Repeated: "), Serial.print (_repeats), Serial.print ("; Delay: "),
        Serial.println (_repDly);
 	    Serial.print ("Command datagram: "), Serial.println (cmd,BIN), Serial.println ();
}
/***********************************************************************************************************************/

/***************************************************** ookPreSend ******************************************************
* Function:  	Save and configure RFM registers for OOK tramission and wait for clear media before sending a OOK frame
* Parameters: 	RFM69 radio instance
/***********************************************************************************************************************/
 void RFM69OOK::ookPreSend (RFM69 &radio)
{
    radio.writeReg(REG_PACKETCONFIG2, (radio.readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
    uint32_t now = millis();
    while (!radio.canSend() && millis() - now < RF69_CSMA_LIMIT_MS) radio.receiveDone();
	_mode = radio.readReg(REG_OPMODE);			// Record the previous Operation mode
   	_modulation = radio.readReg(REG_DATAMODUL); // Record the previous Modulation Mode
  	_bitRateMsb = radio.readReg(REG_BITRATEMSB);// Record the previous value of the BitRate MSB
   	_bitRateLsb = radio.readReg(REG_BITRATELSB);// Record the previous value of the BitRate LSB
   	// Set Modulation to OOK continuous mode without synchronisation
   	radio.writeReg(REG_DATAMODUL, RF_DATAMODUL_DATAMODE_CONTINUOUSNOBSYNC|RF_DATAMODUL_MODULATIONTYPE_OOK);
   	// Set the Operation mode to transmit
	radio.writeReg(REG_OPMODE, RF_OPMODE_TRANSMITTER);
	radio.setHighPower(false);
#if 1
   	pinMode(_ookDataPin, OUTPUT);	// Set OOK pin to output
   	digitalWrite(_ookDataPin,LOW);	// with default low value
#endif
}
/***********************************************************************************************************************/
void RFM69OOK::ookPostSend(RFM69 &radio)
 {
   	radio.writeReg(REG_OPMODE,_mode);                    			// Restore previous OPMODE
  	radio.writeReg(REG_DATAMODUL,_modulation);           			// Restore previous MODULATION
  	radio.writeReg(REG_BITRATEMSB,_bitRateMsb);			            // Restore previous BIT RATE value
  	radio.writeReg(REG_BITRATELSB,_bitRateLsb);			            // Restore previous BIT RATE value
 }