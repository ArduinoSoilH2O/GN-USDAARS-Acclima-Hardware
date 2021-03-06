/* SDI-12 library for Acclima Node based off Stroud Water Research Center Arduino-SDI-12 library
 *  
 *  Header file
 *  
 *  Alondra Thompson
 *  September 2018
 */

 #ifndef AcclimaSDI12_h
 #define AcclimaSDI12_h

  //  Import Required Libraries

  #include <inttypes.h>           // integer types library
  #include <Arduino.h>            // Arduino core library
  #include <Stream.h>             // Arduino Stream library

  typedef const __FlashStringHelper *FlashString;

#define NO_IGNORE_CHAR '\x01' // a char not found in a valid ASCII numeric field
#define SDI12_BUFFER_SIZE 75   // max Rx buffer size
#define Tx_En 3     // Tx enable on level shifter: HIGH when uC talks to sensor, LOW when sensor talks to uC

class SDI12 : public Stream
{
protected:
  // hides the version from the stream to allow custom timeout value
  int peekNextDigit(LookaheadMode lookahead, bool detectDecimal);

private:

  // For the various SDI12 states
  typedef enum SDI12_STATES
  {
    DISABLED = 0,
    ENABLED = 1,
    HOLDING = 2,
    TRANSMITTING = 3,
    LISTENING = 4
  } SDI12_STATES;

  static SDI12 *_activeObject;    // static pointer to active SDI12 instance

  void setPinInterrupts(bool enable);  // Turns pin interrupts on or off
  void setState(SDI12_STATES state);   // sets the state of the SDI12 objects
  void wakeSensors();             // used to wake up the SDI12 bus
  void writeChar(uint8_t out);    // used to send a char out on the data line
  void startChar();               // creates a blank slate for a new incoming character
  void receiveISR();              // the actual function responding to changes in rx line state
  void charToBuffer(uint8_t c);   // puts a finished character into the SDI12 buffer

  #ifndef _AVR_
    static uint8_t parity_even_bit(uint8_t v);
  #endif
  
  uint8_t _dataPin;                // reference to data pin (will be pin 10)
  
  static uint8_t _rxBuffer[SDI12_BUFFER_SIZE];  // A single buffer for ALL SDI-12 objects
  static volatile uint8_t _rxBufferTail;
  static volatile uint8_t _rxBufferHead;
  bool _bufferOverflow;           // buffer overflow status

public:
 // SDI12(bool initSDI12 = true);
  SDI12();                          // constructor - without argument, for better library integration
  SDI12(uint8_t dataPin);                   // constructor
  ~SDI12();                         // destructor
  void begin();                     // enable SDI-12 object
  void begin(uint8_t dataPin);              // enable SDI-12 object - if you use the empty constuctor, USE THIS
  void end();                       // disable SDI-12 object
  int TIMEOUT;                      // value to return if a parse times out
  void setTimeoutValue(int value);  // sets the value to return if a parse int or parse float times out
  uint8_t getDataPin();

  void forceHold();                     // sets line state to HOLDING
  void forceListen();                   // sets line state to LISTENING
  void sendCommand(String &cmd);        // sends the String cmd out on the data line
  void sendCommand(const char *cmd);    // sends the String cmd out on the data line
  void sendCommand(FlashString cmd);    // sends the String cmd out on the data line
  void sendResponse(String &resp);      // sends the String resp out on the data line (for slave use)
  void sendResponse(const char *resp);  // sends the String resp out on the data line (for slave use)
  void sendResponse(FlashString resp);  // sends the String resp out on the data line (for slave use)

  int available();            // returns the number of bytes available in buffer
  int peek();                 // reveals next byte in buffer without consuming
  int read();                 // returns next byte in the buffer (consumes)
  void clearBuffer();         // clears the buffer
  void flush(){};             // Waits for sending to finish - because no TX buffering, does nothing
  virtual size_t write(uint8_t byte);  // standard stream function
  
  bool setActive();         // set this instance as the active SDI-12 instance
  bool isActive();          // check if this instance is active
  bool findSensorType();
  
  // hide the Stream equivalents to allow custom value to be returned on timeout
  long parseInt(LookaheadMode lookahead = SKIP_ALL, char ignore = NO_IGNORE_CHAR);
  float parseFloat(LookaheadMode lookahead = SKIP_ALL, char ignore = NO_IGNORE_CHAR);

  static void handleInterrupt();  // intermediary used by the ISR

};

//#ifdef ARDUINO_ARCH_AVR
//extern SDI12 SDI12port;
//#endif

#endif  // Acclima_SDI12_h
