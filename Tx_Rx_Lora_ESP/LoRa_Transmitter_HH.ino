#include "LoRaWan_APP.h"
#include "Arduino.h"

#define RF_FREQUENCY                                868000000 // Hz
#define TX_OUTPUT_POWER                             5        // dBm
#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       12         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // Define the payload size here

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
int num;
int conv=0;
double txNumber;
bool lora_idle = true;

static RadioEvents_t RadioEvents;
void OnTxDone( void );
void OnTxTimeout( void );

void setup() {
    Serial.begin(115200);
    Mcu.begin(ARDUINO_HELTEC_WIFI_LORA_32, 0); 



    txNumber = 0;

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;

    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000);
}

void loop() {
    if (lora_idle == true) {
        delay(2000);
        if (Serial.available() != 0) { 
            // Leer un número entero del puerto serie
            byte num = Serial.read();

            // Aumentar el valor de txNumber
            txNumber += 0.01;

            // Formatear el paquete a enviar
            sprintf(txpacket, "Hello world number %.2f %d" , txNumber, (num));
            Serial.print(num);
             Serial.print(conv);
            // Mostrar el paquete en el monitor serie
            Serial.printf("\r\nEnviando paquete \"%s\", longitud %d\r\n", txpacket, strlen(txpacket));

            // Enviar el paquete LoRa
            Radio.Send((uint8_t *)txpacket, strlen(txpacket));
            lora_idle = false;
        } 
    }
    Radio.IrqProcess();
}

void OnTxDone(void) {
    Serial.println("TX done...");
    lora_idle = true;
}

void OnTxTimeout(void) {
    Radio.Sleep();
    Serial.println("TX Timeout...");
    lora_idle = true;
}
