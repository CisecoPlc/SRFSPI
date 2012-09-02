/*
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
// SRFSPI.h

#ifndef _SRFSPI_h
#define _SRFSPI_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SPI.h>

#define INTERRUPT 1
#define SRFSELECTPIN 10

#define RXBUFFERSIZE 64
#define TXBUFFERSIZE 12

class SRFSPI : 
public Stream
{
private:
  volatile uint8_t rxHead;
  uint8_t rxTail;
  uint8_t rxBuffer[RXBUFFERSIZE];
  uint8_t txHead;
  volatile uint8_t txTail;
  uint8_t txBuffer[TXBUFFERSIZE];



  //void SPItransfer();
public:
  void init();
  int available();
  int read();
  int peek();
  void flush();
  size_t write(uint8_t);
  size_t write(const uint8_t *buffer, size_t size);

  // public only for easy access by interrupt handlers
  void startTransfer();
  void SPItransfer();
  uint8_t bTransfering;
};

extern SRFSPI SRF;

#endif


