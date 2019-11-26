/* FastCRC library code is placed under the MIT license
 * Copyright (c) 2014,2015,2016 Frank Bösing
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

//
// Thanks to:
// - Catalogue of parametrised CRC algorithms, CRC RevEng
// http://reveng.sourceforge.net/crc-catalogue/
//
// - Danjel McGougan (CRC-Table-Generator)
//

#include "Arduino.h"
#if !defined(KINETISK)

#include "FastCRC.h"
#include "FastCRC_cpu.h"
#include "FastCRC_tables.h"

// ================= 8-BIT CRC ===================

/** Constructor
 */
FastCRC8::FastCRC8() {}

/** SMBUS CRC
 * aka CRC-8
 * @param data Pointer to Data
 * @param datalen Length of Data
 * @return CRC value
 */
uint8_t FastCRC8::smbus_upd(const uint8_t *data, uint16_t datalen)
{
	uint8_t crc = seed;
	if (datalen)
		do
		{
			crc = pgm_read_byte(&crc_table_smbus[crc ^ *data]);
			data++;
		} while (--datalen);
	seed = crc;
	return crc;
}

uint8_t FastCRC8::smbus(const uint8_t *data, const uint16_t datalen)
{
	// poly=0x07 init=0x00 refin=false refout=false xorout=0x00 check=0xf4
	seed = 0x00;
	return smbus_upd(data, datalen);
}

#endif // #if !defined(KINETISK)
