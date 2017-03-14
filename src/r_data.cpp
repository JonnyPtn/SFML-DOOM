#include "i_system.hpp"
#include "i_video.hpp"

#include "w_wad.hpp"

#include "doomdef.hpp"
#include "r_local.hpp"
#include "p_local.hpp"

#include "doomstat.hpp"
#include "r_sky.hpp"


#include "r_data.hpp"

//JONNY//
#include <stdlib.h>
#include <cstring>

//
// Graphics.
// DOOM graphics for walls and sprites
// is stored in vertical runs of opaque pixels (posts).
// A column is composed of zero or more posts,
// a patch or sprite is composed of zero or more columns.
// 


//
// Texture definition.
// Each texture is composed of one or more patches,
// with patches being lumps stored in the WAD.
// The lumps are referenced by number, and patched
// into the rectangular texture space using origin
// and possibly other attributes.
//
typedef struct
{
    short	originx;
    short	originy;
    short	patch;
    short	stepdir;
    short	colormap;
} mappatch_t;


//
// Texture definition.
// A DOOM wall texture is a list of patches
// which are to be combined in a predefined order.
//
typedef struct
{
    char		name[8];
    bool		masked;	
	bool		beep;	//padding
	bool		bop;	//padding
	bool		boop;	//padding
    short		width;
    short		height;
    int		    columndirectory;	// OBSOLETE
    short		patchcount;
    mappatch_t	patches[1];
} maptexture_t;


// A single patch from a texture definition,
//  basically a rectangular area within
//  the texture rectangle.
typedef struct
{
    // Block origin (allways UL),
    // which has allready accounted
    // for the internal origin of the patch.
    int		originx;	
    int		originy;
    int		patch;
} texpatch_t;


// A maptexturedef_t describes a rectangular texture,
//  which is composed of one or more mappatch_t structures
//  that arrange graphic patches.
typedef struct
{
    // Keep name for switch changing, etc.
    char	name[8];		
    short	width;
    short	height;
    
    // All the patches[patchcount]
    //  are drawn back to front into the cached texture.
    short	patchcount;
    texpatch_t	patches[1];		
    
} texture_t;



int		firstflat;
int		lastflat;
int		numflats;

int		firstpatch;
int		lastpatch;
int		numpatches;

int		firstspritelump;
int		lastspritelump;
int		numspritelumps;

int		numtextures;
texture_t**	textures;


int*			texturewidthmask;
// needed for texture pegging
int*		textureheight;		
int*			texturecompositesize;
short**			texturecolumnlump;
unsigned short**	texturecolumnofs;
unsigned char**			texturecomposite;

// for global animation
int*		flattranslation;
int*		texturetranslation;

// needed for pre rendering
int*	spritewidth;	
int*	spriteoffset;
int*	spritetopoffset;

unsigned char	*colormaps;


//
// MAPTEXTURE_T CACHING
// When a texture is first needed,
//  it counts the number of composite columns
//  required in the texture and allocates space
//  for a column directory and any new columns.
// The directory will simply point inside other patches
//  if there is only one patch in a given column,
//  but any columns with multiple patches
//  will have new column_ts generated.
//



//
// R_DrawColumnInCache
// Clip and draw a column
//  from a patch into a cached post.
//
void
R_DrawColumnInCache
( column_t*	patch,
  unsigned char*		cache,
  int		originy,
  int		cacheheight )
{
    int		count;
    int		position;
    unsigned char*	source;
    unsigned char*	dest;
	
    dest = (unsigned char *)cache + 3;
	
    while (patch->topdelta != 0xff)
    {
	source = (unsigned char *)patch + 3;
	count = patch->length;
	position = originy + patch->topdelta;

	if (position < 0)
	{
	    count += position;
	    position = 0;
	}

	if (position + count > cacheheight)
	    count = cacheheight - position;

	if (count > 0)
	    memcpy (cache + position, source, count);
		
	patch = (column_t *)(  (unsigned char *)patch + patch->length + 4); 
    }
}



//
// R_GenerateComposite
// Using the texture definition,
//  the composite texture is created from the patches,
//  and each column is cached.
//
void R_GenerateComposite (int texnum)
{
    texture_t*		texture;
    texpatch_t*		patch;	
    patch_t*		realpatch;
    int			x;
    int			x1;
    int			x2;
    int			i;
    column_t*		patchcol;
    short*		collump;
    unsigned short*	colofs;
	
    texture = textures[texnum];

    texturecomposite[texnum] = (unsigned char*)malloc (texturecompositesize[texnum]);	

    collump = texturecolumnlump[texnum];
    colofs = texturecolumnofs[texnum];
    
    // Composite the columns together.
    patch = texture->patches;
		
    for (i=0 , patch = texture->patches;
	 i<texture->patchcount;
	 i++, patch++)
    {
	realpatch = (patch_t*)WadManager::getLump (patch->patch);
	x1 = patch->originx;
	x2 = x1 + realpatch->width;

	if (x1<0)
	    x = 0;
	else
	    x = x1;
	
	if (x2 > texture->width)
	    x2 = texture->width;

	for ( ; x<x2 ; x++)
	{
	    // Column does not have multiple patches?
	    if (collump[x] >= 0)
		continue;
	    
	    patchcol = (column_t *)((unsigned char *)realpatch
				    + realpatch->columnofs[x-x1]);
	    R_DrawColumnInCache (patchcol,
			texturecomposite[texnum] + colofs[x],
				 patch->originy,
				 texture->height);
	}
						
    }
}



//
// R_GenerateLookup
//
void R_GenerateLookup (int texnum)
{
    texture_t*		texture;
    unsigned char*		patchcount;	// patchcount[texture->width]
    texpatch_t*		patch;	
    patch_t*		realpatch;
    int			x;
    int			x1;
    int			x2;
    int			i;
    short*		collump;
    unsigned short*	colofs;
	
    texture = textures[texnum];

    // Composited texture not created yet.
    texturecomposite[texnum] = 0;
    
    texturecompositesize[texnum] = 0;
    collump = texturecolumnlump[texnum];
    colofs = texturecolumnofs[texnum];
    
    // Now count the number of columns
    //  that are covered by more than one patch.
    // Fill in the lump / offset, so columns
    //  with only a single patch are all done.
    patchcount = (unsigned char *)alloca (texture->width);
    memset (patchcount, 0, texture->width);
    patch = texture->patches;
		
    for (i=0 , patch = texture->patches;
	 i<texture->patchcount;
	 i++, patch++)
    {
	realpatch = (patch_t*)WadManager::getLump (patch->patch);
	x1 = patch->originx;
	x2 = x1 + realpatch->width;
	
	if (x1 < 0)
	    x = 0;
	else
	    x = x1;

	if (x2 > texture->width)
	    x2 = texture->width;
	for ( ; x<x2 ; x++)
	{
	    patchcount[x]++;
	    collump[x] = patch->patch;
	    colofs[x] = realpatch->columnofs[x-x1]+3;
	}
    }
	
    for (x=0 ; x<texture->width ; x++)
    {
	if (!patchcount[x])
	{
	    printf ("R_GenerateLookup: column without a patch (%s)\n",
		    texture->name);
	    return;
	}
	// I_Error ("R_GenerateLookup: column without a patch");
	
	if (patchcount[x] > 1)
	{
	    // Use the cached block.
	    collump[x] = -1;	
	    colofs[x] = texturecompositesize[texnum];
	    
	    if (texturecompositesize[texnum] > 0x10000-texture->height)
	    {
		I_Error ("R_GenerateLookup: texture %i is >64k",
			 texnum);
	    }
	    
	    texturecompositesize[texnum] += texture->height;
	}
    }	
}




//
// R_GetColumn
//
unsigned char*
R_GetColumn
( int		tex,
  int		col )
{
    int		lump;
    int		ofs;
	
    col &= texturewidthmask[tex];
    lump = texturecolumnlump[tex][col];
    ofs = texturecolumnofs[tex][col];
    
    if (lump > 0)
	return (unsigned char *)WadManager::getLump(lump)+ofs;

    if (!texturecomposite[tex])
	R_GenerateComposite (tex);

    return texturecomposite[tex] + ofs;
}




//
// R_InitTextures
// Initializes the texture list
//  with the textures from the world map.
//
void R_InitTextures (void)
{
    maptexture_t*	mtexture;
    texture_t*		texture;
    mappatch_t*		mpatch;
    texpatch_t*		patch;

    int			i;
    int			j;

    int*		maptex;
    int*		maptex2;
    int*		maptex1;
    
    std::string		name(9,'0');
    char*		names;
    char*		name_p;
    
    int*		patchlookup;
    
    int			totalwidth;
    int			nummappatches;
    int			offset;
    int			maxoff;
    int			maxoff2;
    int			numtextures1;
    int			numtextures2;

    int*		directory;
    
    int			temp1;
    int			temp2;
    int			temp3;

    
    // Load the patch names from pnames.lmp.
    names = (char*)WadManager::getLump ("PNAMES");
    nummappatches =  *((int *)names) ;
    name_p = names+4;
    patchlookup = (int*)alloca (nummappatches*sizeof(*patchlookup));
    
    for (i=0 ; i<nummappatches ; i++)
    {
	    name = std::string(name_p+i*8, 8);
	    patchlookup[i] = WadManager::checkNumForName (name);
    }
    
    // Load the map texture definitions from textures.lmp.
    // The data is contained in one or two lumps,
    //  TEXTURE1 for shareware, plus TEXTURE2 for commercial.
    maptex = maptex1 = (int*)WadManager::getLump ("TEXTURE1");

    numtextures1 = *maptex;
    maxoff = WadManager::getLumpLength (WadManager::getNumForName ("TEXTURE1"));
    directory = maptex+1;
	
    if (WadManager::checkNumForName ("TEXTURE2") != -1)
    {
	maptex2 = (int*)WadManager::getLump ("TEXTURE2");
	numtextures2 = *maptex2;
	maxoff2 = WadManager::getLumpLength (WadManager::getNumForName ("TEXTURE2"));
    }
    else
    {
	maptex2 = NULL;
	numtextures2 = 0;
	maxoff2 = 0;
    }
    numtextures = numtextures1 + numtextures2;
	
    textures = (texture_t**)malloc (numtextures*sizeof(void*));
    texturecolumnlump = (short**)malloc (numtextures*sizeof(void*));
    texturecolumnofs = (unsigned short**)malloc (numtextures*sizeof(void*));
    texturecomposite = (unsigned char**)malloc (numtextures*sizeof(void*));
    texturecompositesize = (int*)malloc (numtextures*sizeof(void*));
    texturewidthmask = (int*)malloc (numtextures*sizeof(void*));
    textureheight = (int*)malloc (numtextures*sizeof(void*));

    totalwidth = 0;
    
    //	Really complex printing shit...
    temp1 = WadManager::getNumForName ("S_START");  // P_???????
    temp2 = WadManager::getNumForName ("S_END") - 1;
    temp3 = ((temp2-temp1+63)/64) + ((numtextures+63)/64);
    printf("[");
    for (i = 0; i < temp3; i++)
	printf(" ");
    printf("         ]");
    for (i = 0; i < temp3; i++)
	printf("\x8");
    printf("\x8\x8\x8\x8\x8\x8\x8\x8\x8\x8");	
	
    for (i=0 ; i<numtextures ; i++, directory++)
    {
	if (!(i&63))
	    printf (".");

	if (i == numtextures1)
	{
	    // Start looking in second texture file.
	    maptex = maptex2;
	    maxoff = maxoff2;
	    directory = maptex+1;
	}
		
	offset = *directory;

	if (offset > maxoff)
	    I_Error ("R_InitTextures: bad texture directory");
	
	mtexture = (maptexture_t *) ( (unsigned char *)maptex + offset);

	texture = textures[i] =
	    (texture_t*)malloc (sizeof(texture_t)
		      + sizeof(texpatch_t)*(mtexture->patchcount-1));
	
	texture->width = mtexture->width;
	texture->height = mtexture->height;
	texture->patchcount = mtexture->patchcount;

	memcpy (texture->name, mtexture->name, sizeof(texture->name));
	mpatch = &mtexture->patches[0];
	patch = &texture->patches[0];

	for (j=0 ; j<texture->patchcount ; j++, mpatch++, patch++)
	{
	    patch->originx = mpatch->originx;
	    patch->originy = mpatch->originy;
	    patch->patch = patchlookup[short(mpatch->patch)];
	    if (patch->patch == -1)
	    {
		I_Error ("R_InitTextures: Missing patch in texture %s",
			 texture->name);
	    }

	}		
	texturecolumnlump[i] = (short*)malloc (texture->width*2);
	texturecolumnofs[i] = (unsigned short*)malloc (texture->width*2);

	j = 1;
	while (j*2 <= texture->width)
	    j<<=1;

	texturewidthmask[i] = j-1;
	textureheight[i] = texture->height<<FRACBITS;
		
	totalwidth += texture->width;
    }
    
    // Precalculate whatever possible.	
    for (i=0 ; i<numtextures ; i++)
	R_GenerateLookup (i);
    
    // Create translation table for global animation.
    texturetranslation = (int*)malloc ((numtextures+1)*4);
    
    for (i=0 ; i<numtextures ; i++)
	texturetranslation[i] = i;
}



//
// R_InitFlats
//
void R_InitFlats (void)
{
    int		i;
	
    firstflat = WadManager::getNumForName ("F_START") + 1;
    lastflat = WadManager::getNumForName ("F_END") - 1;
    numflats = lastflat - firstflat + 1;
	
    // Create translation table for global animation.
    flattranslation = (int*)malloc ((numflats+1)*4);
    
    for (i=0 ; i<numflats ; i++)
	flattranslation[i] = i;
}


//
// R_InitSpriteLumps
// Finds the width and hoffset of all sprites in the wad,
//  so the sprite does not need to be cached completely
//  just for having the header info ready during rendering.
//
void R_InitSpriteLumps (void)
{
    int		i;
    patch_t	*patch;
	
    firstspritelump = WadManager::getNumForName ("S_START") + 1;
    lastspritelump = WadManager::getNumForName ("S_END") - 1;
    
    numspritelumps = lastspritelump - firstspritelump + 1;
    spritewidth = (int*)malloc (numspritelumps*4);
    spriteoffset = (int*)malloc (numspritelumps*4);
    spritetopoffset = (int*)malloc (numspritelumps*4);
	
    for (i=0 ; i< numspritelumps ; i++)
    {
	if (!(i&63))
	    printf (".");

	patch = (patch_t*)WadManager::getLump (firstspritelump+i);
	spritewidth[i] = patch->width<<FRACBITS;
	spriteoffset[i] = patch->leftoffset<<FRACBITS;
	spritetopoffset[i] = patch->topoffset<<FRACBITS;
    }
}



//
// R_InitColormaps
//
void R_InitColormaps (void)
{
    int	lump, length;
    
    // Load in the light tables, 
    //  256 unsigned char align tables.
    lump = WadManager::getNumForName("COLORMAP");
    length = WadManager::getLumpLength (lump) + 255; 
    colormaps = (unsigned char*)malloc (length); 
    std::memcpy( colormaps, WadManager::getLump(lump),length);
}



//
// R_InitData
// Locates all the lumps
//  that will be used by all views
// Must be called after W_Init.
//
void R_InitData (void)
{
    R_InitTextures ();
    printf ("\nInitTextures");
    R_InitFlats ();
    printf ("\nInitFlats");
    R_InitSpriteLumps ();
    printf ("\nInitSprites");
    R_InitColormaps ();
    printf ("\nInitColormaps");
}



//
// R_FlatNumForName
// Retrieval, get a flat number for a flat name.
//
int R_FlatNumForName (const std::string& name)
{
    auto i = WadManager::checkNumForName (name);

    if (i == -1)
    {
	    I_Error ("R_FlatNumForName: %s not found",name.c_str());
    }
    return i - firstflat;
}




//
// R_CheckTextureNumForName
// Check whether texture is available.
// Filter out NoTexture indicator.
//
int	R_CheckTextureNumForName (char *name)
{
    int		i;

    // "NoTexture" marker.
    if (name[0] == '-')		
	return 0;
		
    for (i=0 ; i<numtextures ; i++)
	if (std::string(textures[i]->name,8) == std::string(name,8))
	    return i;
		
    return -1;
}



//
// R_TextureNumForName
// Calls R_CheckTextureNumForName,
//  aborts with error message.
//
int	R_TextureNumForName (char* name)
{
    int		i;
	
    i = R_CheckTextureNumForName (name);

    if (i==-1)
    {
		//JONNY// ...bad idea?
	/*I_Error ("R_TextureNumForName: %s not found",
		 name);*/
    }
    return i;
}




//
// R_PrecacheLevel
// Preloads all relevant graphics for the level.
//
int		flatmemory;
int		texturememory;
int		spritememory;

void R_PrecacheLevel (void)
{
    char*		flatpresent;
    char*		texturepresent;
    char*		spritepresent;

    int			i;
    int			j;
    int			k;
    int			lump;
    
    texture_t*		texture;
    thinker_t*		th;
    spriteframe_t*	sf;

    if (demoplayback)
	return;
    
    // Precache flats.
    flatpresent = (char*)malloc(numflats);
    memset (flatpresent,0,numflats);	

    for (i=0 ; i<numsectors ; i++)
    {
	flatpresent[sectors[i].floorpic] = 1;
	flatpresent[sectors[i].ceilingpic] = 1;
    }
	
    flatmemory = 0;

    for (i=0 ; i<numflats ; i++)
    {
	if (flatpresent[i])
	{
	    lump = firstflat + i;
	    flatmemory += WadManager::getLumpLength(i);
	    WadManager::getLump(lump);
	}
    }
    
    // Precache textures.
    texturepresent = (char*)malloc(numtextures);
    memset (texturepresent,0, numtextures);
	
    for (i=0 ; i<numsides ; i++)
    {
	texturepresent[sides[i].toptexture] = 1;
	texturepresent[sides[i].midtexture] = 1;
	texturepresent[sides[i].bottomtexture] = 1;
    }

    // Sky texture is always present.
    // Note that F_SKY1 is the name used to
    //  indicate a sky floor/ceiling as a flat,
    //  while the sky texture is stored like
    //  a wall texture, with an episode dependend
    //  name.
    texturepresent[skytexture] = 1;
	
    texturememory = 0;
    for (i=0 ; i<numtextures ; i++)
    {
	if (!texturepresent[i])
	    continue;

	texture = textures[i];
	
	for (j=0 ; j<texture->patchcount ; j++)
	{
	    lump = texture->patches[j].patch;
	    texturememory += WadManager::getLumpLength(lump);
	    auto data = WadManager::getLump(lump);

		//JONNY//	extracts and saves texture
		/*unsigned short* header = (unsigned short*)data;
		//get the patch header data
		unsigned short width = *header++;
		unsigned short height = *header++;
		//ignore offset for now
		header += 2;

		sf::Image image;
		image.create(width, height);
		unsigned int* columns = (unsigned int*)header;
		std::vector<unsigned int*> columnVector;
		auto tempWidth = width;
		while (tempWidth--)
			columnVector.push_back(columns++);
		//now we need to get the columns
		for (int x = 0; x < width;x++)
		{
			unsigned char* column = ((unsigned char*)data)+*columnVector[x];

			//read the header
			auto rowStart = *column++;
			auto pixelCount = *column+=2; //+=2 because dummy value
			for (int y = rowStart; y < rowStart + pixelCount; y++)
			{
				auto thisPixel = colors[*column++];
				if (x < width && y < height)
					image.setPixel(x, y, thisPixel);
			}
			//dummy value?
			column++;
		}
		std::string spriteName = texture->name;
		spriteName.append(".png");
		image.saveToFile(spriteName);*/
	}
    }
    
    // Precache sprites.
    spritepresent = (char*)malloc(numsprites);
    memset (spritepresent,0, numsprites);
	
    for (th = thinkercap.next ; th != &thinkercap ; th=th->next)
    {
	if (th->function.acp1 == (actionf_p1)P_MobjThinker)
	    spritepresent[((mobj_t *)th)->sprite] = 1;
    }
	
    spritememory = 0;
    for (i=0 ; i<numsprites ; i++)
    {
	if (!spritepresent[i])
	    continue;

	for (j=0 ; j<sprites[i].numframes ; j++)
	{
	    sf = &sprites[i].spriteframes[j];
		for (k = 0; k < 8; k++)
		{
			lump = firstspritelump + sf->lump[k];
			auto data = WadManager::getLump(lump);
			
			//JONNY//	this extracts the texture data
			/*
			//adapt pixels to 32bit colour
			unsigned short* header = (unsigned short*)data;
			//get the patch header data
			unsigned short width = *header++;
			unsigned short height = *header++;
			//ignore offset for now
			header += 2;

			sf::Image image;
			image.create(width, height);
			unsigned int* columns = (unsigned int*)header;
			std::vector<unsigned int*> columnVector;
			auto tempWidth = width;
			while (tempWidth--)
				columnVector.push_back(columns++);
			//now we need to get the columns
			for (int x = 0; x < width; x++)
			{
				unsigned char* column = ((unsigned char*)data) + *columnVector[x];

				//read the header
				auto rowStart = *column++;
				auto pixelCount = *column += 2; //+=2 because dummy value
				for (int y = rowStart; y < rowStart + pixelCount; y++)
				{
					auto thisPixel = colors[*column++];
					if (x < width && y < height)
						image.setPixel(x, y, thisPixel);
				}
				//dummy value?
				column++;
			}
			std::string spriteName = sprnames[i];
			spriteName.append(std::to_string(k) +".png");
			image.saveToFile(spriteName);*/
		}
	}
    }
}




