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
// 
// 
// 
#include <avr/interrupt.h>

#include "SRFSPI.h"

SRFSPI SRF;
void handle_interrupt();

void SRFSPI::init()
{
  // setup SPI
  pinMode(SRFSELECTPIN,OUTPUT);
  digitalWrite(SRFSELECTPIN,HIGH);
  pinMode(3,INPUT);
  digitalWrite(3,LOW);	//disable pullup as we use an external pulldown
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  rxHead = rxTail = txHead = txTail = bTransfering = 0;
  startTransfer();	// get anything waiting for us
  attachInterrupt(INTERRUPT, handle_interrupt, RISING);
}

int SRFSPI::available()
{
  return (int)((uint8_t)(rxHead - rxTail) % RXBUFFERSIZE);
}

int SRFSPI::read()
{
  int c = -1;
  if (rxHead != rxTail)
  {
    c = rxBuffer[rxTail++];
    rxTail %= RXBUFFERSIZE;
  }

  return c & 0xff;
}

int SRFSPI::peek()
{
  int c = -1;
  if (rxHead != rxTail)
  {
    c = rxBuffer[rxTail];
  }
  return c & 0xff;
}

void SRFSPI::flush()
{

}

size_t SRFSPI::write(uint8_t c)
{
  if ((txHead - txTail) % TXBUFFERSIZE  < TXBUFFERSIZE-1)
  {
    txBuffer[txHead++] = c;
    txHead %= TXBUFFERSIZE;

    startTransfer();
    return 1;
  }
  return 0;
}

size_t SRFSPI::write(const uint8_t *buffer, size_t size)
{
  int ret=0;
  for (size_t i=0; i < size; i++)
  {
    if (!write(buffer[i])) break;
    ret ++;
/*
if ((txHead - txTail) % TXBUFFERSIZE  < TXBUFFERSIZE-1)
	{
		txBuffer[txHead++] = c;
		txHead %= TXBUFFERSIZE;
	}
	else break;
	ret ++;
  }
  startTransfer();
  */
  }
  return ret;
}

void SRFSPI::startTransfer()
{
  noInterrupts();
  if (bTransfering)
  {
    interrupts();
    return;
  }
  bTransfering = 1;	// say we are transfering
  interrupts();
  SPItransfer();
}

void SRFSPI::SPItransfer()
{
  uint8_t c;

  digitalWrite(SRFSELECTPIN,LOW);
  while (bTransfering)
  {
    // do we have anything to send?
    if (txHead != txTail)
    {
      c = txBuffer[txTail++];
      txTail %= TXBUFFERSIZE;
    }
    else c = 0xFF;

    c = SPI.transfer(c);	// exchange a byte

    if (c != (uint8_t)0xFF)
    {
      if ((rxHead - rxTail) %  RXBUFFERSIZE < RXBUFFERSIZE-1)
      {
        rxBuffer[rxHead++] = c;
        rxHead %= RXBUFFERSIZE;

      }	

    }
    if (txHead == txTail && digitalRead(3) == 0) // nothing left to do
    {
      bTransfering = 0;	// say we have done
      digitalWrite(SRFSELECTPIN,HIGH);
    }
  }
}

void handle_interrupt()
{
  if (!SRF.bTransfering)
  {
    SRF.bTransfering = 1;	// say we are transfering
    SRF.SPItransfer();
  }
}

/*
#if defined(SPI_STC_vect)
 ISR(SPI_STC_vect)
 {
 SRFSPI::handle_interrupt();
 }
 #endif
 */

