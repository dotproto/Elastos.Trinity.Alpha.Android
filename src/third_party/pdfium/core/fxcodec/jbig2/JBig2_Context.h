// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_JBIG2_JBIG2_CONTEXT_H_
#define CORE_FXCODEC_JBIG2_JBIG2_CONTEXT_H_

#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fxcodec/fx_codec_def.h"
#include "core/fxcodec/jbig2/JBig2_Page.h"
#include "core/fxcodec/jbig2/JBig2_Segment.h"
#include "core/fxcrt/fx_safe_types.h"

class CJBig2_ArithDecoder;
class CJBig2_GRDProc;
class CPDF_StreamAcc;
class PauseIndicatorIface;

// Cache is keyed by the ObjNum of a stream and an index within the stream.
using CJBig2_CacheKey = std::pair<uint32_t, uint32_t>;
using CJBig2_CachePair =
    std::pair<CJBig2_CacheKey, std::unique_ptr<CJBig2_SymbolDict>>;

#define JBIG2_SUCCESS 0
#define JBIG2_FAILED -1
#define JBIG2_ERROR_TOO_SHORT -2
#define JBIG2_ERROR_FATAL -3
#define JBIG2_END_OF_PAGE 2
#define JBIG2_END_OF_FILE 3
#define JBIG2_ERROR_LIMIT -6
#define JBIG2_MIN_SEGMENT_SIZE 11

class CJBig2_Context {
 public:
  CJBig2_Context(const RetainPtr<CPDF_StreamAcc>& pGlobalStream,
                 const RetainPtr<CPDF_StreamAcc>& pSrcStream,
                 std::list<CJBig2_CachePair>* pSymbolDictCache,
                 bool bIsGlobal);
  ~CJBig2_Context();

  static bool HuffmanAssignCode(JBig2HuffmanCode* SBSYMCODES, uint32_t NTEMP);

  int32_t GetFirstPage(uint8_t* pBuf,
                       int32_t width,
                       int32_t height,
                       int32_t stride,
                       PauseIndicatorIface* pPause);

  int32_t Continue(PauseIndicatorIface* pPause);
  FXCODEC_STATUS GetProcessingStatus() const { return m_ProcessingStatus; }

 private:
  int32_t DecodeSequential(PauseIndicatorIface* pPause);
  int32_t DecodeRandomFirstPage(PauseIndicatorIface* pPause);
  int32_t DecodeRandom(PauseIndicatorIface* pPause);

  CJBig2_Segment* FindSegmentByNumber(uint32_t dwNumber);
  CJBig2_Segment* FindReferredTableSegmentByIndex(CJBig2_Segment* pSegment,
                                                  int32_t nIndex);

  int32_t ParseSegmentHeader(CJBig2_Segment* pSegment);
  int32_t ParseSegmentData(CJBig2_Segment* pSegment,
                           PauseIndicatorIface* pPause);
  int32_t ProcessingParseSegmentData(CJBig2_Segment* pSegment,
                                     PauseIndicatorIface* pPause);
  int32_t ParseSymbolDict(CJBig2_Segment* pSegment);
  int32_t ParseTextRegion(CJBig2_Segment* pSegment);
  int32_t ParsePatternDict(CJBig2_Segment* pSegment,
                           PauseIndicatorIface* pPause);
  int32_t ParseHalftoneRegion(CJBig2_Segment* pSegment,
                              PauseIndicatorIface* pPause);
  int32_t ParseGenericRegion(CJBig2_Segment* pSegment,
                             PauseIndicatorIface* pPause);
  int32_t ParseGenericRefinementRegion(CJBig2_Segment* pSegment);
  int32_t ParseTable(CJBig2_Segment* pSegment);
  int32_t ParseRegionInfo(JBig2RegionInfo* pRI);

  std::vector<JBig2HuffmanCode> DecodeSymbolIDHuffmanTable(uint32_t SBNUMSYMS);

  std::unique_ptr<CJBig2_Context> m_pGlobalContext;
  std::unique_ptr<CJBig2_BitStream> m_pStream;
  std::vector<std::unique_ptr<CJBig2_Segment>> m_SegmentList;
  std::vector<std::unique_ptr<JBig2PageInfo>> m_PageInfoList;
  std::unique_ptr<CJBig2_Image> m_pPage;
  size_t m_nSegmentDecoded;
  bool m_bInPage;
  bool m_bBufSpecified;
  int32_t m_PauseStep;
  FXCODEC_STATUS m_ProcessingStatus;
  std::vector<JBig2ArithCtx> m_gbContext;
  std::unique_ptr<CJBig2_ArithDecoder> m_pArithDecoder;
  std::unique_ptr<CJBig2_GRDProc> m_pGRD;
  std::unique_ptr<CJBig2_Segment> m_pSegment;
  FX_SAFE_UINT32 m_dwOffset;
  JBig2RegionInfo m_ri;
  std::list<CJBig2_CachePair>* const m_pSymbolDictCache;
  bool m_bIsGlobal;
};

#endif  // CORE_FXCODEC_JBIG2_JBIG2_CONTEXT_H_
