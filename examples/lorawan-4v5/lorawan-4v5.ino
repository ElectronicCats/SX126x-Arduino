
#include <Arduino.h>
#include <LoRaWan-Arduino.h> //http://librarymanager/All#SX126x
#include <SPI.h>

#define SCHED_MAX_EVENT_DATA_SIZE   APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE            60                              /**< Maximum number of events in the scheduler queue. */

#define LORAWAN_APP_DATA_BUFF_SIZE 64                     /**< buffer size of the data to be transmitted. */
#define LORAWAN_APP_INTERVAL 2000000 
#define LORAWAN_APP_PORT            2       /**< LoRaWAN application port, do not use 224. It is reserved for certification */ 

hw_config hwConfig;

#ifdef ESP32
// ESP32 - SX126x pin configuration
int PIN_LORA_RESET = 4;  // LORA RESET
int PIN_LORA_NSS = 5;  // LORA SPI CS
int PIN_LORA_SCLK = 18;  // LORA SPI CLK
int PIN_LORA_MISO = 19;  // LORA SPI MISO
int PIN_LORA_DIO_1 = 21; // LORA DIO_1
int PIN_LORA_BUSY = 22;  // LORA SPI BUSY
int PIN_LORA_MOSI = 23;  // LORA SPI MOSI
int RADIO_TXEN = -1;   // LORA ANTENNA TX ENABLE
int RADIO_RXEN = -1;   // LORA ANTENNA RX ENABLE
#endif
#ifdef ESP8266
// ESP32 - SX126x pin configuration
int PIN_LORA_RESET = 0;   // LORA RESET
int PIN_LORA_NSS = 2;  // LORA SPI CS
int PIN_LORA_DIO_1 = 15;  // LORA DIO_1
int PIN_LORA_BUSY = 16;   // LORA SPI BUSY
int PIN_LORA_SCLK = SCK;  // LORA SPI CLK
int PIN_LORA_MISO = MISO; // LORA SPI MISO
int PIN_LORA_MOSI = MOSI; // LORA SPI MOSI
int RADIO_TXEN = -1;    // LORA ANTENNA TX ENABLE
int RADIO_RXEN = -1;    // LORA ANTENNA RX ENABLE
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

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
*/
static lmh_param_t lora_param_init = {LORAWAN_ADR_ON, LORAWAN_DATERATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF};




//ABP keys
uint32_t nodeDevAddr = 0x11111111;
uint8_t nodeNwsKey[16] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
uint8_t nodeAppsKey[16] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};

// Foward declaration
//static void lmh_rx_data_handler(lmh_app_rx_data_t *app_data);
//static void lmh_evt_handler(lmh_evt_type_t type, void *data);

static void lorawan_has_joined_handler(void);
static void lorawan_rx_handler(lmh_app_rx_data_t *app_data);
//static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void send_lora_frame(void);

TimerEvent_t appTimer;                                                    ///< LoRa tx timer instance.
static uint8_t m_lora_rx_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];                   ///< LoRa user application rx data buffer.
static lmh_app_rx_data_t m_lora_rx_data = {m_lora_rx_data_buffer, 0 ,0, 0, 0};      ///< LoRa user application rx data structure.
static uint8_t m_lora_tx_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];                   ///< LoRa user application tx data buffer.
static lmh_app_tx_data_t m_lora_tx_data = {m_lora_tx_data_buffer, 0 ,0, 0, 0};      ///< LoRa user application tx data structure.

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
*/
static lmh_callback_t lora_callbacks = {    BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
                                            lmh_rx_data_handler, lmh_evt_handler};




volatile bool flag = false;
bool doOTAA = false;

void setup() {
  // Initialize Serial for debug output
  Serial.begin(9600);
  while (!Serial)
  Serial.println("=====================================");
  Serial.println("Welcome to Example with BastFrontier in LoRaWan!");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

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

#if defined(REGION_AS923)
  Serial.println("Region: AS923");
#elif defined(REGION_AU915)
  Serial.println("Region: AU915");
#elif defined(REGION_CN470)
  Serial.println("Region: CN470");
#elif defined(REGION_CN779)
  Serial.println("Region: CN779");
#elif defined(REGION_EU433)
  Serial.println("Region: EU433");
#elif defined(REGION_IN865)
  Serial.println("Region: IN865");
#elif defined(REGION_EU868)
  Serial.println("Region: EU868");
#elif defined(REGION_KR920)
  Serial.println("Region: KR920");
#elif defined(REGION_US915)
  Serial.println("Region: US915");
#elif defined(REGION_US915_HYBRID)
  Serial.println("Region: US915_HYBRID");
#else
  Serial.println("Please define a region in the compiler options.");
#endif
  Serial.println("=====================================");

  // Initialize LoRa chip.
  if (!lora_hardware_init(hwConfig))
  {
    Serial.println("lora hardware init failed");
    while (true);
  }

  lmh_setNwkSKey(nodeNwsKey);
  lmh_setAppSKey(nodeAppsKey);
  lmh_setDevAddr(nodeDevAddr);

  // Initialize LoRaWan
  if (lmh_init(&lora_callbacks, lora_param_init, doOTAA))
  {
    Serial.println("lmh_init failed");
    while (true);
  }

  Serial.println("LoRaWAN ABP!!");
  Serial.println("=====================================");

  timers_init();
  
  // Start Join procedure
  lmh_join();
}

void loop() {
  // Handle Radio events
  Radio.IrqProcess();

}

/**@brief Function for handling event from the LMH layer
 *
 * @param[in] type  event type 
 * @param[in] data  event data
 */
static void lmh_evt_handler(lmh_evt_type_t type, void *data)
{
    switch(type)
    {
        case LHM_EVT_NWK_JOINED:
        {
#if (OVER_THE_AIR_ACTIVATION != 0)
            Serial.println("Network Joined");
#endif
        } 
        break;

        case LHM_EVT_RX_ACK:
        {
            Serial.println("Acknowledge received");
        } 
        break;

        case LHM_EVT_CLASS_CHANGED:
        {
            int new_class = *(int *)data; 
            //Serial.println("Device class changed to %c", "ABC"[new_class]);
        }
        break;

        default:
            break;
    }
}

/**@brief Function for handling LoRaWan received data from Gateway

   @param[in] app_data  Pointer to rx data
*/
void lorawan_rx_handler(lmh_app_rx_data_t *app_data)
{
  
  Serial.println("lorawan_rx_handler");
  /*Serial.print("LoRa Packet received on port: ");
  Serial.println(app_data->port);
  Serial.print("LoRa Packet received on size:: ");
  Serial.println(app_data->buffsize);
  Serial.print("LoRa Packet received on rssi: ");
  Serial.println(app_data->rssi);
  Serial.print("LoRa Packet received on snr: ");
  Serial.println(app_data->snr);
  Serial.print("LoRa Packet received on data: ");
  */
  //Serial.println(app_data->buffer);
}

void send_lora_frame(void)
{
  flag = true;
}

/**@brief Function for handling user timerout event.
*/
void tx_lora_periodic_handler(void)
{
  TimerSetValue(&appTimer, LORAWAN_APP_INTERVAL);
  TimerStart(&appTimer);
  send_lora_frame();
}

/**@brief Function for the Timer initialization.

   @details Initializes the timer module. This creates and starts application timers.
*/
void timers_init(void)
{
  appTimer.timerNum = 3;
  TimerInit(&appTimer, tx_lora_periodic_handler);
}
