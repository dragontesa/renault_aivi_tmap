#ifndef _PLATFORM_CONNECTIVITY_H_
#define _PLATFORM_CONNECTIVITY_H_
namespace SKT {
class PlatformConnectivity
{
    public:
       virtual ~PlatformConnectivity() {} ;
       virtual int init() = 0;
       virtual int release() = 0;
};
}
#endif /* _PLATFORM_CONNECTIVITY_H_ */
