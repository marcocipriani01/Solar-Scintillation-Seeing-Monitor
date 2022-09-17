/* Solar Scintillation Seeing Monitor Sketch
 *
 *  (Putative V1.0) E.J Seykora, Department of Physics, East Carolina University, Greenville, NC, USA (Original Sketch, Mode 1 output)
 *  https://www.ecu.edu/cs-cas/physics/upload/An-Inexpensive-Solar-Scintillation-Seeing-Monitor-Circuit-with-Arduino-Interface-final2.pdf
 *  (Putative V1.1) Joachim Stehle, info@jahreslauf.de (Mode 2 output, Firecapture Addin compatibility, LCD output)
 *  http://www.joachim-stehle.de/sssm_eng.html
 *  V1.2 Modifications Copyright (C) 2016 Ian Lauwerys, http://www.blackwaterskies.co.uk/ (Mode 3 output, OLED output, code refactoring)
 *
 *  The original work ("V1.0") and modified version "V1.1" contain no copyright notices or licences and thus are copyright their
 *  respective authors.
 *
 *  Modifications (only) in V1.2 are provided without warranty of any kind and may be used freely and for any purpose (insofar as they can be
 *  separated from the original copyrighted works).
 *
 *  Changelog:
 *    V1.0 - Vanilla version
 *    V1.1 - Firecapture addin and LCD shield output
 *    V1.2 - Refactored code, OLED output
 *
 *  Features:
 *  - Reads NUM_SAMPLES of analog input readings from ADC1 and ADC2 to find  4.46 * RMS(Intensity_ADC2) / AVERAGE(Intensity_ADC1)
 *  - Mode 1: Result is sent to serial port in plain text mode
 *  - Mode 2: Result is sent to serial port in format compatible with the Firecapture addin (http://www.joachim-stehle.de/sssm_eng.html)
 *
 *  Notes:
 *  - Use with the hardware described in the paper linked above
 *  - External Scintillation Monitor circuit gain set to 425.5X
 *  - Adjust intensity value to approximately 1.0 in by changing feedback resistor on U1
 *      You should probably substitute a 2K variable resistor for the 220R resistor in the paper linked above to make this easier!
 *  - For more information see the following references:
 *
 *    http://solarchatforum.com/viewtopic.php?f=9&t=16746
 *
 */

#include <Arduino.h>

#define NUM_SAMPLES \
    2000  // ** Number of samples used to find the RMS value, default 2000 for about 2 readings per second on 16MHz device, decrease to output more
          // readings per second

#define ADC1 A0  // ** Arduino analog input pin, reads intensity from LMC6484 pin 14
#define ADC2 A1  // ** Arduino analog input pin, reads variation from LMC6484 pin 8

#define CLOUD_DISCRIMINATE true  // ** Discriminate for clouds if true, default true, comment out to disable
#ifdef CLOUD_DISCRIMINATE
#define DISCRIMINATE_LOW 0.5    // ** Set variation value to zero if ***intensity*** is too low, default 0.5
#define DISCRIMINATE_HIGH 10.0  // ** Set variation value to zero if ***variation*** is too high, default 10
#endif

#define INTENSITY_OFFSET \
    0.0  // ** Intensity dc offset, default 0.0, should not need to change if resistor on U1 can be adjusted to keep value between 0.5V and 1.0V
#define VARIATION_OFFSET 0  // ** Variation dc offset, default -0.05, may need to adjust to keep variation output the the range > 0.0 and < 10.0

#define MOVING_AVERAGE_PTS 20  // ** Number of points to use for calculating the variation moving average, default 20, comment out to disable

#define MODE 2
#ifdef MODE
#define SERIAL_RATE 115200  // ** Set the serial communications rate (9600 or 115200 are good values to try), default 115200
#endif

// #define LED_OUTPUT         // ** Define this to enable LED shield support, otherwise comment it out
#define OLED_OUTPUT  // ** Define this to enable OLED module support, otherwise comment it out
                     //    N.B. You must edit Adafruit_SH1106.h at comment "SH1106 Displays" to choose a display size
                     //    SH1106_LCDWIDTH and SH1106_LCDHeight will then be defined by the .h with display size
                     //    Current W x H options are: 128 x 64 | 128 x 32 | 96 x 16
#ifdef OLED_OUTPUT
#define OLED_RESET 20  // ** For software and hardware SPI, define digital pin wired to OLED RST (not available on my OLED module, -1 ignores)
#define OLED_DC 10     // ** For software and hardware SPI, define digital pin wired to OLED DC (or D/C)
#define OLED_CS 21     // ** For software and hardware SPI, define digital pin wired to OLED CS (21 is A3 on Pro Micro!)
// Note that SCLK (Pro Micro pin 15) and MOSI (Pro Micro pin 16) are specific pins in hardware SPI mode so don't need defining

#define VG_WIDTH 125  // ** Variation graph width (pixels), default 125 (sized for 128 pixel wide display), comment out to hide graph
#ifdef VG_WIDTH       // N.B. Enabling the variation graph requires display to be taller than 16 pixels
#define VG_MAX 20     // ** Variation graph max plottable input value, default 20
#endif

#if SH1106_LCDHEIGHT != 16  // Can't plot graphs on 96 x 16 screen
#define IG_WIDTH 45         // ** Intensity graph width (pixels), default 45 (sized for 128 pixel wide display), comment out to hide graph
#ifdef IG_WIDTH
#define IG_MAX 1.5  // ** Intensity graph max plottable input value, default 1.5
#endif

#define VG_WIDTH 125  // ** Variation graph width (pixels), default 125 (sized for 128 pixel wide display), comment out to hide graph
#ifdef VG_WIDTH       // N.B. Enabling the variation graph requires display to be taller than 16 pixels
#define VG_MAX 20     // ** Variation graph max plottable input value, default 20
#endif

//#define SCATTER_PLOT  // ** Define this to graph as a scatter plot (quicker to draw), comment out and a filled graph will be drawn
#endif
#endif

// *************************************** End user modifiable defines ***************************************

// #define DEBUG_SSM               // Turn on debugging tests

#define SSM_VERSION "V1.3"    // SSM sketch version number
#define SSM_DATE "July 2022"  // SSM sketch date

#ifdef OLED_OUTPUT
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <SPI.h>
#include <Wire.h>
#endif
