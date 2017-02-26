#pragma once

#include <vector>
#include <fstream>

typedef struct
{
    int			filepos;
    int			size;
    char		name[8];

} filelump_t;

//
// WADFILE I/O related stuff.
//
typedef struct
{
    char	name[8];
    std::vector<char> data;
    int		size;
} lumpinfo_t;

/////////////////////////////////////////////
/// Manages the WAD files loaded in the game
///
/////////////////////////////////////////////
class WadManager
{
public:

    int W_NumLumps();

    static void W_AddFile(const std::string& fileName);
    //static			lumpcache;
    static std::vector<lumpinfo_t>	lumpinfo;

    static void    W_InitMultipleFiles(std::vector<std::string> filenames);
 
    static int	   W_CheckNumForName(const std::string& name);
    static int	   W_GetNumForName(const std::string& name);
  
    static int	          W_LumpLength(int lump);
  
    static void*   W_CacheLumpNum(int lump);
    static void*   W_CacheLumpName(const std::string& name);

private:
    static int numlumps;
    std::vector<unsigned char>   fileBytes;
};