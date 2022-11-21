	////가위바위보
	////A가 키패드 스위치 0x11(주먹), 0x21(보), 0x41(가위)중 하나를 택하여 선택하면 GLCD에 A가 선택한것 출력
	////B가 키패드 스위치 0x12(주먹), 0x22(보), 0x42(가위)중 하나를 택하여 선택하면 GLCD에 B가 선택한것 출력
	////A와 B가 선택한것에 따라 GLCD에 무승부면 DRAW, A가 승리하면 A-WIN, B가 승리하면 B-WIN출력
	////결과를 LED를 통해서도 출력- A승리시 빨간색, B승리시 파란색, 무승부일 경우 초록색을 출력

	#include <avr/io.h>
	#define F_CPU 16000000
	#include <util/delay.h>

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
	#define DISPOFF 		0x3E

	// 키패드 명령어 셋팅
	#define KEY_DDR DDRF
	#define KEY_PORT PORTF
	#define KEY_PIN PINF

	//LED 명령어 셋팅
	#define  OCR1_R OCR1A
	#define  OCR1_G OCR1B
	#define  OCR1_B OCR1C


		#define GLCD_CS1 0x08
		#define GLCD_CS2 0x10
		#define GLCD_CS1_2 0x18

		#define GLCD_SET_X_ADDR 0xB8
		#define GLCD_SET_Y_ADDR 0x40
		
		
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
		int key_convert2(unsigned char in);
		int result(int a, int b);
		
		unsigned char redV;             //빨간 LED아날로그 값 설정 (0~255)
		unsigned char greenV;			//초록 LED아날로그 값 설정 (0~255)
		unsigned char blueV;
		
		void GLCD_Command(unsigned char signal, unsigned char command);
		void GLCD_Clear(void);
		void GLCD_xy(unsigned char x, unsigned char y);


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
		
			//display floating-point number xx.x
			//00.0로 소수점 표현
			void GLCD_2Dot1Float(float number){
			unsigned int i,j;
			
			j=(int)(number *10);
			i=j/100;
			if(i==9){GLCD_Character(' ');}
			else{GLCD_Character(i+'0');}
			
			j=j%100;
			i=j/10;
			GLCD_Character(i+'0');
			GLCD_Character('.');
			
			i=j%10;
			GLCD_Character(i+'0');
			}
			
			//display 1-digit decimal number
			//1자리의 10진수 값을 표시
			unsigned char GLCD_1DigitDecimal(unsigned char number,unsigned char flag){
			number %= 10;
			
			if((number==0)&&(flag==0)){
			GLCD_Character(' ');
			return 0;
			}
			
			GLCD_Character(number + '0');
			return 1;
			}
			
			unsigned char GLCD_2DigitDecimal(unsigned char number){
			unsigned int i;
			unsigned char flag;
			
			flag=0;
			number=number %100;
			i=number/10;
			flag=GLCD_1DigitDecimal(i,flag);
			
			i=number %10;
			GLCD_Character((i+'0'));
			}
			
			// display 3-digit decimal number
			// 3자리의 10진수 값을 표시합니다.
			void GLCD_3DigitDecimal(unsigned int number)
			{
			unsigned int i;
			unsigned char flag;
			flag = 0;
			number = number % 1000;
			i = number/100;
			flag = GLCD_1DigitDecimal(i, flag); // 10^2
			number = number % 100;
			i = number/10;
			flag = GLCD_1DigitDecimal(i, flag); // 10^1
			i = number % 10;
			GLCD_Character(i + '0'); // 10^0
			}
			
			// display 4-digit decimal number
			// 4자리의 10진수 값을 표시합니다.
			void GLCD_4DigitDecimal(unsigned int number)
			{
			unsigned int i;
			unsigned char flag;
			flag = 0;
			number = number % 10000;
			i = number/1000;
			flag = GLCD_1DigitDecimal(i, flag); // 10^3
			number = number % 1000;
			i = number/100;
			flag = GLCD_1DigitDecimal(i, flag); // 10^2
			number = number % 100;
			i = number/10;
			flag = GLCD_1DigitDecimal(i, flag); // 10^1
			i = number % 10;
			GLCD_Character(i + '0'); // 10^0
			}
			
			// display 1-digit hex number
			// 1자리의 16진수 값을 표시합니다.
			void GLCD_1DigitHex(unsigned char i)
			{
			i &= 0x0F; // 16^0
			if(i <= 9) { GLCD_Character(i + '0'); }
			else { GLCD_Character(i - 10 + 'A'); }
			}
			// display 2-digit hex number
			// 2자리의 16진수 값을 표시합니다.
			void GLCD_2DigitHex(unsigned char number)
			{
			GLCD_1DigitHex(number >> 4); // 16^1
			GLCD_1DigitHex(number ); // 16^0
			}			
			// display 4-digit hex number
			// 4자리의 16진수 값을 표시합니다.
			void GLCD_4DigitHex(unsigned int number)
			{
			GLCD_1DigitHex(number >> 12); // 16^3
			GLCD_1DigitHex(number >> 8 ); // 16^2
			GLCD_1DigitHex(number >> 4 ); // 16^1
			GLCD_1DigitHex(number ); // 16^0
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
		
		//void GLCD_Draw_Clear(){
			//for(int j=0; j<128; j++)
			//for(int k=0; k<8; k++)
			//ScreenBuffer[k][j] = 0;
		//}
		// 드로잉 버퍼 지우기 함수의 수정 버전

		void GLCD_Draw_Clear(){

			// 1단계: 드로잉 버퍼에 남아 있는 이전 그림 데이터 화면 지우기 (LCD 내부의 데이터 지우기)

			for(int x=0; x<8; x++){

				for(int y=0; y<128; y++){

					// 드로잉 버퍼에 이전 그림이 남아 있을 때만 해당 부분 화면 지우기

					if(ScreenBuffer[x][y] != 0){

						GLCD_Command(GLCD_CS1_2, GLCD_SET_X_ADDR+x);

						GLCD_Command(GLCD_CS1_2, GLCD_SET_Y_ADDR+y);

						if(y <= 63) { GLCD_Data(GLCD_CS1, 0x00);  }

						else        { GLCD_Data(GLCD_CS2, 0x00);  }

					}

				}

			}

			// 2단계: 드로잉 버퍼 0으로 리셋하기 1024=128*8

			memset(ScreenBuffer, 0, 1024);



		}
		int main(void)
		{
			unsigned char in_a,in_b,in0,in1,in2,in3;
			unsigned int a,b,res,i;
			//int i=10,j=10,k=10;
			int flag=0;
			
			DDRB= _BV(5) | _BV(6) | _BV(7);
			timer_init();
			
			_delay_ms(50);                                 // wait for system stabilization
			GLCD_Port_Init();                             // initialize MCU and kit
			GLCD_Init();

			while(1)
			{
				// A가 낼것 정하기
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
				
				switch(in_a)
				{
					case 1:       //바위
					GLCD_String(1,0,"==========A=========");
					GLCD_String(3,0,"  Rock ");
					GLCD_Circle(30,70,10);
					GLCD_String(5,0,"winning%");
					GLCD_xy(6,0);
					GLCD_2Dot1Float(33.3);
					_delay_ms(1000);
					GLCD_Draw_Clear();
					GLCD_Clear();
					a=1;
					break;

					case 2 :         //보
					GLCD_String(1,0,"==========A=========");
					GLCD_String(3,0,"  paper ");
					GLCD_Circle(30,70,10);
					GLCD_Line(20,70,15,80);
					GLCD_Line(23,80,20,85);
					GLCD_Line(30,80,30,90);
					GLCD_Line(34,80,40,90);
					GLCD_Line(40,70,45,80);
					GLCD_String(5,0,"winning%");
					GLCD_xy(6,0);
					GLCD_2Dot1Float(33.3);
					_delay_ms(1000);
					GLCD_Draw_Clear();
					GLCD_Clear();
					a=2;
					break;

					case 3 :          //가위
					GLCD_String(1,0,"==========A=========");
					GLCD_String(3,0,"   scissors ");
					GLCD_Rectangle(30,80,20,85);
					GLCD_Rectangle(30,80,50,100);
					GLCD_Rectangle(30,100,35,115);
					GLCD_String(5,0,"winning%");
					GLCD_xy(6,0);
					GLCD_2Dot1Float(33.3);
					_delay_ms(1000);
					GLCD_Draw_Clear();
					GLCD_Clear();
					a=3;
					break;
					
					
					default : break;
				}
				
				//B가 낼것 정하기
				in0 = key_scan(0x01);
				
				in1 = key_scan(0x02);
				
				in2 = key_scan(0x04);
				
				in3 = key_scan(0x08);
				
				in_b = (in3 | in2 | in1 | in0); //4줄 입력 값 취합
				
				in_b = key_convert2(in_b);   //키 입력 값을 숫자로 변환
				
				switch(in_b)
				{
					case 1:   //바위
					GLCD_String(1,0,"==========B=========");
					GLCD_String(3,0,"  Rock ");
					GLCD_String(5,0,"winning%");
					GLCD_xy(6,0);
					GLCD_2Dot1Float(33.3);
					GLCD_Circle(30,80,10);
					_delay_ms(1000);
					GLCD_Draw_Clear();
					GLCD_Clear();
					b=1;
					flag=1;
					break;

					case 2 :   //보
					GLCD_String(1,0,"==========B=========");
					GLCD_String(3,0,"  paper ");
					GLCD_Circle(30,80,10);
					GLCD_Line(20,75,17,70);
					GLCD_Line(26,70,22,65);
					GLCD_Line(30,70,30,60);
					GLCD_Line(34,70,38,65);
					GLCD_Line(40,75,45,70);
					GLCD_String(5,0,"winning%");
					GLCD_xy(6,0);
					GLCD_2Dot1Float(33.3);
					_delay_ms(1000);
					GLCD_Draw_Clear();
					GLCD_Clear();
					b=2;
					flag=1;
					break;

					case 3 :    //가위
					GLCD_String(1,0,"==========B=========");
					GLCD_String(3,0,"   scissors ");
					GLCD_Rectangle(30,120,20,115);
					GLCD_Rectangle(30,100,50,120);
					GLCD_Rectangle(30,100,35,85);
					GLCD_String(5,0,"winning%");
					GLCD_xy(6,0);
					GLCD_2Dot1Float(33.3);
					_delay_ms(1000);
					GLCD_Draw_Clear();
					GLCD_Clear();
					b=3;
					flag=1;
					break;
					
					default : break;
				}

				res=result(a,b);
				//승부 결과 출력
				if(flag==1){
						for(i=0; i<20; i++){
							GLCD_String(1,5,"Loding.....");
							GLCD_Rectangle(40, 40, 50, 50+i);
							_delay_ms(10);
							GLCD_Draw_Clear(); // 드로잉 버퍼(이전 그림) 지우기
						}
						GLCD_Clear();
					switch(res){
						//무승부
						case 0:
						GLCD_String(1,0,"=======result=======");
						GLCD_String(3,0,"  DRAW ");
						redV=0;
						blueV=0;
						greenV=255;
						set_color(redV,greenV,blueV);          //초록색 LED ON
						_delay_ms(5000);
						break;
						
						//A승리
						case 1:
						GLCD_String(1,0,"=======result=======");
						GLCD_String(3,0,"  A- WIN ");
						redV=255;
						blueV=0;
						greenV=0;
						set_color(redV,greenV,blueV);		//빨간색 LED ON
						_delay_ms(5000);
						break;
						
						//B승리
						case 2:
						GLCD_String(1,0,"=======result=======");
						GLCD_String(3,0,"  B- WIN ");
						redV=0;
						blueV=255;
						greenV=0;
						set_color(redV,greenV,blueV);				//파란색 LED ON
						_delay_ms(5000);
						break;
						
						default : break;
					}
				}
				flag=0;
				set_color(0,0,0);					//LED OFF
				GLCD_Clear();
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
				
				default : returnvalue = 0;  break;
			}
			return returnvalue;
		}


		int key_convert2(unsigned char in)
		{
			int returnvalue;

			switch(in)
			{
				case 0x12 :
				returnvalue = 1;
				break;

				case 0x22 :
				returnvalue = 2;
				break;

				case 0x42 :
				returnvalue = 3;
				break;
				
				default : returnvalue = 0;  break;
			}
			return returnvalue;
		}

		int result(int a, int b){
			
			if(a == b){
				return 0;
			}

			else if(a==1 && b== 2){
				return 2;
			}
			
			else if(a==1 && b==3){
				return 1;
			}
			
			else if(a==2 && b== 3){
				return 2;
			}
			
			else if(a==2 && b==1){
				return 1;
			}

			else if(a==3 && b== 1){
				return 2;
			}
			
			else if(a==3 && b==2){
				return 1;
			}
		}
	
	
	
	
	////예제
	//#include <avr/io.h>
	//#define F_CPU 16000000
	//#include <util/delay.h>
	//
	//#define GLCD_DATABUS	PORTD // GLCD data
	//#define GLCD_CONTROL	PORTC // GLCD control signal
	//#define GLCD_DATABUS_DDR DDRD
	//#define GLCD_CONTROL_DDR DDRC
	//
	//// Graphic LCD 명령어 셋팅
	//#define GLCD_CS1 0x08		 // GLCD_CS1 Select 0000 1000
	//#define GLCD_CS2 0x10		 // GLCD_CS2 Select 0001 0000
	//#define GLCD_CS1_2 0x18		 // GLCD_CS1, GLCD_CS2 Select 0001 1000
	//
	//#define GLCD_START_LINE		0xC0	// 11XXXXXX: set lcd start line
	//#define GLCD_SET_X_ADDR		0xB8	// 10111XXX: set lcd X address
	//#define GLCD_SET_Y_ADDR		0x40	// 01YYYYYY: set lcd Y address
	//
	//#define DISPON			0x3F
	//#define DISPOFF 			0x3E
	//
	//unsigned char xcharacter, ycharacter;		// x character(0-7), y character(0-19)
	//unsigned char cursor_flag, xcursor, ycursor;	// x and y cursor position(0-7, 0-19)
	//
	//unsigned char font[95][5] = {	            /* 5x7 ASCII character font */
	//{0x00, 0x00, 0x00, 0x00, 0x00},		// 0x20 space
	//{0x00, 0x00, 0x4f, 0x00, 0x00},		// 0x21 !
	//{0x00, 0x07, 0x00, 0x07, 0x00},		// 0x22 "
	//{0x14, 0x7f, 0x14, 0x7f, 0x14},		// 0x23 #
	//{0x24, 0x2a, 0x7f, 0x2a, 0x12},		// 0x24 $
	//{0x23, 0x13, 0x08, 0x64, 0x62},		// 0x25 %
	//{0x36, 0x49, 0x55, 0x22, 0x50},		// 0x26 &
	//{0x00, 0x05, 0x03, 0x00, 0x00},		// 0x27 '
	//{0x00, 0x1c, 0x22, 0x41, 0x00},		// 0x28 (
	//{0x00, 0x41, 0x22, 0x1c, 0x00},		// 0x29 )
	//{0x14, 0x08, 0x3e, 0x08, 0x14},		// 0x2a *
	//{0x08, 0x08, 0x3e, 0x08, 0x08},		// 0x2b +
	//{0x00, 0x50, 0x30, 0x00, 0x00},		// 0x2c ,
	//{0x08, 0x08, 0x08, 0x08, 0x08},		// 0x2d -
	//{0x00, 0x60, 0x60, 0x00, 0x00},		// 0x2e .
	//{0x20, 0x10, 0x08, 0x04, 0x02},		// 0x2f /
	//{0x3e, 0x51, 0x49, 0x45, 0x3e},		// 0x30 0
	//{0x00, 0x42, 0x7f, 0x40, 0x00},		// 0x31 1
	//{0x42, 0x61, 0x51, 0x49, 0x46},		// 0x32 2
	//{0x21, 0x41, 0x45, 0x4b, 0x31},		// 0x33 3
	//{0x18, 0x14, 0x12, 0x7f, 0x10},		// 0x34 4
	//{0x27, 0x45, 0x45, 0x45, 0x39},		// 0x35 5
	//{0x3c, 0x4a, 0x49, 0x49, 0x30},		// 0x36 6
	//{0x01, 0x71, 0x09, 0x05, 0x03},		// 0x37 7
	//{0x36, 0x49, 0x49, 0x49, 0x36},		// 0x38 8
	//{0x06, 0x49, 0x49, 0x29, 0x1e},		// 0x39 9
	//{0x00, 0x36, 0x36, 0x00, 0x00},		// 0x3a :
	//{0x00, 0x56, 0x36, 0x00, 0x00},		// 0x3b ;
	//{0x08, 0x14, 0x22, 0x41, 0x00},		// 0x3c <
	//{0x14, 0x14, 0x14, 0x14, 0x14},		// 0x3d =
	//{0x00, 0x41, 0x22, 0x14, 0x08},		// 0x3e >
	//{0x02, 0x01, 0x51, 0x09, 0x06},		// 0x3f ?
	//{0x32, 0x49, 0x79, 0x41, 0x3e},		// 0x40 @
	//{0x7e, 0x11, 0x11, 0x11, 0x7e},		// 0x41 A
	//{0x7f, 0x49, 0x49, 0x49, 0x36},		// 0x42 B
	//{0x3e, 0x41, 0x41, 0x41, 0x22},		// 0x43 C
	//{0x7f, 0x41, 0x41, 0x22, 0x1c},		// 0x44 D
	//{0x7f, 0x49, 0x49, 0x49, 0x41},		// 0x45 E
	//{0x7f, 0x09, 0x09, 0x09, 0x01},		// 0x46 F
	//{0x3e, 0x41, 0x49, 0x49, 0x7a},		// 0x47 G
	//{0x7f, 0x08, 0x08, 0x08, 0x7f},		// 0x48 H
	//{0x00, 0x41, 0x7f, 0x41, 0x00},		// 0x49 I
	//{0x20, 0x40, 0x41, 0x3f, 0x01},		// 0x4a J
	//{0x7f, 0x08, 0x14, 0x22, 0x41},		// 0x4b K
	//{0x7f, 0x40, 0x40, 0x40, 0x40},		// 0x4c L
	//{0x7f, 0x02, 0x0c, 0x02, 0x7f},		// 0x4d M
	//{0x7f, 0x04, 0x08, 0x10, 0x7f},		// 0x4e N
	//{0x3e, 0x41, 0x41, 0x41, 0x3e},		// 0x4f O
	//{0x7f, 0x09, 0x09, 0x09, 0x06},		// 0x50 P
	//{0x3e, 0x41, 0x51, 0x21, 0x5e},		// 0x51 Q
	//{0x7f, 0x09, 0x19, 0x29, 0x46},		// 0x52 R
	//{0x26, 0x49, 0x49, 0x49, 0x32},		// 0x53 S
	//{0x01, 0x01, 0x7f, 0x01, 0x01},		// 0x54 T
	//{0x3f, 0x40, 0x40, 0x40, 0x3f},		// 0x55 U
	//{0x1f, 0x20, 0x40, 0x20, 0x1f},		// 0x56 V
	//{0x3f, 0x40, 0x38, 0x40, 0x3f},		// 0x57 W
	//{0x63, 0x14, 0x08, 0x14, 0x63},		// 0x58 X
	//{0x07, 0x08, 0x70, 0x08, 0x07},		// 0x59 Y
	//{0x61, 0x51, 0x49, 0x45, 0x43},		// 0x5a Z
	//{0x00, 0x7f, 0x41, 0x41, 0x00},		// 0x5b [
	//{0x02, 0x04, 0x08, 0x10, 0x20},		// 0x5c (\)
	//{0x00, 0x41, 0x41, 0x7f, 0x00},		// 0x5d ]
	//{0x04, 0x02, 0x01, 0x02, 0x04},		// 0x5e ^
	//{0x40, 0x40, 0x40, 0x40, 0x40},		// 0x5f _
	//{0x00, 0x01, 0x02, 0x04, 0x00},		// 0x60 `
	//{0x20, 0x54, 0x54, 0x54, 0x78},		// 0x61 a
	//{0x7f, 0x48, 0x44, 0x44, 0x38},		// 0x62 b
	//{0x38, 0x44, 0x44, 0x44, 0x20},		// 0x63 c
	//{0x38, 0x44, 0x44, 0x48, 0x7f},		// 0x64 d
	//{0x38, 0x54, 0x54, 0x54, 0x18},		// 0x65 e
	//{0x08, 0x7e, 0x09, 0x01, 0x02},		// 0x66 f
	//{0x0c, 0x52, 0x52, 0x52, 0x3e},		// 0x67 g
	//{0x7f, 0x08, 0x04, 0x04, 0x78},		// 0x68 h
	//{0x00, 0x04, 0x7d, 0x00, 0x00},		// 0x69 i
	//{0x20, 0x40, 0x44, 0x3d, 0x00},		// 0x6a j
	//{0x7f, 0x10, 0x28, 0x44, 0x00},		// 0x6b k
	//{0x00, 0x41, 0x7f, 0x40, 0x00},		// 0x6c l
	//{0x7c, 0x04, 0x18, 0x04, 0x7c},		// 0x6d m
	//{0x7c, 0x08, 0x04, 0x04, 0x78},		// 0x6e n
	//{0x38, 0x44, 0x44, 0x44, 0x38},		// 0x6f o
	//{0x7c, 0x14, 0x14, 0x14, 0x08},		// 0x70 p
	//{0x08, 0x14, 0x14, 0x18, 0x7c},		// 0x71 q
	//{0x7c, 0x08, 0x04, 0x04, 0x08},		// 0x72 r
	//{0x48, 0x54, 0x54, 0x54, 0x20},		// 0x73 s
	//{0x04, 0x3f, 0x44, 0x40, 0x20},		// 0x74 t
	//{0x3c, 0x40, 0x40, 0x20, 0x7c},		// 0x75 u
	//{0x1c, 0x20, 0x40, 0x20, 0x1c},		// 0x76 v
	//{0x3c, 0x40, 0x30, 0x40, 0x3c},		// 0x77 w
	//{0x44, 0x28, 0x10, 0x28, 0x44},		// 0x78 x
	//{0x0c, 0x50, 0x50, 0x50, 0x3c},		// 0x79 y
	//{0x44, 0x64, 0x54, 0x4c, 0x44},		// 0x7a z
	//{0x00, 0x08, 0x36, 0x41, 0x00},		// 0x7b {
	//{0x00, 0x00, 0x77, 0x00, 0x00},		// 0x7c |
	//{0x00, 0x41, 0x36, 0x08, 0x00},		// 0x7d }
	//{0x08, 0x04, 0x08, 0x10, 0x08}};	// 0x7e ~
	//void GLCD_Command(unsigned char signal, unsigned char command);
	//void GLCD_Clear(void);
	//void GLCD_xy(unsigned char x, unsigned char y);
	//
	//void GLCD_Port_Init(void)
	//{
	//GLCD_DATABUS_DDR = 0xFF;
	//GLCD_CONTROL_DDR = 0xFF;
	//}
	//
	//void GLCD_Init()
	//{
	//GLCD_Command( GLCD_CS1_2, DISPON );
	//GLCD_Command( GLCD_CS1_2, 0xC0 );	/* 디스플레이 시작라인	*/
	//GLCD_Command( GLCD_CS1_2, 0xB8 );	/* X 어드레스 셋 = 0	*/
	//GLCD_Command( GLCD_CS1_2, 0x40 );	/* Y 어드레스 셋 = 0	*/
	//GLCD_Clear();
	//}
	//
	//void GLCD_Command(unsigned char signal, unsigned char command)
	//{
	//GLCD_CONTROL = signal & 0x18;
	//GLCD_CONTROL |= 0x04;
	//GLCD_DATABUS = command;
	//_delay_us(10);
	//GLCD_CONTROL &= ~0x04;
	//_delay_us(10);
	//GLCD_CONTROL=0x00;
	//_delay_ms(1);
	//}
	//
	//void GLCD_Data(unsigned char signal, unsigned char character)
	//{
	//GLCD_CONTROL = (signal & 0x18) | 0x01;
	//GLCD_CONTROL |= 0x04;
	//GLCD_DATABUS = character;
	//_delay_us(10);
	//GLCD_CONTROL &= ~0x04;
	//_delay_us(10);
	//GLCD_CONTROL=0x00;
	//_delay_ms(1);
	//}
	//
	///* clear GLCD screen */
	//void GLCD_Clear(void)
	//{
	//unsigned char i, j, x;
	//
	//GLCD_CONTROL = 0x00;			// clear all control signals
	//GLCD_Command(GLCD_CS1_2, DISPON);		// clear all control signals
	//GLCD_Command(GLCD_CS1_2, 0xC0);		// CS1,CS2 display position
	//
	//x = GLCD_SET_X_ADDR;
	//for(i = 0; i <= 7; i++)
	//{
	//GLCD_Command(GLCD_CS1_2, x);		// X start addtess
	//GLCD_Command(GLCD_CS1_2, GLCD_SET_Y_ADDR);	// Y start address
	//for(j = 0; j <= 63; j++)
	//{
	//GLCD_Data(GLCD_CS1_2, 0x00);	// clear CS1 and CS2 screen
	//}
	//
	//x++;
	//}
	//}
	//
	//void GLCD_xy(unsigned char x, unsigned char y)		/* set character position */
	//{
	//xcharacter = x;
	//ycharacter = y;
	//
	//GLCD_Command(GLCD_CS1_2, 0xB8+xcharacter);	// X address
	//
	//if(ycharacter <= 9)				// if y <= 9, CS1 Y address
	//GLCD_Command(GLCD_CS1, 0x40+ycharacter*6+4);
	//else					// if y >= 10, CS2 Y address
	//GLCD_Command(GLCD_CS2, 0x40+(ycharacter-10)*6);
	//}
	//
	//void GLCD_Character(unsigned char character)		/* display a character */
	//{
	//unsigned char i, signal; //, font_data;
	//
	//GLCD_xy(xcharacter,ycharacter);
	//
	//if(ycharacter <= 9)				// if y <= 9, CS1
	//signal = GLCD_CS1;
	//else					// if y >= 10, CS2
	//signal = GLCD_CS2;
	//
	//for(i = 0; i <= 4; i++)
	//{
	//if((cursor_flag == 1) && (xcharacter == xcursor) && (ycharacter == ycursor))
	//GLCD_Data(signal,(font[character - 0x20][i]) | 0x80);
	//else
	//GLCD_Data(signal,font[character - 0x20][i]);
	//}
	//GLCD_Data(signal,0x00);		  	// last byte 0x00
	//
	//ycharacter++;                                 	// go next character position
	//if(ycharacter == 20)
	//{
	//ycharacter = 0;
	//xcharacter++;
	//}
	//}
	//
	///* display a string */
	//void GLCD_String(unsigned char x, unsigned char y, unsigned char *string){
	//xcharacter = x;
	//ycharacter = y;
	//
	//while(*string != '\0')
	//{
	//GLCD_Character(*string);	// display a charcater
	//string++;
	//}
	//}
	//
	////display floating-point number xx.x
	////00.0로 소수점 표현
	//void GLCD_2Dot1Float(float number){
		//unsigned int i,j;
		//
		//j=(int)(number *10);
		//i=j/100;
		//if(i==9){GLCD_Character(' ');}
			//else{GLCD_Character(i+'0');}
				//
		//j=j%100;
		//i=j/10;
		//GLCD_Character(i+'0');
		//GLCD_Character('.');
		//
		//i=j%10;
		//GLCD_Character(i+'0');
	//}
	//
	////display 1-digit decimal number
	////1자리의 10진수 값을 표시
	//unsigned char GLCD_1DigitDecimal(unsigned char number,unsigned char flag){
		//number %= 10;
		//
		//if((number==0)&&(flag==0)){
			//GLCD_Character(' ');
			//return 0;
		//}
		//
		//GLCD_Character(number + '0');
		//return 1;
	//}
	//
	//unsigned char GLCD_2DigitDecimal(unsigned char number){
			//unsigned int i;
			//unsigned char flag;
			//
			//flag=0;
			//number=number %100;
			//i=number/10;
			//flag=GLCD_1DigitDecimal(i,flag); 
			//
			//i=number %10;
			//GLCD_Character((i+'0'));
		//}
		//
		//// display 3-digit decimal number
		//// 3자리의 10진수 값을 표시합니다.
		//void GLCD_3DigitDecimal(unsigned int number)
		//{
			//unsigned int i;
			//unsigned char flag;
			//flag = 0;
			//number = number % 1000;
			//i = number/100;
			//flag = GLCD_1DigitDecimal(i, flag); // 10^2
			//number = number % 100;
			//i = number/10;
			//flag = GLCD_1DigitDecimal(i, flag); // 10^1
			//i = number % 10;
			//GLCD_Character(i + '0'); // 10^0
		//}
		//
		//// display 4-digit decimal number
		//// 4자리의 10진수 값을 표시합니다.
		//void GLCD_4DigitDecimal(unsigned int number)
		//{
			//unsigned int i;
			//unsigned char flag;
			//flag = 0;
			//number = number % 10000;
			//i = number/1000;
			//flag = GLCD_1DigitDecimal(i, flag); // 10^3
			//number = number % 1000;
			//i = number/100;
			//flag = GLCD_1DigitDecimal(i, flag); // 10^2
			//number = number % 100;
			//i = number/10;
			//flag = GLCD_1DigitDecimal(i, flag); // 10^1
			//i = number % 10;
			//GLCD_Character(i + '0'); // 10^0
		//}
	//
	//// display 1-digit hex number
	//// 1자리의 16진수 값을 표시합니다.
	//void GLCD_1DigitHex(unsigned char i)
	//{
		//i &= 0x0F; // 16^0
		//if(i <= 9) { GLCD_Character(i + '0'); }
		//else { GLCD_Character(i - 10 + 'A'); }
	//}
	//// display 2-digit hex number
	//// 2자리의 16진수 값을 표시합니다.
	//void GLCD_2DigitHex(unsigned char number)
	//{
		//GLCD_1DigitHex(number >> 4); // 16^1
		//GLCD_1DigitHex(number ); // 16^0
	//}//
	//// display 4-digit hex number
	//// 4자리의 16진수 값을 표시합니다.
	//void GLCD_4DigitHex(unsigned int number)
	//{
		//GLCD_1DigitHex(number >> 12); // 16^3
		//GLCD_1DigitHex(number >> 8 ); // 16^2
		//GLCD_1DigitHex(number >> 4 ); // 16^1
		//GLCD_1DigitHex(number ); // 16^0
	//}
	//
	//
//
//int main(void)
//{
	////unsigned char i;
	//
	//_delay_ms(50); // wait for system stabilization
	//GLCD_Port_Init(); // initialize MCU and kit
	//GLCD_Init();
	//while(1)
	//{
		//GLCD_xy(0,0);
		//GLCD_4DigitDecimal(1023);
		//GLCD_xy(1,0);
		//GLCD_4DigitHex(1023);
		//GLCD_xy(2,0);
		//GLCD_2Dot1Float(12.3);
	//}
//}
	//
	
	
	
	
	////예제
	//#include <avr/io.h>
	//#define F_CPU 16000000
	//#include <util/delay.h>
	//
	//#define GLCD_DATABUS	PORTD // GLCD data
	//#define GLCD_CONTROL	PORTC // GLCD control signal
	//#define GLCD_DATABUS_DDR DDRD
	//#define GLCD_CONTROL_DDR DDRC
	//
	//// Graphic LCD 명령어 셋팅
	//#define GLCD_CS1 0x08		 // GLCD_CS1 Select 0000 1000
	//#define GLCD_CS2 0x10		 // GLCD_CS2 Select 0001 0000
	//#define GLCD_CS1_2 0x18		 // GLCD_CS1, GLCD_CS2 Select 0001 1000
	//
	//#define GLCD_START_LINE		0xC0	// 11XXXXXX: set lcd start line
	//#define GLCD_SET_X_ADDR		0xB8	// 10111XXX: set lcd X address
	//#define GLCD_SET_Y_ADDR		0x40	// 01YYYYYY: set lcd Y address
	//
	//#define DISPON			0x3F
	//#define DISPOFF 			0x3E
//
	//#define GLCD_CS1 0x08
	//#define GLCD_CS2 0x10
	//#define GLCD_CS1_2 0x18
//
	//#define GLCD_SET_X_ADDR 0xB8
	//#define GLCD_SET_Y_ADDR 0x40
	//
	//unsigned char xcharacter, ycharacter;		// x character(0-7), y character(0-19)
	//unsigned char cursor_flag, xcursor, ycursor;	// x and y cursor position(0-7, 0-19)
	//
	//unsigned char ScreenBuffer[8][128]={0};           // screen buffer
	//
	//unsigned char font[95][5] = {	            /* 5x7 ASCII character font */
	//{0x00, 0x00, 0x00, 0x00, 0x00},		// 0x20 space
	//{0x00, 0x00, 0x4f, 0x00, 0x00},		// 0x21 !
	//{0x00, 0x07, 0x00, 0x07, 0x00},		// 0x22 "
	//{0x14, 0x7f, 0x14, 0x7f, 0x14},		// 0x23 #
	//{0x24, 0x2a, 0x7f, 0x2a, 0x12},		// 0x24 $
	//{0x23, 0x13, 0x08, 0x64, 0x62},		// 0x25 %
	//{0x36, 0x49, 0x55, 0x22, 0x50},		// 0x26 &
	//{0x00, 0x05, 0x03, 0x00, 0x00},		// 0x27 '
	//{0x00, 0x1c, 0x22, 0x41, 0x00},		// 0x28 (
	//{0x00, 0x41, 0x22, 0x1c, 0x00},		// 0x29 )
	//{0x14, 0x08, 0x3e, 0x08, 0x14},		// 0x2a *
	//{0x08, 0x08, 0x3e, 0x08, 0x08},		// 0x2b +
	//{0x00, 0x50, 0x30, 0x00, 0x00},		// 0x2c ,
	//{0x08, 0x08, 0x08, 0x08, 0x08},		// 0x2d -
	//{0x00, 0x60, 0x60, 0x00, 0x00},		// 0x2e .
	//{0x20, 0x10, 0x08, 0x04, 0x02},		// 0x2f /
	//{0x3e, 0x51, 0x49, 0x45, 0x3e},		// 0x30 0
	//{0x00, 0x42, 0x7f, 0x40, 0x00},		// 0x31 1
	//{0x42, 0x61, 0x51, 0x49, 0x46},		// 0x32 2
	//{0x21, 0x41, 0x45, 0x4b, 0x31},		// 0x33 3
	//{0x18, 0x14, 0x12, 0x7f, 0x10},		// 0x34 4
	//{0x27, 0x45, 0x45, 0x45, 0x39},		// 0x35 5
	//{0x3c, 0x4a, 0x49, 0x49, 0x30},		// 0x36 6
	//{0x01, 0x71, 0x09, 0x05, 0x03},		// 0x37 7
	//{0x36, 0x49, 0x49, 0x49, 0x36},		// 0x38 8
	//{0x06, 0x49, 0x49, 0x29, 0x1e},		// 0x39 9
	//{0x00, 0x36, 0x36, 0x00, 0x00},		// 0x3a :
	//{0x00, 0x56, 0x36, 0x00, 0x00},		// 0x3b ;
	//{0x08, 0x14, 0x22, 0x41, 0x00},		// 0x3c <
	//{0x14, 0x14, 0x14, 0x14, 0x14},		// 0x3d =
	//{0x00, 0x41, 0x22, 0x14, 0x08},		// 0x3e >
	//{0x02, 0x01, 0x51, 0x09, 0x06},		// 0x3f ?
	//{0x32, 0x49, 0x79, 0x41, 0x3e},		// 0x40 @
	//{0x7e, 0x11, 0x11, 0x11, 0x7e},		// 0x41 A
	//{0x7f, 0x49, 0x49, 0x49, 0x36},		// 0x42 B
	//{0x3e, 0x41, 0x41, 0x41, 0x22},		// 0x43 C
	//{0x7f, 0x41, 0x41, 0x22, 0x1c},		// 0x44 D
	//{0x7f, 0x49, 0x49, 0x49, 0x41},		// 0x45 E
	//{0x7f, 0x09, 0x09, 0x09, 0x01},		// 0x46 F
	//{0x3e, 0x41, 0x49, 0x49, 0x7a},		// 0x47 G
	//{0x7f, 0x08, 0x08, 0x08, 0x7f},		// 0x48 H
	//{0x00, 0x41, 0x7f, 0x41, 0x00},		// 0x49 I
	//{0x20, 0x40, 0x41, 0x3f, 0x01},		// 0x4a J
	//{0x7f, 0x08, 0x14, 0x22, 0x41},		// 0x4b K
	//{0x7f, 0x40, 0x40, 0x40, 0x40},		// 0x4c L
	//{0x7f, 0x02, 0x0c, 0x02, 0x7f},		// 0x4d M
	//{0x7f, 0x04, 0x08, 0x10, 0x7f},		// 0x4e N
	//{0x3e, 0x41, 0x41, 0x41, 0x3e},		// 0x4f O
	//{0x7f, 0x09, 0x09, 0x09, 0x06},		// 0x50 P
	//{0x3e, 0x41, 0x51, 0x21, 0x5e},		// 0x51 Q
	//{0x7f, 0x09, 0x19, 0x29, 0x46},		// 0x52 R
	//{0x26, 0x49, 0x49, 0x49, 0x32},		// 0x53 S
	//{0x01, 0x01, 0x7f, 0x01, 0x01},		// 0x54 T
	//{0x3f, 0x40, 0x40, 0x40, 0x3f},		// 0x55 U
	//{0x1f, 0x20, 0x40, 0x20, 0x1f},		// 0x56 V
	//{0x3f, 0x40, 0x38, 0x40, 0x3f},		// 0x57 W
	//{0x63, 0x14, 0x08, 0x14, 0x63},		// 0x58 X
	//{0x07, 0x08, 0x70, 0x08, 0x07},		// 0x59 Y
	//{0x61, 0x51, 0x49, 0x45, 0x43},		// 0x5a Z
	//{0x00, 0x7f, 0x41, 0x41, 0x00},		// 0x5b [
	//{0x02, 0x04, 0x08, 0x10, 0x20},		// 0x5c (\)
	//{0x00, 0x41, 0x41, 0x7f, 0x00},		// 0x5d ]
	//{0x04, 0x02, 0x01, 0x02, 0x04},		// 0x5e ^
	//{0x40, 0x40, 0x40, 0x40, 0x40},		// 0x5f _
	//{0x00, 0x01, 0x02, 0x04, 0x00},		// 0x60 `
	//{0x20, 0x54, 0x54, 0x54, 0x78},		// 0x61 a
	//{0x7f, 0x48, 0x44, 0x44, 0x38},		// 0x62 b
	//{0x38, 0x44, 0x44, 0x44, 0x20},		// 0x63 c
	//{0x38, 0x44, 0x44, 0x48, 0x7f},		// 0x64 d
	//{0x38, 0x54, 0x54, 0x54, 0x18},		// 0x65 e
	//{0x08, 0x7e, 0x09, 0x01, 0x02},		// 0x66 f
	//{0x0c, 0x52, 0x52, 0x52, 0x3e},		// 0x67 g
	//{0x7f, 0x08, 0x04, 0x04, 0x78},		// 0x68 h
	//{0x00, 0x04, 0x7d, 0x00, 0x00},		// 0x69 i
	//{0x20, 0x40, 0x44, 0x3d, 0x00},		// 0x6a j
	//{0x7f, 0x10, 0x28, 0x44, 0x00},		// 0x6b k
	//{0x00, 0x41, 0x7f, 0x40, 0x00},		// 0x6c l
	//{0x7c, 0x04, 0x18, 0x04, 0x7c},		// 0x6d m
	//{0x7c, 0x08, 0x04, 0x04, 0x78},		// 0x6e n
	//{0x38, 0x44, 0x44, 0x44, 0x38},		// 0x6f o
	//{0x7c, 0x14, 0x14, 0x14, 0x08},		// 0x70 p
	//{0x08, 0x14, 0x14, 0x18, 0x7c},		// 0x71 q
	//{0x7c, 0x08, 0x04, 0x04, 0x08},		// 0x72 r
	//{0x48, 0x54, 0x54, 0x54, 0x20},		// 0x73 s
	//{0x04, 0x3f, 0x44, 0x40, 0x20},		// 0x74 t
	//{0x3c, 0x40, 0x40, 0x20, 0x7c},		// 0x75 u
	//{0x1c, 0x20, 0x40, 0x20, 0x1c},		// 0x76 v
	//{0x3c, 0x40, 0x30, 0x40, 0x3c},		// 0x77 w
	//{0x44, 0x28, 0x10, 0x28, 0x44},		// 0x78 x
	//{0x0c, 0x50, 0x50, 0x50, 0x3c},		// 0x79 y
	//{0x44, 0x64, 0x54, 0x4c, 0x44},		// 0x7a z
	//{0x00, 0x08, 0x36, 0x41, 0x00},		// 0x7b {
	//{0x00, 0x00, 0x77, 0x00, 0x00},		// 0x7c |
	//{0x00, 0x41, 0x36, 0x08, 0x00},		// 0x7d }
	//{0x08, 0x04, 0x08, 0x10, 0x08}};	// 0x7e ~
	//void GLCD_Command(unsigned char signal, unsigned char command);
	//void GLCD_Clear(void);
	//void GLCD_xy(unsigned char x, unsigned char y);
	//
	//void GLCD_Port_Init(void)
	//{
	//GLCD_DATABUS_DDR = 0xFF;
	//GLCD_CONTROL_DDR = 0xFF;
	//}
	//
	//void GLCD_Init()
	//{
	//GLCD_Command( GLCD_CS1_2, DISPON );
	//GLCD_Command( GLCD_CS1_2, 0xC0 );	/* 디스플레이 시작라인	*/
	//GLCD_Command( GLCD_CS1_2, 0xB8 );	/* X 어드레스 셋 = 0	*/
	//GLCD_Command( GLCD_CS1_2, 0x40 );	/* Y 어드레스 셋 = 0	*/
	//GLCD_Clear();
	//}
	//
	//void GLCD_Command(unsigned char signal, unsigned char command)
	//{
	//GLCD_CONTROL = signal & 0x18;
	//GLCD_CONTROL |= 0x04;
	//GLCD_DATABUS = command;
	//_delay_us(10);
	//GLCD_CONTROL &= ~0x04;
	//_delay_us(10);
	//GLCD_CONTROL=0x00;
	//_delay_ms(1);
	//}
	//
	//void GLCD_Data(unsigned char signal, unsigned char character)
	//{
	//GLCD_CONTROL = (signal & 0x18) | 0x01;
	//GLCD_CONTROL |= 0x04;
	//GLCD_DATABUS = character;
	//_delay_us(10);
	//GLCD_CONTROL &= ~0x04;
	//_delay_us(10);
	//GLCD_CONTROL=0x00;
	//_delay_ms(1);
	//}
	//
	///* clear GLCD screen */
	//void GLCD_Clear(void)
	//{
	//unsigned char i, j, x;
	//
	//GLCD_CONTROL = 0x00;			// clear all control signals
	//GLCD_Command(GLCD_CS1_2, DISPON);		// clear all control signals
	//GLCD_Command(GLCD_CS1_2, 0xC0);		// CS1,CS2 display position
	//
	//x = GLCD_SET_X_ADDR;
	//for(i = 0; i <= 7; i++)
	//{
	//GLCD_Command(GLCD_CS1_2, x);		// X start addtess
	//GLCD_Command(GLCD_CS1_2, GLCD_SET_Y_ADDR);	// Y start address
	//for(j = 0; j <= 63; j++)
	//{
	//GLCD_Data(GLCD_CS1_2, 0x00);	// clear CS1 and CS2 screen
	//}
	//
	//x++;
	//}
	//}
	//
	//void GLCD_xy(unsigned char x, unsigned char y)		/* set character position */
	//{
	//xcharacter = x;
	//ycharacter = y;
	//
	//GLCD_Command(GLCD_CS1_2, 0xB8+xcharacter);	// X address
	//
	//if(ycharacter <= 9)				// if y <= 9, CS1 Y address
	//GLCD_Command(GLCD_CS1, 0x40+ycharacter*6+4);
	//else					// if y >= 10, CS2 Y address
	//GLCD_Command(GLCD_CS2, 0x40+(ycharacter-10)*6);
	//}
	//
	//void GLCD_Character(unsigned char character)		/* display a character */
	//{
	//unsigned char i, signal; //, font_data;
	//
	//GLCD_xy(xcharacter,ycharacter);
	//
	//if(ycharacter <= 9)				// if y <= 9, CS1
	//signal = GLCD_CS1;
	//else					// if y >= 10, CS2
	//signal = GLCD_CS2;
	//
	//for(i = 0; i <= 4; i++)
	//{
	//if((cursor_flag == 1) && (xcharacter == xcursor) && (ycharacter == ycursor))
	//GLCD_Data(signal,(font[character - 0x20][i]) | 0x80);
	//else
	//GLCD_Data(signal,font[character - 0x20][i]);
	//}
	//GLCD_Data(signal,0x00);		  	// last byte 0x00
	//
	//ycharacter++;                                 	// go next character position
	//if(ycharacter == 20)
	//{
	//ycharacter = 0;
	//xcharacter++;
	//}
	//}
	//
	///* display a string */
	//void GLCD_String(unsigned char x, unsigned char y, unsigned char *string){
	//xcharacter = x;
	//ycharacter = y;
	//
	//while(*string != '\0')
	//{
	//GLCD_Character(*string);	// display a charcater
	//string++;
	//}
	//}
	//
	//// 점을 찍을 좌표를 잡습니다.
	//void GLCD_Axis_xy(unsigned char x, unsigned char y)
	//{
		//GLCD_Command(GLCD_CS1_2, GLCD_SET_X_ADDR + x); // X address
		//if(y <= 63)
		//{
			//GLCD_Command(GLCD_CS1, GLCD_SET_Y_ADDR + y); // CS1 Y address
		//}
		//else
		//{
			//GLCD_Command(GLCD_CS2, GLCD_SET_Y_ADDR + y - 64); // CS2 Y address
		//}
	//}
//
//
	//// draw a dot on GLCD
	//// 점을 그립니다.
	//void GLCD_Dot(unsigned char xx,unsigned char y)
	//{
		//unsigned char x, i;
//
		//// 해상도 범위(128.64) 인지 검사합니다.
		//if((xx > 63) || (y > 127)) return;
//
		//x = xx / 8;				  // calculate x address
		//i = xx % 8;
		//if(i == 0)      { i = 0x01; }
		//else if(i == 1) { i = 0x02; }
		//else if(i == 2) { i = 0x04; }
		//else if(i == 3) { i = 0x08; }
		//else if(i == 4) { i = 0x10; }
		//else if(i == 5) { i = 0x20; }
		//else if(i == 6) { i = 0x40; }
		//else            { i = 0x80; }
		//
		//ScreenBuffer[x][y] |= i;			// OR old data with new data
//
		//GLCD_Axis_xy(x, y);				      // draw dot on GLCD screen
		//if(y <= 63) { GLCD_Data(GLCD_CS1, ScreenBuffer[x][y]);  }
		//else        { GLCD_Data(GLCD_CS2, ScreenBuffer[x][y]);  }
	//}
//
//
//
	//// draw a straight line
	//// 직선을 그립니다.
	//void GLCD_Line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
	//{
		//int x, y;
//
		//if(y1 != y2)
		//{
			//if(y1 < y2)
			//{
				//for(y = y1; y <= y2; y++)
				//{
					//x = x1 + (y - y1)*(x2 - x1)/(y2 - y1);
					//GLCD_Dot(x,y);
				//}
			//}
			//else
			//{
				//for(y = y1; y >= y2; y--)
				//{
					//x = x1 + (y - y1)*(x2 - x1)/(y2 - y1);
					//GLCD_Dot(x,y);
				//}
			//}
		//}
		//else if(x1 != x2)	  // if x1 != x2, x is variable
		//{
			//if(x1 < x2)
			//{
				//for(x = x1; x <= x2; x++)
				//{ y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);
					//GLCD_Dot(x,y);
				//}
			//}
			//else
			//{
				//for(x = x1; x >= x2; x--)
				//{
					//y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);
					//GLCD_Dot(x,y);
				//}
			//}
		//}
		//else                  // if x1 == x2 and y1 == y2,
		//{
			//GLCD_Dot(x1,y1);		//             it is a dot
		//}
	//}
//
//
	//// draw a rectangle
	//// 직사각형을 그립니다.
	//void GLCD_Rectangle(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
	//{
		//GLCD_Line(x1,y1,x1,y2);				// horizontal line
		//GLCD_Line(x2,y1,x2,y2);
		//GLCD_Line(x1,y1,x2,y1);				// vertical line
		//GLCD_Line(x1,y2,x2,y2);
	//}
//
//
	//// draw a circle
	//// 원을 그립니다.
	//void GLCD_Circle(unsigned char x1,unsigned char y1,unsigned char r)
	//{
		//int x, y;
		//float s;
//
		//for(y = y1 - r*3/4; y <= y1 + r*3/4; y++)	// draw with y variable
		//{
			//s = sqrt(r*r - (y-y1)*(y-y1)) + 0.5;
			//x = x1 + (unsigned char)s;
			//GLCD_Dot(x,y);
			//x = x1 - (unsigned char)s;
			//GLCD_Dot(x,y);
		//}
//
		//for(x = x1 - r*3/4; x <= x1 + r*3/4; x++)	// draw with x variable
		//{
			//s = sqrt(r*r - (x-x1)*(x-x1)) + 0.5;
			//y = y1 + (unsigned char)s;
			//GLCD_Dot(x,y);
			//y = y1 - (unsigned char)s;
			//GLCD_Dot(x,y);
		//}
	//}
//
	//
//int main(void)
//{
//
	//_delay_ms(50); // wait for system stabilization
	//GLCD_Port_Init(); // initialize MCU and kit
	//GLCD_Init();
	//while(1)
	//{
		//GLCD_Dot(0,6);
		//GLCD_Line(10, 10, 60, 60);
		//GLCD_Rectangle(10, 10, 60, 60);
		//GLCD_Circle(10, 80, 10);
	//}
//}
	//
	
	
	
	
	
////원이동 + 사각형 늘리기 예제
		//#include <avr/io.h>
		//#define F_CPU 16000000
		//#include <util/delay.h>
//
		//#define GLCD_DATABUS	PORTD // GLCD data
		//#define GLCD_CONTROL	PORTC // GLCD control signal
		//#define GLCD_DATABUS_DDR DDRD
		//#define GLCD_CONTROL_DDR DDRC
//
		//// Graphic LCD 명령어 셋팅
		//#define GLCD_CS1 0x08		 // GLCD_CS1 Select 0000 1000
		//#define GLCD_CS2 0x10		 // GLCD_CS2 Select 0001 0000
		//#define GLCD_CS1_2 0x18		 // GLCD_CS1, GLCD_CS2 Select 0001 1000
//
		//#define GLCD_START_LINE		0xC0	// 11XXXXXX: set lcd start line
		//#define GLCD_SET_X_ADDR		0xB8	// 10111XXX: set lcd X address
		//#define GLCD_SET_Y_ADDR		0x40	// 01YYYYYY: set lcd Y address
//
		//#define DISPON			0x3F
		//#define DISPOFF 		0x3E
//
		//// 키패드 명령어 셋팅
		//#define KEY_DDR DDRF
		//#define KEY_PORT PORTF
		//#define KEY_PIN PINF
//
		////LED 명령어 셋팅
		//#define  OCR1_R OCR1A
		//#define  OCR1_G OCR1B
		//#define  OCR1_B OCR1C
//
//
		//#define GLCD_CS1 0x08
		//#define GLCD_CS2 0x10
		//#define GLCD_CS1_2 0x18
//
		//#define GLCD_SET_X_ADDR 0xB8
		//#define GLCD_SET_Y_ADDR 0x40
		//
		//
		//unsigned char xcharacter, ycharacter;		// x character(0-7), y character(0-19)
		//unsigned char cursor_flag, xcursor, ycursor;	// x and y cursor position(0-7, 0-19)
//
		//unsigned char ScreenBuffer[8][128]={0};           // screen buffer
		//
		//
		//unsigned char font[95][5] = {	            /* 5x7 ASCII character font */
			//{0x00, 0x00, 0x00, 0x00, 0x00},		// 0x20 space
			//{0x00, 0x00, 0x4f, 0x00, 0x00},		// 0x21 !
			//{0x00, 0x07, 0x00, 0x07, 0x00},		// 0x22 "
			//{0x14, 0x7f, 0x14, 0x7f, 0x14},		// 0x23 #
			//{0x24, 0x2a, 0x7f, 0x2a, 0x12},		// 0x24 $
			//{0x23, 0x13, 0x08, 0x64, 0x62},		// 0x25 %
			//{0x36, 0x49, 0x55, 0x22, 0x50},		// 0x26 &
			//{0x00, 0x05, 0x03, 0x00, 0x00},		// 0x27 '
			//{0x00, 0x1c, 0x22, 0x41, 0x00},		// 0x28 (
			//{0x00, 0x41, 0x22, 0x1c, 0x00},		// 0x29 )
			//{0x14, 0x08, 0x3e, 0x08, 0x14},		// 0x2a *
			//{0x08, 0x08, 0x3e, 0x08, 0x08},		// 0x2b +
			//{0x00, 0x50, 0x30, 0x00, 0x00},		// 0x2c ,
			//{0x08, 0x08, 0x08, 0x08, 0x08},		// 0x2d -
			//{0x00, 0x60, 0x60, 0x00, 0x00},		// 0x2e .
			//{0x20, 0x10, 0x08, 0x04, 0x02},		// 0x2f /
			//{0x3e, 0x51, 0x49, 0x45, 0x3e},		// 0x30 0
			//{0x00, 0x42, 0x7f, 0x40, 0x00},		// 0x31 1
			//{0x42, 0x61, 0x51, 0x49, 0x46},		// 0x32 2
			//{0x21, 0x41, 0x45, 0x4b, 0x31},		// 0x33 3
			//{0x18, 0x14, 0x12, 0x7f, 0x10},		// 0x34 4
			//{0x27, 0x45, 0x45, 0x45, 0x39},		// 0x35 5
			//{0x3c, 0x4a, 0x49, 0x49, 0x30},		// 0x36 6
			//{0x01, 0x71, 0x09, 0x05, 0x03},		// 0x37 7
			//{0x36, 0x49, 0x49, 0x49, 0x36},		// 0x38 8
			//{0x06, 0x49, 0x49, 0x29, 0x1e},		// 0x39 9
			//{0x00, 0x36, 0x36, 0x00, 0x00},		// 0x3a :
			//{0x00, 0x56, 0x36, 0x00, 0x00},		// 0x3b ;
			//{0x08, 0x14, 0x22, 0x41, 0x00},		// 0x3c <
			//{0x14, 0x14, 0x14, 0x14, 0x14},		// 0x3d =
			//{0x00, 0x41, 0x22, 0x14, 0x08},		// 0x3e >
			//{0x02, 0x01, 0x51, 0x09, 0x06},		// 0x3f ?
			//{0x32, 0x49, 0x79, 0x41, 0x3e},		// 0x40 @
			//{0x7e, 0x11, 0x11, 0x11, 0x7e},		// 0x41 A
			//{0x7f, 0x49, 0x49, 0x49, 0x36},		// 0x42 B
			//{0x3e, 0x41, 0x41, 0x41, 0x22},		// 0x43 C
			//{0x7f, 0x41, 0x41, 0x22, 0x1c},		// 0x44 D
			//{0x7f, 0x49, 0x49, 0x49, 0x41},		// 0x45 E
			//{0x7f, 0x09, 0x09, 0x09, 0x01},		// 0x46 F
			//{0x3e, 0x41, 0x49, 0x49, 0x7a},		// 0x47 G
			//{0x7f, 0x08, 0x08, 0x08, 0x7f},		// 0x48 H
			//{0x00, 0x41, 0x7f, 0x41, 0x00},		// 0x49 I
			//{0x20, 0x40, 0x41, 0x3f, 0x01},		// 0x4a J
			//{0x7f, 0x08, 0x14, 0x22, 0x41},		// 0x4b K
			//{0x7f, 0x40, 0x40, 0x40, 0x40},		// 0x4c L
			//{0x7f, 0x02, 0x0c, 0x02, 0x7f},		// 0x4d M
			//{0x7f, 0x04, 0x08, 0x10, 0x7f},		// 0x4e N
			//{0x3e, 0x41, 0x41, 0x41, 0x3e},		// 0x4f O
			//{0x7f, 0x09, 0x09, 0x09, 0x06},		// 0x50 P
			//{0x3e, 0x41, 0x51, 0x21, 0x5e},		// 0x51 Q
			//{0x7f, 0x09, 0x19, 0x29, 0x46},		// 0x52 R
			//{0x26, 0x49, 0x49, 0x49, 0x32},		// 0x53 S
			//{0x01, 0x01, 0x7f, 0x01, 0x01},		// 0x54 T
			//{0x3f, 0x40, 0x40, 0x40, 0x3f},		// 0x55 U
			//{0x1f, 0x20, 0x40, 0x20, 0x1f},		// 0x56 V
			//{0x3f, 0x40, 0x38, 0x40, 0x3f},		// 0x57 W
			//{0x63, 0x14, 0x08, 0x14, 0x63},		// 0x58 X
			//{0x07, 0x08, 0x70, 0x08, 0x07},		// 0x59 Y
			//{0x61, 0x51, 0x49, 0x45, 0x43},		// 0x5a Z
			//{0x00, 0x7f, 0x41, 0x41, 0x00},		// 0x5b [
			//{0x02, 0x04, 0x08, 0x10, 0x20},		// 0x5c (\)
			//{0x00, 0x41, 0x41, 0x7f, 0x00},		// 0x5d ]
			//{0x04, 0x02, 0x01, 0x02, 0x04},		// 0x5e ^
			//{0x40, 0x40, 0x40, 0x40, 0x40},		// 0x5f _
			//{0x00, 0x01, 0x02, 0x04, 0x00},		// 0x60 `
			//{0x20, 0x54, 0x54, 0x54, 0x78},		// 0x61 a
			//{0x7f, 0x48, 0x44, 0x44, 0x38},		// 0x62 b
			//{0x38, 0x44, 0x44, 0x44, 0x20},		// 0x63 c
			//{0x38, 0x44, 0x44, 0x48, 0x7f},		// 0x64 d
			//{0x38, 0x54, 0x54, 0x54, 0x18},		// 0x65 e
			//{0x08, 0x7e, 0x09, 0x01, 0x02},		// 0x66 f
			//{0x0c, 0x52, 0x52, 0x52, 0x3e},		// 0x67 g
			//{0x7f, 0x08, 0x04, 0x04, 0x78},		// 0x68 h
			//{0x00, 0x04, 0x7d, 0x00, 0x00},		// 0x69 i
			//{0x20, 0x40, 0x44, 0x3d, 0x00},		// 0x6a j
			//{0x7f, 0x10, 0x28, 0x44, 0x00},		// 0x6b k
			//{0x00, 0x41, 0x7f, 0x40, 0x00},		// 0x6c l
			//{0x7c, 0x04, 0x18, 0x04, 0x7c},		// 0x6d m
			//{0x7c, 0x08, 0x04, 0x04, 0x78},		// 0x6e n
			//{0x38, 0x44, 0x44, 0x44, 0x38},		// 0x6f o
			//{0x7c, 0x14, 0x14, 0x14, 0x08},		// 0x70 p
			//{0x08, 0x14, 0x14, 0x18, 0x7c},		// 0x71 q
			//{0x7c, 0x08, 0x04, 0x04, 0x08},		// 0x72 r
			//{0x48, 0x54, 0x54, 0x54, 0x20},		// 0x73 s
			//{0x04, 0x3f, 0x44, 0x40, 0x20},		// 0x74 t
			//{0x3c, 0x40, 0x40, 0x20, 0x7c},		// 0x75 u
			//{0x1c, 0x20, 0x40, 0x20, 0x1c},		// 0x76 v
			//{0x3c, 0x40, 0x30, 0x40, 0x3c},		// 0x77 w
			//{0x44, 0x28, 0x10, 0x28, 0x44},		// 0x78 x
			//{0x0c, 0x50, 0x50, 0x50, 0x3c},		// 0x79 y
			//{0x44, 0x64, 0x54, 0x4c, 0x44},		// 0x7a z
			//{0x00, 0x08, 0x36, 0x41, 0x00},		// 0x7b {
			//{0x00, 0x00, 0x77, 0x00, 0x00},		// 0x7c |
			//{0x00, 0x41, 0x36, 0x08, 0x00},		// 0x7d }
			//{0x08, 0x04, 0x08, 0x10, 0x08}};	// 0x7e ~
			//
			//#define delaytime 30
			//
			//unsigned char key_scan(unsigned char a);
			//int key_convert1(unsigned char in);
			//int key_convert2(unsigned char in);
			//int result(int a, int b);
			//
			//unsigned char redV;             //빨간 LED아날로그 값 설정 (0~255)
			//unsigned char greenV;			//초록 LED아날로그 값 설정 (0~255)
			//unsigned char blueV;
			//
			//void GLCD_Command(unsigned char signal, unsigned char command);
			//void GLCD_Clear(void);
			//void GLCD_xy(unsigned char x, unsigned char y);
//
//
			//void set_color(unsigned char red, unsigned char green, unsigned char blue){
				//OCR1_R=red;
				//OCR1_G= green;
				//OCR1_B=blue;
			//}
			//
			//void timer_init(){
				////fast PWM 8bit
				////WGM13(0), WGM12(1), WGM11(0), WGM10(1)
				////COM1A1/COM1B1/COM1C1: 1, COM1A0/COM1B0/COM1C0: 0
				//TCCR1A |= _BV(WGM10) | _BV(COM1A1) |_BV(COM1B1) | _BV(COM1C1);
				//TCCR1B |= _BV(WGM12) | _BV(CS11);
				//set_color(0,0,0);
			//}
			//
			//
			//void GLCD_Port_Init(void)
			//{
				//GLCD_DATABUS_DDR = 0xFF;
				//GLCD_CONTROL_DDR = 0xFF;
			//}
//
			//void GLCD_Init()
			//{
				//GLCD_Command( GLCD_CS1_2, DISPON );
				//GLCD_Command( GLCD_CS1_2, 0xC0 );	/* 디스플레이 시작라인	*/
				//GLCD_Command( GLCD_CS1_2, 0xB8 );	/* X 어드레스 셋 = 0	*/
				//GLCD_Command( GLCD_CS1_2, 0x40 );	/* Y 어드레스 셋 = 0	*/
				//GLCD_Clear();
			//}
//
			//void GLCD_Command(unsigned char signal, unsigned char command)
			//{
				//GLCD_CONTROL = signal & 0x18;
				//GLCD_CONTROL |= 0x04;
				//GLCD_DATABUS = command;
				//_delay_us(10);
				//GLCD_CONTROL &= ~0x04;
				//_delay_us(10);
				//GLCD_CONTROL=0x00;
				//_delay_ms(1);
			//}
//
			//void GLCD_Data(unsigned char signal, unsigned char character)
			//{
				//GLCD_CONTROL = (signal & 0x18) | 0x01;
				//GLCD_CONTROL |= 0x04;
				//GLCD_DATABUS = character;
				//_delay_us(10);
				//GLCD_CONTROL &= ~0x04;
				//_delay_us(10);
				//GLCD_CONTROL=0x00;
				//_delay_ms(1);
			//}
//
			///* clear GLCD screen */
			//void GLCD_Clear(void)
			//{
				//unsigned char i, j, x;
				//
				//GLCD_CONTROL = 0x00;			// clear all control signals
				//GLCD_Command(GLCD_CS1_2, DISPON);		// clear all control signals
				//GLCD_Command(GLCD_CS1_2, 0xC0);		// CS1,CS2 display position
//
				//x = GLCD_SET_X_ADDR;
				//for(i = 0; i <= 7; i++)
				//{
					//GLCD_Command(GLCD_CS1_2, x);		// X start addtess
					//GLCD_Command(GLCD_CS1_2, GLCD_SET_Y_ADDR);	// Y start address
					//for(j = 0; j <= 63; j++)
					//{
						//GLCD_Data(GLCD_CS1_2, 0x00);	// clear CS1 and CS2 screen
					//}
//
					//x++;
				//}
			//}
//
			//void GLCD_xy(unsigned char x, unsigned char y)		/* set character position */
			//{
				//xcharacter = x;
				//ycharacter = y;
//
				//GLCD_Command(GLCD_CS1_2, 0xB8+xcharacter);	// X address
//
				//if(ycharacter <= 9)				// if y <= 9, CS1 Y address
				//GLCD_Command(GLCD_CS1, 0x40+ycharacter*6+4);
				//else					// if y >= 10, CS2 Y address
				//GLCD_Command(GLCD_CS2, 0x40+(ycharacter-10)*6);
			//}
//
			//void GLCD_Character(unsigned char character)		/* display a character */
			//{
				//unsigned char i, signal; //, font_data;
//
				//GLCD_xy(xcharacter,ycharacter);
				//
				//if(ycharacter <= 9)				// if y <= 9, CS1
				//signal = GLCD_CS1;
				//else					// if y >= 10, CS2
				//signal = GLCD_CS2;
				//
				//for(i = 0; i <= 4; i++)
				//{
					//if((cursor_flag == 1) && (xcharacter == xcursor) && (ycharacter == ycursor))
					//GLCD_Data(signal,(font[character - 0x20][i]) | 0x80);
					//else
					//GLCD_Data(signal,font[character - 0x20][i]);
				//}
				//GLCD_Data(signal,0x00);		  	// last byte 0x00
//
				//ycharacter++;                                 	// go next character position
				//if(ycharacter == 20)
				//{
					//ycharacter = 0;
					//xcharacter++;
				//}
			//}
//
			///* display a string */
			//void GLCD_String(unsigned char x, unsigned char y, unsigned char *string){
				//xcharacter = x;
				//ycharacter = y;
//
				//while(*string != '\0')
				//{
					//GLCD_Character(*string);	// display a charcater
					//string++;
				//}
			//}
			//
			////display floating-point number xx.x
			////00.0로 소수점 표현
			//void GLCD_2Dot1Float(float number){
				//unsigned int i,j;
				//
				//j=(int)(number *10);
				//i=j/100;
				//if(i==9){GLCD_Character(' ');}
				//else{GLCD_Character(i+'0');}
				//
				//j=j%100;
				//i=j/10;
				//GLCD_Character(i+'0');
				//GLCD_Character('.');
				//
				//i=j%10;
				//GLCD_Character(i+'0');
			//}
			//
			////display 1-digit decimal number
			////1자리의 10진수 값을 표시
			//unsigned char GLCD_1DigitDecimal(unsigned char number,unsigned char flag){
				//number %= 10;
				//
				//if((number==0)&&(flag==0)){
					//GLCD_Character(' ');
					//return 0;
				//}
				//
				//GLCD_Character(number + '0');
				//return 1;
			//}
			//
			//unsigned char GLCD_2DigitDecimal(unsigned char number){
				//unsigned int i;
				//unsigned char flag;
				//
				//flag=0;
				//number=number %100;
				//i=number/10;
				//flag=GLCD_1DigitDecimal(i,flag);
				//
				//i=number %10;
				//GLCD_Character((i+'0'));
			//}
			//
			//// display 3-digit decimal number
			//// 3자리의 10진수 값을 표시합니다.
			//void GLCD_3DigitDecimal(unsigned int number)
			//{
				//unsigned int i;
				//unsigned char flag;
				//flag = 0;
				//number = number % 1000;
				//i = number/100;
				//flag = GLCD_1DigitDecimal(i, flag); // 10^2
				//number = number % 100;
				//i = number/10;
				//flag = GLCD_1DigitDecimal(i, flag); // 10^1
				//i = number % 10;
				//GLCD_Character(i + '0'); // 10^0
			//}
			//
			//// display 4-digit decimal number
			//// 4자리의 10진수 값을 표시합니다.
			//void GLCD_4DigitDecimal(unsigned int number)
			//{
				//unsigned int i;
				//unsigned char flag;
				//flag = 0;
				//number = number % 10000;
				//i = number/1000;
				//flag = GLCD_1DigitDecimal(i, flag); // 10^3
				//number = number % 1000;
				//i = number/100;
				//flag = GLCD_1DigitDecimal(i, flag); // 10^2
				//number = number % 100;
				//i = number/10;
				//flag = GLCD_1DigitDecimal(i, flag); // 10^1
				//i = number % 10;
				//GLCD_Character(i + '0'); // 10^0
			//}
			//
			//// display 1-digit hex number
			//// 1자리의 16진수 값을 표시합니다.
			//void GLCD_1DigitHex(unsigned char i)
			//{
				//i &= 0x0F; // 16^0
				//if(i <= 9) { GLCD_Character(i + '0'); }
				//else { GLCD_Character(i - 10 + 'A'); }
			//}
			//// display 2-digit hex number
			//// 2자리의 16진수 값을 표시합니다.
			//void GLCD_2DigitHex(unsigned char number)
			//{
				//GLCD_1DigitHex(number >> 4); // 16^1
				//GLCD_1DigitHex(number ); // 16^0
			//}			//
			//// display 4-digit hex number
			//// 4자리의 16진수 값을 표시합니다.
			//void GLCD_4DigitHex(unsigned int number)
			//{
				//GLCD_1DigitHex(number >> 12); // 16^3
				//GLCD_1DigitHex(number >> 8 ); // 16^2
				//GLCD_1DigitHex(number >> 4 ); // 16^1
				//GLCD_1DigitHex(number ); // 16^0
			//}
			//
			//
			//
//
			//// 점을 찍을 좌표를 잡습니다.
			//void GLCD_Axis_xy(unsigned char x, unsigned char y)
			//{
				//GLCD_Command(GLCD_CS1_2, GLCD_SET_X_ADDR + x); // X address
				//if(y <= 63)
				//{
					//GLCD_Command(GLCD_CS1, GLCD_SET_Y_ADDR + y); // CS1 Y address
				//}
				//else
				//{
					//GLCD_Command(GLCD_CS2, GLCD_SET_Y_ADDR + y - 64); // CS2 Y address
				//}
			//}
//
//
			//// draw a dot on GLCD
			//// 점을 그립니다.
			//void GLCD_Dot(unsigned char xx,unsigned char y)
			//{
				//unsigned char x, i;
//
				//// 해상도 범위(128.64) 인지 검사합니다.
				//if((xx > 63) || (y > 127)) return;
//
				//x = xx / 8;				  // calculate x address
				//i = xx % 8;
				//if(i == 0)      { i = 0x01; }
				//else if(i == 1) { i = 0x02; }
				//else if(i == 2) { i = 0x04; }
				//else if(i == 3) { i = 0x08; }
				//else if(i == 4) { i = 0x10; }
				//else if(i == 5) { i = 0x20; }
				//else if(i == 6) { i = 0x40; }
				//else            { i = 0x80; }
				//
				//ScreenBuffer[x][y] |= i;			// OR old data with new data
//
				//GLCD_Axis_xy(x, y);				      // draw dot on GLCD screen
				//if(y <= 63) { GLCD_Data(GLCD_CS1, ScreenBuffer[x][y]);  }
				//else        { GLCD_Data(GLCD_CS2, ScreenBuffer[x][y]);  }
			//}
//
//
//
			//// draw a straight line
			//// 직선을 그립니다.
			//void GLCD_Line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
			//{
				//int x, y;
//
				//if(y1 != y2)
				//{
					//if(y1 < y2)
					//{
						//for(y = y1; y <= y2; y++)
						//{
							//x = x1 + (y - y1)*(x2 - x1)/(y2 - y1);
							//GLCD_Dot(x,y);
						//}
					//}
					//else
					//{
						//for(y = y1; y >= y2; y--)
						//{
							//x = x1 + (y - y1)*(x2 - x1)/(y2 - y1);
							//GLCD_Dot(x,y);
						//}
					//}
				//}
				//else if(x1 != x2)	  // if x1 != x2, x is variable
				//{
					//if(x1 < x2)
					//{
						//for(x = x1; x <= x2; x++)
						//{ y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);
							//GLCD_Dot(x,y);
						//}
					//}
					//else
					//{
						//for(x = x1; x >= x2; x--)
						//{
							//y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);
							//GLCD_Dot(x,y);
						//}
					//}
				//}
				//else                  // if x1 == x2 and y1 == y2,
				//{
					//GLCD_Dot(x1,y1);		//             it is a dot
				//}
			//}
//
//
			//// draw a rectangle
			//// 직사각형을 그립니다.
			//void GLCD_Rectangle(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
			//{
				//GLCD_Line(x1,y1,x1,y2);				// horizontal line
				//GLCD_Line(x2,y1,x2,y2);
				//GLCD_Line(x1,y1,x2,y1);				// vertical line
				//GLCD_Line(x1,y2,x2,y2);
			//}
//
//
			//// draw a circle
			//// 원을 그립니다.
			//void GLCD_Circle(unsigned char x1,unsigned char y1,unsigned char r)
			//{
				//int x, y;
				//float s;
//
				//for(y = y1 - r*3/4; y <= y1 + r*3/4; y++)	// draw with y variable
				//{
					//s = sqrt(r*r - (y-y1)*(y-y1)) + 0.5;
					//x = x1 + (unsigned char)s;
					//GLCD_Dot(x,y);
					//x = x1 - (unsigned char)s;
					//GLCD_Dot(x,y);
				//}
//
				//for(x = x1 - r*3/4; x <= x1 + r*3/4; x++)	// draw with x variable
				//{
					//s = sqrt(r*r - (x-x1)*(x-x1)) + 0.5;
					//y = y1 + (unsigned char)s;
					//GLCD_Dot(x,y);
					//y = y1 - (unsigned char)s;
					//GLCD_Dot(x,y);
				//}
			//}
			//
			////void GLCD_Draw_Clear(){
			////for(int j=0; j<128; j++)
			////for(int k=0; k<8; k++)
			////ScreenBuffer[k][j] = 0;
			////}
			//// 드로잉 버퍼 지우기 함수의 수정 버전
//
			//void GLCD_Draw_Clear(){
//
				//// 1단계: 드로잉 버퍼에 남아 있는 이전 그림 데이터 화면 지우기 (LCD 내부의 데이터 지우기)
//
				//for(int x=0; x<8; x++){
//
					//for(int y=0; y<128; y++){
//
						//// 드로잉 버퍼에 이전 그림이 남아 있을 때만 해당 부분 화면 지우기
//
						//if(ScreenBuffer[x][y] != 0){
//
							//GLCD_Command(GLCD_CS1_2, GLCD_SET_X_ADDR+x);
//
							//GLCD_Command(GLCD_CS1_2, GLCD_SET_Y_ADDR+y);
//
							//if(y <= 63) { GLCD_Data(GLCD_CS1, 0x00);  }
//
							//else        { GLCD_Data(GLCD_CS2, 0x00);  }
//
						//}
//
					//}
//
				//}
//
				//// 2단계: 드로잉 버퍼 0으로 리셋하기 1024=128*8
//
				//memset(ScreenBuffer, 0, 1024);
//
//
//
			//}
			//int main(void)
			//{
					//unsigned char i;
					//int sw=1;
					//_delay_ms(50);                                 // wait for system stabilization
//
					//GLCD_Port_Init();                             // initialize MCU and kit
//
					//GLCD_Init();
//
//
//
					//while(1)
//
					//{
						//if(sw==0){
						//for(i=0; i<20; i++){
//
							//GLCD_Circle(10, 20+i, 10);
//
							//_delay_ms(1000);
//
							//GLCD_Draw_Clear(); // 드로잉 버퍼(이전 그림) 지우기
//
						//}
						//}
						//
						//if(sw==1){
									//for(i=0; i<10; i++){
										//GLCD_Rectangle(0, 0, 10, 10+i);
										//_delay_ms(500);
										//GLCD_Draw_Clear(); // 드로잉 버퍼(이전 그림) 지우기
									//}
									//for(i=10; i>0; i--){
										//GLCD_Rectangle(0, 0, 10, 10+i);
										//_delay_ms(500);
										//GLCD_Draw_Clear(); // 드로잉 버퍼(이전 그림) 지우기
									//}
						//}
					//}
			//}
//
			//unsigned char key_scan(unsigned char a)
			//{
				//unsigned char in;
//
				//KEY_DDR = 0xFF;    //all out
				//KEY_PORT = a;      //키패드 줄 위치
				//KEY_DDR = 0x0F;    //Port 7bit~4bit(In) 3bit~0bit(Out)
				//_delay_us(50);
//
				//// 키패드 입력 포트의 상위 4자리 값 추출
				//in = (PINF & 0xF0);
				//if(in != 0x00)
				//{
					//in |= a;
					//return in;
				//}
				//return 0;
			//}
//
			//int key_convert1(unsigned char in)
			//{
				//int returnvalue;
//
				//switch(in)
				//{
					//case 0x11 :
					//returnvalue = 1;
					//break;
//
					//case 0x21 :
					//returnvalue = 2;
					//break;
//
					//case 0x41 :
					//returnvalue = 3;
					//break;
					//
					//default : returnvalue = 0;  break;
				//}
				//return returnvalue;
			//}
//
//
			//int key_convert2(unsigned char in)
			//{
				//int returnvalue;
//
				//switch(in)
				//{
					//case 0x12 :
					//returnvalue = 1;
					//break;
//
					//case 0x22 :
					//returnvalue = 2;
					//break;
//
					//case 0x42 :
					//returnvalue = 3;
					//break;
					//
					//default : returnvalue = 0;  break;
				//}
				//return returnvalue;
			//}
//
			//int result(int a, int b){
				//
				//if(a == b){
					//return 0;
				//}
//
				//else if(a==1 && b== 2){
					//return 2;
				//}
				//
				//else if(a==1 && b==3){
					//return 1;
				//}
				//
				//else if(a==2 && b== 3){
					//return 2;
				//}
				//
				//else if(a==2 && b==1){
					//return 1;
				//}
//
				//else if(a==3 && b== 1){
					//return 2;
				//}
				//
				//else if(a==3 && b==2){
					//return 1;
				//}
			//}