#include "w_wad.hpp"
#include "i_system.hpp"

/// \brief  The wad manager lumpinfo.
std::vector<lumpinfo_t>		WadManager::lumpinfo;		
/// \brief  The wad manager numlumps.
int			                WadManager::numlumps;

////////////////////////////////////////////////////////////////////////////////////////////////////
void WadManager::addFile (const std::string& filename)
{
    lumpinfo_t*		lump_p;
    int		i;
    std::ifstream	fileHandle;
    int			    length = 0;
    int			    startlump;
    std::vector<filelump_t>		fileinfo;
    
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
		/*singleinfo.filepos = 0;
		singleinfo.size = myfilelength(fileHandle);
		ExtractFileBase (const_cast<char*>(filename.c_str()), singleinfo.name);
		numlumps++;*/
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
		lump_p->name = std::string(fileInfo_p->name,8);

        //read the data
        fileHandle.read(reinterpret_cast<char*>(lump_p->data.data()), lump_p->size);
        printf("Loaded %s \n", lump_p->name.c_str());
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
void WadManager::initMultipleFiles (std::vector<std::string> filenames)
{	    
    // open all the files, load headers and lumps
    numlumps = 0;

    for (auto& filename : filenames)
		addFile (filename);

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

int WadManager::checkNumForName (const std::string& name)
{
    //convert the name to upper case first
    auto n = name;
    std::transform(n.begin(), n.end(),n.begin(), toupper);

    // We search in reverse, so the most recently added patches are found first
    for (int i=lumpinfo.size()-1;i>=0;i--)
    {
	    // Only check as many chars as there are in the parameter
        auto* l = &lumpinfo[i].name[0];
        bool match = true;
        for (auto c : n)
        {
            if (c != *l)
            {
                match = false;
                break;
            }
            l++;
        }
        if (match)
            return i;
    }

    // TFB. Not found.
    return -1;
}

//
// W_GetNumForName
// Calls WadManager::W_CheckNumForName, but bombs out if not found.
//
int WadManager::getNumForName (const std::string& name)
{
    int	i;

    i = WadManager::checkNumForName (name);
    
    if (i == -1)
      I_Error ("W_GetNumForName: %s not found!", name.c_str());
      
    return i;
}

std::string WadManager::getNameForNum(int i)
{
    return lumpinfo[i].name;
}


//
// WadManager::W_LumpLength
// Returns the buffer size needed to load the given lump.
//
int WadManager::getLumpLength (int lump)
{
    if (lump >= numlumps)
	I_Error ("WadManager::W_LumpLength: %i >= numlumps",lump);

    return lumpinfo[lump].size;
}

//
// WadManager::W_CacheLumpNum
//
void* WadManager::getLump( std::size_t lump)
{
    if (lumpinfo.size() < lump)
        I_Error("Lump %d requested when only %d lumps available", lump, lumpinfo.size());
	
    return lumpinfo[lump].data.data();
}

//
// WadManager::W_CacheLumpName
//
void*
WadManager::getLump( const std::string& name )
{
    return WadManager::getLump (getNumForName(name));
}