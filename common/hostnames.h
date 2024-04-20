#ifndef TMAP_RSM2018_COMMON_HOSTNAMES_H_
#define TMAP_RSM2018_COMMON_HOSTNAMES_H_

namespace SKT {
     namespace TmapHostNames {
        enum eHostDeploymentId {
            HOST_DEPLOY_DEV = 0, // full development servers
            HOST_DEPLOY_QC, // development or commercial , (current)
            HOST_DEPLOY_QC_AVNEXT, // stg avnext proxies  deployment
            HOST_DEPLOY_BETA, // most primary vnext proxies or stg avnext
            HOST_DEPLOY_RELEASE, // full avnext primary production deployment
            HOST_DEPLOY_CUSTOM, // customized by hidden menu
            HOST_DEPLOY_MAX
        };

        enum eHostIndex {
            HOST_NDDS_INDEX = 0,
            HOST_NDDS2_INDEX,
            HOST_AUTOCOMP_INDEX,
            HOST_STREAMMAP_INDEX,
            HOST_COMPLEX_INDEX,
            HOST_RESOURCE_INDEX,
            HOST_REVERSGEO_INDEX,
            HOST_AQI_INDEX,
            HOST_MAX
        };

        enum eHostType {
            HOST_PRD = 0, // 상용
            HOST_PRD_FREE, // 상용 비과금
            HOST_STG, // 스테이징
            HOST_DEV, // 개발
            HOST_OLD, // 구 상용
            HOST_AVNEXT_PRD, // VNEXT 상용
            HOST_AVNEXT_STG, // VNEXT 스테이징
            HOST_TYPE_MAX
        };

        eHostDeploymentId getHostDeploymentId();
        const char* getHostName(eHostIndex hostIndex);
        const char* getHostNameBy(eHostIndex hostIndex, const char* yourHostName);
        eHostType getHostType (eHostIndex hostIndex);
        int getHostPort(eHostIndex hostIndex);
        
        // return deployment specified with deployId, so you can customize deployment by yourself,
        void getHostDeployment(const char* outDeployHosts[HOST_MAX], eHostDeploymentId deployId=HOST_DEPLOY_RELEASE);

        // update your custom deployment
        void setHostDeployment(const char* inCustomDeployHosts[HOST_MAX]);
        void resettHostDeployment();
        void setupHostDeployment();

        // add a new host dynamically
        void exchangeHost(eHostIndex hostIndex, const char* hostName);
        void exchangeHostPort(eHostIndex hostIndex, int port);
        void exchangeBaseName(const char* baseName);

         // SKT HOSTS
         constexpr const char* NDDS_PRD_SERVER               = "ntmap.tmap.co.kr";  // NDDS 상용 domain
         constexpr const char* NDDS_PRD_SERVER2              = "ntmap2.tmap.co.kr"; // NDDS 상용 domain
         constexpr const char* NDDS_DEV_SERVER               = "ntmapdev.tmap.co.kr"; // NDDS 개발  domain
         constexpr const char* NDDS_STG_SERVER               = "ntmapstg.tmap.co.kr"; // NDDS staging domain
         
         constexpr const char* AUTOCOMP_PRD_SERVER           = "https://acf.tmap.co.kr:13132"; // NDDS autocomplete server commercial (과금)
         constexpr const char* AUTOCOMP_PRD_SERVER2          = "https://acnf.tmap.co.kr:13132"; // NDDS autocomplete server commercial (비과금)
         constexpr const char* AUTOCOMP_DEV_SERVER           = "http://searchdev.skplanet.co.kr:13131";
         
         constexpr const char* COMPLEX_IMG_PRD_SERVER        = "cdn.tmap.co.kr/rsd";
         constexpr const char* COMPLEX_IMG_DEV_SERVER        = "cdndev.tmap.co.kr/rsd";
         
         constexpr const char* STREAM_MAP_PRD_SERVER         = "vsmtile.tmap.co.kr";
         constexpr const char* STREAM_MAP_DEV_SERVER         = "dev-vsmtile.tmap.co.kr";
         constexpr const char* STREAM_MAP_OLD_SERVER         = "farm43.ids.skplanet.com";
         
         constexpr const char* RESOURCE_PRD_SERVER           = "vsmres.tmap.co.kr";
         constexpr const char* RESOURCE_DEV_SERVER           = "dev-vsmres.tmap.co.kr";
         
         constexpr const char* REVERS_GEO_SERVER             = "https://toptmap.tmap.co.kr:10612"; // revers-geocoding
         constexpr const char* AQI_SERVER                    = "https://tmc.tmap.co.kr";//
         
        //  constexpr const int  NDDS_PORTNUMBER                = 9443; // NDDS TCP port, this is common for all NDDS server
         
         // AVNEXT PROXY HOSTS
         constexpr const char* AVNEXT_STG_NDDS_SERVER        = "skt.spp.stg.kor.avnext.renault.com"; // vNext NDDS
         constexpr const char* AVNEXT_STG_NDDS_SERVER2       = "skt2.spp.stg.kor.avnext.renault.com"; // vNext NDDS
         constexpr const char* AVNEXT_STG_STREAM_MAP_SERVER  = "skt-config.spp.stg.kor.avnext.renault.com"; // vNext streaming map, voice guidance, safe db
         constexpr const char* AVNEXT_STG_RESOURCE_SERVER    = "skt-resource.spp.stg.kor.avnext.renault.com"; // vNext resource for map, voice guidance
         constexpr const char* AVNEXT_STG_COMPLEX_IMG_SERVER = "skt-compinter.spp.stg.kor.avnext.renault.com"; // vNext complex intersaction image,
         constexpr const char* AVNEXT_STG_AUTOCOMP_SERVER    = "skt-autocomp.spp.stg.kor.avnext.renault.com"; // vNext auto-complete server staging ";
         constexpr const char* AVNEXT_STG_REVERS_GEO_SERVER  = "skt-openmap.spp.stg.kor.avnext.renault.com"; // vNext revers geocoding
         constexpr const char* AVNEXT_STG_AQI_SERVER         = "skt-openmap.spp.stg.kor.avnext.renault.com"; // vNext revers geocoding
         
         constexpr const char* AVNEXT_PRD_NDDS_SERVER        = "skt.spp.prd.kor.avnext.renault.com"; // vNext NDDS
         constexpr const char* AVNEXT_PRD_NDDS_SERVER2       = "skt2.spp.prd.kor.avnext.renault.com"; // vNext NDDS
         constexpr const char* AVNEXT_PRD_STREAM_MAP_SERVER  = "skt-config.spp.prd.kor.avnext.renault.com"; // vNext streaming map, voice guidance, safe db
         constexpr const char* AVNEXT_PRD_RESOURCE_SERVER    = "skt-resource.spp.prd.kor.avnext.renault.com"; // vNext resource for map, voice guidance
         constexpr const char* AVNEXT_PRD_COMPLEX_IMG_SERVER = "skt-compinter.spp.prd.kor.avnext.renault.com"; // vNext complex intersaction image,
         constexpr const char* AVNEXT_PRD_AUTOCOMP_SERVER    = "skt-autocomp.spp.prd.kor.avnext.renault.com"; // vNext auto-complete server staging ";
         constexpr const char* AVNEXT_PRD_REVERS_GEO_SERVER  = "skt-openmap.spp.prd.kor.avnext.renault.com"; // vNext revers geocoding
         constexpr const char* AVNEXT_PRD_AQI_SERVER         = "skt-openmap.spp.prd.kor.avnext.renault.com"; // vNext revers geocoding

         // service name built in vnext spec
         constexpr const char* AVNEXT_SERVICE_NDDS           = "skt.spp";
         constexpr const char* AVNEXT_SERVICE_NDDS2          = "skt2.spp";
         constexpr const char* AVNEXT_SERVICE_STREAMMAP      = "skt-config.spp";
         constexpr const char* AVNEXT_SERVICE_RESOURCE       = "skt-resource.spp";
         constexpr const char* AVNEXT_SERVICE_COMPLEXINTER   = "skt-compinter.spp";
         constexpr const char* AVNEXT_SERVICE_AUTOCOMP       = "skt-autocomp.spp";
         constexpr const char* AVNEXT_SERVICE_REVERS_GEO     = "skt-openmap.spp";
         constexpr const char* AVNEXT_SERVICE_AQI            = "skt-openmap.spp";


    }
}

#endif // TMAP_RSM2018_COMMON_HOSTNAMES_H_
