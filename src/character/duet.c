/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "duet.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Dad character structure
enum
{
	duet_ArcMain_duet0,
	duet_ArcMain_duet1,
	duet_ArcMain_duet2,
	duet_ArcMain_duet3,
	duet_ArcMain_duet4,
	duet_ArcMain_duet5,
	
	duet_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[duet_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_duet;

//Dad character definitions
static const CharFrame char_duet_frame[] = {
	{duet_ArcMain_duet0, {  0,   0,  82, 116}, { 27, 115}}, //0 idle 1
	{duet_ArcMain_duet0, { 82,   0,  84, 113}, { 28, 112}}, //1 idle 2
	{duet_ArcMain_duet0, {  0, 116, 102, 113}, { 36, 112}}, //2 idle 3
	{duet_ArcMain_duet0, {102, 113, 102, 115}, { 36, 114}}, //3 idle 4
	{duet_ArcMain_duet1, {  0,   0,  97, 116}, { 33, 115}}, //4 idle 5
	{duet_ArcMain_duet1, { 97,   0,  84, 113}, { 28, 112}}, //5 idle 6
	
	{duet_ArcMain_duet2, {  0,   0, 80, 114}, { 34, 112}}, //6 left 1
	{duet_ArcMain_duet2, { 80,   0, 80, 114}, { 34, 112}}, //7 left 2
	
	{duet_ArcMain_duet1, {  0, 116, 91, 111}, { 34, 110}}, //8 down 1
	{duet_ArcMain_duet1, { 91, 113, 91, 111}, { 34, 110}}, //9 down 2
	
	{duet_ArcMain_duet3, {  0,   0, 81, 116}, { 31, 115}}, //10 up 1
	{duet_ArcMain_duet3, { 81,   0, 81, 116}, { 31, 115}}, //11 up 2
	
	{duet_ArcMain_duet2, {  0, 114, 100, 113}, { 31, 111}}, //12 right 1
	{duet_ArcMain_duet2, {100, 114, 101, 113}, { 31, 111}}, //13 right 2

	//alt
	{duet_ArcMain_duet3, { 93, 117, 95, 115}, { 31, 114}}, //14 leftb 1
	{duet_ArcMain_duet4, {  0,   0, 96, 116}, { 32, 115}}, //15 leftb 2
	
	{duet_ArcMain_duet3, {162,   0, 94, 113}, { 29, 112}}, //16 downb 1
	{duet_ArcMain_duet3, {  0, 116, 93, 113}, { 28, 112}}, //17 downb 2
	
	{duet_ArcMain_duet4, { 90, 115, 93, 123}, { 31, 122}}, //18 upb 1
	{duet_ArcMain_duet5, {  0,   0, 94, 123}, { 31, 122}}, //19 upb 2
	
	{duet_ArcMain_duet4, { 96,   0, 90, 115}, { 26, 114}}, //20 rightb 1
	{duet_ArcMain_duet4, {  0, 116, 90, 115}, { 26, 114}}, //21 rightb 2
};

static const Animation char_duet_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 0, 0, 1, 2, 3, 4, 5, ASCR_BACK, 0}}, //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //special	
	{2, (const u8[]){ 6,  7, ASCR_BACK, 0}},             //CharAnim_Left
	{2, (const u8[]){ 14, 15, ASCR_BACK, 0}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 0}},             //CharAnim_Down
	{2, (const u8[]){ 16, 17, ASCR_BACK, 0}},       //CharAnim_DownAlt
	{2, (const u8[]){ 10, 11, ASCR_BACK, 0}},             //CharAnim_Up
	{2, (const u8[]){ 18, 19, ASCR_BACK, 0}},       //CharAnim_UpAlt
	{2, (const u8[]){ 12, 13, ASCR_BACK, 0}},             //CharAnim_Right
	{2, (const u8[]){ 20, 21, ASCR_BACK, 0}},       //CharAnim_RightAlt
};

static const Animation char_duet_animb[CharAnim_Max] = {
	{2, (const u8[]){ 0, 0, 0, 1, 2, 3, 4, 5, ASCR_BACK, 0}}, //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}}, //special
	{2, (const u8[]){ 14, 15, ASCR_BACK, 0}},             //CharAnim_Left
	{2, (const u8[]){ 14, 15, ASCR_BACK, 0}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 16, 17, ASCR_BACK, 0}},             //CharAnim_Down
	{2, (const u8[]){ 16, 17, ASCR_BACK, 0}},       //CharAnim_DownAlt
	{2, (const u8[]){ 18, 19, ASCR_BACK, 0}},             //CharAnim_Up
	{2, (const u8[]){ 18, 19, ASCR_BACK, 0}},       //CharAnim_UpAlt
	{2, (const u8[]){ 20, 21, ASCR_BACK, 0}},             //CharAnim_Right
	{2, (const u8[]){ 20, 21, ASCR_BACK, 0}},       //CharAnim_RightAlt
};


//Dad character functions
void Char_duet_SetFrame(void *user, u8 frame)
{
	Char_duet *this = (Char_duet*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_duet_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_duet_Tick(Character *character)
{
	Char_duet *this = (Char_duet*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	if (stage.stage_id == StageId_1_2 && stage.song_step >= 712 && stage.song_step <= 724)
		Animatable_Animate(&character->animatableb, (void*)this, Char_duet_SetFrame);
	else if (stage.stage_id == StageId_1_2 && stage.song_step >= 745 && stage.song_step <= 756)
		Animatable_Animate(&character->animatableb, (void*)this, Char_duet_SetFrame);
	else
		Animatable_Animate(&character->animatable, (void*)this, Char_duet_SetFrame);
	Character_Draw(character, &this->tex, &char_duet_frame[this->frame]);
}

void Char_duet_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Animatable_SetAnim(&character->animatableb, anim);
	Character_CheckStartSing(character);
}

void Char_duet_Free(Character *character)
{
	Char_duet *this = (Char_duet*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_duet_New(fixed_t x, fixed_t y)
{
	//Allocate duet object
	Char_duet *this = Mem_Alloc(sizeof(Char_duet));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_duet_New] Failed to allocate duet object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_duet_Tick;
	this->character.set_anim = Char_duet_SetAnim;
	this->character.free = Char_duet_Free;
	
	Animatable_Init(&this->character.animatable, char_duet_anim);
	Animatable_Init(&this->character.animatableb, char_duet_animb);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;
	
	this->character.focus_x = FIXED_DEC(24,1);
	this->character.focus_y = FIXED_DEC(-66,1);
	this->character.focus_zoom = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\DUET.ARC;1");
	
	const char **pathp = (const char *[]){
		"duet0.tim", //duet_ArcMain_duet0
		"duet1.tim", //duet_ArcMain_duet1
		"duet2.tim", //duet_ArcMain_duet1
		"duet3.tim", //duet_ArcMain_duet1
		"duet4.tim", //duet_ArcMain_duet1
		"duet5.tim", //duet_ArcMain_duet1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
