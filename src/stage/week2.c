/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week2.h"

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
	Gfx_Tex tex_back0; //Background 
	Gfx_Tex tex_back1; //sky
} Back_week2;

//Week 6 background functions
void Back_week2_DrawFG(StageBack *back)
{
	Back_week2 *this = (Back_week2*)back;
	
	fixed_t fx, fy;

	//Draw black
	RECT black_src = {254, 254, 2, 2};
	RECT_FIXED black_dst = {
		FIXED_DEC(-90,1),
		FIXED_DEC(-90,1),
		FIXED_DEC(400,1),
		FIXED_DEC(400,1)
	};

	if (stage.song_step <= 131)
		Stage_DrawTex(&this->tex_back0, &black_src, &black_dst, stage.camera.bzoom);
	if (stage.song_step >= 897)
		Stage_DrawTex(&this->tex_back0, &black_src, &black_dst, stage.camera.bzoom);
	
}


void Back_week2_DrawBG(StageBack *back)
{
	Back_week2 *this = (Back_week2*)back;
	
	fixed_t fx, fy;

	//Draw bg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT bg_src = {0, 0, 256, 256};
	RECT_FIXED bg_dst = {
		FIXED_DEC(-128,1) - fx,
		FIXED_DEC(-72,1) - fy,
		FIXED_DEC(310,1),
		FIXED_DEC(155,1)
	};

	Stage_DrawTex(&this->tex_back1, &bg_src, &bg_dst, stage.camera.bzoom);

	//Draw sky
	fx = stage.camera.x * 3 / 6;
	fy = stage.camera.y * 3 / 6;
	
	RECT sky_src = {0, 0, 256, 256};
	RECT_FIXED sky_dst = {
		FIXED_DEC(-128,1) - fx,
		FIXED_DEC(-72,1) - fy,
		FIXED_DEC(310,1),
		FIXED_DEC(155,1)
	};

	Stage_DrawTex(&this->tex_back0, &sky_src, &sky_dst, stage.camera.bzoom);
}

void Back_week2_Free(StageBack *back)
{
	Back_week2 *this = (Back_week2*)back;

	//Free structure
	Mem_Free(this);
}

StageBack *Back_week2_New(void)
{
	//Allocate background structure
	Back_week2 *this = (Back_week2*)Mem_Alloc(sizeof(Back_week2));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_week2_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_week2_DrawBG;
	this->back.free = Back_week2_Free;
		
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK2\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);

	return (StageBack*)this;
}