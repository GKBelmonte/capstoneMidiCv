#ifndef FAST_DIVIDE
#define FAST_DIVIDE
/**
  Multiplies by val by weight / 16
*/

uint8_t FastDivide8(uint8_t val, uint8_t weight)
{
	// Returns val * weight/16;
	switch (weight)
	{
		case 16:
			return (val);
		case 15:
			return (val >> 1)+ (val >> 2) + (val >> 3);
		case 14:
			return (val >> 1)+ (val >> 2) + (val >> 3);
		case 13:
			return (val >> 1)+ (val >> 2) + (val >> 4);
		case 12:
			return (val >> 1) + (val >> 2);
		case 11:
			return (val >> 1)+ (val >> 3) + (val >> 4);
		case 10:
			return (val >> 1) + (val >> 3);
		case 9:
			return (val >> 1) + (val >> 4);
		case 8:
			return val >> 1;
		case 7:
			return (val >> 2) + (val >> 3) + (val >> 4);
		case 6:
			return 6*(val >> 4);
		case 5:
			return 5*(val >> 4);
		case 4:
			return val >> 2;
		case 3:
			return 3*(val >> 4);
		case 2:
			return (val >> 3);
		case 1:
			return (val >> 4);
		default:
			return 0;
	}
}

uint16_t FastDivide16(uint16_t val, uint8_t weight)
{
	// Returns val * weight/16;
	switch (weight)
	{
		case 16:
			return (val);
		case 15:
			return (val >> 1)+ (val >> 2) + (val >> 3);
		case 14:
			return (val >> 1)+ (val >> 2) + (val >> 3);
		case 13:
			return (val >> 1)+ (val >> 2) + (val >> 4);
		case 12:
			return (val >> 1) + (val >> 2);
		case 11:
			return (val >> 1)+ (val >> 3) + (val >> 4);
		case 10:
			return (val >> 1) + (val >> 3);
		case 9:
			return (val >> 1) + (val >> 4);
		case 8:
			return val >> 1;
		case 7:
			return (val >> 2) + (val >> 3) + (val >> 4);
		case 6:
			return 6*(val >> 4);
		case 5:
			return 5*(val >> 4);
		case 4:
			return val >> 2;
		case 3:
			return 3*(val >> 4);
		case 2:
			return (val >> 3);
		case 1:
			return (val >> 4);
		default:
			return 0;
	}
}
#endif
