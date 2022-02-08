/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "sayori.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//sayori character structure
enum
{
	sayori_ArcMain_Idle0,
	sayori_ArcMain_Idle1,
	sayori_ArcMain_Idle2,
	sayori_ArcMain_Idle3,
	sayori_ArcMain_Left,
	sayori_ArcMain_Down,
	sayori_ArcMain_Up,
	sayori_ArcMain_Right,

	sayori_ArcMain_Zad0,
	sayori_ArcMain_Zad1,
	
	sayori_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[sayori_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_sayori;

//sayori character definitions
static const CharFrame char_sayori_frame[] = {
	{sayori_ArcMain_Idle0, {  0,   77, 88, 165}, { 80, 162}}, //0 idle 1
	{sayori_ArcMain_Idle0, { 88,   75, 88, 167}, { 81, 164}}, //1 idle 2
	{sayori_ArcMain_Idle0, {166,   0,  85, 169}, { 78, 166}}, //2 idle 3
	{sayori_ArcMain_Idle1, {  0,   0,  84, 169}, { 78, 166}}, //3 idle 4
	{sayori_ArcMain_Idle1, { 84,   0,  77, 169}, { 72, 166}}, //0 idle 1
	{sayori_ArcMain_Idle1, {161,   0,  77, 169}, { 71, 166}}, //1 idle 2
	{sayori_ArcMain_Idle2, {  0,   2,  63, 165}, { 59, 162}}, //2 idle 3
	{sayori_ArcMain_Idle2, { 63,   0,  64, 167}, { 63, 164}}, //3 idle 4
	{sayori_ArcMain_Idle2, {127,   0,  69, 167}, { 65, 164}}, //0 idle 1
	{sayori_ArcMain_Idle3, {  0,   2,  73, 167}, { 68, 164}}, //1 idle 2
	{sayori_ArcMain_Idle3, { 73,   0,  77, 169}, { 71, 166}}, //2 idle 3
	{sayori_ArcMain_Idle3, {150,   0,  77, 169}, { 72, 166}}, //3 idle 4
	
	{sayori_ArcMain_Left, {  0,   0, 61, 167}, { 61, 166}}, //4 left 1
	{sayori_ArcMain_Left, { 61,   4, 61, 163}, { 58, 162}}, //5 left 2
	{sayori_ArcMain_Left, {122,   4, 61, 163}, { 57, 162}}, //5 left 2
	
	{sayori_ArcMain_Down, {  0,   4, 62, 151}, { 51, 150}}, //6 down 1
	{sayori_ArcMain_Down, { 62,   1, 61, 154}, { 51, 153}}, //7 down 2
	{sayori_ArcMain_Down, {123,   0, 61, 155}, { 51, 154}}, //7 down 2
	
	{sayori_ArcMain_Up, {  0,   0, 73, 178}, { 66, 178}}, //8 up 1
	{sayori_ArcMain_Up, { 73,   1, 73, 177}, { 67, 177}}, //9 up 2
	{sayori_ArcMain_Up, {146,   5, 73, 173}, { 66, 173}}, //9 up 2
	
	{sayori_ArcMain_Right, {  0,   0, 68, 165}, { 59, 162}}, //10 right 1
	{sayori_ArcMain_Right, { 68,   4, 69, 161}, { 58, 158}}, //11 right 2
	{sayori_ArcMain_Right, {137,   7, 74, 158}, { 59, 155}}, //11 right 2

	{sayori_ArcMain_Zad0, {  0,   0, 68, 165}, {67, 162}}, //10 right 1
	{sayori_ArcMain_Zad0, { 68,   6, 65, 159}, {57, 156}}, //10 right 1
	{sayori_ArcMain_Zad0, {133,   7, 71, 158}, {60, 155}}, //10 right 1
	{sayori_ArcMain_Zad1, {  0,   0, 79, 156}, {64, 156}}, //10 right 1
	{sayori_ArcMain_Zad1, { 79,   0, 75, 156}, {60, 153}}, //10 right 1
	{sayori_ArcMain_Zad1, {154,   0, 73, 156}, {58, 153}}, //10 right 1
};

static const Animation char_sayori_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, ASCR_BACK, 0}}, //CharAnim_Idle
	{2, (const u8[]){ 24, 25, 26, 27, 28, 29, 29, 29, 29, 29, ASCR_BACK, 0}}, //special
	{2, (const u8[]){ 12, 13, 14, ASCR_BACK, 0}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 15, 16, 17, ASCR_BACK, 0}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 18, 19, 20, ASCR_BACK, 0}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 21, 22, 23, ASCR_BACK, 0}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//sayori character functions
void Char_sayori_SetFrame(void *user, u8 frame)
{
	Char_sayori *this = (Char_sayori*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_sayori_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_sayori_Tick(Character *character)
{
	Char_sayori *this = (Char_sayori*)character;
	
	//depressed anim and zoom lol
	if (stage.song_step == 751 && stage.stage_id == StageId_2_2)
		character->set_anim(character, CharAnim_Special);
	
	if (stage.song_step >= 749 && stage.song_step <= 769 && stage.stage_id == StageId_2_2) 
	{
		this->character.focus_x = FIXED_DEC(0,1);
		this->character.focus_y = FIXED_DEC(-100,1);
		this->character.focus_zoom = FIXED_DEC(14,10);
	}
	else 
	{
		this->character.focus_x = FIXED_DEC(35,1);
		this->character.focus_y = FIXED_DEC(-100,1);
		this->character.focus_zoom = FIXED_DEC(1,1);
	}

	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_sayori_SetFrame);
	Character_Draw(character, &this->tex, &char_sayori_frame[this->frame]);
}

void Char_sayori_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_sayori_Free(Character *character)
{
	Char_sayori *this = (Char_sayori*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_sayori_New(fixed_t x, fixed_t y)
{
	//Allocate sayori object
	Char_sayori *this = Mem_Alloc(sizeof(Char_sayori));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_sayori_New] Failed to allocate sayori object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_sayori_Tick;
	this->character.set_anim = Char_sayori_SetAnim;
	this->character.free = Char_sayori_Free;
	
	Animatable_Init(&this->character.animatable, char_sayori_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(35,1);
	this->character.focus_y = FIXED_DEC(-100,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SAYORI.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //sayori_ArcMain_Idle0
		"idle1.tim", //sayori_ArcMain_Idle1
		"idle2.tim", //sayori_ArcMain_Idle1
		"idle3.tim", //sayori_ArcMain_Idle1
		"left.tim",  //sayori_ArcMain_Left
		"down.tim",  //sayori_ArcMain_Down
		"up.tim",    //sayori_ArcMain_Up
		"right.tim", //sayori_ArcMain_Right
		"zad0.tim", //sayori_ArcMain_Right
		"zad1.tim", //sayori_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
