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
	IO_Data arc_hench, arc_hench_ptr[1];

	Gfx_Tex tex_back0; //Background
	Gfx_Tex tex_back1; //Trees

	//Henchmen state
	Gfx_Tex tex_hench;
	u8 hench_frame, hench_tex_id;
	
	Animatable hench_animatable;
	Animatable hench_animatable2;

} Back_Week6;

//Henchmen animation and rects
static const CharFrame henchmen_frame[10] = {
	{0, { 35,   0,  38,  72}, { 21,  72}}, //0 left 1
	{0, {108,   0,  51,  68}, { 28,  68}}, //1 left 2
	{0, {194,   0,  39,  65}, { 21,  65}}, //2 left 3
	{0, { 34,  84,  45,  68}, { 25,  68}}, //3 left 4
	{0, {114,  81,  51,  68}, { 28,  68}}, //4 left 5
	{0, {  0,   0,  33,  77}, { 90,  72}}, //0 left 1
	{0, { 74,   0,  34,  84}, { 90,  68}}, //1 left 2
	{0, {194,   0,  39,  65}, { 21,  65}}, //2 left 3
	{0, { 34,  84,  45,  68}, { 25,  68}}, //3 left 4
	{0, {114,  81,  51,  68}, { 28,  68}}, //4 left 5
};

static const Animation henchmen_anim[1] = {
	{2, (const u8[]){0, 0, 1, 1, 2, 2, 3, 3, 4, 4, ASCR_BACK, 1}}, //Left
};

static const Animation henchmen_anim2[1] = {
	{2, (const u8[]){5, 5, 6, 6, 2, 2, 3, 3, 4, 4, ASCR_BACK, 1}}, //Left
};

//Henchmen functions
void Week6_Henchmen_SetFrame(void *user, u8 frame)
{
	Back_Week6 *this = (Back_Week6*)user;
	
	//Check if this is a new frame
	if (frame != this->hench_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &henchmen_frame[this->hench_frame = frame];
		if (cframe->tex != this->hench_tex_id)
			Gfx_LoadTex(&this->tex_hench, this->arc_hench_ptr[this->hench_tex_id = cframe->tex], 0);
	}
}

void Week6_Henchmen_Draw(Back_Week6 *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &henchmen_frame[this->hench_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_hench, &src, &dst, stage.camera.bzoom);
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
				Animatable_SetAnim(&this->hench_animatable, 0);
				Animatable_SetAnim(&this->hench_animatable2, 0);
				break;
		}
	}
    
	Animatable_Animate(&this->hench_animatable2, (void*)this, Week6_Henchmen_SetFrame);
	Animatable_Animate(&this->hench_animatable, (void*)this, Week6_Henchmen_SetFrame);
	Week6_Henchmen_Draw(this, FIXED_DEC(100,1) - fx, FIXED_DEC(30,1) - fy);

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

static fixed_t week6_back_paraly[] = {
	FIXED_DEC(15,100),
	FIXED_DEC(15,100),
	FIXED_DEC(15,100),
	FIXED_DEC(15,100),
	FIXED_DEC(7,10),
	FIXED_DEC(13,10),
};

static fixed_t week6_back_warpx[] = {
	FIXED_DEC(5,1),
	FIXED_DEC(5,1),
	FIXED_DEC(5,1),
	FIXED_DEC(4,1),
	FIXED_DEC(3,1),
	FIXED_DEC(3,1),
};

static fixed_t week6_back_warpy[] = {
	FIXED_DEC(25,10),
	FIXED_DEC(20,10),
	FIXED_DEC(15,10),
	FIXED_DEC(10,10),
	FIXED_DEC(0,10),
	FIXED_DEC(0,10),
};

static s32 Back_Week6_GetX(int x, int y)
{
	return ((fixed_t)x << (FIXED_SHIFT + 5)) + FIXED_DEC(-128,1) - FIXED_MUL(stage.camera.x, week6_back_paraly[y]) + ((MUtil_Cos((animf_count << 2) + ((x + y) << 5)) * week6_back_warpx[y]) >> 8);
}

static s32 Back_Week6_GetY(int x, int y)
{
	return ((fixed_t)y << (FIXED_SHIFT + 5)) + FIXED_DEC(-86,1) - FIXED_MUL(stage.camera.y, week6_back_paraly[y]) + ((MUtil_Sin((animf_count << 2) + ((x + y) << 5)) * week6_back_warpy[y]) >> 8);
}

void Back_Week6_DrawBG3(StageBack *back)
{
	Back_Week6 *this = (Back_Week6*)back;
	
	//Get quad points
	POINT_FIXED back_dst[6][9];
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 9; x++)
		{
			back_dst[y][x].x = Back_Week6_GetX(x, y);
			back_dst[y][x].y = Back_Week6_GetY(x, y);
		}
	}
	
	//Draw 32x32 quads of the background
	for (int y = 0; y < 5; y++)
	{
		RECT back_src = {0, y * 32, 32, 32};
		for (int x = 0; x < 8; x++)
		{
			//Draw quad and increment source rect
			Stage_DrawTexArb(&this->tex_back0, &back_src, &back_dst[y][x], &back_dst[y][x + 1], &back_dst[y + 1][x], &back_dst[y + 1][x + 1], stage.camera.bzoom);
			if ((back_src.x += 32) >= 0xE0)
				back_src.w--;
		}
	}
}

void Back_Week6_Free(StageBack *back)
{
	Back_Week6 *this = (Back_Week6*)back;
	
	//Free henchmen archive
	Mem_Free(this->arc_hench);

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week6_New(void)
{
	//Allocate background structure
	Back_Week6 *this = (Back_Week6*)Mem_Alloc(sizeof(Back_Week6));
	if (this == NULL)
		return NULL;
	
	if (stage.stage_id != StageId_6_3)
	{
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

	}
	else
	{
		//Set background functions
		this->back.draw_fg = NULL;
		this->back.draw_md = NULL;
		this->back.draw_bg = Back_Week6_DrawBG3;
		this->back.free = Back_Week6_Free;
		
		//Load background texture
		Gfx_LoadTex(&this->tex_back0, IO_Read("\\WEEK6\\BACK3.TIM;1"), GFX_LOADTEX_FREE);
	}
	
	//Load henchmen textures
	this->arc_hench = IO_Read("\\WEEK6\\BACK.ARC;1");
	this->arc_hench_ptr[0] = Archive_Find(this->arc_hench, "back2.tim");
	
	//Initialize henchmen state
	Animatable_Init(&this->hench_animatable, henchmen_anim);
	Animatable_SetAnim(&this->hench_animatable, 0);
	Animatable_Init(&this->hench_animatable2, henchmen_anim2);
	Animatable_SetAnim(&this->hench_animatable2, 0);
	this->hench_frame = this->hench_tex_id = 0xFF; //Force art load

	return (StageBack*)this;
}
