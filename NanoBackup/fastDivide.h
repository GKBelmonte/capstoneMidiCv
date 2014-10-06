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
			return (val); //
		case 15:
			return (val >> 1)+ (val >> 2) + (val >> 3) + (val >> 4);
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


/*
str = "{\n";
for(var ii =0 ; ii <= 16 ; ++ii)
{
    str += "case " + ii + ": return";
    for(var jj = 0; jj < 5 ; ++jj)
    {
       if((ii >> jj) & 1 == 1)
         str += "(val >> " + (5-(jj+1)) +") + ";
    }
    str += ";\n";
}
str += "}\n"
*/
uint16_t FastDivide16(uint16_t val, uint8_t weight)
{
	// Returns val * weight/16;
	switch (weight)
	{
		case 16:
			return (val);
		case 15:
			return (val >> 1)+ (val >> 2) + (val >> 3) + (val >> 4);
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

uint16_t FastDivide16_64Res(uint16_t val, uint8_t weight)
{
  switch(weight)
  {
  case 0: return 0;
  case 1: return(val >> 6) ;
  case 2: return(val >> 5) ;
  case 3: return(val >> 6) + (val >> 5) ;
  case 4: return(val >> 4) ;
  case 5: return(val >> 6) + (val >> 4) ;
  case 6: return(val >> 5) + (val >> 4) ;
  case 7: return(val >> 6) + (val >> 5) + (val >> 4) ;
  case 8: return(val >> 3) ;
  case 9: return(val >> 6) + (val >> 3) ;
  case 10: return(val >> 5) + (val >> 3) ;
  case 11: return(val >> 6) + (val >> 5) + (val >> 3) ;
  case 12: return(val >> 4) + (val >> 3) ;
  case 13: return(val >> 6) + (val >> 4) + (val >> 3) ;
  case 14: return(val >> 5) + (val >> 4) + (val >> 3) ;
  case 15: return(val >> 6) + (val >> 5) + (val >> 4) + (val >> 3) ;
  case 16: return(val >> 2) ;
  case 17: return(val >> 6) + (val >> 2) ;
  case 18: return(val >> 5) + (val >> 2) ;
  case 19: return(val >> 6) + (val >> 5) + (val >> 2) ;
  case 20: return(val >> 4) + (val >> 2) ;
  case 21: return(val >> 6) + (val >> 4) + (val >> 2) ;
  case 22: return(val >> 5) + (val >> 4) + (val >> 2) ;
  case 23: return(val >> 6) + (val >> 5) + (val >> 4) + (val >> 2) ;
  case 24: return(val >> 3) + (val >> 2) ;
  case 25: return(val >> 6) + (val >> 3) + (val >> 2) ;
  case 26: return(val >> 5) + (val >> 3) + (val >> 2) ;
  case 27: return(val >> 6) + (val >> 5) + (val >> 3) + (val >> 2) ;
  case 28: return(val >> 4) + (val >> 3) + (val >> 2) ;
  case 29: return(val >> 6) + (val >> 4) + (val >> 3) + (val >> 2) ;
  case 30: return(val >> 5) + (val >> 4) + (val >> 3) + (val >> 2) ;
  case 31: return(val >> 6) + (val >> 5) + (val >> 4) + (val >> 3) + (val >> 2) ;
  case 32: return(val >> 1) ;
  case 33: return(val >> 6) + (val >> 1) ;
  case 34: return(val >> 5) + (val >> 1) ;
  case 35: return(val >> 6) + (val >> 5) + (val >> 1) ;
  case 36: return(val >> 4) + (val >> 1) ;
  case 37: return(val >> 6) + (val >> 4) + (val >> 1) ;
  case 38: return(val >> 5) + (val >> 4) + (val >> 1) ;
  case 39: return(val >> 6) + (val >> 5) + (val >> 4) + (val >> 1) ;
  case 40: return(val >> 3) + (val >> 1) ;
  case 41: return(val >> 6) + (val >> 3) + (val >> 1) ;
  case 42: return(val >> 5) + (val >> 3) + (val >> 1) ;
  case 43: return(val >> 6) + (val >> 5) + (val >> 3) + (val >> 1) ;
  case 44: return(val >> 4) + (val >> 3) + (val >> 1) ;
  case 45: return(val >> 6) + (val >> 4) + (val >> 3) + (val >> 1) ;
  case 46: return(val >> 5) + (val >> 4) + (val >> 3) + (val >> 1) ;
  case 47: return(val >> 6) + (val >> 5) + (val >> 4) + (val >> 3) + (val >> 1) ;
  case 48: return(val >> 2) + (val >> 1) ;
  case 49: return(val >> 6) + (val >> 2) + (val >> 1) ;
  case 50: return(val >> 5) + (val >> 2) + (val >> 1) ;
  case 51: return(val >> 6) + (val >> 5) + (val >> 2) + (val >> 1) ;
  case 52: return(val >> 4) + (val >> 2) + (val >> 1) ;
  case 53: return(val >> 6) + (val >> 4) + (val >> 2) + (val >> 1) ;
  case 54: return(val >> 5) + (val >> 4) + (val >> 2) + (val >> 1) ;
  case 55: return(val >> 6) + (val >> 5) + (val >> 4) + (val >> 2) + (val >> 1) ;
  case 56: return(val >> 3) + (val >> 2) + (val >> 1) ;
  case 57: return(val >> 6) + (val >> 3) + (val >> 2) + (val >> 1) ;
  case 58: return(val >> 5) + (val >> 3) + (val >> 2) + (val >> 1) ;
  case 59: return(val >> 6) + (val >> 5) + (val >> 3) + (val >> 2) + (val >> 1) ;
  case 60: return(val >> 4) + (val >> 3) + (val >> 2) + (val >> 1) ;
  case 61: return(val >> 6) + (val >> 4) + (val >> 3) + (val >> 2) + (val >> 1) ;
  case 62: return(val >> 5) + (val >> 4) + (val >> 3) + (val >> 2) + (val >> 1) ;
  case 63: return(val >> 6) + (val >> 5) + (val >> 4) + (val >> 3) + (val >> 2) + (val >> 1) ;
  case 64: return(val >> 0) ;
  }
  return 0;
}
#endif
