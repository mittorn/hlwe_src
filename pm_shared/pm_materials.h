#if !defined( PM_MATERIALSH )
#define PM_MATERIALSH
#pragma once

#define CTEXTURESMAX		1024		// max number of textures loaded
#define CBTEXTURENAMEMAX	25		// only load first n chars of name

#define CHAR_TEX_CONCRETE	'C'
#define CHAR_TEX_METAL		'M'
#define CHAR_TEX_DIRT		'D'
#define CHAR_TEX_VENT		'V'
#define CHAR_TEX_GRATE		'G'
#define CHAR_TEX_TILE		'T'
#define CHAR_TEX_SLOSH		'S'
#define CHAR_TEX_WOOD		'W'
#define CHAR_TEX_COMPUTER	'P'
#define CHAR_TEX_GLASS		'Y'
#define CHAR_TEX_FLESH		'F'
#define CHAR_TEX_SNOW		'N'
#define CHAR_TEX_GRASS		'A'
#define CHAR_TEX_BRICK		'B'
#define CHAR_TEX_SAND		'X'
#define CHAR_TEX_ROCK		'R'
#define CHAR_TEX_ENERGYSHIELD	'E'
#define CHAR_TEX_ASPHALT	'Z'
#define CHAR_TEX_SANDWALL	'J'
#define CHAR_TEX_EMPTY		'K'
#define CHAR_TEX_SNOWROCK	'O'
#define CHAR_TEX_LEAVES		'L'

#define CHAR_TEX_BP_CONCRETE	'U'
#define CHAR_TEX_BP_METAL	'Q'
#define CHAR_TEX_BP_GLASS	'I'

#define STEP_CONCRETE		0		// default step sound
#define STEP_METAL		1		// metal floor
#define STEP_DIRT		2		// dirt, sand, rock
#define STEP_VENT		3		// ventillation duct
#define STEP_GRATE		4		// metal grating
#define STEP_TILE		5		// floor tiles
#define STEP_SLOSH		6		// shallow liquid puddle
#define STEP_WADE		7		// wading in liquid
#define STEP_LADDER		8		// climbing ladder
#define STEP_SNOW		9		// snow
#define STEP_GRASS		10		// grass
#define STEP_SAND		11		// sand
#define STEP_ENERGYSHIELD	12		// energy shield

#endif // !PM_MATERIALSH
