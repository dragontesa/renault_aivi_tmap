#include "hostnames.h"
#include "environment.h"
#include <string.h>
#include <QFile>
#include <QSettings>

namespace SKT {
     namespace TmapHostNames {
         // Deployment Declarations
         namespace {
                eHostDeploymentId HostDeploymentId = HOST_DEPLOY_RELEASE; // default deployment

                // Deployment Host Table
                const char* TMAP_HOST_DEPLOYMENT[HOST_DEPLOY_MAX][HOST_MAX] = {
                // DEVELOPMENT DEPLOYMENT, id: HOST_DEPLOY_DEV
                {
                    NDDS_DEV_SERVER,
                    NDDS_DEV_SERVER,
                    AUTOCOMP_DEV_SERVER,
                    STREAM_MAP_DEV_SERVER,
                    COMPLEX_IMG_DEV_SERVER,
                    RESOURCE_DEV_SERVER,
                    REVERS_GEO_SERVER,
                    AQI_SERVER
                },
                // QC TESTING DEPLOYMENT id: HOST_DEPLOY_QC (current )
                {
                    NDDS_PRD_SERVER,
                    NDDS_PRD_SERVER2,
                    AUTOCOMP_PRD_SERVER,
                    STREAM_MAP_DEV_SERVER,
                    COMPLEX_IMG_PRD_SERVER,
                    RESOURCE_DEV_SERVER,
                    REVERS_GEO_SERVER,
                    AQI_SERVER
                },
                // QC FOR STG AVNEXT  id: HOST_DEPLOY_QC_AVNEXT
                {
                    AVNEXT_STG_NDDS_SERVER,
                    AVNEXT_STG_NDDS_SERVER2,
                    AVNEXT_STG_AUTOCOMP_SERVER,
                    AVNEXT_STG_STREAM_MAP_SERVER,
                    AVNEXT_STG_COMPLEX_IMG_SERVER,
                    AVNEXT_STG_RESOURCE_SERVER,
                    AVNEXT_STG_REVERS_GEO_SERVER,
                    AVNEXT_STG_AQI_SERVER
                },
                // BETA FULL PROD AVNEXT  id: HOST_DEPLOY_BETA
                {
                    AVNEXT_PRD_NDDS_SERVER,
                    AVNEXT_PRD_NDDS_SERVER2,
                    AVNEXT_PRD_AUTOCOMP_SERVER,
                    AVNEXT_PRD_STREAM_MAP_SERVER,
                    AVNEXT_PRD_COMPLEX_IMG_SERVER,
                    AVNEXT_PRD_RESOURCE_SERVER,
                    AVNEXT_PRD_REVERS_GEO_SERVER,
                    AVNEXT_PRD_AQI_SERVER
                },
                // RELEASE FULL PRIMARY PRODUCTION AVNEXT id:HOST_DEPLOY_RELEASE
                {
                    AVNEXT_PRD_NDDS_SERVER,
                    AVNEXT_PRD_NDDS_SERVER2,
                    AVNEXT_PRD_AUTOCOMP_SERVER,
                    AVNEXT_PRD_STREAM_MAP_SERVER,
                    AVNEXT_PRD_COMPLEX_IMG_SERVER,
                    AVNEXT_PRD_RESOURCE_SERVER,
                    AVNEXT_PRD_REVERS_GEO_SERVER,
                    AVNEXT_PRD_AQI_SERVER
                },
                // User Custom Deployment id: HOST_DEPLOY_CUSTOM
                {
                    // NDDS_DEV_SERVER,
                    // NDDS_PRD_SERVER2,
                    // AUTOCOMP_DEV_SERVER,
                    // STREAM_MAP_DEV_SERVER,
                    // COMPLEX_IMG_DEV_SERVER,
                    // RESOURCE_DEV_SERVER,
                    // REVERS_GEO_SERVER,
                    // AQI_SERVER
                    AVNEXT_PRD_NDDS_SERVER,
                    AVNEXT_PRD_NDDS_SERVER2,
                    AVNEXT_PRD_AUTOCOMP_SERVER,
                    AVNEXT_PRD_STREAM_MAP_SERVER,
                    AVNEXT_PRD_COMPLEX_IMG_SERVER,
                    AVNEXT_PRD_RESOURCE_SERVER,
                    AVNEXT_PRD_REVERS_GEO_SERVER,
                    AVNEXT_PRD_AQI_SERVER
                }
              };

              // Reserved for Host and Port would be added dynamically 
              char TMAP_HOST_EXCHANGE_DEPLOYMENT[HOST_MAX][256] = {
                      {
                          0,
                      },
              };

              int TMAP_HOST_PORT_EXCHANGE_DEPLOYMENT[HOST_MAX] = {
                  9443, // NDDS
                  9443, // NDDS2
                  13132, // AUTOCOMP
                  443, // STREAM MAP
                  443, // COMPLEX
                  443, // RESOURCE
                  10612, // REVERS GEO
                  443  // AQI
              };

                const char* hostGroupByType[HOST_MAX][HOST_TYPE_MAX] = {
                    // NDDS
                    {
                        NDDS_PRD_SERVER, // RPD
                        NDDS_PRD_SERVER, // PRD+FREE
                        NDDS_STG_SERVER, // STG
                        NDDS_DEV_SERVER, // DEV
                        NDDS_DEV_SERVER, // OLD
                        AVNEXT_PRD_NDDS_SERVER, // AVNEXT PRD
                        AVNEXT_STG_NDDS_SERVER  // AVNEXT STG
                    },
                    // NDDS 2
                    {
                        NDDS_PRD_SERVER2,
                        NDDS_PRD_SERVER2,
                        NDDS_STG_SERVER,
                        NDDS_DEV_SERVER,
                        NDDS_DEV_SERVER,
                        AVNEXT_PRD_NDDS_SERVER2,
                        AVNEXT_STG_NDDS_SERVER2
                    },
                    // AUTO COMPLETED
                    {
                        AUTOCOMP_PRD_SERVER,
                        AUTOCOMP_PRD_SERVER2,
                        AUTOCOMP_PRD_SERVER2,
                        AUTOCOMP_DEV_SERVER,
                        AUTOCOMP_DEV_SERVER,
                        AVNEXT_PRD_AUTOCOMP_SERVER,
                        AVNEXT_STG_AUTOCOMP_SERVER
                    },
                    // STREAM MAP
                    {
                        STREAM_MAP_PRD_SERVER,
                        STREAM_MAP_PRD_SERVER,
                        STREAM_MAP_PRD_SERVER,
                        STREAM_MAP_DEV_SERVER,
                        STREAM_MAP_OLD_SERVER,
                        AVNEXT_PRD_STREAM_MAP_SERVER,
                        AVNEXT_STG_STREAM_MAP_SERVER
                    },
                    // COMPLEX RSD
                    {
                        COMPLEX_IMG_PRD_SERVER,
                        COMPLEX_IMG_PRD_SERVER,
                        COMPLEX_IMG_PRD_SERVER,
                        COMPLEX_IMG_DEV_SERVER,
                        COMPLEX_IMG_DEV_SERVER,
                        AVNEXT_PRD_COMPLEX_IMG_SERVER,
                        AVNEXT_STG_COMPLEX_IMG_SERVER
                    },
                    // RESOURCE
                    {
                        RESOURCE_PRD_SERVER,
                        RESOURCE_PRD_SERVER,
                        RESOURCE_PRD_SERVER,
                        RESOURCE_DEV_SERVER,
                        RESOURCE_DEV_SERVER,
                        AVNEXT_PRD_RESOURCE_SERVER,
                        AVNEXT_STG_RESOURCE_SERVER
                    },
                    // REVERSE GEO
                    {
                        REVERS_GEO_SERVER,
                        REVERS_GEO_SERVER,
                        REVERS_GEO_SERVER,
                        REVERS_GEO_SERVER,
                        REVERS_GEO_SERVER,
                        AVNEXT_PRD_REVERS_GEO_SERVER,
                        AVNEXT_STG_REVERS_GEO_SERVER
                    },
                    // AQI
                    {
                        AQI_SERVER,
                        AQI_SERVER,
                        AQI_SERVER,
                        AQI_SERVER,
                        AQI_SERVER,
                        AVNEXT_PRD_AQI_SERVER,
                        AVNEXT_STG_AQI_SERVER
                    }
                };

         }

         // return Host Deployment Id
         eHostDeploymentId getHostDeploymentId() {
             return HostDeploymentId;
         }

        // return Host Name
        const char* getHostName(eHostIndex hostIndex) {
             return TMAP_HOST_DEPLOYMENT[HostDeploymentId][hostIndex];
        }
        const char* getHostNameBy(eHostIndex hostIndex, const char* yourHostName) {
            for (int i=0;i<HOST_TYPE_MAX;i++) {
                if (strcmp(hostGroupByType[hostIndex][i], yourHostName) == 0)
                    return hostGroupByType[hostIndex][i];
            }
            return 0;
        }

        // return Host server type
        eHostType getHostType(eHostIndex hostIndex) {
            eHostType typ = HOST_PRD;
            const char* hostname = TMAP_HOST_DEPLOYMENT[HostDeploymentId][hostIndex];
            switch (hostIndex) {
            case HOST_NDDS_INDEX:
                if (hostname == NDDS_PRD_SERVER)
                    typ = HOST_PRD;
                else if (hostname == AVNEXT_PRD_NDDS_SERVER)
                    typ = HOST_AVNEXT_PRD;
                else if (hostname == AVNEXT_STG_NDDS_SERVER)
                    typ = HOST_AVNEXT_STG;
                else if (hostname == NDDS_STG_SERVER)
                    typ = HOST_STG;
                else if (hostname == TMAP_HOST_EXCHANGE_DEPLOYMENT[HOST_NDDS_INDEX])
                    typ = HOST_PRD;
                else
                    typ = HOST_DEV;
                break;
            case HOST_NDDS2_INDEX:
                if (hostname == NDDS_PRD_SERVER2 || hostname == NDDS_PRD_SERVER)
                   typ = HOST_PRD;
                else if (hostname == AVNEXT_PRD_NDDS_SERVER2)
                   typ = HOST_AVNEXT_PRD;
                else if (hostname == AVNEXT_STG_NDDS_SERVER2)
                   typ = HOST_AVNEXT_STG;
                else if (hostname == NDDS_STG_SERVER)
                   typ = HOST_STG;
                else if (hostname == TMAP_HOST_EXCHANGE_DEPLOYMENT[HOST_NDDS2_INDEX])
                   typ = HOST_PRD;
                else
                   typ = HOST_DEV;
               break;
            case HOST_AUTOCOMP_INDEX:
                if (hostname == AUTOCOMP_PRD_SERVER)
                    typ = HOST_PRD;
                else if (hostname == AUTOCOMP_PRD_SERVER2)
                    typ = HOST_PRD_FREE;
                else if (hostname == AVNEXT_PRD_AUTOCOMP_SERVER)
                    typ = HOST_AVNEXT_PRD;
                else if (hostname == AVNEXT_STG_AUTOCOMP_SERVER)
                    typ = HOST_AVNEXT_STG;
                else if (hostname == TMAP_HOST_EXCHANGE_DEPLOYMENT[HOST_AUTOCOMP_INDEX])
                   typ = HOST_PRD;
                else
                    typ = HOST_DEV;
               break;
            case HOST_STREAMMAP_INDEX:
               if (hostname == STREAM_MAP_PRD_SERVER)
                    typ = HOST_PRD;
               else if (hostname == AVNEXT_PRD_STREAM_MAP_SERVER)
                    typ = HOST_AVNEXT_PRD;
               else if (hostname == AVNEXT_STG_STREAM_MAP_SERVER)
                    typ = HOST_AVNEXT_STG;
               else if(hostname == STREAM_MAP_OLD_SERVER)
                     typ = HOST_OLD;
               else if (hostname == TMAP_HOST_EXCHANGE_DEPLOYMENT[HOST_STREAMMAP_INDEX])
                   typ = HOST_PRD;
               else
                     typ = HOST_DEV;
                break;
            case HOST_COMPLEX_INDEX:
                if (hostname == COMPLEX_IMG_PRD_SERVER)
                    typ = HOST_PRD;
                else if (hostname == AVNEXT_PRD_COMPLEX_IMG_SERVER)
                    typ = HOST_AVNEXT_PRD;
                else if (hostname == AVNEXT_STG_COMPLEX_IMG_SERVER)
                    typ = HOST_AVNEXT_STG;
                else if (hostname == TMAP_HOST_EXCHANGE_DEPLOYMENT[HOST_COMPLEX_INDEX])
                   typ = HOST_PRD;
                else
                    typ = HOST_DEV;
                break;
            case HOST_RESOURCE_INDEX:
                if (hostname == RESOURCE_PRD_SERVER)
                    typ = HOST_PRD;
                else if (hostname == AVNEXT_PRD_RESOURCE_SERVER)
                    typ = HOST_AVNEXT_PRD;
                else if (hostname == AVNEXT_STG_RESOURCE_SERVER)
                    typ = HOST_AVNEXT_STG;
                else if (hostname == TMAP_HOST_EXCHANGE_DEPLOYMENT[HOST_RESOURCE_INDEX])
                   typ = HOST_PRD;
                else
                    typ = HOST_DEV;
                break;
            case HOST_REVERSGEO_INDEX:
                if (hostname == REVERS_GEO_SERVER)
                    typ = HOST_PRD;
                else if (hostname == AVNEXT_PRD_REVERS_GEO_SERVER)
                    typ = HOST_AVNEXT_PRD;
                else if (hostname == AVNEXT_STG_REVERS_GEO_SERVER)
                    typ = HOST_AVNEXT_STG;
                else if (hostname == TMAP_HOST_EXCHANGE_DEPLOYMENT[HOST_REVERSGEO_INDEX])
                   typ = HOST_PRD;
                else
                    typ = HOST_PRD;
                break;
            case HOST_AQI_INDEX:
                if (hostname == AQI_SERVER)
                    typ = HOST_PRD;
                else if (hostname == AVNEXT_PRD_AQI_SERVER)
                    typ = HOST_AVNEXT_PRD;
                else if (hostname == AVNEXT_STG_AQI_SERVER)
                    typ = HOST_AVNEXT_STG;
                else if (hostname == TMAP_HOST_EXCHANGE_DEPLOYMENT[HOST_AQI_INDEX])
                   typ = HOST_PRD;
                else
                    typ = HOST_PRD;
                break;
              default:
              break;
            }
            return typ;
        }
        
        int getHostPort(eHostIndex hostIndex) {
            return TMAP_HOST_PORT_EXCHANGE_DEPLOYMENT[hostIndex];
        }
        
        // return deployment specified with deployId, so you can customize deployment for yourself,
        void getHostDeployment(const char* outDeployHosts[HOST_MAX], eHostDeploymentId deployId) {
            eHostDeploymentId deploymentId = deployId;
            if (deploymentId < HOST_DEPLOY_DEV || deploymentId >= HOST_DEPLOY_MAX)
                deploymentId = HostDeploymentId;
                 
            for (int i=0;i<HOST_MAX;i++) {
                outDeployHosts[i] = TMAP_HOST_DEPLOYMENT[deploymentId%HOST_DEPLOY_MAX][i];
            }
        }

        void setHostDeployment(const char* customDeployHosts[HOST_MAX]) {
            for (int i=0;i<HOST_MAX;i++) {
                TMAP_HOST_DEPLOYMENT[HOST_DEPLOY_CUSTOM][i] = customDeployHosts[i];
            }

            HostDeploymentId = HOST_DEPLOY_CUSTOM;

            // save into file
            QString hostIniPath = SKT::Environment::getPreferencePath() + QStringLiteral("/host.ini");
            QFile file(hostIniPath);
            if (!file.exists()) {
                file.open(QFile::WriteOnly);
                file.close();
            }

            QSettings hostIni(hostIniPath, QSettings::IniFormat);
            QStringList lastDeployment;
            for (int i=0;i<TmapHostNames::HOST_MAX;i++)
                lastDeployment.append(customDeployHosts[i]);

            hostIni.setValue(QStringLiteral("DeploymentId"),  TmapHostNames::HOST_DEPLOY_CUSTOM);
            hostIni.setValue(QStringLiteral("Deployment"), QVariant::fromValue(lastDeployment));
        }

         // reset deployment as default
         void resettHostDeployment() {
             HostDeploymentId = HOST_DEPLOY_RELEASE;
         }


        void exchangeHostDeploymentWithBaseName(const char* baseName) {
            for (int i=0;i<HOST_MAX;i++) {
                memset(TMAP_HOST_EXCHANGE_DEPLOYMENT[i],0,256);
            }

            strcpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[0],AVNEXT_SERVICE_NDDS);
            strcpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[1],AVNEXT_SERVICE_NDDS2);
            strcpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[2],AVNEXT_SERVICE_AUTOCOMP);
            strcpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[3],AVNEXT_SERVICE_STREAMMAP);
            strcpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[4],AVNEXT_SERVICE_COMPLEXINTER);
            strcpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[5],AVNEXT_SERVICE_RESOURCE);
            strcpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[6],AVNEXT_SERVICE_REVERS_GEO);
            strcpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[7],AVNEXT_SERVICE_AQI);

            // prevent buffer overflow and guarantee null termianted
            strncpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[0]+strlen(AVNEXT_SERVICE_NDDS),baseName,255-strlen(AVNEXT_SERVICE_NDDS));
            strncpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[1]+strlen(AVNEXT_SERVICE_NDDS2),baseName,255-strlen(AVNEXT_SERVICE_NDDS2));
            strncpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[2]+strlen(AVNEXT_SERVICE_AUTOCOMP),baseName,255-strlen(AVNEXT_SERVICE_AUTOCOMP));
            strncpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[3]+strlen(AVNEXT_SERVICE_STREAMMAP),baseName,255-strlen(AVNEXT_SERVICE_STREAMMAP));
            strncpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[4]+strlen(AVNEXT_SERVICE_COMPLEXINTER),baseName,255-strlen(AVNEXT_SERVICE_COMPLEXINTER));
            strncpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[5]+strlen(AVNEXT_SERVICE_RESOURCE),baseName,255-strlen(AVNEXT_SERVICE_RESOURCE));
            strncpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[6]+strlen(AVNEXT_SERVICE_REVERS_GEO),baseName,255-strlen(AVNEXT_SERVICE_REVERS_GEO));
            strncpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[7]+strlen(AVNEXT_SERVICE_AQI),baseName,255-strlen(AVNEXT_SERVICE_AQI));

            const char* customDeployment[HOST_MAX] = {
                TMAP_HOST_EXCHANGE_DEPLOYMENT[0],
                TMAP_HOST_EXCHANGE_DEPLOYMENT[1],
                TMAP_HOST_EXCHANGE_DEPLOYMENT[2],
                TMAP_HOST_EXCHANGE_DEPLOYMENT[3],
                TMAP_HOST_EXCHANGE_DEPLOYMENT[4],
                TMAP_HOST_EXCHANGE_DEPLOYMENT[5],
                TMAP_HOST_EXCHANGE_DEPLOYMENT[6],
                TMAP_HOST_EXCHANGE_DEPLOYMENT[7]
            };
            setHostDeployment(customDeployment);
        }

        // setup deployment at first time
         void setupHostDeployment() {
            eHostDeploymentId deploymentId = HostDeploymentId;
            QString hostIniPath = SKT::Environment::getPreferencePath() + QStringLiteral("/host.ini");
            QFile file(hostIniPath);
            if (file.exists()) {
                QSettings hostIni(hostIniPath, QSettings::IniFormat);
                deploymentId = (TmapHostNames::eHostDeploymentId)hostIni.value(QStringLiteral("DeploymentId")).toInt();
                if (deploymentId < HOST_DEPLOY_DEV || deploymentId >= HOST_DEPLOY_MAX) {
                    resettHostDeployment();
                    return;
                }

                HostDeploymentId = deploymentId;

                // update deployment if deployment is modifed
                if (HOST_DEPLOY_CUSTOM == deploymentId) {
                    QStringList deployment = hostIni.value(QStringLiteral("Deployment")).value<QStringList>();
                    const char *custom[TmapHostNames::HOST_MAX];
                    // init with custom preset
                    for (int i = 0; i < HOST_MAX; i++) {
                        custom[i] = TMAP_HOST_DEPLOYMENT[HOST_DEPLOY_CUSTOM][i];
                    }
                    // read custom
                    for (int i = 0; i < TmapHostNames::HOST_MAX && i < deployment.size(); i++) {
                        custom[i] = TmapHostNames::getHostNameBy((TmapHostNames::eHostIndex)i, deployment.at(i).toLatin1().constData());
                        if (custom[i] == 0) {
                            exchangeHost((TmapHostNames::eHostIndex)i, deployment.at(i).toLatin1().constData());
                            custom[i] = TMAP_HOST_EXCHANGE_DEPLOYMENT[i];
                        }
                    }
                    // setup
                    for (int i = 0; i < HOST_MAX; i++) {
                        TMAP_HOST_DEPLOYMENT[HOST_DEPLOY_CUSTOM][i] = custom[i];
                    }
                }
            }
            else {
                resettHostDeployment();
            }
         }

         void exchangeHost(eHostIndex hostIndex, const char* hostName) {
             strncpy(TMAP_HOST_EXCHANGE_DEPLOYMENT[hostIndex%HOST_MAX],hostName,255);
         }

         void exchangeHostPort(eHostIndex hostIndex, int port) {
             TMAP_HOST_PORT_EXCHANGE_DEPLOYMENT[hostIndex%HOST_MAX] = port;
         }

         void exchangeBaseName(const char* baseName) {
             char baseNameBuff[256] = {0,};
             strncpy(baseNameBuff, baseName, 255);
             exchangeHostDeploymentWithBaseName(baseNameBuff);
         }


     } // TmapHostNames

}
