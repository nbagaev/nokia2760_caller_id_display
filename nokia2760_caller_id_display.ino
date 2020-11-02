//Arduino test sketch for Nokia 2760 caller id display
//Assuming the display controller is PCF8814

#define charge_pump_on B00101111
#define display_on B10101111
#define display_all_off B10100100
#define set_x_lower_bits B00000000
#define set_x_upper_bits B00010000
#define set_y_address B10110000
#define dat B00000001
#define com B00000000

uint8_t pin_sclk = 4; //serial clock
uint8_t pin_sdin = 5; //data input
uint8_t pin_sce = 6;  //chip enable
uint8_t pin_reset = 7;//reset
        
void send_byte(uint8_t dc, uint8_t bits)
{
  digitalWrite(pin_sce, LOW);//A falling edge on SCE enables the serial interface and indicates the start of data transmission
    
  digitalWrite(pin_sdin, dc);//send D/C bit, HIGH=data LOW=command
    
  digitalWrite(pin_sclk, HIGH);//make clock pulse
  digitalWrite(pin_sclk, LOW);
  
  shiftOut(pin_sdin, pin_sclk, MSBFIRST, bits);//send command/data bits
    
  digitalWrite(pin_sce, HIGH);//disable the serial interface
}

void set_cursor(uint8_t column, uint8_t row)
{
  //display has 96 columns (X) and 8 8-pixel high rows + 1 4-pixel row (Y)
  //each byte in RAM represents an 8-pixel column

  //Horizontal addressing
  send_byte(com, set_x_lower_bits | (column & 0x0F));//set X-address, lower 4 bits
  send_byte(com, set_x_upper_bits | (column >> 5));//set X-address, upper 3 bits
  
  //Vertical addressing
  send_byte(com, set_y_address | row);//set Y-address
}

void clear_disp()
{
  set_cursor(0, 0);  
  for(int i = 0; i < 96 * 9; i++)
    send_byte(dat, 0x00);
}

void print_char()
{
  //a char is 5 columns wide and 8 pix (bits) high
  const uint8_t chr[] = {0x7e, 0x11, 0x11, 0x11, 0x7e};
  set_cursor(0, 0);  
  for(uint8_t i = 0; i < 5; i++)
    send_byte(dat, chr[i]);
}

void setup()
{
  pinMode(pin_sclk, OUTPUT);
  pinMode(pin_sdin, OUTPUT);
  pinMode(pin_reset, OUTPUT);
  pinMode(pin_sce, OUTPUT);
    
  //reset 
  digitalWrite(pin_reset, LOW);
  digitalWrite(pin_reset, HIGH);
      
  //init    
  digitalWrite(pin_sce, HIGH);
  digitalWrite(pin_sclk, LOW);//At the falling SCE edge SCLK must be LOW

  send_byte(com, charge_pump_on);
  send_byte(com, display_on);
  send_byte(com, display_all_off);

  clear_disp();
  print_char();
}

void loop()
{
}
