	//test
	// 추가한 기능 이동할때 마다 led색 랜덤으로 점등
	#include <avr/io.h>
	#define F_CPU 16000000
	#include <util/delay.h>
	#include <avr/interrupt.h>
	
	#define  OCR1_R OCR1A
	#define  OCR1_G OCR1B
	#define  OCR1_B OCR1C

	unsigned char redV;             //빨간 LED아날로그 값 설정 (0~255)
	unsigned char greenV;			//초록 LED아날로그 값 설정 (0~255)
	unsigned char blueV;			//파란 LED아날로그 값 설정 (0~255)


	#define TRIG 2 // HC-SR04 Trigger 신호 (출력)
	#define ECHO 3 // HC-SR04 Echo 신호 (입력)
	
	#define SONIC_DDR	DDRB
	#define SONIC_PORT	PORTB
	#define SONIC_PIN	PINB
	
	#define TRIG_HIGH	(1<<TRIG)
	#define TRIG_LOW	~(1<<TRIG)
	#define ECHO_HIGH	(1<<ECHO)
	#define ECHO_LOW	~(1<<ECHO)
	
	#define GLCD_DATABUS	PORTD // GLCD data
	#define GLCD_CONTROL	PORTC // GLCD control signal
	#define GLCD_DATABUS_DDR DDRD
	#define GLCD_CONTROL_DDR DDRC
	
	// Graphic LCD 명령어 셋팅
	#define GLCD_CS1 0x08		 // GLCD_CS1 Select 0000 1000
	#define GLCD_CS2 0x10		 // GLCD_CS2 Select 0001 0000
	#define GLCD_CS1_2 0x18		 // GLCD_CS1, GLCD_CS2 Select 0001 1000
	
	#define GLCD_START_LINE		0xC0	// 11XXXXXX: set lcd start line
	#define GLCD_SET_X_ADDR		0xB8	// 10111XXX: set lcd X address
	#define GLCD_SET_Y_ADDR		0x40	// 01YYYYYY: set lcd Y address
	
	#define DISPON			0x3F
	#define DISPOFF 			0x3E
	
	#define GLCD_CS1 0x08
	#define GLCD_CS2 0x10
	#define GLCD_CS1_2 0x18
	
	#define GLCD_SET_X_ADDR 0xB8
	#define GLCD_SET_Y_ADDR 0x40
	
	
	// 키패드 명령어 셋팅
	#define KEY_DDR DDRF
	#define KEY_PORT PORTF
	#define KEY_PIN PINF
	
	unsigned char xcharacter, ycharacter;		// x character(0-7), y character(0-19)
	unsigned char cursor_flag, xcursor, ycursor;	// x and y cursor position(0-7, 0-19)
	
	unsigned char ScreenBuffer[8][128]={0};           // screen buffer
	
	unsigned char font[95][5] = {	            /* 5x7 ASCII character font */
	{0x00, 0x00, 0x00, 0x00, 0x00},		// 0x20 space
	{0x00, 0x00, 0x4f, 0x00, 0x00},		// 0x21 !
	{0x00, 0x07, 0x00, 0x07, 0x00},		// 0x22 "
	{0x14, 0x7f, 0x14, 0x7f, 0x14},		// 0x23 #
	{0x24, 0x2a, 0x7f, 0x2a, 0x12},		// 0x24 $
	{0x23, 0x13, 0x08, 0x64, 0x62},		// 0x25 %
	{0x36, 0x49, 0x55, 0x22, 0x50},		// 0x26 &
	{0x00, 0x05, 0x03, 0x00, 0x00},		// 0x27 '
	{0x00, 0x1c, 0x22, 0x41, 0x00},		// 0x28 (
	{0x00, 0x41, 0x22, 0x1c, 0x00},		// 0x29 )
	{0x14, 0x08, 0x3e, 0x08, 0x14},		// 0x2a *
	{0x08, 0x08, 0x3e, 0x08, 0x08},		// 0x2b +
	{0x00, 0x50, 0x30, 0x00, 0x00},		// 0x2c ,
	{0x08, 0x08, 0x08, 0x08, 0x08},		// 0x2d -
	{0x00, 0x60, 0x60, 0x00, 0x00},		// 0x2e .
	{0x20, 0x10, 0x08, 0x04, 0x02},		// 0x2f /
	{0x3e, 0x51, 0x49, 0x45, 0x3e},		// 0x30 0
	{0x00, 0x42, 0x7f, 0x40, 0x00},		// 0x31 1
	{0x42, 0x61, 0x51, 0x49, 0x46},		// 0x32 2
	{0x21, 0x41, 0x45, 0x4b, 0x31},		// 0x33 3
	{0x18, 0x14, 0x12, 0x7f, 0x10},		// 0x34 4
	{0x27, 0x45, 0x45, 0x45, 0x39},		// 0x35 5
	{0x3c, 0x4a, 0x49, 0x49, 0x30},		// 0x36 6
	{0x01, 0x71, 0x09, 0x05, 0x03},		// 0x37 7
	{0x36, 0x49, 0x49, 0x49, 0x36},		// 0x38 8
	{0x06, 0x49, 0x49, 0x29, 0x1e},		// 0x39 9
	{0x00, 0x36, 0x36, 0x00, 0x00},		// 0x3a :
	{0x00, 0x56, 0x36, 0x00, 0x00},		// 0x3b ;
	{0x08, 0x14, 0x22, 0x41, 0x00},		// 0x3c <
	{0x14, 0x14, 0x14, 0x14, 0x14},		// 0x3d =
	{0x00, 0x41, 0x22, 0x14, 0x08},		// 0x3e >
	{0x02, 0x01, 0x51, 0x09, 0x06},		// 0x3f ?
	{0x32, 0x49, 0x79, 0x41, 0x3e},		// 0x40 @
	{0x7e, 0x11, 0x11, 0x11, 0x7e},		// 0x41 A
	{0x7f, 0x49, 0x49, 0x49, 0x36},		// 0x42 B
	{0x3e, 0x41, 0x41, 0x41, 0x22},		// 0x43 C
	{0x7f, 0x41, 0x41, 0x22, 0x1c},		// 0x44 D
	{0x7f, 0x49, 0x49, 0x49, 0x41},		// 0x45 E
	{0x7f, 0x09, 0x09, 0x09, 0x01},		// 0x46 F
	{0x3e, 0x41, 0x49, 0x49, 0x7a},		// 0x47 G
	{0x7f, 0x08, 0x08, 0x08, 0x7f},		// 0x48 H
	{0x00, 0x41, 0x7f, 0x41, 0x00},		// 0x49 I
	{0x20, 0x40, 0x41, 0x3f, 0x01},		// 0x4a J
	{0x7f, 0x08, 0x14, 0x22, 0x41},		// 0x4b K
	{0x7f, 0x40, 0x40, 0x40, 0x40},		// 0x4c L
	{0x7f, 0x02, 0x0c, 0x02, 0x7f},		// 0x4d M
	{0x7f, 0x04, 0x08, 0x10, 0x7f},		// 0x4e N
	{0x3e, 0x41, 0x41, 0x41, 0x3e},		// 0x4f O
	{0x7f, 0x09, 0x09, 0x09, 0x06},		// 0x50 P
	{0x3e, 0x41, 0x51, 0x21, 0x5e},		// 0x51 Q
	{0x7f, 0x09, 0x19, 0x29, 0x46},		// 0x52 R
	{0x26, 0x49, 0x49, 0x49, 0x32},		// 0x53 S
	{0x01, 0x01, 0x7f, 0x01, 0x01},		// 0x54 T
	{0x3f, 0x40, 0x40, 0x40, 0x3f},		// 0x55 U
	{0x1f, 0x20, 0x40, 0x20, 0x1f},		// 0x56 V
	{0x3f, 0x40, 0x38, 0x40, 0x3f},		// 0x57 W
	{0x63, 0x14, 0x08, 0x14, 0x63},		// 0x58 X
	{0x07, 0x08, 0x70, 0x08, 0x07},		// 0x59 Y
	{0x61, 0x51, 0x49, 0x45, 0x43},		// 0x5a Z
	{0x00, 0x7f, 0x41, 0x41, 0x00},		// 0x5b [
	{0x02, 0x04, 0x08, 0x10, 0x20},		// 0x5c (\)
	{0x00, 0x41, 0x41, 0x7f, 0x00},		// 0x5d ]
	{0x04, 0x02, 0x01, 0x02, 0x04},		// 0x5e ^
	{0x40, 0x40, 0x40, 0x40, 0x40},		// 0x5f _
	{0x00, 0x01, 0x02, 0x04, 0x00},		// 0x60 `
	{0x20, 0x54, 0x54, 0x54, 0x78},		// 0x61 a
	{0x7f, 0x48, 0x44, 0x44, 0x38},		// 0x62 b
	{0x38, 0x44, 0x44, 0x44, 0x20},		// 0x63 c
	{0x38, 0x44, 0x44, 0x48, 0x7f},		// 0x64 d
	{0x38, 0x54, 0x54, 0x54, 0x18},		// 0x65 e
	{0x08, 0x7e, 0x09, 0x01, 0x02},		// 0x66 f
	{0x0c, 0x52, 0x52, 0x52, 0x3e},		// 0x67 g
	{0x7f, 0x08, 0x04, 0x04, 0x78},		// 0x68 h
	{0x00, 0x04, 0x7d, 0x00, 0x00},		// 0x69 i
	{0x20, 0x40, 0x44, 0x3d, 0x00},		// 0x6a j
	{0x7f, 0x10, 0x28, 0x44, 0x00},		// 0x6b k
	{0x00, 0x41, 0x7f, 0x40, 0x00},		// 0x6c l
	{0x7c, 0x04, 0x18, 0x04, 0x7c},		// 0x6d m
	{0x7c, 0x08, 0x04, 0x04, 0x78},		// 0x6e n
	{0x38, 0x44, 0x44, 0x44, 0x38},		// 0x6f o
	{0x7c, 0x14, 0x14, 0x14, 0x08},		// 0x70 p
	{0x08, 0x14, 0x14, 0x18, 0x7c},		// 0x71 q
	{0x7c, 0x08, 0x04, 0x04, 0x08},		// 0x72 r
	{0x48, 0x54, 0x54, 0x54, 0x20},		// 0x73 s
	{0x04, 0x3f, 0x44, 0x40, 0x20},		// 0x74 t
	{0x3c, 0x40, 0x40, 0x20, 0x7c},		// 0x75 u
	{0x1c, 0x20, 0x40, 0x20, 0x1c},		// 0x76 v
	{0x3c, 0x40, 0x30, 0x40, 0x3c},		// 0x77 w
	{0x44, 0x28, 0x10, 0x28, 0x44},		// 0x78 x
	{0x0c, 0x50, 0x50, 0x50, 0x3c},		// 0x79 y
	{0x44, 0x64, 0x54, 0x4c, 0x44},		// 0x7a z
	{0x00, 0x08, 0x36, 0x41, 0x00},		// 0x7b {
	{0x00, 0x00, 0x77, 0x00, 0x00},		// 0x7c |
	{0x00, 0x41, 0x36, 0x08, 0x00},		// 0x7d }
	{0x08, 0x04, 0x08, 0x10, 0x08}};	// 0x7e ~
		
	#define delaytime 30
			
	unsigned char key_scan(unsigned char a);
	int key_convert1(unsigned char in);
	void led(int a, int b, int c);
	
	void set_color(unsigned char red, unsigned char green, unsigned char blue){
		OCR1_R=red;
		OCR1_G= green;
		OCR1_B=blue;
	}

	void timer_init(){
		//fast PWM 8bit
		//WGM13(0), WGM12(1), WGM11(0), WGM10(1)
		//COM1A1/COM1B1/COM1C1: 1, COM1A0/COM1B0/COM1C0: 0
		TCCR1A |= _BV(WGM10) | _BV(COM1A1) |_BV(COM1B1) | _BV(COM1C1);
		TCCR1B |= _BV(WGM12) | _BV(CS11);
		set_color(0,0,0);
	}
	
	void GLCD_Command(unsigned char signal, unsigned char command);
	void GLCD_Clear(void);
	void GLCD_xy(unsigned char x, unsigned char y);
	
	void GLCD_Port_Init(void)
	{
	GLCD_DATABUS_DDR = 0xFF;
	GLCD_CONTROL_DDR = 0xFF;
	}
	
	void GLCD_Init()
	{
	GLCD_Command( GLCD_CS1_2, DISPON );
	GLCD_Command( GLCD_CS1_2, 0xC0 );	/* 디스플레이 시작라인	*/
	GLCD_Command( GLCD_CS1_2, 0xB8 );	/* X 어드레스 셋 = 0	*/
	GLCD_Command( GLCD_CS1_2, 0x40 );	/* Y 어드레스 셋 = 0	*/
	GLCD_Clear();
	}
	
	void GLCD_Command(unsigned char signal, unsigned char command)
	{
	GLCD_CONTROL = signal & 0x18;
	GLCD_CONTROL |= 0x04;
	GLCD_DATABUS = command;
	_delay_us(10);
	GLCD_CONTROL &= ~0x04;
	_delay_us(10);
	GLCD_CONTROL=0x00;
	_delay_ms(1);
	}
	
	void GLCD_Data(unsigned char signal, unsigned char character)
	{
	GLCD_CONTROL = (signal & 0x18) | 0x01;
	GLCD_CONTROL |= 0x04;
	GLCD_DATABUS = character;
	_delay_us(10);
	GLCD_CONTROL &= ~0x04;
	_delay_us(10);
	GLCD_CONTROL=0x00;
	_delay_ms(1);
	}
	
	/* clear GLCD screen */
	void GLCD_Clear(void)
	{
	unsigned char i, j, x;
	
	GLCD_CONTROL = 0x00;			// clear all control signals
	GLCD_Command(GLCD_CS1_2, DISPON);		// clear all control signals
	GLCD_Command(GLCD_CS1_2, 0xC0);		// CS1,CS2 display position
	
	x = GLCD_SET_X_ADDR;
	for(i = 0; i <= 7; i++)
	{
	GLCD_Command(GLCD_CS1_2, x);		// X start addtess
	GLCD_Command(GLCD_CS1_2, GLCD_SET_Y_ADDR);	// Y start address
	for(j = 0; j <= 63; j++)
	{
	GLCD_Data(GLCD_CS1_2, 0x00);	// clear CS1 and CS2 screen
	}
	
	x++;
	}
	}
	
	void GLCD_xy(unsigned char x, unsigned char y)		/* set character position */
	{
	xcharacter = x;
	ycharacter = y;
	
	GLCD_Command(GLCD_CS1_2, 0xB8+xcharacter);	// X address
	
	if(ycharacter <= 9)				// if y <= 9, CS1 Y address
	GLCD_Command(GLCD_CS1, 0x40+ycharacter*6+4);
	else					// if y >= 10, CS2 Y address
	GLCD_Command(GLCD_CS2, 0x40+(ycharacter-10)*6);
	}
	
	void GLCD_Character(unsigned char character)		/* display a character */
	{
	unsigned char i, signal; //, font_data;
	
	GLCD_xy(xcharacter,ycharacter);
	
	if(ycharacter <= 9)				// if y <= 9, CS1
	signal = GLCD_CS1;
	else					// if y >= 10, CS2
	signal = GLCD_CS2;
	
	for(i = 0; i <= 4; i++)
	{
	if((cursor_flag == 1) && (xcharacter == xcursor) && (ycharacter == ycursor))
	GLCD_Data(signal,(font[character - 0x20][i]) | 0x80);
	else
	GLCD_Data(signal,font[character - 0x20][i]);
	}
	GLCD_Data(signal,0x00);		  	// last byte 0x00
	
	ycharacter++;                                 	// go next character position
	if(ycharacter == 20)
	{
	ycharacter = 0;
	xcharacter++;
	}
	}
	
	/* display a string */
	void GLCD_String(unsigned char x, unsigned char y, unsigned char *string){
	xcharacter = x;
	ycharacter = y;
	
	while(*string != '\0')
	{
	GLCD_Character(*string);	// display a charcater
	string++;
	}
	}
	
	// 점을 찍을 좌표를 잡습니다.
	void GLCD_Axis_xy(unsigned char x, unsigned char y)
	{
	GLCD_Command(GLCD_CS1_2, GLCD_SET_X_ADDR + x); // X address
	if(y <= 63)
	{
	GLCD_Command(GLCD_CS1, GLCD_SET_Y_ADDR + y); // CS1 Y address
	}
	else
	{
	GLCD_Command(GLCD_CS2, GLCD_SET_Y_ADDR + y - 64); // CS2 Y address
	}
	}
	
	
	// draw a dot on GLCD
	// 점을 그립니다.
	void GLCD_Dot(unsigned char xx,unsigned char y)
	{
	unsigned char x, i;
	
	// 해상도 범위(128.64) 인지 검사합니다.
	if((xx > 63) || (y > 127)) return;
	
	x = xx / 8;				  // calculate x address
	i = xx % 8;
	if(i == 0)      { i = 0x01; }
	else if(i == 1) { i = 0x02; }
	else if(i == 2) { i = 0x04; }
	else if(i == 3) { i = 0x08; }
	else if(i == 4) { i = 0x10; }
	else if(i == 5) { i = 0x20; }
	else if(i == 6) { i = 0x40; }
	else            { i = 0x80; }
	
	ScreenBuffer[x][y] |= i;			// OR old data with new data
	
	GLCD_Axis_xy(x, y);				      // draw dot on GLCD screen
	if(y <= 63) { GLCD_Data(GLCD_CS1, ScreenBuffer[x][y]);  }
	else        { GLCD_Data(GLCD_CS2, ScreenBuffer[x][y]);  }
	}
	
	
	
	// draw a straight line
	// 직선을 그립니다.
	void GLCD_Line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
	{
	int x, y;
	
	if(y1 != y2)
	{
	if(y1 < y2)
	{
	for(y = y1; y <= y2; y++)
	{
	x = x1 + (y - y1)*(x2 - x1)/(y2 - y1);
	GLCD_Dot(x,y);
	}
	}
	else
	{
	for(y = y1; y >= y2; y--)
	{
	x = x1 + (y - y1)*(x2 - x1)/(y2 - y1);
	GLCD_Dot(x,y);
	}
	}
	}
	else if(x1 != x2)	  // if x1 != x2, x is variable
	{
	if(x1 < x2)
	{
	for(x = x1; x <= x2; x++)
	{ y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);
	GLCD_Dot(x,y);
	}
	}
	else
	{
	for(x = x1; x >= x2; x--)
	{
	y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);
	GLCD_Dot(x,y);
	}
	}
	}
	else                  // if x1 == x2 and y1 == y2,
	{
	GLCD_Dot(x1,y1);		//             it is a dot
	}
	}
	
	
	// draw a rectangle
	// 직사각형을 그립니다.
	void GLCD_Rectangle(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
	{
	GLCD_Line(x1,y1,x1,y2);				// horizontal line
	GLCD_Line(x2,y1,x2,y2);
	GLCD_Line(x1,y1,x2,y1);				// vertical line
	GLCD_Line(x1,y2,x2,y2);
	}
	
	
	// draw a circle
	// 원을 그립니다.
	void GLCD_Circle(unsigned char x1,unsigned char y1,unsigned char r)
	{
	int x, y;
	float s;
	
	for(y = y1 - r*3/4; y <= y1 + r*3/4; y++)	// draw with y variable
	{
	s = sqrt(r*r - (y-y1)*(y-y1)) + 0.5;
	x = x1 + (unsigned char)s;
	GLCD_Dot(x,y);
	x = x1 - (unsigned char)s;
	GLCD_Dot(x,y);
	}
	
	for(x = x1 - r*3/4; x <= x1 + r*3/4; x++)	// draw with x variable
	{
	s = sqrt(r*r - (x-x1)*(x-x1)) + 0.5;
	y = y1 + (unsigned char)s;
	GLCD_Dot(x,y);
	y = y1 - (unsigned char)s;
	GLCD_Dot(x,y);
	}
	}
	
	void GLCD_Draw_Clear(){
		for(int j=0; j<128; j++)
		for(int k=0; k<8; k++)
		ScreenBuffer[k][j] = 0;
	}
	
	
	int main(void)
	{
	int i=0;
	unsigned int distance;
	unsigned char in_a,in0,in1,in2,in3;
	unsigned int x1=0,y1=0,x2=7,y2=7;
	_delay_ms(50); // wait for system stabilization
	GLCD_Port_Init(); // initialize MCU and kit
	GLCD_Init();
	
	// 포트 B의 TRIG 포트(2번핀)은 출력, ECHO 포트(3번핀)는 입력으로 설정
	SONIC_DDR |= (TRIG_HIGH & ECHO_LOW);

	
	DDRB= _BV(5) | _BV(6) | _BV(7);
	timer_init();
		
	while(1)
	{
		
		SONIC_PORT &= ~TRIG_HIGH; // Trig = LOW
		_delay_us(10);
		SONIC_PORT |= TRIG_HIGH; // Trig = HIGH
		_delay_us(10);
		SONIC_PORT &= ~TRIG_HIGH; // Trig = LOW
		
		
		// 2. ECHO 입력으로 거리 계산
		TCCR3B = 0x02; // Timer/Counter 1 클럭 0.5us
				
		while(!(SONIC_PIN & ECHO_HIGH)); // wait until Echo = HIGH;
				TCNT3 = 0x0000;  // Timer/Counter 1 값 동작 시작

		while(SONIC_PIN & ECHO_HIGH); // wait until Echo = LOW;
				TCCR3B = 0x00;  // Timer/Counter 1 값 동작 정지

		// TCNT1에는 Echo 신호가 HIGH로 유지된 카운트 수 저장
		distance = (unsigned int) (TCNT3 / 2 / 58);
		GLCD_Rectangle(x1, y1, x2, y2);
			_delay_ms(1000);
			GLCD_Draw_Clear();
			GLCD_Clear();
		
		// 키패드 첫번째줄 스캔
		in0 = key_scan(0x01);
		// 키패드 두번째줄 스캔
		in1 = key_scan(0x02);
		// 키패드 세번째줄 스캔
		in2 = key_scan(0x04);
		// 키패드 네번째줄 스캔
		in3 = key_scan(0x08);
		
		in_a = (in3 | in2 | in1 | in0); //4줄 입력 값 취합
		
		in_a = key_convert1(in_a);   //키 입력 값을 숫자로 변환
		
		
	switch(in_a){
		case 1:
			x1 -= 8;
			x2 -= 8;
			 y1 -= 8;
			 y2 -= 8;
			 GLCD_Rectangle(x1, y1, x2, y2);
			 led(redV,blueV,greenV);
			 _delay_ms(1000);
			 GLCD_Draw_Clear();
			 GLCD_Clear();
		break;
		
		case 2:
		 if(y1>0){
			 y1 -= 8;
			 y2 -= 8;
			 GLCD_Rectangle(x1, y1, x2, y2);
			 led(redV,blueV,greenV);
			 _delay_ms(1000);
			 GLCD_Draw_Clear();
			 GLCD_Clear();
		 }
		break;
		
		case 3:
			x1 += 8;
			x2 += 8;
			y1 -= 8;
			y2 -= 8;
			GLCD_Rectangle(x1, y1, x2, y2);
			led(redV,blueV,greenV);
			_delay_ms(1000);
			GLCD_Draw_Clear();
			GLCD_Clear();
		break;
		
		case 4:
				 if(x1>0){
					 x1 -= 8;
					 x2 -= 8;
					 GLCD_Rectangle(x1, y1, x2, y2);
					 led(redV,blueV,greenV);
					 _delay_ms(1000);
					 GLCD_Draw_Clear();
					 GLCD_Clear();
				 }
		break;
		
		case 5:
		
		break;
	
		case 6:
				x1 += 8;
				x2 += 8;
				GLCD_Rectangle(x1, y1, x2, y2);
				led(redV,blueV,greenV); 	
				_delay_ms(1000);
				GLCD_Draw_Clear();
				GLCD_Clear();
		break;
		
		case 7:
			x1 -= 8;
			x2 -= 8;
			y1 += 8;
			y2 += 8;
			GLCD_Rectangle(x1, y1, x2, y2);
			led(redV,blueV,greenV);
			_delay_ms(1000);
			GLCD_Draw_Clear();
			GLCD_Clear();
		break;			
	
		case 8:
				y1 += 8;
				y2 += 8;
				GLCD_Rectangle(x1, y1, x2, y2);
				led(redV,blueV,greenV);
				_delay_ms(1000);
				GLCD_Draw_Clear();
				GLCD_Clear();
		break;
		
		case 9:
			x1 += 8;
			x2 += 8;
			y1 += 8;
			y2 += 8;
			GLCD_Rectangle(x1, y1, x2, y2);
			led(redV,blueV,greenV);
			_delay_ms(1000);
			GLCD_Draw_Clear();
			GLCD_Clear();
		break;
		
		default : break;
	}
				
	if(distance <= 18){
			x1= rand() %60 +1;
			y1= rand() %60 +1;
			x2= rand() %60 +1;
			y2= rand() %60 +1;
			GLCD_Rectangle(x1, y1, x2, y2);
			led(redV,blueV,greenV);
			_delay_ms(1000);
			GLCD_Draw_Clear();
			GLCD_Clear();
			}
	}
	}
	
		unsigned char key_scan(unsigned char a)
		{
			unsigned char in;

			KEY_DDR = 0xFF;    //all out
			KEY_PORT = a;      //키패드 줄 위치
			KEY_DDR = 0x0F;    //Port 7bit~4bit(In) 3bit~0bit(Out)
			_delay_us(50);

			// 키패드 입력 포트의 상위 4자리 값 추출
			in = (PINF & 0xF0);
			if(in != 0x00)
			{
				in |= a;
				return in;
			}
			return 0;
		}

		int key_convert1(unsigned char in)
		{
			int returnvalue;

			switch(in)
			{
				case 0x11 :
				returnvalue = 1;
				break;

				case 0x21 :
				returnvalue = 2;
				break;

				case 0x41 :
				returnvalue = 3;
				break;
				/////////////////////////////////
				case 0x12 :
				returnvalue = 4;
				break;

				case 0x22 :
				returnvalue = 5;
				break;

				case 0x42 :
				returnvalue = 6;
				break;
				////////////////////////////////////////
				case 0x14 :
				returnvalue = 7;
				break;

				case 0x24 :
				returnvalue = 8;
				break;

				case 0x44 :
				returnvalue = 9;
				break;
							
				default : returnvalue = 0;  break;
			}
			return returnvalue;
		}
		
		void led(int a, int b, int c){
				a=rand() %256;
				b=rand() %256;
				c=rand() %256;
				set_color(a,b,c);
		}