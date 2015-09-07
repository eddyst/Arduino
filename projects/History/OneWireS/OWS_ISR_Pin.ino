
PIN_INT {
  uint8_t lwmode=wmode;  //let this variables in registers
  uint8_t lmode=mode;
  if ((lwmode==OWW_WRITE_0)) {
    SET_LOW;
    lwmode=OWW_NO_WRITE;
  }    //if necessary set 0-Bit 
  DIS_OWINT; //disable interrupt, only in OWM_SLEEP mode it is active
  switch (lmode) {
  case OWM_SLEEP:  
    TCNT_REG=~(OWT_MIN_RESET);
    EN_OWINT; //other edges ?
    break;
    //start of reading with falling edge from master, reading closed in timer isr
  case OWM_MATCH_ROM:  //falling edge wait for receive 
  case OWM_GET_ADRESS:
  case OWM_WRITE_SCRATCHPAD:
  case OWM_READ_COMMAND:
    TCNT_REG=~(OWT_READLINE); //wait a time for reading
    break;
  case OWM_SEARCH_ROM:   //Search algorithm waiting for receive or send
    if (srcount<2) { //this means bit or complement is writing, 
      TCNT_REG=~(OWT_LOWTIME);					
    } 
    else 
      TCNT_REG=~(OWT_READLINE);  //init for read answer of master 
    break;
  case OWM_READ_ROM:
  case OWM_READ_MEMORY_COUNTER: //a bit is sending 
  case OWM_READ_SCRATCHPAD:  //a bit is sending 
    TCNT_REG=~(OWT_LOWTIME);
    break;
  case OWM_CHK_RESET:  //rising edge of reset pulse
    SET_FALLING 
      TCNT_REG=~(OWT_RESET_PRESENCE);  //waiting for sending presence pulse
    lmode=OWM_RESET;
    break;
  }
  EN_TIMER;
  mode=lmode;
  wmode=lwmode;

}
