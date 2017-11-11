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

#ifndef RFM69OOK_h
#define RFM69OOK_h

#include "RFM69.h"
#include "RFM69registers.h"
#include <Arduino.h>

extern boolean RFM69OOK_DEBUG; 		// Debug option defined by the 
#define MAJOR 1						// Major version
#define MINOR 4						// Minor version
/**************************************** Revision History **************************************************************
* 1.0 - First release
* 1.1 - Correct ATMEGA2560 DIO2 pin from 19 to 18
* 1.2 - Correction is made to the ookOldKakuRfmPulse and ookCogexRfmPulse function to cope with no 0 integer 
*	    value calcutation crash with the ESP8266
* 1.3 - Save and restore of Bit rate MSB and LSB register while calling a OOK function 
* 1.4 - Add pre and post sending procedure for OOK frame sending
************************************************************************************************************************/
// RFM69 DIO2 pin should be connected on ATmega328 pin D3
#if defined(__AVR_ATmega328P__) 
	#define RF69_OOK_PIN          3
// RFM69 DIO2 pin should be connected on ATmega1284P pin D11
#elif defined(__AVR_ATmega1284P__)
	#define RF69_OOK_PIN          11
// RFM69 DIO2 pin should be connected on ATmega2560 pin D18 using RFM69 Shield
#elif defined(__AVR_ATmega2560__)
	#define RF69_OOK_PIN          18
// RFM69 DIO2 pin should be connected on ATmega32U4 pin D2	 
#elif defined(__AVR_ATmega32U4__)
	#define RF69_OOK_PIN          2
// Default value enabling the change with the setOokPin function
#else 
    #define RF69_OOK_PIN          3
#endif

class RFM69OOK {
public: 
    // Define a RFM69OOK Class with default parameters
    RFM69OOK();                      		
    // Define a RFM69OOK Class with individual parameters
    RFM69OOK (byte _ookDataPin, unsigned int periodusec, byte repeats, byte repDly);
    // Modify the OOK Data output pin
    void setOokPin(uint8_t newOokDataPin);
    // Modify the OOK transmitter timing parameters
    void setOokParams(unsigned int periodusec, byte repeats, byte repDly);
    // Send OOK Kaku datagram using the New protocol
    void sendKakuNew(RFM69 &radio, unsigned long int addr, byte unit, boolean on, boolean group, byte dimLevel);
    // Send OOK Kaku datagram using the Old protocol
    void sendKakuOld(RFM69 &radio, char addr, byte unit, byte on);
    // Send OOK Kaku datagram using the Cogex protocol
    void sendKakuCogex(RFM69 &radio, byte addr, byte unit, byte on);   
private:
    byte _ookDataPin;						// ATMEGA328 - RFM69 OOK Data port
	byte _repeats;							// Number of time a datagram is to be repeated
	byte _repDly;							// Delay between repeated datagrams
	unsigned int _periodusec;				// OOK pulse time for OOK SAW devices	
 	int _mode;								// Used to record the previous Operation mode
	int _modulation; 						// Used to record the previous Modulation Mode
	int _bitRateMsb; 						// Used to record the previous value of the BitRate MSB
 	int _bitRateLsb;						// Used to record the previous value of the BitRate LSB
	// New Kaku datagram output to RFM69 
    void ookNewKakuRfmPulse(int l1, int l2);
   	// Old Kaku datagram output to RFM69 
    void ookOldKakuRfmPulse(int on, int off);
    // Cogex Kaku datagram output to RFM69 
    void ookCogexRfmPulse(int on, int off);
    // Print OOK settings informations
	void printOokInfos (unsigned long int addr, unsigned long int unit, boolean on, boolean group, byte dimLevel, int cmd);
	// Prepare RFM69 registers and media before sending an OOK frame
	void ookPreSend (RFM69 &radio);
	// Restore RFM69 register after sending an OOK frame
	void ookPostSend (RFM69 &radio);
};
#endif
