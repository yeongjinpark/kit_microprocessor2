//mission 시리얼+수분센서
#include <avr/io.h>
#include <stdio.h>
#define F_CPU 16000000UL
#include <util/delay.h>


#define delaytime 30
#define  SOIL1 300
#define  SOIL2 395
#define  SOIL3 490
#define  SOIL4 585
#define  SOIL5 680
#define  SOIL6 775
#define  SOIL7 870
#define  SOIL8 950

#define  OCR1_R OCR1A
#define  OCR1_G OCR1B
#define  OCR1_B OCR1C

unsigned char redV;
unsigned char greenV;
unsigned char blueV;

unsigned char digit[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67};   //0~9까지 설정
unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};                           //fnd 자리 설정
unsigned char fnd[4];

static int putchar0(char c,FILE *stream);
static FILE mystdout=FDEV_SETUP_STREAM(putchar0,NULL,_FDEV_SETUP_WRITE);

int putchar0(char c, FILE *stream)
{
	if(c=='\n')
	putchar0('\r',stream);                   //1charater 수신하는 함수

	while(!(UCSR0A & (1<<UDRE0)));          //UCSR0A 5번 비트 = UDRE0
	UDR0=c;                              //1charater 전송
	return 0;
}

char getchar0()                        //1charater 수신하는 함수
{
	while(!(UCSR0A & (1<<RXC0)));          //UCSR0A 7번 비트= RXC
	return(UDR0);                          //1charater 수신
}

void init_uart()
{
	UBRR0H=0;               //12번 비트가 의미를 가짐
	UBRR0L=8;               // 16Mhz, 115200baud

	UCSR0B=0x18;             //Receive(RX) 및 Transmit(TX) Enable
	UCSR0C=0x06;              //UATR 모드, 8비트 데이터, No 패리티, 1 스탑비트
}


void init_adc();
unsigned short read_adc();
void show_adc(unsigned short value);
void display_fnd(unsigned short value);

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


int main(void)
{
	char c;
	unsigned short value;
	init_uart();
	stdout= &mystdout;
	
	DDRB= _BV(5) | _BV(6) | _BV(7);
	timer_init();
	
	DDRA=0xff;
	DDRC=0xff;
	DDRG=0x0f;
	init_adc();
	
	while(1)
	{
		value=read_adc();
		printf("1.value:");
		c=getchar0();
		c=c-'0';
		if(c==1){
			printf("%d\n",value);
		}
		else{
			printf("%d\n",value);
		}
		show_adc(value);
		display_fnd(value);

	}
}


void init_adc(){
	ADMUX=0x00;
	//REFS(1:0)="00" AREF(+5V)기준전압 사용
	//ADLAR='0' 디폴트 오른쪽 정렬
	//MUX(4:0)="00000" ADC0 사용, 단극입력
	ADCSRA=0X87;
	//ADEN='1' ADC를 Enable
	//ADFR='0' single conversion 모드
	//ADPS(2:0)="111" 프리스케일러 128분주
}

unsigned short read_adc(){
	unsigned char adc_low, adc_high;
	unsigned short value;
	ADCSRA |= (1<< ADSC);
	
	while((ADCSRA  &(1<<ADIF))==0)
	;
	adc_low=ADCL;
	adc_high =ADCH;
	value=(adc_high << 8)| adc_low;
	
	return value;
	
}

void show_adc(unsigned short value){
	if(value <= SOIL1){
		redV=0;
		blueV=0;
		greenV=85;
		set_color(redV,greenV,blueV);
		PORTA = 0x80;
		_delay_ms(delaytime);
	}
	
	else if(value <= SOIL2){
		redV=0;
		blueV=0;
		greenV=170;
		set_color(redV,greenV,blueV);
		PORTA = 0x40;
		_delay_ms(delaytime);
	}
	
	else if(value <= SOIL3){
		redV=0;
		blueV=0;
		greenV=255;
		set_color(redV,greenV,blueV);
		PORTA = 0x20;
		_delay_ms(delaytime);
	}
	
	else if(value <= SOIL4){
		redV=0;
		blueV=75;
		greenV=0;
		set_color(redV,greenV,blueV);
		PORTA = 0x10;
		_delay_ms(delaytime);
	}
	
	else if(value <= SOIL5){
		redV=0;
		blueV=170;
		greenV=0;
		set_color(redV,greenV,blueV);
		PORTA = 0x08;
		_delay_ms(delaytime);
	}
	
	else if(value <= SOIL6){
		redV=75;
		blueV=0;
		greenV=0;
		set_color(redV,greenV,blueV);
		PORTA = 0x04;
		_delay_ms(delaytime);
	}
	
	else if(value <= SOIL7){
		redV=170;
		blueV=0;
		greenV=0;
		set_color(redV,greenV,blueV);
		PORTA = 0x02;
		_delay_ms(delaytime);
	}
	
	else if(value <= SOIL8){
		redV=255;
		blueV=0;
		greenV=0;
		set_color(redV,greenV,blueV);
		PORTA = 0x01;
		_delay_ms(delaytime);
	}
	
	else{
		redV=0;
		blueV=0;
		greenV=0;
		set_color(redV,greenV,blueV);
		PORTA=0x00;
		_delay_ms(delaytime);
	}
}

void display_fnd(unsigned short value)   //fnd출력하는 함수
{
	int i;  //반복 루프를 위한 변수 선언
	
	fnd[3] =(value/1000)%10;   //천 자리 숫자 저장
	fnd[2] =(value/100)%10;   //백 자리 숫자 저장
	fnd[1] =(value/10)%10;    //십 자리 숫자 저장
	fnd[0] =value%10;         //일 자리 숫자 저장
	
	for(int ms=0; ms<50; ms++){   //ms 50번 반복 루프 생성
		for(i=0;i<4;i++)           //i  4번 반복 루프 생성
		{
			{
				PORTC = digit[fnd[i]];    //fnd[i]값에 따라 fnd 점등
			}

			PORTG = fnd_sel[i];    //fnd_sel[4]={0x01,0x02,0x04,0x08};  // 점등되는 자리 지정
			_delay_ms(5);           //지연율 설정
		}
	}
}


//#include <avr/io.h>
//#define F_CPU 16000000UL
//#include <util/delay.h>
//
//#define  DRY_SOIL 300
//#define  HUMID_SOIL 700
//#define  IN_WATER 950
//
//void init_adc();
//unsigned short read_adc();
//void show_adc(unsigned short value);
//
//
//int main(void)
//{
	//unsigned short value;
	//DDRA=0xff;
	//init_adc();
	//
	//while(1)
	//{
		//value=read_adc();
		//show_adc(value);
		//_delay_ms(500);
	//}
//}
//
//void init_adc(){
	//ADMUX=0x00;
	//
	//ADCSRA=0X87;
//}
//
//unsigned short read_adc(){
	//unsigned char adc_low, adc_high;
	//unsigned short value;
	//ADCSRA |= (1<< ADSC);
	//
	//while((ADCSRA  &(1<<ADIF))==0)
	//;
	//adc_low=ADCL;
	//adc_high =ADCH;
	//value=(adc_high << 8)| adc_low;
	//
	//return value;
	//
//}
//
//void show_adc(unsigned short value){
	//if(value <= DRY_SOIL)
	//PORTA=0xff;
	//
	//else if(value <= HUMID_SOIL)
	//PORTA=0x55;
	//
	//else if(value <= IN_WATER)
	//PORTA=0x11;
	//
	//else
	//PORTA=0x00;
//}



//#include <avr/io.h>
//#define F_CPU 16000000UL
//#include <util/delay.h>
//
//#define delaytime 30
//#define  SOIL1 300
//#define  SOIL2 395
//#define  SOIL3 490
//#define  SOIL4 585
//#define  SOIL5 680
//#define  SOIL6 775
//#define  SOIL7 870
//#define  SOIL8 950
//
//#define  OCR1_R OCR1A
//#define  OCR1_G OCR1B
//#define  OCR1_B OCR1C
//
//unsigned char redV;
//unsigned char greenV;
//unsigned char blueV;
//
//unsigned char digit[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67};   //0~9까지 설정
//unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};                           //fnd 자리 설정
//unsigned char fnd[4];
//
//void init_adc();
//unsigned short read_adc();
//void show_adc(unsigned short value);
//void display_fnd(unsigned short value);
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
//int main(void)
//{
	//unsigned short value;
	//DDRB= _BV(5) | _BV(6) | _BV(7);
	//timer_init();
	//
	//DDRA=0xff;
	//DDRC=0xff;
	//DDRG=0x0f;
	//init_adc();
	//
	//while(1)
	//{
		//value=read_adc();
		//show_adc(value);
		//display_fnd(value);
		////_delay_ms(500);
		//
	//}
//}
//
//void init_adc(){
	//ADMUX=0x00;
	//
	//ADCSRA=0X87;
//}
//
//unsigned short read_adc(){
	//unsigned char adc_low, adc_high;
	//unsigned short value;
	//ADCSRA |= (1<< ADSC);
	//
	//while((ADCSRA  &(1<<ADIF))==0)
	//;
	//adc_low=ADCL;
	//adc_high =ADCH;
	//value=(adc_high << 8)| adc_low;
	//
	//return value;
	//
//}
//
//void show_adc(unsigned short value){
	//if(value <= SOIL1){
		//redV=0;
		//blueV=0;
		//greenV=85;
		//set_color(redV,greenV,blueV);
		//PORTA = 0x80; 
		//_delay_ms(delaytime);
	//}
	//
	//else if(value <= SOIL2){
		//redV=0;
		//blueV=0;
		//greenV=170;
		//set_color(redV,greenV,blueV);
		//PORTA = 0x40;
		//_delay_ms(delaytime);
	//}
	//
	//else if(value <= SOIL3){
		//redV=0;
		//blueV=0;
		//greenV=255;
		//set_color(redV,greenV,blueV);
		//PORTA = 0x20;
		//_delay_ms(delaytime);
	//}
	//
	//else if(value <= SOIL4){
		//redV=0;
		//blueV=75;
		//greenV=0;
		//set_color(redV,greenV,blueV);
		//PORTA = 0x10;
		//_delay_ms(delaytime);
	//}
	//
	//else if(value <= SOIL5){
		//redV=0;
		//blueV=170;
		//greenV=0;
		//set_color(redV,greenV,blueV);
		//PORTA = 0x08;
		//_delay_ms(delaytime);
	//}
	//
	//else if(value <= SOIL6){
		//redV=75;
		//blueV=0;
		//greenV=0;
		//set_color(redV,greenV,blueV);
		//PORTA = 0x04; 
		//_delay_ms(delaytime);
	//}
	//
	//else if(value <= SOIL7){
		//redV=170;
		//blueV=0;
		//greenV=0;
		//set_color(redV,greenV,blueV);
		//PORTA = 0x02; 
		//_delay_ms(delaytime);
	//}
	//
	//else if(value <= SOIL8){
		//redV=255;
		//blueV=0;
		//greenV=0;
		//set_color(redV,greenV,blueV);
		//PORTA = 0x01; 
		//_delay_ms(delaytime);
	//}
	//
	//else{
		//redV=0;
		//blueV=0;
		//greenV=0;
		//set_color(redV,greenV,blueV);
		//PORTA=0x00;
		//_delay_ms(delaytime);
	//}
//}
//
//void display_fnd(unsigned short value)   //fnd출력하는 함수
//{
	//int i;  //반복 루프를 위한 변수 선언
	//
	//fnd[3] =(value/1000)%10;   //천 자리 숫자 저장
	//fnd[2] =(value/100)%10;   //백 자리 숫자 저장
	//fnd[1] =(value/10)%10;    //십 자리 숫자 저장
	//fnd[0] =value%10;         //일 자리 숫자 저장
	//
	//for(int ms=0; ms<50; ms++){   //ms 50번 반복 루프 생성
		//for(i=0;i<4;i++)           //i  4번 반복 루프 생성
		//{
			//{
				//PORTC = digit[fnd[i]];    //fnd[i]값에 따라 fnd 점등
			//}
//
			//PORTG = fnd_sel[i];    //fnd_sel[4]={0x01,0x02,0x04,0x08};  // 점등되는 자리 지정
			//_delay_ms(5);           //지연율 설정
		//}
	//}
//}














