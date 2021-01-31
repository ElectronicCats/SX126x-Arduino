#include <Arduino.h>
#include <SX126x-Arduino.h>  //http://librarymanager/All#SX126x
#include <SPI.h>

hw_config hwConfig;

#ifdef ESP32
// ESP32 - SX126x pin configuration
int PIN_LORA_RESET = 4;  // LORA RESET
int PIN_LORA_DIO_1 = 21; // LORA DIO_1
int PIN_LORA_BUSY = 22;  // LORA SPI BUSY
int PIN_LORA_NSS = 5;	// LORA SPI CS
int PIN_LORA_SCLK = 18;  // LORA SPI CLK
int PIN_LORA_MISO = 19;  // LORA SPI MISO
int PIN_LORA_MOSI = 23;  // LORA SPI MOSI
int RADIO_TXEN = -1;	 // LORA ANTENNA TX ENABLE
int RADIO_RXEN = -1;	 // LORA ANTENNA RX ENABLE
#endif
#ifdef ESP8266
// ESP32 - SX126x pin configuration
int PIN_LORA_RESET = 0; // LORA RESET
int PIN_LORA_DIO_1 = 15;  // LORA DIO_1
int PIN_LORA_BUSY = 16;   // LORA SPI BUSY
int PIN_LORA_NSS = 2;   // LORA SPI CS
int PIN_LORA_SCLK = 14;  // LORA SPI CLK
int PIN_LORA_MISO = 12;  // LORA SPI MISO
int PIN_LORA_MOSI = 13;  // LORA SPI MOSI
int RADIO_TXEN = -1;	 // LORA ANTENNA TX ENABLE
int RADIO_RXEN = -1;		 // LORA ANTENNA RX ENABLE
#endif
#ifdef ARDUINO_ARCH_NRF52840
// nRF52840 - SX126x pin configuration
int PIN_LORA_RESET = RFM_RST;  // LORA RESET
int PIN_LORA_DIO_1 = RFM_DIO1; // LORA DIO_1
int PIN_LORA_BUSY = RFM_BUSY;  // LORA SPI BUSY
int PIN_LORA_NSS = SS;   // LORA SPI CS
int PIN_LORA_SCLK = SCK;  // LORA SPI CLK
int PIN_LORA_MISO = MISO;  // LORA SPI MISO
int PIN_LORA_MOSI = MOSI;  // LORA SPI MOSI
int RADIO_TXEN = RFM_DIO2;   // LORA ANTENNA TX ENABLE
int RADIO_RXEN = ANTSW;  // LORA ANTENNA RX ENABLE
#endif

// Function declarations
void OnTxDone(void);
void OnTxTimeout(void);

// Define LoRa parameters
#define RF_FREQUENCY 915000000  // Hz
#define TX_OUTPUT_POWER 22    // dBm
#define LORA_BANDWIDTH 1    // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR 10 // [SF7..SF12]
#define LORA_CODINGRATE 1   // [1: 4/5, 2: 4/6,  3: 4/7,  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8  // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0 // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false
#define RX_TIMEOUT_VALUE 3000
#define TX_TIMEOUT_VALUE 3000

static RadioEvents_t RadioEvents;
static uint8_t TxdBuffer[64];

void setup()
{

  // Define the HW configuration between MCU and SX126x
  hwConfig.CHIP_TYPE = SX1262_CHIP;     // Example uses an eByte E22 module with an SX1262
  hwConfig.PIN_LORA_RESET = PIN_LORA_RESET; // LORA RESET
  hwConfig.PIN_LORA_NSS = PIN_LORA_NSS;  // LORA SPI CS
  hwConfig.PIN_LORA_SCLK = PIN_LORA_SCLK;   // LORA SPI CLK
  hwConfig.PIN_LORA_MISO = PIN_LORA_MISO;   // LORA SPI MISO
  hwConfig.PIN_LORA_DIO_1 = PIN_LORA_DIO_1; // LORA DIO_1
  hwConfig.PIN_LORA_BUSY = PIN_LORA_BUSY;   // LORA SPI BUSY
  hwConfig.PIN_LORA_MOSI = PIN_LORA_MOSI;   // LORA SPI MOSI
  hwConfig.RADIO_TXEN = RADIO_TXEN;     // LORA ANTENNA TX ENABLE
  hwConfig.RADIO_RXEN = RADIO_RXEN;     // LORA ANTENNA RX ENABLE
  hwConfig.USE_DIO2_ANT_SWITCH = true;    // Example uses an CircuitRocks Alora RFM1262 which uses DIO2 pins as antenna control
  hwConfig.USE_DIO3_TCXO = true;        // Example uses an CircuitRocks Alora RFM1262 which uses DIO3 to control oscillator voltage
  hwConfig.USE_DIO3_ANT_SWITCH = false;  // Only Insight ISP4520 module uses DIO3 as antenna control
  hwConfig.USE_RXEN_ANT_PWR = true;

  // Initialize Serial for debug output
  Serial.begin(9600);
  while (!Serial);
  Serial.println("LoRa Sender");

  // Initialize the LoRa chip
    if (!lora_hardware_init(hwConfig))
  {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  // Initialize the Radio callbacks
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = NULL;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = NULL;
  RadioEvents.RxError = NULL;
  RadioEvents.CadDone = NULL;

  // Initialize the Radio
  Radio.Init(&RadioEvents);

  // Set Radio channel
  Radio.SetChannel(RF_FREQUENCY);

  // Set Radio TX configuration
  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
            LORA_SPREADING_FACTOR, LORA_CODINGRATE,
            LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
            true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);
  send();
}

void loop()
{
  // Handle Radio events
  Radio.IrqProcess();

  // We are on FreeRTOS, give other tasks a chance to run
  delay(100);
  yield();
}

/**@brief Function to be executed on Radio Tx Done event
 */
void OnTxDone(void)
{
  Serial.println("OnTxDone");
  delay(5000);
  send();
}

/**@brief Function to be executed on Radio Tx Timeout event
 */
void OnTxTimeout(void)
{
  Serial.println("OnTxTimeout");
}

void send()
{
  TxdBuffer[0] = 'H';
  TxdBuffer[1] = 'e';
  TxdBuffer[2] = 'l';
  TxdBuffer[3] = 'l';
  TxdBuffer[4] = 'o';
  Radio.Send(TxdBuffer, 5);
}
