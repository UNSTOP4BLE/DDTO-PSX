/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week6.h"

#include "../stage.h"
#include "../archive.h"
#include "../mem.h"
#include "../mutil.h"
#include "../timer.h"

//Week 6 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_freak0, arc_freak0_ptr[1];
	IO_Data arc_freak1, arc_freak1_ptr[1];


	Gfx_Tex tex_back0; //Background
	Gfx_Tex tex_back1; //Trees

	//freak0 state
	Gfx_Tex tex_freak0;
	Gfx_Tex tex_freak1;
	u8 freak0_frame, freak0_tex_id;
	u8 freak1_frame, freak1_tex_id;
	
	Animatable freak0_animatable;
	Animatable freak1_animatable;

} Back_Week6;

//freak1 animation and rects
static const CharFrame freak1_frame[] = {
	{0, { 0,   0,  35,  77}, { 14,  77}}, //0 left 1
	{0, {73,   0,  35,  84}, { 15,  84}}, //1 left 2
	{0, {159,  0,  35,  81}, { 14,  81}}, //2 left 3
	{0, { 0,  84,  34,  81}, { 14,  81}}, //3 left 4
	{0, { 79,  84, 35,  81}, { 14,  81}}, //4 left 5
};

static const Animation freak1_anim[] = {
	{2, (const u8[]){0, 0, 1, 1, 2, 2, 3, 3, 4, 4, ASCR_CHGANI, 0}}, //Left
};

//freak1 functions
void Week6_freak1_SetFrame(void *user, u8 frame)
{
	Back_Week6 *this = (Back_Week6*)user;
	
	//Check if this is a new frame
	if (frame != this->freak1_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &freak1_frame[this->freak1_frame = frame];
		if (cframe->tex != this->freak1_tex_id)
			Gfx_LoadTex(&this->tex_freak1, this->arc_freak1_ptr[this->freak1_tex_id = cframe->tex], 0);
	}
}

void Week6_freak1_Draw(Back_Week6 *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &freak1_frame[this->freak1_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_freak1, &src, &dst, stage.camera.bzoom);
}

//freak0 animation and rects
static const CharFrame freak0_frame[] = {
	{0, { 35,   0,  38,  72}, { 21,  72}}, //0 left 1
	{0, {108,   0,  51,  68}, { 28,  68}}, //1 left 2
	{0, {194,   0,  39,  65}, { 21,  65}}, //2 left 3
	{0, { 34,  84,  45,  68}, { 25,  68}}, //3 left 4
	{0, {114,  81,  51,  68}, { 28,  68}}, //4 left 5
};

static const Animation freak0_anim[] = {
	{2, (const u8[]){0, 0, 1, 1, 2, 2, 3, 3, 4, 4, ASCR_CHGANI, 0}}, //Left
};

//freak0 functions
void Week6_freak0_SetFrame(void *user, u8 frame)
{
	Back_Week6 *this = (Back_Week6*)user;
	
	//Check if this is a new frame
	if (frame != this->freak0_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &freak0_frame[this->freak0_frame = frame];
		if (cframe->tex != this->freak0_tex_id)
			Gfx_LoadTex(&this->tex_freak0, this->arc_freak0_ptr[this->freak0_tex_id = cframe->tex], 0);
	}
}

void Week6_freak0_Draw(Back_Week6 *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &freak0_frame[this->freak0_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_freak0, &src, &dst, stage.camera.bzoom);
}

//Week 6 background functions
void Back_Week6_DrawBG(StageBack *back)
{
	Back_Week6 *this = (Back_Week6*)back;
	
	fixed_t fx, fy;
	
	//Animate and draw freaks
	fx = (stage.camera.x << 2) / 5;
	fy = (stage.camera.y << 2) / 5;

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step & 7)
		{
			case 0:
				Animatable_SetAnim(&this->freak0_animatable, 0);
				Animatable_SetAnim(&this->freak1_animatable, 0);
				break;
		}
	}
	Animatable_Animate(&this->freak0_animatable, (void*)this, Week6_freak0_SetFrame);
	Animatable_Animate(&this->freak1_animatable, (void*)this, Week6_freak1_SetFrame);
	
	if (stage.stage_id == StageId_1_2) {
		Week6_freak0_Draw(this, FIXED_DEC(-120,1) - fx, FIXED_DEC(40,1) - fy);
		Week6_freak0_Draw(this, FIXED_DEC(50,1) - fx, FIXED_DEC(40,1) - fy);
		Week6_freak0_Draw(this, FIXED_DEC(-40,1) - fx, FIXED_DEC(40,1) - fy);
		Week6_freak1_Draw(this, FIXED_DEC(90,1) - fx, FIXED_DEC(40,1) - fy);
		Week6_freak1_Draw(this, FIXED_DEC(0,1) - fx, FIXED_DEC(40,1) - fy);
		Week6_freak1_Draw(this, FIXED_DEC(-80,1) - fx, FIXED_DEC(40,1) - fy);
	}

	//Draw foreground trees
	fx = stage.camera.x >> 1;
	fy = stage.camera.y >> 1;
	
	static const struct Back_Week6_FGTree
	{
		RECT src;
		fixed_t x, y;
		fixed_t off[4];
	} fg_tree[] = {
		{{  0, 0, 100, 82}, FIXED_DEC(-116,1), FIXED_DEC(-88,1), {FIXED_DEC(32,10), FIXED_DEC(34,10), FIXED_DEC(28,10), FIXED_DEC(30,10)}},
		{{101, 0,  99, 80},    FIXED_DEC(0,1), FIXED_DEC(-84,1), {FIXED_DEC(31,10), FIXED_DEC(30,10), FIXED_DEC(29,10), FIXED_DEC(32,10)}},
	};
	
	const struct Back_Week6_FGTree *fg_tree_p = fg_tree;
	for (size_t i = 0; i < COUNT_OF(fg_tree); i++, fg_tree_p++)
	{
		//Get distorted points
		POINT_FIXED treep[4] = {
			{fg_tree_p->x,                                 fg_tree_p->y},
			{fg_tree_p->x + FIXED_DEC(fg_tree_p->src.w,1), fg_tree_p->y},
			{fg_tree_p->x,                                 fg_tree_p->y + FIXED_DEC(fg_tree_p->src.h,1)},
			{fg_tree_p->x + FIXED_DEC(fg_tree_p->src.w,1), fg_tree_p->y + FIXED_DEC(fg_tree_p->src.h,1)},
		};
		for (int j = 0; j < 4; j++)
		{
			treep[j].x += ((MUtil_Cos(FIXED_MUL(animf_count, fg_tree_p->off[j])) * FIXED_DEC(3,1)) >> 8) - fx;
			treep[j].y += ((MUtil_Sin(FIXED_MUL(animf_count, fg_tree_p->off[j])) * FIXED_DEC(3,1)) >> 8) - fy;
		}
		
		Stage_DrawTexArb(&this->tex_back1, &fg_tree_p->src, &treep[0], &treep[1], &treep[2], &treep[3], stage.camera.bzoom);
	}
	
	//Draw background trees
	RECT bg_tree_l_src = {0, 83, 62, 45};
	RECT_FIXED bg_tree_l_dst = {
		FIXED_DEC(-106,1) - fx,
		FIXED_DEC(-26,1) - fy,
		FIXED_DEC(62,1),
		FIXED_DEC(45,1)
	};
	
	RECT bg_tree_r_src = {63, 83, 61, 50};
	RECT_FIXED bg_tree_r_dst = {
		FIXED_DEC(44,1) - fx,
		FIXED_DEC(-26,1) - fy,
		FIXED_DEC(61,1),
		FIXED_DEC(50,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &bg_tree_l_src, &bg_tree_l_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &bg_tree_r_src, &bg_tree_r_dst, stage.camera.bzoom);
	
	//Draw school
	fx = stage.camera.x >> 3;
	fy = stage.camera.y >> 3;
	
	RECT school_src = {0, 0, 255, 75};
	RECT_FIXED school_dst = {
		FIXED_DEC(-128,1) - fx,
		FIXED_DEC(-56,1) - fy,
		FIXED_DEC(255,1),
		FIXED_DEC(75,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &school_src, &school_dst, stage.camera.bzoom);
	
	//Draw street
	//fx = stage.camera.x >> 3;
	//fy = stage.camera.y >> 3;
	
	POINT_FIXED street_d0 = {
		FIXED_DEC(-128,1) - fx,
		FIXED_DEC(19,1) - FIXED_DEC(1,1) - fy,
	};
	POINT_FIXED street_d1 = {
		FIXED_DEC(-128,1) + FIXED_DEC(255,1) - fx,
		FIXED_DEC(19,1) - FIXED_DEC(1,1) - fy,
	};
	
	fx = stage.camera.x * 3 / 2;
	fy = stage.camera.y * 3 / 2;
	
	POINT_FIXED street_d2 = {
		FIXED_DEC(-128,1) - fx,
		FIXED_DEC(19,1) + FIXED_DEC(58,1) - fy,
	};
	POINT_FIXED street_d3 = {
		FIXED_DEC(-128,1) + FIXED_DEC(255,1) - fx,
		FIXED_DEC(19,1) + FIXED_DEC(58,1) - fy,
	};
	
	RECT street_src = {0, 75, 255, 54};
	
	Stage_DrawTexArb(&this->tex_back0, &street_src, &street_d0, &street_d1, &street_d2, &street_d3, stage.camera.bzoom);
	
	//Draw sky
	fx = stage.camera.x >> 4;
	fy = stage.camera.y >> 4;
	
	RECT sky_src = {0, 130, 255, 125};
	RECT_FIXED sky_dst = {
		FIXED_DEC(-128,1) - fx,
		FIXED_DEC(-72,1) - fy,
		FIXED_DEC(255,1),
		FIXED_DEC(125,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &sky_src, &sky_dst, stage.camera.bzoom);
}

void Back_Week6_Free(StageBack *back)
{
	Back_Week6 *this = (Back_Week6*)back;
	
	//Free freak0 archive
	Mem_Free(this->arc_freak0);

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week6_New(void)
{
	//Allocate background structure
	Back_Week6 *this = (Back_Week6*)Mem_Alloc(sizeof(Back_Week6));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week6_DrawBG;
	this->back.free = Back_Week6_Free;
		
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK6\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);

	//Load freak0 textures
	this->arc_freak0 = IO_Read("\\WEEK6\\BACK.ARC;1");
	this->arc_freak0_ptr[0] = Archive_Find(this->arc_freak0, "back2.tim");
	
	this->arc_freak1 = IO_Read("\\WEEK6\\BACK.ARC;1");
	this->arc_freak1_ptr[0] = Archive_Find(this->arc_freak1, "back2.tim");
	
	//Initialize freak0 state
	Animatable_Init(&this->freak0_animatable, freak0_anim);
	Animatable_SetAnim(&this->freak0_animatable, 0);
	this->freak0_frame = this->freak0_tex_id = 0xFF; //Force art load

	Animatable_Init(&this->freak1_animatable, freak1_anim);
	Animatable_SetAnim(&this->freak1_animatable, 0);
	this->freak1_frame = this->freak1_tex_id = 0xFF; //Force art load

	return (StageBack*)this;
}