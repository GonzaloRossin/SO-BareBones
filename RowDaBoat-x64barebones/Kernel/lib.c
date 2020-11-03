#include <stdint.h>
#include <lib.h>

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}
//get data of register b of cmos
static uint8_t regBData = 0;
//initialize regbdata
static void initializeregBData()
{
	//get regbdata from cmos
	regBData = readCMOS(0x0B);
	//get only bit 1(wether hours are in 12 or 24 mode)
	//and 2 (Enables Binary mode if set Binary mode is exactly what you would expect the value to be. If the time is 1:59:48 AM, then the value of hours would be 1, minutes would be 59 = 0x3b, and seconds would be 48 = 0x30.) 
	regBData = ((regBData & 0x06) >> 1) + 1;
}

//read seconds
uint8_t readSeconds()
{
	//check if regbdata is initialized
	if (regBData == 0)
	{
		initializeregBData();
	}
	//read seconds data from cmos
	uint8_t seconds = readCMOS(0x00);
	//if it is in hex change it to bin
	if (!((regBData - 1) & 0x02))
	{
		seconds = ((seconds & 0xF0) >> 1) + ((seconds & 0xF0) >> 3) + (seconds & 0xf);
	}
	return seconds;
}
//read minutes
uint8_t readMinutes()
{
	//check if regbdata is initialized
	if (regBData == 0)
	{
		initializeregBData();
	}
	uint8_t minutes = readCMOS(0x02);
	//if it is in hex change it to bin
	if (!((regBData - 1) & 0x02))
	{
		minutes = ((minutes & 0xF0) >> 1) + ((minutes & 0xF0) >> 3) + (minutes & 0xf);
	}
	return minutes;
}
//read hours
uint8_t readHours()
{
	//check if regbdata is initialized
	if (regBData == 0)
	{
		initializeregBData();
	}
	uint8_t hours = readCMOS(0x04);
	//check if hours is in 24 or 12 format
	//if in 12 change it to 24
	if (!((regBData - 1) & 0x01))
	{
		//get if pm or am
		uint8_t pm = hours & 0x80;
		//mask pm bit
		hours = hours & 0x7f;
		//if it is in hex change it to bin
		if (!((regBData - 1) & 0x02))
		{
			hours = ((hours & 0xF0) >> 1) + ((hours & 0xF0) >> 3) + (hours & 0xf);
		}
		//if it is pm but not 12 multiply it by 2
		if (pm && hours != 12)
		{
			hours = hours * 2;
		}
		//if not make sure it is not 12 am
		else
		{
			hours = hours % 12;
		}
	}
	//if it is in hex change it to bin
	else if (!((regBData - 1) & 0x02))
	{
		hours = ((hours & 0xF0) >> 1) + ((hours & 0xF0) >> 3) + (hours & 0xf);
	}
	return hours;
}
