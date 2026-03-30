// Simple citro2d sprite drawing example
// Images borrowed from:
//   https://kenney.nl/assets/space-shooter-redux
#include <citro2d.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_SPRITES   200
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define BOX_WIDTH 256
#define BOX_HEIGHT 142

// Simple sprite struct
typedef struct
{
	C2D_Sprite spr;
	float dx, dy; // velocity
} Sprite;

typedef struct
{
	C2D_Sprite spr;
	float x, y;
	float dx, dy;
	float w, h;
	float scale;
	u32 colour;
	int tintMode;
} Box;

static C2D_SpriteSheet spriteSheet;
static Sprite sprites[MAX_SPRITES];
static Box boxes[MAX_SPRITES];
static size_t numSprites = MAX_SPRITES/2;
static size_t numBoxes = 50;
static float zoom = 1;
static const float zoomStep = 0.1;

float unitToRange(float unit, float min, float max) {
	return ((max-min)*unit)+min;
}

float randomRange(float min, float max) {
	//printf("\x1b[21;1H unit: %f", (float)rand()/(float)RAND_MAX);
	return unitToRange((float)rand()/(float)RAND_MAX, min, max);
}


u32 rgbToInt(u8 r, u8 g, u8 b, u8 a) {
	u32 colour;
	colour = (r<<24)+(g<<16)+(b<<8)+(a);
	return colour;
}



static void initBoxes() {
	srand(time(NULL));
	for (size_t i = 0; i < MAX_SPRITES; i++) {
		Box* box = &boxes[i];

		float minSpeed = 0.1f;
		float maxSpeed = 1.0f;
		int minWidth = 15;
		int maxWidth = 40;
		float minScale = (float)minWidth / (float)BOX_WIDTH;
		float maxScale = (float)maxWidth / (float)BOX_WIDTH;

		/*
		realW = w*scale
		minW = w*minScale
		minScale = minW/w
		*/
		

		// random velocity
		// random scale -> calculate w,h
		// get sprite from spritesheet

		float dx, dy, scale, w, h, x, y;
		u8 r,g,b;
		dx = randomRange(minSpeed, maxSpeed);
		if (rand()%2 == 0) {
			dx *= -1;
		}
		if (rand()%2 == 0) {
			dy *= -1;
		}
		dy = randomRange(minSpeed, maxSpeed);
		scale = randomRange(minScale, maxScale);
		//printf("\x1b[20;1H min scale: %f, maxScale: %f", minScale, maxScale);
		w = (float)BOX_WIDTH * scale;
		h = (float)BOX_HEIGHT * scale;
		x = randomRange(0, SCREEN_WIDTH - w);
		y = randomRange(0, SCREEN_HEIGHT - h);
		r = (u8)randomRange(0, 255);
		g = (u8)randomRange(0, 255);
		b = (u8)randomRange(0, 255);
		u32 colour;
		colour = rgbToInt(r,g,b, 255);
		int tintMode = (rand() % 1)+5;



		C2D_SpriteFromSheet(&box->spr, spriteSheet, rand()%11); // 9th sprite
		C2D_SpriteSetScale(&box->spr, scale, scale);
		box->dx = dx;
		box->dy = dy;
		box->w = w;
		box->h = h;
		box->scale = scale;
		box->x = x;
		box->y = y;
		box->colour = colour;
		box->tintMode = tintMode;
	}
}

//---------------------------------------------------------------------------------
static void initSprites() {
//---------------------------------------------------------------------------------
	size_t numImages = C2D_SpriteSheetCount(spriteSheet);
	srand(time(NULL));

	for (size_t i = 0; i < MAX_SPRITES; i++)
	{
		Sprite* sprite = &sprites[i];

		//float maxScale = 1.0;
		float minScale = 0.1f;
		float maxScale = 0.8f;
		float scale = minScale + (maxScale*(rand() % 1000)/1000.0f);
		scale = 50.0/256.0;

		// Random image, position, rotation and speed
		C2D_SpriteFromSheet(&sprite->spr, spriteSheet, 9); //rand() % numImages);
		C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		C2D_SpriteSetPos(&sprite->spr, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
		C2D_SpriteSetRotation(&sprite->spr, C3D_Angle(0));
		C2D_SpriteSetScale(&sprite->spr, scale, scale);
		sprite->dx = rand()*4.0f/RAND_MAX - 2.0f;
		sprite->dy = rand()*4.0f/RAND_MAX - 2.0f;
	}
}

void updateBoxes() {
	for (size_t i = 0; i < numBoxes; i++) {
		Box* box = &boxes[i];
		// calculate new positon, do bounds, do collision
		// set sprite location
		float x1min, x1max, x2min, x2max, y1min, y1max, y2min, y2max, x, y;
		x = box->x;
		y = box->y;
		x1min = x;
		x1max = x + box->w;
		y1min = y;
		y1max = y + box->h;

		box->x = x + box->dx;
		box->y = y + box->dy;

		// bounds
		if (x<=0) {box->dx = -box->dx; box->x = 1;}
		if (y<=0) {box->dy = -box->dy; box->y = 1;}
		if (x>= SCREEN_WIDTH - box->w) {box->dx = -box->dx; box->x = SCREEN_WIDTH - box->w - 1;}
		if (y>= SCREEN_HEIGHT - box->h) {box->dy = -box->dy; box->y = SCREEN_HEIGHT - box->h - 1;}

		/*
		// right difference is our right edge to their left
		let rd = Math.abs(x1max - x2min);
		// left difference is our left to their right
		let ld = Math.abs(x1min - x2max);
		// top differecnce is our top to their bottom
		let td = Math.abs(y1min - y2max);
		// bottom difference is our bottom to their top
		let bd = Math.abs(y1max - y2min);
		let min = Math.min(rd, ld, td, bd);
		*/

		for (size_t j = i+1; j < numBoxes; j++) {
			Box* other = &boxes[j];
			x2min = other->x;
			x2max = other->x + other->w;
			y2min = other->y;
			y2max = other->y + other->h;
			bool isOverlapping = (x1min < x2max && x2min < x1max && y1min < y2max && y2min < y1max);
			if (isOverlapping) {
				float rd, ld, td, bd, min;
				rd = fabs(x1max - x2min)+1;
				ld = fabs(x1min - x2max)+1;
				td = fabs(y1min - y2max)+1;
				bd = fabs(y1max - y2min)+1;
				min = fmin(fmin(rd, ld), fmin(td, bd));
				float dx, dy;
				int sc = 0;
				if (min == rd) {sc = 0;}
				if (min == ld) {sc = 1;}
				if (min == td) {sc = 2;}
				if (min == bd) {sc = 3;}
				switch (sc) {
					case 0:
						// move left
						box->x -= rd/2;
						other->x += rd/2;
						dx = box->dx;
						box->dx = other->dx;
						other->dx = dx;
						break;
					case 1:
						// move right
						box->x += ld/2;
						other->x -= ld/2;
						dx = box->dx;
						box->dx = other->dx;
						other->dx = dx;
						break;
					case 2:
						// move down
						box->y += td/2;
						other->y -= td/2;
						dy = box->dy;
						box->dy = other->dy;
						other->dy = dy;
						break;
					case 3:
						// move up
						box->y -= bd/2;
						other->y += bd/2;
						dy = box->dy;
						box->dy = other->dy;
						other->dy = dy;
						break;
				}
			}
		}
			/*				case td:
					// move down
					this.y += td / 2;
					box.y -= td / 2;
					vy = this.vy;
					this.vy = box.vy;
					box.vy = vy;
					break;
				case bd:
					// move up
					this.y -= bd / 2;
					box.y += bd / 2;
					vy = this.vy;
					this.vy = box.vy;
					box.vy = vy;
					break;*/



		C2D_SpriteSetPos(&box->spr, box->x, box->y);

	}
	/*
	for each box:
		box.pos += vel
		do bounds:
			if x<=0: = -vx
			if y<=0: vy = -vy
			if x>= window.w - this.w: vx = -vx
			if y=> window.h - this.h: vy = -vy

		check boxes beyond it in box array:
			float x1min, x1max, x2min, x2max, y1min, y1max, y2min, y2max
			

		let x1min = this.x;
		let x1max = this.x + this.w;
		let x2min = box.x;
		let x2max = box.x + box.w;
		let y1min = this.y;
		let y1max = this.y + this.h;
		let y2min = box.y;
		let y2max = box.y + box.h;
	*/
}

//---------------------------------------------------------------------------------
static void moveSprites() {
//---------------------------------------------------------------------------------
	for (size_t i = 0; i < numSprites; i++)
	{
		Sprite* sprite = &sprites[i];
		C2D_SpriteMove(&sprite->spr, sprite->dx, sprite->dy);
		C2D_SpriteRotateDegrees(&sprite->spr, 0.2f);

		// Check for collision with the screen boundaries
		if ((sprite->spr.params.pos.x < sprite->spr.params.pos.w / 2.0f && sprite->dx < 0.0f) ||
			(sprite->spr.params.pos.x > (SCREEN_WIDTH-(sprite->spr.params.pos.w / 2.0f)) && sprite->dx > 0.0f))
			sprite->dx = -sprite->dx;

		if ((sprite->spr.params.pos.y < sprite->spr.params.pos.h / 2.0f && sprite->dy < 0.0f) ||
			(sprite->spr.params.pos.y > (SCREEN_HEIGHT-(sprite->spr.params.pos.h / 2.0f)) && sprite->dy > 0.0f))
			sprite->dy = -sprite->dy;
	}
}

C2D_ImageTint makeTint(u32 colour, float blend) {
	C2D_Tint tint;
	tint.color = colour;
	tint.blend = blend;

	C2D_Tint corners[4] = {tint, tint, tint, tint};

	C2D_ImageTint imageTint;
	memcpy(imageTint.corners, corners, sizeof(tint)*4);
	return imageTint;
}


const char* getTintModeName(int tintMode) {
	switch (tintMode) {
		case C2D_TintSolid: return "C2D_TintSolid";
		case C2D_TintMult: return "C2D_TintMult";
		case C2D_TintLuma: return "C2D_TintLuma";
		case C2D_TintAdd: return "C2D_TintAdd";
		case C2D_TintSub: return "C2D_TintSub";
		case C2D_TintOneMinusAdd: return "C2D_TintOneMinusAdd";
		case C2D_TintOneMinusSub: return "C2D_TintOneMinusSub";
	}
	return "none";
}

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);
	//C2D_ViewTranslate(SCREEN_WIDTH*(1-zoom), SCREEN_HEIGHT*(1-zoom));
	//C2D_ViewTranslate(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);



	// set tint mode
	int tintMode = C2D_TintSolid;
	C2D_SetTintMode(tintMode);

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	int t = 0;

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	//if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	// Initialize sprites
	//initSprites();
	initBoxes();

	printf("\x1b[8;1HPress Up for more Boxes");
	printf("\x1b[9;1HPress Down for less Boxes");
	
	u8 r,g,b;
	r = 255;
	g = 0;
	b = 0;
	u32 colour;
	colour = rgbToInt(r,g,b, 255);
	float blend = 0.0f;
	//printf("\x1b[12;1H %u, %u, %u", r, g, b);
	//printf("\x1b[13;1H %lu", colour);

	numSprites = 1;

	

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		if (kDown & KEY_A) {
			tintMode++;
			tintMode = tintMode % (C2D_TintOneMinusSub+1);
			C2D_SetTintMode(tintMode);
		}



		u32 kHeld = hidKeysHeld();
		if ((kHeld & KEY_UP) && numBoxes < MAX_SPRITES)
			numBoxes ++;
		if ((kHeld & KEY_DOWN) && numBoxes > 0)
			numBoxes --;

		//moveSprites();
		updateBoxes();
		/*
		printf("\x1b[1;1HSprites: %zu/%u\x1b[K", numSprites, MAX_SPRITES);
		printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
		printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
		printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);
		*/
		printf("\x1b[4;5HGell St. Software (C) 2026");
		printf("\x1b[30;2Hver0.8");
		/*
		printf("\x1b[14;1Hblend: %f", blend);
		printf("\x1b[15;1H \33[2K");
		printf("\x1b[15;1Htint mode: %s", getTintModeName(tintMode));
		printf("\x1b[16;1H x: %f, y: %f", boxes[0].x, boxes[0].y);
		printf("\x1b[17;1H sin: %f", (sin(((float)t)/500) +1)/2   );
		*/

		//printf("\x1b[14;2Hzoom: %f", zoom);


		C2D_ImageTint imageTint;
		

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		if (!(kHeld & KEY_B)) {
			C2D_TargetClear(top, C2D_Color32f((sin(((float)t)/500) +1)/2, 0.15f, 0.85f, 0.25f));
		}
		C2D_SceneBegin(top);
		for (size_t i = 0; i < numBoxes; i ++) {
			//C2D_DrawSprite(&sprites[i].spr);
			C2D_SetTintMode(boxes[i].tintMode);
			imageTint = makeTint(boxes[i].colour, blend);
			C2D_DrawSpriteTinted(&boxes[i].spr, &imageTint);
		}
		t++;
		C3D_FrameEnd(0);
	}

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
