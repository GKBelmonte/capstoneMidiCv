/*
  inttoarr.h
	Places the 16 bit number in the string buffer
    ugly implementation, with no loops, but guaranteed speed for sure.
    Keep buffer at least of size 6 (2^16 -1 = 65524 which is 5 characters + null term)
*/
void ResultToAscii(uint16_t res, char * buffer)
{
	/*************************************************************/
    uint8_t charCount = 0;
	if(res >= 50000)
	{
		if(res >=90000)
		{
			buffer[charCount++] = '9';
			res -= 90000;
		}
		else if(res >= 80000)
		{
			buffer[charCount++] = '8';
			res -= 80000;
		}
		else if(res >= 70000)
		{
			buffer[charCount++] = '7';
			res -= 70000;
		}
		else if(res >= 60000)
		{
			buffer[charCount++] = '6';
			res -= 60000;
		}
		else
		{
			buffer[charCount++] = '5';
			res -= 50000;
		}			
	}
	else
	{
		if(res >= 40000)
		{
			buffer[charCount++] = '4';
			res -= 40000;
		}
		else if(res >= 30000)
		{
			buffer[charCount++] = '3';
			res -= 30000;
		}
		else if(res >= 20000)
		{
			buffer[charCount++] = '2';
			res -= 20000;
		}
		else if(res >= 10000)
		{
			buffer[charCount++] = '1';
			res -= 10000;
		}	
	}
	/*************************************************************/
	if(res >= 5000)
	{
		if(res >=9000)
		{
			buffer[charCount++] = '9';
			res -= 9000;
		}
		else if(res >= 8000)
		{
			buffer[charCount++] = '8';
			res -= 8000;
		}
		else if(res >= 7000)
		{
			buffer[charCount++] = '7';
			res -= 7000;
		}
		else if(res >= 6000)
		{
			buffer[charCount++] = '6';
			res -= 6000;
		}
		else
		{
			buffer[charCount++] = '5';
			res -= 5000;
		}			
	}
	else
	{
		if(res >= 4000)
		{
			buffer[charCount++] = '4';
			res -= 4000;
		}
		else if(res >= 3000)
		{
			buffer[charCount++] = '3';
			res -= 3000;
		}
		else if(res >= 2000)
		{
			buffer[charCount++] = '2';
			res -= 2000;
		}
		else if(res >= 1000)
		{
			buffer[charCount++] = '1';
			res -= 1000;
		}
	}
	/*************************************************************/
	if(res >= 500)
	{
		if(res >=900)
		{
			buffer[charCount++] = '9';
			res -= 900;
		}
		else if(res >= 800)
		{
			buffer[charCount++] = '8';
			res -= 800;
		}
		else if(res >= 700)
		{
			buffer[charCount++] = '7';
			res -= 700;
		}
		else if(res >= 600)
		{
			buffer[charCount++] = '6';
			res -= 600;
		}
		else
		{
			buffer[charCount++] = '5';
			res -= 500;
		}			
	}
	else
	{
		if(res >= 400)
		{
			buffer[charCount++] = '4';
			res -= 400;
		}
		else if(res >= 300)
		{
			buffer[charCount++] = '3';
			res -= 300;
		}
		else if(res >= 200)
		{
			buffer[charCount++] = '2';
			res -= 200;
		}
		else if(res >= 100)
		{
			buffer[charCount++] = '1';
			res -= 100;
		}	
	}
	/*************************************************************/
	if(res >= 50)
	{
		if(res >=90)
		{
			buffer[charCount++] = '9';
			res -= 90;
		}
		else if(res >= 80)
		{
			buffer[charCount++] = '8';
			res -= 80;
		}
		else if(res >= 70)
		{
			buffer[charCount++] = '7';
			res -= 70;
		}
		else if(res >= 60)
		{
			buffer[charCount++] = '6';
			res -= 60;
		}
		else
		{
			buffer[charCount++] = '5';
			res -= 50;
		}			
	}
	else
	{
		if(res >=40)
		{
			buffer[charCount++] = '4';
			res -= 40;
		}
		else if(res >= 30)
		{
			buffer[charCount++] = '3';
			res -= 30;
		}
		else if(res >= 20)
		{
			buffer[charCount++] = '2';
			res -= 20;
		}
		else if(res >= 10)
		{
			buffer[charCount++] = '1';
			res -= 10;
		}
		else
		{
			buffer[charCount++] = '0';
		}	
	}
	/*************************************************************/
	res += 0x30; //transform the last one to a digit on the fly
	buffer[charCount++] = (char) res;
	buffer[charCount++] = 0; //max here is 5
}