/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "monikapix.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Dad character structure
enum
{
	monikapix_ArcMain_monikapix0,
	monikapix_ArcMain_monikapix1,
	
	monikapix_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[monikapix_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_monikapix;

//Dad character definitions
static const CharFrame char_monikapix_frame[] = {
	{monikapix_ArcMain_monikapix0, {  0,   0, 76, 118}, { 35, 118}}, //0 idle 1
	{monikapix_ArcMain_monikapix0, { 76,   0, 75, 114}, { 33, 114}}, //1 idle 2
	{monikapix_ArcMain_monikapix0, {151,   0, 76, 117}, { 35, 117}}, //2 idle 3
	{monikapix_ArcMain_monikapix0, {  0, 118, 76, 120}, { 35, 120}}, //3 idle 4
	{monikapix_ArcMain_monikapix0, { 76, 114, 79, 114}, { 38, 114}}, //4 idle 5
	{monikapix_ArcMain_monikapix0, {155, 117, 76, 117}, { 35, 117}}, //5 idle 6
	
	{monikapix_ArcMain_monikapix1, {122,   0, 64, 114}, { 25, 114}}, //6 left 1
	{monikapix_ArcMain_monikapix1, {186,   0, 63, 114}, { 25, 114}}, //7 left 2
	
	{monikapix_ArcMain_monikapix1, {  0,   0, 61, 99}, { 35, 99}}, //8 down 1
	{monikapix_ArcMain_monikapix1, { 61,   0, 61, 100}, { 35, 100}}, //9 down 2
	
	{monikapix_ArcMain_monikapix1, {150, 114, 46, 116}, { 23, 116}}, //10 up 1
	{monikapix_ArcMain_monikapix1, {196, 114, 47, 116}, { 24, 116}}, //11 up 2
	
	{monikapix_ArcMain_monikapix1, {  0,  99, 75, 110}, { 29, 110}}, //12 right 1
	{monikapix_ArcMain_monikapix1, { 75, 114, 75, 110}, { 27, 110}}, //13 right 2
};

static const Animation char_monikapix_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 4, 4, 5, ASCR_BACK, 0}}, //CharAnim_Idle
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
void Char_monikapix_SetFrame(void *user, u8 frame)
{
	Char_monikapix *this = (Char_monikapix*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_monikapix_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_monikapix_Tick(Character *character)
{
	Char_monikapix *this = (Char_monikapix*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_monikapix_SetFrame);
	Character_Draw(character, &this->tex, &char_monikapix_frame[this->frame]);
}

void Char_monikapix_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_monikapix_Free(Character *character)
{
	Char_monikapix *this = (Char_monikapix*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_monikapix_New(fixed_t x, fixed_t y)
{
	//Allocate monikapix object
	Char_monikapix *this = Mem_Alloc(sizeof(Char_monikapix));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_monikapix_New] Failed to allocate monikapix object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_monikapix_Tick;
	this->character.set_anim = Char_monikapix_SetAnim;
	this->character.free = Char_monikapix_Free;
	
	Animatable_Init(&this->character.animatable, char_monikapix_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;
	
	this->character.focus_x = FIXED_DEC(24,1);
	this->character.focus_y = FIXED_DEC(-66,1);
	this->character.focus_zoom = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MONIKAP.ARC;1");
	
	const char **pathp = (const char *[]){
		"monika0.tim", //monikapix_ArcMain_monikapix0
		"monika1.tim", //monikapix_ArcMain_monikapix1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
