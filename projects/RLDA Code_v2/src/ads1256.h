/*
Based on
    https://curiousscientist.tech/blog/ads1256-arduino-stm32-sourcecode

*/

/* Information
Pin configuration for Arduino
    SPI default pins:
        MOSI  - 11 // DIN
        MISO  - 12 // DOUT
        SCK	  - 13 // SCLK
        SS	  -	10 // CS
        MOSI: Master OUT Slave IN -> DIN
        MISO: Master IN Slave OUT -> DOUT
--------------------
--------------------
Other pins - You can assign them to any pins
    RST	  -	PA3
    DRDY  - PA2 // this is an interrupt pin
    PDWN  - +3.3 V
    PDWN - PA1 (Alternatively, if you want to switch it)
//--------------------------------------------------------------------------------
Clock rate
    f_CLKIN = 7.68 MHz
    tau = 130.2 ns
//--------------------------------------------------------------------------------
REGISTERS
    REG   VAL     USE
    0     54      Status Register, Everything Is Default, Except ACAL and BUFEN
    1     1       Multiplexer Register, AIN0 POS, AIN1 NEG
    2     0       ADCON, Everything is OFF, PGA = 1
    3     99      DataRate = 50 SPS
    4     225     GPIO, Everything Is Default


*/

#ifndef ADS1256_H
#define ADS1256_H

#include <Arduino.h>

// ----------------------

//
//
//
#define ZERO B00000000
//-------------------------------START OF REGISTER MAP---------------------------------//

/* ADS1256 Register (see p30 for Register Map) */
#define STATUSREG 0x00 // Status Control Register 0
#define MUXREG 0x01    // Multiplexer Control Register 0
#define ADCONREG 0x02  // A/D Control Register 0
#define DRATEREG 0x03  // A/D Data Rate Control Register 0
#define IOREG 0X04     // GPIO Control Register 0
#define OFC0REG 0x05   // Offset Calibration Coefficient Register 1
#define OFC1REG 0x06   // Offset Calibration Coefficient Register 2
#define OFC2REG 0x07   // Offset Calibration Coefficient Register 2
#define FSC0REG 0x08   // Full scale Callibration Coefficient Register 0
#define FSC1REG 0x09   // Full scale Callibration Coefficient Register 1
#define FSC2REG 0x0A   // Full scale Callibration Coefficient REgister 2

//-------------------STATUS : STATUS REGISTER (ADDRESS 00h)------------------//
/*STATUS - Status Control Register 0 ( see p30)*/
/* BIT7 - BIT6 -  BIT5   -  BIT4   -  BIT3   -  BIT2   -  BIT1   -  BIT0 */
/* ID   - ID   -  ID     -  ID     -  ORDER  -  ACAL   -  BUFEN  -  DRDY */
#define STATUS_RESET 0x01 // Reset STATUS Register
// Bits 7-4 ID3, ID2, ID1, ID0 Factory Programmed Identification Bits (Read Only)
// Bit 3 ORDER: Data Output Bit Order
#define ORDER_MSB B00000000 // Most significant Bit first (default)
#define ORDER_LSB B00001000 // Least significant Bit first
/*Input data is always shifted in most significant byte and bit first. Output data is always shifted out most
significant byte first. The ORDER bit only controls the bit order of the output data within the byte.*/
// Bit 2 ACAL: Auto-Calibration
#define ACAL_OFF B00000000 // Auto Calibration Disabled (default)
#define ACAL_ON B00000100  // Auto Calibration Enabled
/*When Auto-Calibration is enabled, self-calibration begins at the completion of the WREG command that changes
the PGA (bits 0-2 of ADCON register), DR (bits 7-0 in the DRATE register) or BUFEN (bit 1 in the STATUS register)
values.*/
// Bit 1 BUFEN: Analog Input Buffer Enable
#define BUFEN_OFF B00000000 // Buffer Disabled (default)
#define BUFEN_ON B00000010  // BUffer Enabled
/*DRDY1:0 Data Ready (Read Only) Duplicates the state of the DRDY pin*/

//-------------------MUX : Input Multiplexer Control Register (Address 01h)------------------//
/* MUX - Multiplexer Control Register 0 (see p31 - bring together with bitwise OR | */
/* BIT7  - BIT6  -  BIT5   -  BIT4   -  BIT3   -  BIT2   -  BIT1   -  BIT0 */
/* PSEL3 - PSEL2 -  PSEL1  -  PSEL0  -  NSEL3  -  NSEL2   - NSEL1   - NSEL0 */
#define MUX_RESET 0x01 // Reset MUX0 Register
/* PSEL3:0 Positive input channel selection bits */
#define P_AIN0 B00000000 //(default)
#define P_AIN1 B00010000
#define P_AIN2 B00100000
#define P_AIN3 B00110000
#define P_AIN4 B01000000
#define P_AIN5 B01010000
#define P_AIN6 B01100000
#define P_AIN7 B01110000
#define P_AINCOM B10000000
/* NSEL3:0 Negative input channel selection bits */
#define N_AIN0 B00000000
#define N_AIN1 B00000001 //(default)
#define N_AIN2 B00000010
#define N_AIN3 B00000011
#define N_AIN4 B00000100
#define N_AIN5 B00000101
#define N_AIN6 B00000110
#define N_AIN7 B00000111
#define N_AINCOM B00001000

//-------------------ADCON: A/D Control Register (Address 02h)------------------//
/*ADCON - A/D Control Register 0 ( see p31)*/
/* BIT7 - BIT6   -  BIT5   -  BIT4   -  BIT3   -  BIT2   -  BIT1   -  BIT0 */
/* 0    - CLK1   -  CLK0   -  SDCS1  -  SDCS0  -  PGA2   -  PGA1   -  PAG0 */
#define ADCON_RESET 0x20 // Reset ADCON Register
// Bit 7 Reserved, always 0 (Read Only)
// Bits 6-5 CLK1, CLK0: D0/CLKOUT Clock Out Rate Setting
#define CLK_OFF B00000000 // Clock Out off
#define CLK_1 B00100000   // Clock Out Frequency = fCLKIN (default)
#define CLK_2 B01000000   // Clock Out Frequency = fCLKIN/2
#define CLK_4 B01100000   // Clock Out Frequency = fCLKIN/4
/*When not using CLKOUT, it is recommended that it be turned off. These bits can only be reset using the RESET pin.*/
// Bits 4-2 SDCS1, SCDS0: Sensor Detect Current Sources
#define SDCS_OFF B00000000 // Sensor Detect Off (default)
#define SDCS_05 B00001000  // Sensor Detect Current 0.5uA
#define SDCS_2 B00010000   // Sensor Detect Current 2uA
#define SDCS_10 B00011000  // Sensor Detect Current 10uA
/*The Sensor Detect Current Sources can be activated to verify the integrity of an external sensor supplying a signal to
the ADS1255/6. A shorted sensor produces a very small signal while an open-circuit sensor produces a very large
signal.*/
// Bits 2-0 PGA2, PGA1, PGA0: Programmable Gain Amplifier Setting
#define PGA_1 B00000000 //(default)
#define PGA_2 B00000001
#define PGA_4 B00000010
#define PGA_8 B00000011
#define PGA_16 B00000100
#define PGA_32 B00000101
#define PGA_64 B00000110

//-------------------DRATE: A/D Data Rate (Address 03h)------------------//
/*DRATE - A/D Data Rate Register 0 ( see p32)*/
/* BIT7 - BIT6   -  BIT5   -  BIT4   -  BIT3   -  BIT2   -  BIT1   -  BIT0 */
/* DR7  - DR6    -  DR5    -  DR4    -  DR3    -  DR2    -  DR1    -  DR0 */
#define DRATE_RESET 0xF0 // Reset DRATE Register
// Bits 7-0 DR[7: 0]: Data Rate Setting(
#define DR_30000    B11110000 // 30,000 SPS (default)
#define DR_15000    B11100000 // 15,000 SPS
#define DR_7500     B11010000  // 7,500 SPS
#define DR_3750     B11000000  // 3,750 SPS
#define DR_2000     B10110000  // 2,000 SPS
#define DR_1000     B10100001  // 1,000 SPS
#define DR_500      B10010010   // 500 SPS
#define DR_100      B10000010   // 100 SPS
#define DR_60       B01110010    // 60 SPS
#define DR_50       B01100011    // 50 SPS
#define DR_30       B01010011    // 30 SPS
#define DR_25       B01000011    // 25 SPS
#define DR_15       B00110011    // 15 SPS
#define DR_10       B00100011    // 10 SPS
#define DR_5        B00010011     // 5 SPS
#define DR_2_5      B00000011   // 2.5 SPS

//-------------------I/O: GPIO Control Register (Address 04H)------------------//
/*IO - GPIO Control Register 0 ( see p32)*/
/* BIT7 - BIT6   -  BIT5   -  BIT4   -  BIT3   -  BIT2   -  BIT1   -  BIT0 */
/* DIR3 - DIR2   -  DIR1   -  DIR0   -  DIO3   -  DIO2   -  DIO1   -  DIO0 */
#define IO_RESET 0xE0 // Reset IO Register

// Bit 7 DIR3, Digital I/O Direction for Digital I/O Pin D3 (used on ADS1256 only)
#define DIR3_OUT B00000000 // D3 is an output
#define DIR_IN B10000000   // D3 is an input (default)

// Bit 6 DIR2, Digital I/O Direction for Digital I/O Pin D2 (used on ADS1256 only)
#define DIR2_OUT B00000000 // D2 is an output
#define DIR2_IN B01000000  // D2 is an input (default)

// Bit 5 DIR1, Digital I/O Direction for Digital I/O Pin D1
#define DIR1_OUT B00000000 // D1 is an output
#define DIR1_IN B00100000  // D1 is an input (default)

// Bit 4 DIR0, Digital I/O Direction for Digital I/O Pin D0/CLKOUT
#define DIR0_OUT B00000000 // D0/CLKOUT is an output
#define DIR0_IN B00010000  // D0/CLKOUT is an input (default)
// Bits 3-0 DI0[3:0]: Status of Digital I/O Pins D3, D2, D1, D0/CLKOUT - Read Only

//-------------------------------END OF REGISTER MAP---------------------------------//
//
//
//-------------------------------START OF SPI COMMAND DEFINITIONS---------------------------------//

/* SPI COMMAND DEFINITIONS (p34) */
/*SYSTEM CONTROL */
#define WAKEUP 0x00  // Exit Sleep Mode
#define SYNC 0xFC    // Synchornize the A/D Conversion
#define STANDBY 0xFD // Enter Sleep Mode
#define RESET 0xFE   // Reset To Power UP values
#define NOP 0xFF     // No operation
/*DATA READ*/
#define RDATA 0x01  // Read data once
#define RDATAC 0x03 // Read data continously
#define SDATAC 0x0F // Stop reading data continously
/*READ REGISTER */
#define RREG 0x10 // Read From Register
#define WREG 0x50 // Write To Register
/*Calibration */
#define SELFCAL 0xF0  // Offset and Gain Self-Calibration
#define SELFOCAL 0xF1 // Offset Self-Calibration
#define SELFGCAL 0xF2 // Gain Self-Calibration
#define SYSOCAL 0xF3  // System Offset Calibration
#define SYSGCAL 0xF4  // System Gain Calibration

#define SPISpeedADS1256 1920000 // Max clock freq is 1.92 MHz. (t1_min = 4 * 1/fclkin). fclkin = 7.68 MHz.

//-------------------------------END OF SPI COMMAND DEFINITIONS---------------------------------//
//
//
//

//--------------------------------------------------------------------------------
// Variables

// double VREF = 2.50; // Value of V_ref. In case of internal V_ref, it is 2.5 V
// double voltage = 0; // Converted RAW bits.

// Pins
// const byte DRDY_pin = 2;  // goes to DRDY on ADS1256
// const byte RESET_pin = 3; // goes to RST on ADS1256
// const byte PDWN_PIN = PA1; //Goes to the PDWN/SYNC/RESET pin (notice, that
// some boards have different pins!) The above pins are described for STM32. For
// Arduino, you have to use a different pin definition ("PA" is not used in
// "PA4)

// // Values for registers
// uint8_t registerAddress;  // address of the register, both for reading and writing - selects the register
// uint8_t regValReceived;   // this is used to READ a register
// uint8_t registerValueW;   // this is used to WRTIE a register
// int32_t registerData;     // this is used to store the data read from the register (for the AD-conversion)
// uint8_t directCommand;    // this is used to store the direct command for sending a command to the ADS1256
// String ConversionResults; // Stores the result of the AD conversion
// String PrintMessage;      // this is used to concatenate stuff into before printing it out.

//--------------------------------------------------------------------------------

class ADS1256 {
private:
  int _CSPin;
  int _DRDYPin;
  int _RESETPin;
  int _PGA;
  int _DRATE;
  int _pinNo;
  double _VREF = 2.5;
  double _voltage;
  int32_t _registerData;

  // Values for registers
  uint8_t _registerValueR; // this is used to READ a register
  String _PrintMessage;    // this is used to concatenate stuff into before printing it out.

  void waitforDRDY();
  static const String DRDYIssue;

public:
  ADS1256();
  void begin(int CSPin, int DRDYPin, int RESETPin, int PGA, int DR_Rate);
  void resetADS1256();
  void userDefaultRegisters();

  unsigned long readRegister(uint8_t registerAddress);
  void writeRegister(uint8_t registerAddress, uint8_t registerValueW);
  void sendDirectCommand(uint8_t directCommand);

  double readChannelContinuousMode(int pinNo);
  double convertToVoltage();
};

#endif