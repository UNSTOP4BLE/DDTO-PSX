/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bigmpix.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Dad character structure
enum
{
	bigmpix_ArcMain_idle0,
	bigmpix_ArcMain_idle1,
	bigmpix_ArcMain_hit0,
	bigmpix_ArcMain_hit1,
	bigmpix_ArcMain_hit2,
	bigmpix_ArcMain_hit3,
	bigmpix_ArcMain_hit4,
	
	bigmpix_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[bigmpix_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_bigmpix;

//Dad character definitions
static const CharFrame char_bigmpix_frame[] = {
	{bigmpix_ArcMain_idle0, {  0,   0, 82, 129}, { 69, 117}}, //0 idle 1
	{bigmpix_ArcMain_idle0, { 82,   0, 82, 130}, { 69, 118}}, //1 idle 2
	{bigmpix_ArcMain_idle0, {164,   0, 83, 124}, { 69, 112}}, //2 idle 3
	{bigmpix_ArcMain_idle1, {  0,   0, 82, 128}, { 69, 116}}, //3 idle 4
	{bigmpix_ArcMain_idle1, { 82,   0, 82, 129}, { 69, 117}}, //4 idle 5
	{bigmpix_ArcMain_idle1, {164,   0, 82, 128}, { 69, 116}}, //5 idle 6
	
	{bigmpix_ArcMain_hit0, {164,   0, 87, 126}, { 73, 114}}, //6 left 1
	{bigmpix_ArcMain_hit0, {  0, 116, 85, 128}, { 72, 116}}, //7 left 2
	
	{bigmpix_ArcMain_hit0, {  0,   0, 82, 116}, { 69, 104}}, //8 down 1
	{bigmpix_ArcMain_hit0, { 82,   0, 82, 119}, { 69, 107}}, //9 down 2
	
	{bigmpix_ArcMain_hit0, { 85, 119, 82, 120}, { 69, 118}}, //10 up 1
	{bigmpix_ArcMain_hit0, {167, 126, 82, 119}, { 69, 116}}, //11 up 2
	
	{bigmpix_ArcMain_hit1, {  0,   0, 92, 130}, { 68, 118}}, //12 right 1
	{bigmpix_ArcMain_hit1, { 92,   0, 93, 129}, { 67, 117}}, //13 right 2
};

static const Animation char_bigmpix_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, ASCR_BACK, 0}}, //CharAnim_Idle
	{2, (const u8[]){ 6,  7, ASCR_BACK, 0}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 0}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 10, 11, ASCR_BACK, 0}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 12, 13, ASCR_BACK, 0}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
};

//Dad character functions
void Char_bigmpix_SetFrame(void *user, u8 frame)
{
	Char_bigmpix *this = (Char_bigmpix*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bigmpix_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_bigmpix_Tick(Character *character)
{
	Char_bigmpix *this = (Char_bigmpix*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_bigmpix_SetFrame);
	Character_Draw(character, &this->tex, &char_bigmpix_frame[this->frame]);
}

void Char_bigmpix_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_bigmpix_Free(Character *character)
{
	Char_bigmpix *this = (Char_bigmpix*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_bigmpix_New(fixed_t x, fixed_t y)
{
	//Allocate bigmpix object
	Char_bigmpix *this = Mem_Alloc(sizeof(Char_bigmpix));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_bigmpix_New] Failed to allocate bigmpix object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_bigmpix_Tick;
	this->character.set_anim = Char_bigmpix_SetAnim;
	this->character.free = Char_bigmpix_Free;
	
	Animatable_Init(&this->character.animatable, char_bigmpix_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 8;
	
	this->character.focus_x = FIXED_DEC(-25,1);
	this->character.focus_y = FIXED_DEC(-59,1);
	this->character.focus_zoom = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BIGMPIX.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //bigmpix_ArcMain_bigmpix0
		"idle1.tim", //bigmpix_ArcMain_bigmpix1
		"hit0.tim",
		"hit1.tim",
		"hit2.tim",
		"hit3.tim",
		"hit4.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
