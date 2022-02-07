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

//Week 6 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Background 
	Gfx_Tex tex_back1; //sky
} Back_bgdoki;

//Week 6 background functions
void Back_bgdoki_DrawFG(StageBack *back)
{
	Back_bgdoki *this = (Back_bgdoki*)back;
	
	fixed_t fx, fy;

	//Draw tables
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT table_src = {0, 0, 256, 256};
	RECT_FIXED table_dst = {
		FIXED_DEC(-128,1) - fx,
		FIXED_DEC(-72,1) - fy,
		FIXED_DEC(310,1),
		FIXED_DEC(155,1)
	};

	//Stage_DrawTex(&this->tex_back1, &table_src, &table_dst, stage.camera.bzoom);
}


void Back_bgdoki_DrawBG(StageBack *back)
{
	Back_bgdoki *this = (Back_bgdoki*)back;
	
	fixed_t fx, fy;

	//Draw bg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT bg_src = {0, 0, 256, 256};
	RECT_FIXED bg_dst = {
		FIXED_DEC(-200,1) - fx,
		FIXED_DEC(-72,1) - fy,
		FIXED_DEC(500,1),
		FIXED_DEC(300,1)
	};

	Stage_DrawTex(&this->tex_back0, &bg_src, &bg_dst, stage.camera.bzoom);
}

void Back_bgdoki_Free(StageBack *back)
{
	Back_bgdoki *this = (Back_bgdoki*)back;

	//Free structure
	Mem_Free(this);
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
	Mem_Free(arc_back);

	return (StageBack*)this;
}