
#include "w_wad.hpp"

#include "i_system.hpp"

//STATICS
std::vector<lumpinfo_t>		WadManager::lumpinfo;		
int			                WadManager::numlumps;

void mystrupr(char* s)
{
    while (*s) { *s = toupper(*s); s++; }
}

int myfilelength (std::ifstream& handle) 
{ 
    //store the pos
    auto pos = handle.tellg();
    handle.seekg(std::ios::end);

    //get the size
    auto size = handle.tellg();

    //return to pos
    handle.seekg(pos);

    return size;
}


void
ExtractFileBase
( char*		path,
  char*		dest )
{
    char*	src;
    int		length;

    src = path + strlen(path) - 1;
    
    // back up until a \ or the start
    while (src != path
	   && *(src-1) != '\\'
	   && *(src-1) != '/')
    {
	src--;
    }
    
    // copy up to eight characters
    memset (dest,0,8);
    length = 0;
    
    while (*src && *src != '.')
    {
	if (++length == 9)
	    I_Error ("Filename base of %s >8 chars",path);

	*dest++ = toupper((int)*src++);
    }
}





//
// LUMP BASED ROUTINES.
//

//
// W_AddFile
// All files are optional, but at least one file must be
//  found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
//
void WadManager::W_AddFile (const std::string& filename)
{
    lumpinfo_t*		lump_p;
    int		i;
    std::ifstream	fileHandle;
    int			    length = 0;
    int			    startlump;
    std::vector<filelump_t>		fileinfo;
    filelump_t		singleinfo;
    int			    storehandle;
    
    // open the file and add to directory
    fileHandle.open(filename.c_str(), std::ios::binary);
    if (!fileHandle.good())
    {
		printf (" couldn't open %s\n",filename.c_str());
		return;
    }

    printf (" adding %s\n",filename.c_str());
    startlump = numlumps;
	
    if (filename.substr(filename.find_last_of(".")) == "wad" )
    {
		// single lump file
		//fileinfo = &singleinfo;
		singleinfo.filepos = 0;
		singleinfo.size = myfilelength(fileHandle);
		ExtractFileBase (const_cast<char*>(filename.c_str()), singleinfo.name);
		numlumps++;
    }
    else 
    {
		//Read the WAD file header
        std::string identification(4,0);
		fileHandle.read(const_cast<char*>(identification.data()), identification.size());
		if (identification == "IWAD")
		{
		    // Homebrew levels?
		    if (identification == "PWAD")
		    {
			    I_Error ("Wad file %s doesn't have IWAD or PWAD id\n", filename.c_str());
		    }	
		}

        int numLumps = 0;
        fileHandle.read(reinterpret_cast<char*>(&numLumps),sizeof(numLumps));
		length = numLumps *sizeof(filelump_t);
		fileinfo.resize(numLumps);

        int infotableofs;
        fileHandle.read(reinterpret_cast<char*>(&infotableofs), sizeof(infotableofs));
		fileHandle.seekg(infotableofs,std::ios::beg);
		fileHandle.read (reinterpret_cast<char*>(fileinfo.data()), length);
		numlumps += numLumps;
    }

    // Fill in lumpinfo
    lumpinfo.resize(numlumps);

    lump_p = &lumpinfo[startlump];
	
    auto fileInfo_p = fileinfo.begin();
    for (i=startlump ; i<numlumps ; i++,lump_p++, fileInfo_p++)
    {
        //seek to the position
        fileHandle.seekg(fileInfo_p->filepos, std::ios::beg);

        //reserve memory for the lump
        lump_p->size = fileInfo_p->size;
        lump_p->data.resize(fileInfo_p->size);

        //store the name
		strncpy (lump_p->name, fileInfo_p->name, 8);

        //read the data
        fileHandle.read(reinterpret_cast<char*>(lump_p->data.data()), lump_p->size);
	}
}

//
// W_InitMultipleFiles
// Pass a null terminated list of files to use.
// All files are optional, but at least one file
//  must be found.
// Files with a .wad extension are idlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
// Lump names can appear multiple times.
// The name searcher looks backwards, so a later file
//  does override all earlier ones.
//
void WadManager::W_InitMultipleFiles (std::vector<std::string> filenames)
{	
    int		size;
    
    // open all the files, load headers and lumps
    numlumps = 0;

    for (auto& filename : filenames)
		W_AddFile (filename);

    if (!numlumps)
	    I_Error ("W_InitFiles: no files found");
}


//
// W_NumLumps
//
int WadManager::W_NumLumps (void)
{
    return numlumps;
}


//
// WadManager::W_CheckNumForName
// Returns -1 if name not found.
//

int WadManager::W_CheckNumForName (const std::string& name)
{
    union {
	char	s[9];
	int	x[2];
	
    } name8;
    
    int		v1;
    int		v2;

    // make the name into two integers for easy compares
    strncpy (name8.s,name.c_str(),8);

    // in case the name was a fill 8 chars
    name8.s[8] = 0;	

    // case insensitive
    mystrupr (name8.s);		

    v1 = name8.x[0];
    v2 = name8.x[1];


    for (int i=lumpinfo.size()-1;i>=0;i--)
    {
	    if ( *(int *)lumpinfo[i].name == v1
	         && *(int *)&lumpinfo[i].name[4] == v2)
	    {
	        return i;
	    }
    }

    // TFB. Not found.
    return -1;
}




//
// W_GetNumForName
// Calls WadManager::W_CheckNumForName, but bombs out if not found.
//
int WadManager::W_GetNumForName (const std::string& name)
{
    int	i;

    i = WadManager::W_CheckNumForName (name);
    
    if (i == -1)
      I_Error ("W_GetNumForName: %s not found!", name);
      
    return i;
}


//
// WadManager::W_LumpLength
// Returns the buffer size needed to load the given lump.
//
int WadManager::W_LumpLength (int lump)
{
    if (lump >= numlumps)
	I_Error ("WadManager::W_LumpLength: %i >= numlumps",lump);

    return lumpinfo[lump].size;
}

//
// WadManager::W_CacheLumpNum
//
void* WadManager::W_CacheLumpNum( int lump)
{
    if (lumpinfo.size() < lump)
        I_Error("Lump %d requested when only %d lumps available", lump, lumpinfo.size());
	
    return lumpinfo[lump].data.data();
}

//
// WadManager::W_CacheLumpName
//
void*
WadManager::W_CacheLumpName( const std::string& name )
{
    return WadManager::W_CacheLumpNum (W_GetNumForName(name));
}