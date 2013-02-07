
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>

#define F_CPU 18432000

#define LED_R_PIN_OUT DDRB |= (1<<PD4);
#define LED_G_PIN_OUT DDRB |= (1<<PD3);
#define LED_B_PIN_OUT DDRB |= (1<<PD2);

#define LED_R_ON		PORTB |= (1<<PD4);
#define LED_R_OFF		PORTB &= ~(1<<PD4);
#define LED_B_ON		PORTB |= (1<<PD3);
#define LED_B_OFF		PORTB &= ~(1<<PD3);
#define LED_G_ON		PORTB |= (1<<PD2);
#define LED_G_OFF		PORTB &= ~(1<<PD2);


#define DMX_BAUD 230400 // 115200 ; 460800 (UBRR=4 U2X=1)

volatile unsigned int timer;

volatile unsigned int address = 0;
volatile unsigned char lights[3];

//############################################################################
//DMX Senderoutine
ISR (USART_RX_vect)
//############################################################################
{
	static unsigned int dmx_channel_rx_count = 0;
	static unsigned char dmx_valid = 0;
	unsigned char tmp = 0;
	
	tmp =  UDR;
	
	if(UCSRA&(1<<FE))
	{
		if(dmx_channel_rx_count > 1)
		{
			dmx_lost = 0;
		}
		dmx_channel_rx_count = 0;	
		dmx_buffer[0] = tmp;
		if(dmx_buffer[0] == 0)
		{
			dmx_valid = 1;
			dmx_channel_rx_count++;
		}
		else
		{
			dmx_valid = 0;
		}
		return;
	}
	
	if(dmx_valid)
	{
		if(dmx_channel_rx_count == dmx_adresse) dmx_buffer[1] = tmp;
		if(dmx_channel_rx_count == dmx_adresse+1) dmx_buffer[2] = tmp;
		if(dmx_channel_rx_count == dmx_adresse+2) dmx_buffer[3] = tmp;
	
		if(dmx_channel_rx_count < 514)
		{
			dmx_channel_rx_count++;
		}
		return;
	}
}

//############################################################################
//Hier wird die Zeit gezählt (Tick 1ms)
ISR (TIMER0_COMPA_vect)
//############################################################################
{
	++timer;
}

//############################################################################
//Hier wird die Zeit gezählt (Tick 1ms)
ISR (TIMER1_COMPA_vect)
//############################################################################
{
	static unsigned char pwm_counter = 0;

	pwm_counter++;
	if(pwm_counter == 255)
	{
		pwm_counter = 0;
	}
	
	if(pwm_counter >= lights[0]) 
	{
		LED_R_OFF;
	}
	else
	{
		LED_R_ON;
	}
	if(pwm_counter >= lights[1])
	{
		LED_G_OFF;
	}
	else
	{
		LED_G_ON;
	}
	if(pwm_counter >= lights[2]) 
	{
		LED_B_OFF;
	}
	else
	{
		LED_B_ON;
	}

}

//############################################################################
//State: Test
void teststate (void) {
	static unsigned char direction = 0;
	if (timer<1000)
		return;

	timer = 0;

	if (direction == 0) { // up
		if (lights[0] < 255)
			++lights[0];
		if (lights[0] > 112) {
			if (lights[1] < 255)
				++lights[1];
			if (lights[1] > 112) {
				if (lights[2] < 255)
					++lights[2];
				else direction = 1;
			}
		}
	} else { // down
		if (lights[0] > 0)
			--lights[0];
		if (lights[0] < 112) {
			if (lights[1] > 0)
				--lights[1];
			if (lights[1] < 112) {
				if (lights[2] > 0)
					--lights[2];
				else direction = 0;
			}
		}
	}
	
}

//############################################################################
//Hauptprogramm
int main (void) 
//############################################################################
{  
	unsigned int dmx_adresse_tmp;

	//Init usart DMX-BUS
	UBRRH   = (unsigned char)(F_CPU / (DMX_BAUD * 16L) - 1)>>8;
	UBRRL   = (unsigned char)(F_CPU / (DMX_BAUD * 16L) - 1);
	UCSRB|=(1 << RXEN | 1 << RXCIE | 1 << TXEN);
	UCSRC|=(1<<USBS); //USBS0 2 Stop bits
	
	//Timer0 Timecounter für DMX Ausfall und PWM
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS00);
	TCCR1A |= (1<<WGM11);
	TCCR1B |= (1<<CS10);
	OCR0A = F_CPU/1024/100 - 1; //Tick 1ms
	OCR1A = F_CPU/1024/100 - 1; //Tick 1ms
	TIMSK |= (1<<OCIE0A)|(1<<OCIE1A);
		
	LED_R_PIN_OUT;
	LED_G_PIN_OUT;
	LED_B_PIN_OUT;
	
	LED_R_ON;
	for(unsigned long tmp = 0;tmp<500000;tmp++)asm("nop");
	LED_R_OFF;
	LED_G_ON;
	for(unsigned long tmp = 0;tmp<500000;tmp++)asm("nop");
	LED_G_OFF;
	LED_B_ON;
	for(unsigned long tmp = 0;tmp<500000;tmp++)asm("nop");
	LED_B_OFF;
	
	sei();//Globale Interrupts Enable
		
	PORTB |= 0xFF;
	PORTD |= (1<<PD6);
	
	DDRD |=(1<<PD2);
	PORTD &=~(1<<PD2);

	while(1) {
		asm("nop");
	}
}

