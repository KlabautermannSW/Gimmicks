/*
    ------------------------------------------------------------
                Uwe Jantzen
    ------------------------------------------------------------
    File:       microTelCon.c
    Project:    microTelCon
    Target:     PIC16F84A

    Contents:   Reads data from Conrad digital temperature time
                module (No 19 55 88) and sends the data converted
                onto two bytes via RS232.
                Using 9600bd, 8N1.
                Start is the byte 0xe5, end is a checksum calculated
                as the 8-bit-sum of the three bytes and then subtracted
                from 0x100.

    Author:     Uwe Jantzen
    ------------------------------------------------------------
    Start:      18.12.2005
    Date:       22.2.2006
    ------------------------------------------------------------
*/


#include <16F84A.h>
#use fast_io(B)


static char high_byte;
static char low_byte;
static char temp_is_read;
static char got_first_bit;
static char bit_number;
static BOOLEAN rising_edge;


/*
    function:   get_temperature
    -----------------------
    in:         -
    out:        -
    -----------------------
    comment:    Analyze bitstream sent from thermometer.
*/
#int_ext
get_temperature( )
    {
    int8 time;
    int8 level;
    char * p;

    if( rising_edge )                           // was it a falling edge ?
        {
        ext_int_edge(H_TO_L);                   // then wait for rising edge
        time = get_timer0();                    // pulse width
        level = input(PIN_B1);
        level = (level) ? 0 : 1;
        if( time < 15 )                         // normal clock pulse
            {
            if( bit_number < 4 )
                {
                high_byte <<= 1;                // multiplicate by 2
                high_byte += level;             // add bit
                }
            else
                {
                low_byte <<= 1;                 // multiplicate by 2
                low_byte += level;              // add bit
                }
            ++bit_number;                       // increment bit number
            if( bit_number > 11 )
                temp_is_read = TRUE;
            }
        else if( time > 50 )                    // start pulse
            {
            bit_number = 0;
            high_byte = level;
            low_byte = 0;
            }
        }
    else
        {
        ext_int_edge(L_TO_H);                   // wait for falling edge
        set_timer0(0);                          // start timer
        }

    rising_edge = !rising_edge;
    }


/*
    function:	send_rs
    -----------------------
    in:         int8 data
    out:        -
    -----------------------
    comment:    Sends one character, using 8N1.
                While port is directly connected to PC's serial port
                line is "active high"!
                Transmit pin (B4) has to set to output before first
                call of this function !
*/
void send_rs( unsigned char data )
    {
    #define PORTB	6
    int8 count_bits;
    int8 delay_counter;

    #asm
    send_rs:    movlw    8                      // send 8 bits
                movwf   count_bits              // store number of bits
                bsf     PORTB, 4                // start bit
                call    wait
    bit_loop:   btfss data, 0                   // test if data bit is set
                goto    transmit_0              // bit is clear
                goto    transmit_1              // bit is set
    transmit_0: bsf     PORTB, 4                // transmit a 0 (inverted, you know ?)
                call    wait
                goto    next_bit
    transmit_1:	bcf     PORTB, 4                // transmit a 1 (inverted, you know ?)
                call    wait
                goto    next_bit                // necessary for equal timing for each bit
    next_bit:   rrf     data, 1                 // next bit into position 0
                decfsz  count_bits, 1           // last bit ?
                goto    bit_loop                // no
                bcf     PORTB, 4                // stop bit
                call    wait
                return                          // that's it

    wait:       movlw   33                      // so to get 9600 baud
                movwf   delay_counter
    waitloop:   decfsz  delay_counter, 1        // count down, jump on zero
                goto    waitloop                // loop again
                return                          // finished
    #endasm
    }


/*	function:	main
    -----------------------
    in:         -
    out:        -
    -----------------------
    comment:    The main function.
                It prepares variables, port, timer, interrupt.
                An endless loop sends the temperature telegram
                when all data bits are got by the interrupt
                function.
*/
void main( void )
    {
    int8 cs;
    char c;

    temp_is_read = FALSE;
    got_first_bit = FALSE;
    rising_edge = FALSE;

    set_tris_B(0xef);                           // B0, B1 are inputs, B4 is output, the others don't care
    ext_int_edge(H_TO_L);
    setup_timer_0(RTCC_INTERNAL|RTCC_DIV_16);   // counts in steps of 16 μSec

    enable_interrupts(INT_EXT);                 // now waiting for something from the thermometer
    enable_interrupts(GLOBAL);

    while( TRUE )                               // run forever ...
        {
        if( temp_is_read )
            {
            cs = 0xe5 + high_byte + low_byte;
            cs = ~cs;
            cs += 1;
            send_rs(0xe5);                      // start byte
            send_rs(high_byte);
            send_rs(low_byte);
            send_rs(cs);                        // checksum
            temp_is_read = FALSE;
            }
        }
    }
