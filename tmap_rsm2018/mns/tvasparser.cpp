#include "tvasparser.h"

namespace SKT {

void TvasParse::parse(char *pBuffer, unsigned int nSize){
    memcpy(&parsedData,pBuffer,sizeof(sTvasHeader45));
}

}
