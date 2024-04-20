#ifndef QCTESTINGFEATURES_H
#define QCTESTINGFEATURES_H

#define TMAP_TMC_QC_TESTING // for testing while QC
#define TMAP_TMC_DEV_TESTING  // for development

#ifdef TMAP_TMC_QC_TESTING
// gps
 // mmfb usage [hidden menu spec]
#define TMAP_TMC_QC__MMFB \
   virtual void changeMMFeedback(bool enabled) {} \
   virtual void changeOnlyTunnelMMFB(bool enabled) {} \
   virtual void changeMMFeedbackLog(bool enabled) {} \
   virtual void changeMMFeedbackGpsMethod(int method) {} \
   virtual void changeMMFeedbackAngleOnTunnel(int method) {} \
   virtual void changeMMFeedbackDRCep(bool enabled) {} \
   virtual void changeMMFeedbackDRCepRange(int method) {} \
   virtual void changeMMFeedbackDRCepMMRange(int method) {} \
   virtual void convertGpsLogToKML() {} \
   virtual int getGpsMethod() {} \
   virtual void setLogPath(QString path) {} \
   virtual void changeOpenLRLog(bool enabled) {} \

#define TMAP_TMC_QC__MMFB_OVERRIDE \
  void changeMMFeedback(bool enabled); \
  void changeOnlyTunnelMMFB(bool enabled); \
  void changeMMFeedbackLog(bool enabled); \
  void changeMMFeedbackGpsMethod(int method); \
  void changeMMFeedbackAngleOnTunnel(int method); \
  void changeMMFeedbackDRCep(bool enabled); \
  void changeMMFeedbackDRCepRange(int method); \
  void changeMMFeedbackDRCepMMRange(int method); \
  void convertGpsLogToKML();    \
  int getGpsMethod();    \
  void setLogPath(QString path);  \
  void changeOpenLRLog(bool enabled); \

#define TMAP_TMC_QC_MMFB_DECLARE \
  bool m_useMMFB; \
  bool m_useOnlyTunnelMMFB; \
  bool m_useSaveMMFBLog; \
  int  m_MmfbGpsMethod; \
  int  m_MmfbAngleOnTunnel; \
  bool m_MmfbDrCEP; \
  int m_MmfbDrCEPrange; \
  bool m_useSaveOpenLRLog; \


// vsm
#define MAPVIEW_FPS_TEST
#define MAPVIEW_FLICK_SPEED_TEST
  #ifdef MAPVIEW_FLICK_SPEED_TEST
  #define MAPVIEW_FST_DECLARE \
  bool fst = false; \
  qreal fstSpeed = 0.0; \
  QTime fstTime;
  #else
  #define MAPVIEW_FST_DECLARE
#endif
// mns


// ui


#else

#define TMAP_TMC_QC__MMFB
#define TMAP_TMC_QC__MMFB_OVERRIDE
#define MAPVIEW_FST_DECLARE

#endif





#endif // QCTESTINGFEATURES_H
