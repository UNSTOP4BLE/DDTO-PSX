/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bgdoki.h"

#include "../stage.h"
#include "../archive.h"
#include "../mem.h"
#include "../mutil.h"
#include "../timer.h"

//bgdoki background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_yuri, arc_yuri_ptr[2];
	IO_Data arc_sayori, arc_sayori_ptr[1];
	IO_Data arc_natsuki, arc_natsuki_ptr[1];

	Gfx_Tex tex_back0; //Background 
	Gfx_Tex tex_back1; //sky
	Gfx_Tex tex_back2; //static

	//bopper state
	Gfx_Tex tex_yuri;
	Gfx_Tex tex_sayori;
	Gfx_Tex tex_natsuki;
	u8 yuri_frame, yuri_tex_id;
	u8 sayori_frame, sayori_tex_id;
	u8 natsuki_frame, natsuki_tex_id;
	
	Animatable yuri_animatable;
	Animatable sayori_animatable;
	Animatable natsuki_animatable;
} Back_bgdoki;

//bopper animation and rects
static const CharFrame yuri_frame[] = {
	{0, {  0,   0,  60,  140}, { 42,  140}}, //0 left 1
	{0, { 60,   0,  58,  142}, { 41,  142}}, //1 left 2
	{0, {118,   0,  57,  143}, { 40,  143}}, //2 left 3
	{0, {175,   0,  57,  143}, { 40,  143}}, //2 left 3
	{1, {  0,   0,  55,  143}, { 39,  143}}, //3 left 4
};

static const Animation yuri_anim[] = {
	{2, (const u8[]){0, 1, 2, 3, 4, ASCR_BACK, 0}}, //Left
};
static const CharFrame sayori_frame[] = {
	{0, {  0,   0,  51,  123}, { 23,  123}}, //0 left 1
	{0, { 51,   0,  49,  124}, { 22,  124}}, //1 left 2
	{0, {100,   0,  48,  124}, { 21,  124}}, //2 left 3
	{0, {148,   0,  48,  124}, { 21,  124}}, //2 left 3
	{0, {196,   0,  48,  125}, { 21,  125}}, //3 left 4
};

static const Animation sayori_anim[] = {
	{2, (const u8[]){0, 1, 2, 3, 4, ASCR_BACK, 0}}, //Left
};
static const CharFrame natsuki_frame[] = {
	{0, {  0,   0,  61,  113}, { 52,  113}}, //0 left 1
	{0, { 61,   0,  61,  114}, { 54,  113}}, //1 left 2
	{0, {122,   0,  62,  113}, { 55,  113}}, //2 left 3
	{0, {  0, 114,  61,  113}, { 55,  113}}, //2 left 3
	{0, { 61, 114,  62,  112}, { 56,  112}}, //3 left 4
};

static const Animation natsuki_anim[] = {
	{2, (const u8[]){0, 1, 2, 3, 4, ASCR_BACK, 0}}, //Left
};


//bopper functions
void bgdoki_yuri_SetFrame(void *user, u8 frame)
{
	Back_bgdoki *this = (Back_bgdoki*)user;
	
	//Check if this is a new frame
	if (frame != this->yuri_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &yuri_frame[this->yuri_frame = frame];
		if (cframe->tex != this->yuri_tex_id)
			Gfx_LoadTex(&this->tex_yuri, this->arc_yuri_ptr[this->yuri_tex_id = cframe->tex], 0);
	}
}
void bgdoki_sayori_SetFrame(void *user, u8 frame)
{
	Back_bgdoki *this = (Back_bgdoki*)user;
	
	//Check if this is a new frame
	if (frame != this->sayori_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &sayori_frame[this->sayori_frame = frame];
		if (cframe->tex != this->sayori_tex_id)
			Gfx_LoadTex(&this->tex_sayori, this->arc_sayori_ptr[this->sayori_tex_id = cframe->tex], 0);
	}
}
void bgdoki_natsuki_SetFrame(void *user, u8 frame)
{
	Back_bgdoki *this = (Back_bgdoki*)user;
	
	//Check if this is a new frame
	if (frame != this->natsuki_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &natsuki_frame[this->natsuki_frame = frame];
		if (cframe->tex != this->natsuki_tex_id)
			Gfx_LoadTex(&this->tex_natsuki, this->arc_natsuki_ptr[this->natsuki_tex_id = cframe->tex], 0);
	}
}


void bgdoki_yuri_Draw(Back_bgdoki *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &yuri_frame[this->yuri_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_yuri, &src, &dst, stage.camera.bzoom);
}
void bgdoki_sayori_Draw(Back_bgdoki *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &sayori_frame[this->sayori_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_sayori, &src, &dst, stage.camera.bzoom);
}
void bgdoki_natsuki_Draw(Back_bgdoki *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &natsuki_frame[this->natsuki_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_natsuki, &src, &dst, stage.camera.bzoom);
}

//bgdoki background functions
void Back_bgdoki_DrawFG(StageBack *back)
{
	Back_bgdoki *this = (Back_bgdoki*)back;
	
	fixed_t fx, fy;

	//Draw static
	RECT static_src = {0, 0, 256, 256};
	RECT_FIXED static_dst = {
		FIXED_DEC(-160,1),
		FIXED_DEC(-120,1),
		FIXED_DEC(330,1),
		FIXED_DEC(240,1)
	};
	if (stage.song_step >= 749 && stage.song_step <= 769 && stage.stage_id == StageId_2_2) 
		Stage_DrawTex(&this->tex_back2, &static_src, &static_dst, stage.bump);
		
	//Draw tables
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT table_src = {0, 0, 256, 256};
	RECT_FIXED table_dst = {
		FIXED_DEC(-200,1) - fx,
		FIXED_DEC(-72,1) - fy,
		FIXED_DEC(650,1),
		FIXED_DEC(350,1)
	};

	Stage_DrawTex(&this->tex_back1, &table_src, &table_dst, stage.camera.bzoom);
}


void Back_bgdoki_DrawBG(StageBack *back)
{
	Back_bgdoki *this = (Back_bgdoki*)back;
	
	fixed_t fx, fy;

	//Animate and draw bopper
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step & 7)
		{
			case 0:
				Animatable_SetAnim(&this->yuri_animatable, 0);
				Animatable_SetAnim(&this->sayori_animatable, 0);
				Animatable_SetAnim(&this->natsuki_animatable, 0);
				break;
		}
	}

	//yuri stuffs
	Animatable_Animate(&this->yuri_animatable, (void*)this, bgdoki_yuri_SetFrame);

	bgdoki_yuri_Draw(this, FIXED_DEC(-10,1) - fx, FIXED_DEC(220,1) - fy);

	//sayori stuffs
	Animatable_Animate(&this->sayori_animatable, (void*)this, bgdoki_sayori_SetFrame);
	
	if (stage.stage_id >= StageId_2_3 && stage.stage_id <= StageId_4_1)
		bgdoki_sayori_Draw(this, FIXED_DEC(100,1) - fx, FIXED_DEC(30,1) - fy);

	//natsuki stuffs
	Animatable_Animate(&this->natsuki_animatable, (void*)this, bgdoki_natsuki_SetFrame);

	bgdoki_natsuki_Draw(this, FIXED_DEC(280,1) - fx, FIXED_DEC(215,1) - fy);

	//Draw bg
	RECT bg_src = {0, 0, 256, 256};
	RECT_FIXED bg_dst = {
		FIXED_DEC(-200,1) - fx,
		FIXED_DEC(-72,1) - fy,
		FIXED_DEC(650,1),
		FIXED_DEC(350,1)
	};

	Stage_DrawTex(&this->tex_back0, &bg_src, &bg_dst, stage.camera.bzoom);
}

void Back_bgdoki_Free(StageBack *back)
{
	Back_bgdoki *this = (Back_bgdoki*)back;

	//Free structure
	Mem_Free(this);
	Mem_Free(this->arc_yuri);
	Mem_Free(this->arc_sayori);
	Mem_Free(this->arc_natsuki);
}

StageBack *Back_bgdoki_New(void)
{
	//Allocate background structure
	Back_bgdoki *this = (Back_bgdoki*)Mem_Alloc(sizeof(Back_bgdoki));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_bgdoki_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_bgdoki_DrawBG;
	this->back.free = Back_bgdoki_Free;
		
	//Load background textures
	IO_Data arc_back = IO_Read("\\BGDOKI\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "static.tim"), 0);
	Mem_Free(arc_back);

	//Load bopper textures
	this->arc_yuri = IO_Read("\\BGDOKI\\YURI.ARC;1");
	this->arc_yuri_ptr[0] = Archive_Find(this->arc_yuri, "yuri0.tim");
	this->arc_yuri_ptr[1] = Archive_Find(this->arc_yuri, "yuri1.tim");

	this->arc_sayori = IO_Read("\\BGDOKI\\SAYORI.ARC;1");
	this->arc_sayori_ptr[0] = Archive_Find(this->arc_sayori, "sayori.tim");

	this->arc_natsuki = IO_Read("\\BGDOKI\\NATSUKI.ARC;1");
	this->arc_natsuki_ptr[0] = Archive_Find(this->arc_natsuki, "natsuki.tim");
	
	//Initialize bopper state
	Animatable_Init(&this->yuri_animatable, yuri_anim);
	Animatable_Init(&this->sayori_animatable, sayori_anim);
	Animatable_Init(&this->natsuki_animatable, natsuki_anim);
	Animatable_SetAnim(&this->sayori_animatable, 0);
	Animatable_SetAnim(&this->yuri_animatable, 0);
	Animatable_SetAnim(&this->natsuki_animatable, 0);
	this->yuri_frame = this->yuri_tex_id = 0xFF; //Force art load
	this->sayori_frame = this->sayori_tex_id = 0xFF; //Force art load
	this->natsuki_frame = this->natsuki_tex_id = 0xFF; //Force art load

	return (StageBack*)this;
}