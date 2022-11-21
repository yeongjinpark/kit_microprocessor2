
////////////////////////////////시리얼 좌표
//#include <avr/io.h>
//#include <stdio.h>
//#define F_CPU 16000000UL
//#include <util/delay.h>
//
//#define UP 2
//#define DOWN 8
//#define LEFT 4
//#define RIGHT 6
//
//static int putchar0(char c, FILE *stream);
//
//static FILE mystdout =FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);
//
//static int putchar0(char c, FILE *stream)
//{
//
//if(c=='\n')
//putchar0('\r',stream);
////while(!(UCSR0A & (1<<UDRE0)));
//while(!(UCSR0A & 0x20));
//UDR0=c;
//return 0;
//}
//
//char getchar0()
//{
//while(!(UCSR0A & (1<<RXC0)));
//return(UDR0);
//}
//
//int main()
//{
//char c;
//int x=0, y=0;
//
//UBRR0H=0;               //12번 비트가 의미를 가짐
//UBRR0L=8;               // 16Mhz, 115200baud
//
//UCSR0B=0x18;             //Receive(RX) 및 Transmit(TX) Enable
//UCSR0C=0x06;             //비동기, no parity, stop bit 1, 데이터 비트수 8-bit
//stdout=&mystdout;
//
//while(1)
//{
	//printf("x=%d y=%d",x,y);
//c=getchar0();
//printf("%c\n",c);
//
//c=c-'0';
//
//if(c>0 && c<10){
	//switch(c){
		//case UP:
		//y++;
		//break;
		//case DOWN:
		//y--;
		//break;
		//case LEFT:
		//x--;
		//break;
		//case RIGHT:
		//x++;
		//break;
	//}
	//printf("x=%d y=%d\n", x, y);
//}
//_delay_ms(200);
//}
//}



/////////////////////////////키패드 입력 숫자(1~9) 도트 매트릭스 표시
//#include <avr/io.h>
//#define F_CPU 16000000UL
//#include <util/delay.h>
//
//
//#define KEY_DDR DDRF
//#define KEY_PORT PORTF
//#define KEY_PIN PINF
//
//unsigned char key_scan(unsigned char a);
//int key_convert(unsigned char in);
//
//volatile const unsigned char number[9][8]={
	//{0xef,0xe7,0xeb,0xef,0xef,0xef,0xef,0x83},     // 1
	//{0xff,0xe7,0xdb,0xdf,0xef,0xf7,0xc3,0xff},     // 2
	//{0xff,0xe7,0xdb,0xdf,0xe7,0xdf,0xdb,0xe7},     // 3
	//{0xff,0xef,0xf7,0xeb,0xc1,0xef,0xef,0xff},     // 4
	//{0xff,0xc3,0xfb,0xe3,0xdf,0xdb,0xe7,0xff},     // 5
	//{0xff,0xc7,0xfb,0xe3,0xdb,0xdb,0xe7,0xff},     // 6
	//{0xff,0xc3,0xdb,0xdf,0xdf,0xdf,0xdf,0xff},     // 7
	//{0xff,0xe7,0xdb,0xdb,0xe7,0xdb,0xdb,0xe7},     // 8
	//{0xff,0xe7,0xdb,0xdb,0xc7,0xdf,0xdf,0xdf}		// 9
//};
//
//int main(){
//unsigned char in,in0,in1,in2,in3;
//
//DDRA=0xff;
//DDRE=0xff;
//
	//while (1)
	//{
	//// 키패드 첫번째줄 스캔
	//in0 = key_scan(0x01);
	//// 키패드 두번째줄 스캔
	//in1 = key_scan(0x02);
	//// 키패드 세번째줄 스캔
	//in2 = key_scan(0x04);
	//// 키패드 네번째줄 스캔
	//in3 = key_scan(0x08);
	//
	//in = (in3 | in2 | in1 | in0); //4줄 입력 값 취합
	//
	//in = key_convert(in);   //키 입력 값을 숫자로 변환
	//}
//}
//
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
//int key_convert(unsigned char in)
//{
//int returnvalue;
//int j=0;
//
//switch(in)
//{
//case 0x11 :
 //returnvalue = 1;
 //for(j=0; j<8; j++){
	//PORTA=number[0][j];
	//PORTE= 1<< j;
	//_delay_ms(1);
 //}
 //break;
 //
//case 0x21 :
 //returnvalue = 2;
  //for(j=0; j<8; j++){
	  //PORTA=number[1][j];
	  //PORTE= 1<< j;
	  //_delay_ms(1);
  //}
 //break;
 //
//case 0x41 :
 //returnvalue = 3;
   //for(j=0; j<8; j++){
	   //PORTA=number[2][j];
	   //PORTE= 1<< j;
	   //_delay_ms(1);
   //}
   //break;
   //
//case 0x12 :
 //returnvalue = 4;
   //for(j=0; j<8; j++){
	   //PORTA=number[3][j];
	   //PORTE= 1<< j;
	   //_delay_ms(1);
   //}
   //break;
   //
//case 0x22 :
 //returnvalue = 5;
    //for(j=0; j<8; j++){
	    //PORTA=number[4][j];
	    //PORTE= 1<< j;
	    //_delay_ms(1);
    //}
   //break;
   //
//case 0x42 :
 //returnvalue = 6;
    //for(j=0; j<8; j++){
	    //PORTA=number[5][j];
	    //PORTE= 1<< j;
	    //_delay_ms(1);
    //}
   //break;
   //
//case 0x14 :
 //returnvalue = 7;
    //for(j=0; j<8; j++){
	    //PORTA=number[6][j];
	    //PORTE= 1<< j;
	    //_delay_ms(1);
    //}
   //break;
   //
//case 0x24 :
 //returnvalue = 8;
    //for(j=0; j<8; j++){
	    //PORTA=number[7][j];
	    //PORTE= 1<< j;
	    //_delay_ms(1);
    //}
   //break;
   //
//case 0x44 :
 //returnvalue = 9;
    //for(j=0; j<8; j++){
	    //PORTA=number[8][j];
	    //PORTE= 1<< j;
	    //_delay_ms(1);
    //}
   //break;
//default : returnvalue = 0;  break;
//
//}
//return returnvalue;
//}



//////////////////////////////시리얼 좌표  + 도트  (미완성)
#include <avr/io.h>
#include <stdio.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define UP 2
#define DOWN 8
#define LEFT 4
#define RIGHT 6

static int putchar0(char c, FILE *stream);

static FILE mystdout =FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);

volatile const unsigned char dot[8][8]={
	{0xef,0xe7,0xeb,0xef,0xef,0xef,0xef,0x83},
	{0xff,0xe7,0xdb,0xdf,0xef,0xf7,0xc3,0xff},
	{0xff,0xe7,0xdb,0xdf,0xe7,0xdf,0xdb,0xe7},
	{0xff,0xef,0xf7,0xeb,0xc1,0xef,0xef,0xff},
	{0xff,0xc3,0xfb,0xe3,0xdf,0xdb,0xe7,0xff},
	{0xff,0xc7,0xfb,0xe3,0xdb,0xdb,0xe7,0xff},
	{0xff,0xc3,0xdb,0xdf,0xdf,0xdf,0xdf,0xff},
	{0xff,0xe7,0xdb,0xdb,0xe7,0xdb,0xdb,0xe7},
	{0xff,0xe7,0xdb,0xdb,0xc7,0xdf,0xdf,0xdf}
};

static int putchar0(char c, FILE *stream){
	if(c=='\n')
	putchar0('\r',stream);
	//while(!(UCSR0A & (1<<UDRE0)));
	while(!(UCSR0A & 0x20));
	UDR0=c;
	return 0;
}

char getchar0(){
	while(!(UCSR0A & (1<<RXC0)));
	return(UDR0);
}

int main(){
	char c;
	int x=0, y=0;

	DDRA=0xff;
	DDRE=0xff;

	UBRR0H=0;               //12번 비트가 의미를 가짐
	UBRR0L=8;               // 16Mhz, 115200baud

	UCSR0B=0x18;             //Receive(RX) 및 Transmit(TX) Enable
	UCSR0C=0x06;             //비동기, no parity, stop bit 1, 데이터 비트수 8-bit
	stdout=&mystdout;

	while(1){
		printf("x=%d y=%d",x,y);
		c=getchar0();
		printf("%c\n",c);

		c=c-'0';

		if(c>0 && c<10){
			switch(c){
				case UP:
				y++;
				PORTA= dot[x][y];
				PORTE= 1 << y;
				break;
				case DOWN:
				y--;
				PORTA= dot[x][y];
				PORTE= 1 >> y;
				break;
				case LEFT:
				x--;
				PORTA= dot[x][y];
				PORTE= 1 >> x;
				break;
				case RIGHT:
				x++;
				PORTA= dot[x][y];
				PORTE= 1 << x;
				break;
			}
			printf("x=%d y=%d\n", x, y);
		}
		_delay_ms(200);
	}
}




///////////////////////////////////////////////// TM_KEY100 4x4 Keypad
//#include <avr/io.h>
//
//#define F_CPU 16000000UL
//#include <util/delay.h>
//
//#define FND_DB_DDR		DDRC
//#define FND_DB_PORT		PORTC
//
//#define FND_CT_DDR		DDRG
//#define FND_CT_PORT		PORTG
//
//#define KEY_DDR DDRF
//#define KEY_PORT PORTF
//#define KEY_PIN PINF
//
//unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67};
//unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
//
//void display_fnd(unsigned int value);
//unsigned char key_scan(unsigned char a);
//int key_convert(unsigned char in);
//
//int main(void)
//{
//unsigned char in,in0,in1,in2,in3;
//
////FND를 사용하기 위해 PORTC,PORTG 사용
//FND_DB_DDR = 0xFF;
//FND_CT_DDR = 0x0F;
//
//while (1)
//{
//// 키패드 첫번째줄 스캔
//in0 = key_scan(0x01);
//// 키패드 두번째줄 스캔
//in1 = key_scan(0x02);
//// 키패드 세번째줄 스캔
//in2 = key_scan(0x04);
//// 키패드 네번째줄 스캔
//in3 = key_scan(0x08);
//
//in = (in3 | in2 | in1 | in0); //4줄 입력 값 취합
//
//in = key_convert(in);   //키 입력 값을 숫자로 변환
//
//display_fnd(in);    //FND에 표시
//}
//
//}
//
////FND에 숫자를 표시하는 함수
//void display_fnd(unsigned int key)
//{
//FND_DB_PORT = digit[key]; //key가 나타내는 수 표시
//FND_CT_PORT = fnd_sel[0]; // 가장 우측의 FND 선택
//_delay_ms(200);
//}
//
//
////키패드에 각 줄에 입력 값이 있는지 스캔하는 함수
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
//int key_convert(unsigned char in)
//{
//int returnvalue;
//
//switch(in)
//{
//case 0x11 : returnvalue = 1;  break;
//case 0x21 : returnvalue = 2;  break;
//case 0x41 : returnvalue = 3;  break;
//case 0x12 : returnvalue = 4;  break;
//case 0x22 : returnvalue = 5;  break;
//case 0x42 : returnvalue = 6;  break;
//case 0x14 : returnvalue = 7;  break;
//case 0x24 : returnvalue = 8;  break;
//case 0x44 : returnvalue = 9;  break;
//default : returnvalue = 0;  break;
//
//}
//return returnvalue;
//}