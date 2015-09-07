volatile uint8_t cbuf; //Input buffer for a command
volatile uint8_t bitp;  //pointer to current Byte
volatile uint8_t bytep; //pointer to current Bit

volatile uint8_t mode; //state
volatile uint8_t wmode; //if 0 next bit that send the device is  0
volatile uint8_t actbit; //current
volatile uint8_t srcount; //counter for search rom


//Write a bit after next falling edge from master
//its for sending a zero as soon as possible 
#define OWW_NO_WRITE 2
#define OWW_WRITE_1 1
#define OWW_WRITE_0 0

void OWSInit() {
  mode=OWM_SLEEP;
  wmode=OWW_NO_WRITE;
  OW_DDR&=~OW_PINN;

  SET_FALLING;

  CLKPR=(1<<CLKPCE); 
  CLKPR=0; //*8Mhz
  TIMSK2=0;
  EIMSK=(1<<INT0);  //set direct GIMSK register
  TCCR0B=(1<<CS00)|(1<<CS01); //8mhz /64 couse 8 bit Timer interrupt every 8us
  DIS_TIMER;
  
  }

