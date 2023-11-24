// -------------------------------------------------------------
// teensy-slcan for Teensy 3.2/5/6 CAN bus
// -------------------------------------------------------------
// Requires FlexCAN library by Collin Kidder, Based on CANTest by Pawelsky (based on CANtest by teachop)
// From https://github.com/collin80/FlexCAN_Library
//
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SPI.h>

boolean slcan     = false;
boolean cr        = false;
boolean timestamp = false;

int CANBUSSPEED = 250000;

static uint8_t hexval[17] = "0123456789ABCDEF";

void slcan_ack();
void slcan_nack();

//FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan; //choose wich can used here
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> myCan;
//FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> myCan;
//----------------------------------------------------------------

void send_canmsg(char *buf, boolean rtrFlag) {
  if (slcan) {
    CAN_message_t outMsg;
    int msg_id = 0;
    sscanf(&buf[1], "%03x", &msg_id);
    if (rtrFlag) {
      outMsg.flags.remote = 1;
      //outMsg.rtr = 1;
    } else {
      //outMsg.ext = 0;
      outMsg.flags.extended = 0;
    }
    outMsg.id = msg_id;
    int msg_len = 0;
    sscanf(&buf[4], "%01x", &msg_len);
    outMsg.len = msg_len;
    int candata = 0;
    for (int i = 0; i < msg_len; i++) {
      sscanf(&buf[5 + (i*2)], "%02x", &candata);
      outMsg.buf[i] = candata;
    }
    myCan.write(outMsg);
  }
} // send_canmsg()

// -------------------------------------------------------------
void pars_slcancmd(char *buf)
{                           // LAWICEL PROTOCOL
  switch (buf[0]) {
    case 'O':               // OPEN CAN
      slcan=true;
      myCan.begin();
      myCan.setBaudRate(CANBUSSPEED);
      slcan_ack();
      break;
    case 'C':               // CLOSE CAN
      slcan=false;
    //  Can0.end();
      slcan_ack();
      break;
    case 't':               // send std frame
      send_canmsg(buf,false);
      slcan_ack();
      break;
    case 'T':               // send ext frame
      send_canmsg(buf,false);
      slcan_ack();
      break;
    case 'r':               // send std rtr frame
      send_canmsg(buf,true);
      slcan_ack();
      break;
    case 'R':               // send ext rtr frame
      send_canmsg(buf,true);
      slcan_ack();
      break;
    case 'Z':               // ENABLE TIMESTAMPS
      switch (buf[1]) {
        case '0':           // TIMESTAMP OFF  
          timestamp = false;
          slcan_ack();
          break;
        case '1':           // TIMESTAMP ON
          timestamp = true;
          slcan_ack();
          break;
        default:
          break;
      }
      break;
    case 'M':               ///set ACCEPTANCE CODE ACn REG
      slcan_ack();
      break;
    case 'm':               // set ACCEPTANCE CODE AMn REG
      slcan_ack();
      break;
    case 's':               // CUSTOM CAN bit-rate
      slcan_nack();
      break;
    case 'S':               // CAN bit-rate
      switch (buf[1]) {
        case '0': // 10k  
          // N/A
          slcan_nack();
          break;
        case '1': // 20k
          // N/A
          slcan_nack();
          break;
        case '2': // 50k
          CANBUSSPEED = 50000;
          slcan_ack();
          break;
        case '3': // 100k
          CANBUSSPEED = 100000;
          slcan_ack();
          break;
        case '4': // 125k
          CANBUSSPEED = 125000;
          slcan_ack();
          break;
        case '5': // 250k
          CANBUSSPEED = 250000;
          slcan_ack();
          break;
        case '6': // 500k
          CANBUSSPEED = 500000;
          slcan_ack();
          break;
        case '7': // 800k
          // N/A
          slcan_nack();
          break;
        case '8': // 1000k
          CANBUSSPEED = 1000000;
          slcan_ack();
          break;
        default:
          slcan_nack();
          break;
      }
      break;
    case 'F':               // STATUS FLAGS
      Serial.print("F00");
      slcan_ack();
      break;
    case 'V':               // VERSION NUMBER
      Serial.print("V1234");
      slcan_ack();
      break;
    case 'N':               // SERIAL NUMBER
      Serial.print("N2208");
      slcan_ack();
      break;
    case 'l':               // (NOT SPEC) TOGGLE LINE FEED ON SERIAL
      cr = !cr;
      slcan_nack();
      break;
    case 'h':               // (NOT SPEC) HELP SERIAL
      Serial.println();
      Serial.println(F("mintynet.com - slcan teensy"));
      Serial.println();
      Serial.println(F("O\t=\tStart slcan"));
      Serial.println(F("C\t=\tStop slcan"));
      Serial.println(F("t\t=\tSend std frame"));
      Serial.println(F("r\t=\tSend std rtr frame"));
      Serial.println(F("T\t=\tSend ext frame"));
      Serial.println(F("R\t=\tSend ext rtr frame"));
      Serial.println(F("Z0\t=\tTimestamp Off"));
      Serial.print(F("Z1\t=\tTimestamp On"));
      if (timestamp) Serial.print(F("  ON"));
      Serial.println();
      Serial.println(F("snn\t=\tSpeed 0xnnk  N/A"));
      Serial.println(F("S0\t=\tSpeed 10k    N/A"));
      Serial.println(F("S1\t=\tSpeed 20k    N/A"));
      Serial.println(F("S2\t=\tSpeed 50k"));
      Serial.println(F("S3\t=\tSpeed 100k"));
      Serial.println(F("S4\t=\tSpeed 125k"));
      Serial.println(F("S5\t=\tSpeed 250k"));
      Serial.println(F("S6\t=\tSpeed 500k"));
      Serial.println(F("S7\t=\tSpeed 800k   N/A"));
      Serial.println(F("S8\t=\tSpeed 1000k"));
      Serial.println(F("F\t=\tFlags        N/A"));
      Serial.println(F("N\t=\tSerial No"));
      Serial.println(F("V\t=\tVersion"));
      Serial.println(F("-----NOT SPEC-----"));
      Serial.println(F("h\t=\tHelp"));
      Serial.print(F("l\t=\tToggle CR "));
      if (cr) {
        Serial.println(F("ON"));
      } else {
        Serial.println(F("OFF"));
      }
      Serial.print(F("CAN_SPEED:\t"));
      switch(CANBUSSPEED/1000) {
        case 50:
          Serial.print(F("50"));
          break;
        case 100:
          Serial.print(F("100"));
          break;
        case 125:
          Serial.print(F("125"));
          break;
        case 250:
          Serial.print(F("250"));
          break;
        case 500:
          Serial.print(F("500"));
          break;
        case 1000:
          Serial.print(F("1000"));
          break;
        default:
          break;
      }
      Serial.print(F("kbps"));
      if (timestamp) {
        Serial.print(F("\tT"));
      }
      if (slcan) {
        Serial.print(F("\tON"));
      } else {
        Serial.print(F("\tOFF"));
      }
      Serial.println();
      slcan_nack();
      break;
    default:
      slcan_nack();
      break;
  }
} // pars_slcancmd()

//----------------------------------------------------------------

void slcan_ack()
{
  Serial.write('\r');
} // slcan_ack()

//----------------------------------------------------------------

void slcan_nack()
{
  Serial.write('\a');
} // slcan_nack()

// -------------------------------------------------------------

void xfer_tty2can()
{
  int ser_length;
  static char cmdbuf[32];
  static int cmdidx = 0;
  if ((ser_length = Serial.available()) > 0)
  {
    for (int i = 0; i < ser_length; i++) {
      char val = Serial.read();
      cmdbuf[cmdidx++] = val;
      if (cmdidx == 32)
      {
        slcan_nack();
        cmdidx = 0;
      } else if (val == '\r')
      {
        cmdbuf[cmdidx] = '\0';
        pars_slcancmd(cmdbuf);
        cmdidx = 0;
      }
    }
  }
} //xfer_tty2can()

// -------------------------------------------------------------

void xfer_can2tty()
{
  String command = "";
  long time_now = 0;
  CAN_message_t inMsg;
  //while (myCan.available()) 
  //{
    
    if (slcan) {
      myCan.read(inMsg);
      
      // return if msgid is zero (there are no messages available)
      if (inMsg.id == 0){ 
        return;
      }
      
      if((inMsg.flags.extended) == true) {
        if ((inMsg.flags.remote) == true) {
          command = command + "R";
        } else {
          command = command + "T";
        }
        command = command + char(hexval[ (inMsg.id>>28)&1]);
        command = command + char(hexval[ (inMsg.id>>24)&15]);
        command = command + char(hexval[ (inMsg.id>>20)&15]);
        command = command + char(hexval[ (inMsg.id>>16)&15]);
        command = command + char(hexval[ (inMsg.id>>12)&15]);
        command = command + char(hexval[ (inMsg.id>>8)&15]);
        command = command + char(hexval[ (inMsg.id>>4)&15]);
        command = command + char(hexval[ inMsg.id&15]);
        command = command + char(hexval[ inMsg.len ]);
      } else {
           if ((inMsg.flags.remote) == true) {
          command = command + "r";
        } else {
          command = command + "t";
        }
        command = command + char(hexval[ (inMsg.id>>8)&15]);
        command = command + char(hexval[ (inMsg.id>>4)&15]);
        command = command + char(hexval[ inMsg.id&15]);
        command = command + char(hexval[ inMsg.len ]);
      }
      for(int i = 0; i < inMsg.len; i++){
        command = command + char(hexval[ inMsg.buf[i]>>4 ]);
        command = command + char(hexval[ inMsg.buf[i]&15 ]);
        //printf("%c\t", (char)rx_frame.data.u8[i]);
      }
      if (timestamp) {
        time_now = millis() % 60000;
        command = command + char(hexval[ (time_now>>12)&15 ]);
        command = command + char(hexval[ (time_now>>8)&15 ]);
        command = command + char(hexval[ (time_now>>4)&15 ]);
        command = command + char(hexval[ time_now&15 ]);
      }
      command = command + '\r';
      Serial.print(command);
      if (cr) Serial.println("");
    }
 // }
}

// -------------------------------------------------------------

void setup(void)
{
  delay(1000);

  Serial.begin(2000000);
  if (CANBUSSPEED == 500000) Serial.println (F("STANDARD OBD2 SPEED"));
  Serial.println(F("teensy-slcan"));
  Serial.print(F("SPEED     "));
  Serial.print(CANBUSSPEED);
  Serial.println(F("bps"));
  
  myCan.begin();
  myCan.setBaudRate(CANBUSSPEED);
  
  pars_slcancmd("C/0");

  if (!slcan)
  {
    if (slcan)
    {
      Serial.println(F("SLCAN     ON"));
    } else {
      Serial.println(F("SLCAN     OFF"));
    }
    if (cr)
    {
      Serial.println(F("CR        ON"));
    } else {
      Serial.println(F("CR        OFF"));
    }
    if (timestamp)
    {
      Serial.println(F("TIMESTAMP ON"));
    } else {
      Serial.println(F("TIMESTAMP OFF"));
    }
  }
} //setup()

// -------------------------------------------------------------

void loop(void)
{
  xfer_can2tty();
  xfer_tty2can();
} //loop()
