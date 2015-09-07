TIMER_INT {
  uint8_t lwmode=wmode; //let this variables in registers
  uint8_t lmode=mode;
  uint8_t lbytep=bytep;
  uint8_t lbitp=bitp;
  uint8_t lsrcount=srcount;
  uint8_t lactbit=actbit;
#ifdef DS2423
  uint16_t lscrc=scrc;
#else
  uint8_t lscrc=scrc;
#endif
  //Ask input line sate 
  uint8_t inputLineState=((OW_PIN&OW_PINN)==OW_PINN);  
  //Interrupt still active ?
  if (CHK_INT_EN) {
    //maybe reset pulse
    if (inputLineState==0) { 
      lmode=OWM_CHK_RESET;  //wait for rising edge
      SET_RISING; 
    }
    DIS_TIMER;
  } 
  else
    switch (lmode) {
    case OWM_RESET:  //Reset pulse and time after is finished, now go in presence state
      lmode=OWM_PRESENCE;
      SET_LOW;
      TCNT_REG=~(OWT_PRESENCE);
      DIS_OWINT;  //No Pin interrupt necessary only wait for presence is done
      break;
    case OWM_PRESENCE:
      RESET_LOW;  //Presence is done now wait for a command
      lmode=OWM_READ_COMMAND;
      cbuf=0;
      lbitp=1;  //Command buffer have to set zero, only set bits will write in
      break;
    case OWM_READ_COMMAND:
      if (inputLineState) {  //Set bit if line high 
        cbuf|=lbitp;
      } 
      lbitp=(lbitp<<1);
      if (!lbitp) { //8-Bits read
        lbitp=1;
        switch (cbuf) {
        case 0x55:
          lbytep=0;
          lmode=OWM_MATCH_ROM;
          break;
        case 0xF0:  //initialize search rom
          lmode=OWM_SEARCH_ROM;
          lsrcount=0;
          lbytep=0;
          lactbit=(owid[lbytep]&lbitp)==lbitp; //set actual bit
          lwmode=lactbit;  //prepare for writing when next falling edge
          break;
        case 0xCC:
          lbytep=0;
          cbuf=0;
          lmode=OWM_READ_COMMAND;
          break;
        case 0x33:
          lmode=OWM_READ_ROM;
          lbytep=0;	
          break;
        default:
          lmode=OWM_SLEEP;  //all other commands do nothing
          OWS_CMD( cbuf, lmode ); 
        }		
      }			
      break;
    case OWM_SEARCH_ROM:
      RESET_LOW;  //Set low also if nothing send (branch takes time and memory)
      lsrcount++;  //next search rom mode
      switch (lsrcount) {
      case 1:
        lwmode=!lactbit;  //preparation sending complement
        break;
      case 3:
        if (inputLineState!=(lactbit==1)) {  //check master bit
          lmode=OWM_SLEEP;  //not the same go sleep
        } 
        else {
          lbitp=(lbitp<<1);  //prepare next bit
          if (lbitp==0) {
            lbitp=1;
            lbytep++;
            if (lbytep>=8) {
              lmode=OWM_SLEEP;  //all bits processed 
              break;
            }
          }				
          lsrcount=0;
          lactbit=(owid[lbytep]&lbitp)==lbitp;
          lwmode=lactbit;
        }		
        break;			
      }
      break;
    case OWM_MATCH_ROM:
      if (inputLineState==((owid[lbytep]&lbitp)==lbitp)) {  //Compare with ID Buffer
        lbitp=(lbitp<<1);
        if (!lbitp) {
          lbytep++;
          lbitp=1;
          if (lbytep>=8) {
            lmode=OWM_READ_COMMAND;  //same? get next command

            cbuf=0;
            break;			
          }
        } 
      } 
      else {
        lmode=OWM_SLEEP;
      }
      break;
      case OWM_
#if defined(Barometer) || defined(INTERNTEMP)
    case OWM_WRITE_SCRATCHPAD:
      if (inputLineState) {
        scratchpad.bytes[lbytep]|=lbitp;
      } 
      lbitp=(lbitp<<1);
      if (!lbitp) {		
        lbytep++;
        lbitp=1;
#ifdef Barometer
        if (lbytep==1) {  //only status byte can be written
          if ((scratchpad.state&MS_CONTINUOSLY) !=0) { //start Continuous mode get first values
            scratchpad.state|=MS_PROGRESS;
          }
          lmode=OWM_SLEEP;
          break;
        } 
#endif
#ifdef INTERNTEMP
        if (lbytep==5) {
          lmode=OWM_SLEEP;
          break;
        } 
#endif
        else scratchpad.bytes[lbytep]=0;
      }		
      break;
#endif      	
#ifdef Barometer
    case OWM_READ_SCRATCHPAD:
      RESET_LOW;
      if ((lscrc&1)!=lactbit) lscrc=(lscrc>>1)^0x8c; 
      else lscrc >>=1;
      lbitp=(lbitp<<1);
      if (!lbitp) {		
        lbytep++;
        lbitp=1;
        if (lbytep>=10) {
          lmode=OWM_SLEEP;
          break;			
        } 
        else if (lbytep==9) scratchpad.bytes[9]=lscrc;
      }					
      lactbit=(lbitp&scratchpad.bytes[lbytep])==lbitp;
      lwmode=lactbit;
      break;
#endif
#ifdef INTERNTEMP
    case OWM_READ_SCRATCHPAD:
      RESET_LOW;
      if ((lscrc&1)!=lactbit) lscrc=(lscrc>>1)^0x8c; 
      else lscrc >>=1;
      lbitp=(lbitp<<1);
      if (!lbitp) {		
        lbytep++;
        lbitp=1;
        if (lbytep>=9) {
          lmode=OWM_SLEEP;
          break;			
        } 
        else if (lbytep==8) scratchpad.bytes[8]=lscrc;
      }					
      lactbit=(lbitp&scratchpad.bytes[lbytep])==lbitp;
      lwmode=lactbit;
      break;
#endif
#ifdef DS2423
    case OWM_GET_ADRESS:  
      if (inputLineState) { //Get the Address for reading
        scratchpad.bytes[lbytep]|=lbitp;
      }  
      //address is part of crc
      if ((lscrc&1)!=inputLineState) lscrc=(lscrc>>1)^0xA001; 
      else lscrc >>=1;
      lbitp=(lbitp<<1);
      if (!lbitp) {	
        lbytep++;
        lbitp=1;
        if (lbytep==2) {
          lmode=OWM_READ_MEMORY_COUNTER;
          lactbit=(lbitp&scratchpad.bytes[lbytep])==lbitp;
          lwmode=lactbit;
          lsrcount=(scratchpad.addr&0xfe0)+0x20-scratchpad.addr; 
          //bytes between start and Counter Values, Iam never understanding why so much???
          break;
        } 
        else scratchpad.bytes[lbytep]=0;
      }			
      break;	
    case OWM_READ_MEMORY_COUNTER:
      RESET_LOW;
      //CRC16 Calculation
      if ((lscrc&1)!=lactbit) lscrc=(lscrc>>1)^0xA001; 
      else lscrc >>=1;
      inputLineState=lactbit;
      lbitp=(lbitp<<1);
      if (!lbitp) {		
        lbytep++;
        lbitp=1;
        if (lbytep==3) {
          lsrcount--;
          if (lsrcount) lbytep--;
          else  {//now copy counter in send buffer
            switch (scratchpad.addr&0xFe0) {
            case 0x1E0:
              scratchpad.counter=Counter0;
              break;
            case 0x1C0:
              scratchpad.counter=Counter1;
              break;
            default: 
              scratchpad.counter=0;
            }
          }
        }
        if (lbytep>=13) { //done sending
          lmode=OWM_SLEEP;
          break;			
        }  		 
        if ((lbytep==11)&&(lbitp==1)) { //Send CRC
          scratchpad.crc=~lscrc; 
        }			

      }					
      lactbit=(lbitp&scratchpad.bytes[lbytep])==lbitp;
      lwmode=lactbit;
      break;
#endif
    case OWM_READ_ROM:
      RESET_LOW;
      lbitp=(lbitp<<1);
      if (!lbitp) {		
        lbytep++;
        lbitp=1;
        if (lbytep>=8) {
          lmode=OWM_SLEEP;
          break;			
        } 
      }					
      lactbit=(lbitp&owid[lbytep])==lbitp;
      lwmode=lactbit;
      break;
    }
  if (lmode==OWM_SLEEP) {
    DIS_TIMER;
  }
  if (lmode!=OWM_PRESENCE)  { 
    TCNT_REG=~(OWT_MIN_RESET-OWT_READLINE);  //OWT_READLINE around OWT_LOWTIME
    EN_OWINT;
  }

  mode=lmode;
  wmode=lwmode;
  bytep=lbytep;
  bitp=lbitp;
  srcount=lsrcount;
  actbit=lactbit;
  scrc=lscrc;
}
