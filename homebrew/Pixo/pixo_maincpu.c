/*
author: Felipe Sanches <juca@members.fsf.org>
year: 2014
This code is released to the public domain
*/

#define SYSTEM ((char*) 0xC000)
#define P1 ((char*) 0xC001)
#define P2 ((char*) 0xC002)
#define DSW1 ((char*) 0xC003)
#define DSW2 ((char*) 0xC004)
#define SOUND_COMMAND ((char*) 0xC800)
#define HWCFG ((char*) 0xC804)
#define VIDEORAM ((char*) 0xD000) //videoram area: D000-D3FF
#define COLORRAM ((char*) 0xD400) //videoram area: D400-D7FF
#define SCROLLX ((char*) 0xD800)
#define SCROLLY ((char*) 0xD802)
#define VIDEOCFG ((char*) 0xD806)
#define SPRITERAM ((char*) 0xF000) //spriteram area: F000-FFFF

#define CLEAR_COLOR 0x80 //why?!
#define NO_SOUND 0xff

#define bool char
#define true 0xFF
#define false 0x00

char input_map;
char pixo_y;
bool can_increment;

//routine for placing a character on screen
void set_char(int x, int y, char char_code, char color){
	*(COLORRAM + 32*y + x) = color;
	*(VIDEORAM + 32*y + x) = char_code;
}

// Routine to print a line of text at a
// given screen coordinate
void print_line(char* str, int x, int y, char color){
	char* ptr = str;
	while (*ptr != 0){
		set_char(x, y++, *(ptr++) - 55, color);
	}
}

void clear_screen(){
	int i;
	for (i=0; i<1024; i++)
		*(COLORRAM+i) = CLEAR_COLOR;
}

#define BODY_COLOR 6

#define BLANK_CHAR ('B'-55)
#define PIXO_BODY_E ('C'-55)
#define PIXO_CLOSED_HEAD_E ('G'-55)
#define PIXO_CLOSED_MOUTH_E ('A'-55)
#define PIXO_CLOSED_LIPS_E ('M'-55)
#define PIXO_OPEN_HEAD_E ('T'-55)
#define PIXO_OPEN_MOUTH_E ('O'-55)
#define PIXO_OPEN_MOUTH2_E ('V'-55)
#define PIXO_OPEN_LIPS_E ('P'-55)
#define PIXO_OPEN_LIPS2_E ('I'-55)

void draw_head_open(int x, int y){
	set_char(x, y,   PIXO_OPEN_LIPS_E,  BODY_COLOR);
	set_char(x, y-1, PIXO_OPEN_MOUTH_E, BODY_COLOR);
	set_char(x, y-2, PIXO_OPEN_HEAD_E,  BODY_COLOR);
	set_char(x, y-3, PIXO_BODY_E,       BODY_COLOR);

	set_char(x-1, y,   PIXO_OPEN_LIPS2_E,  BODY_COLOR);
	set_char(x-1, y-1, PIXO_OPEN_MOUTH2_E, BODY_COLOR);

//This blank char is meant to gradually clear the screen as
// the aligator moves forward
//TODO: we should somehow redraw here what was previously drawn on screen
	set_char(x-1, y-2, BLANK_CHAR,         BODY_COLOR);
}

void draw_head_closed(int x, int y){
	set_char(x, y,   PIXO_CLOSED_LIPS_E,  BODY_COLOR);
	set_char(x, y-1, PIXO_CLOSED_MOUTH_E, BODY_COLOR);
	set_char(x, y-2, PIXO_CLOSED_HEAD_E,  BODY_COLOR);
	set_char(x, y-3, PIXO_BODY_E,       BODY_COLOR);

//These blank chars are meant to clear the previously opened mouth chars
//TODO: we should somehow redraw here what was previously drawn on screen
	set_char(x-1, y,   BLANK_CHAR, BODY_COLOR);
	set_char(x-1, y-1, BLANK_CHAR, BODY_COLOR);
	set_char(x-1, y-2, BLANK_CHAR, BODY_COLOR);
	set_char(x-1, y-3, BLANK_CHAR, BODY_COLOR);
}

void init_video(){
	*VIDEOCFG = 0x30; // enables bg / enables sprites / selects sprite3bank #0
	*HWCFG = 0x80; // unflip screen and enable chars

	clear_screen();
}

void init_system(){
	*SOUND_COMMAND = NO_SOUND;
	input_map = 0xFF;
	pixo_y = 8;
	can_increment = true;

	init_video();

	while(true) {
	__asm
    ei
  __endasm;
	}
}

void maybe_increment_y(){
	if (can_increment)
		pixo_y++;

	can_increment = false;
}

void button1_pressed(){
	draw_head_open(10, pixo_y);
	maybe_increment_y();
}

void button2_pressed(){
	draw_head_closed(10, pixo_y);
	maybe_increment_y();
}

void button3_pressed(){
	//nothing
}

void check_user_input(){
	char a = ~input_map ^ *P1;

	if ((a & (1 << 4))==0){
		button1_pressed();
		return;
	}

	if ((a & (1 << 5))==0){
		button2_pressed();
		return;
	}

	if ((a & (1 << 6))==0){
		button3_pressed();
		return;
	}

	can_increment = true;
}

void main_loop(){
	check_user_input();
}


