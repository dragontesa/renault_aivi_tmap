#pragma once

namespace SKT
{

class TMAP_RSM_UI_DLLSHARED_EXPORT TRandomUtils
{
public:
    static TRandomUtils* instance();

    static int randInt( int low, int high );

	TRandomUtils();

	~TRandomUtils();	
};

}

