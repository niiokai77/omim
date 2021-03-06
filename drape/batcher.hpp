#pragma once

#include "drape/attribute_provider.hpp"
#include "drape/glstate.hpp"
#include "drape/overlay_handle.hpp"
#include "drape/pointers.hpp"
#include "drape/render_bucket.hpp"
#include "drape/vertex_array_buffer.hpp"

#include "base/macros.hpp"

#include "std/map.hpp"
#include "std/function.hpp"

namespace dp
{

class RenderBucket;
class AttributeProvider;
class OverlayHandle;

class Batcher
{
public:
  static uint32_t const IndexPerTriangle = 3;
  static uint32_t const IndexPerQuad = 6;
  static uint32_t const VertexPerQuad = 4;

  Batcher(uint32_t indexBufferSize, uint32_t vertexBufferSize);
  ~Batcher();

  void InsertTriangleList(GLState const & state, ref_ptr<AttributeProvider> params);
  IndicesRange InsertTriangleList(GLState const & state, ref_ptr<AttributeProvider> params,
                                  drape_ptr<OverlayHandle> && handle);

  void InsertTriangleStrip(GLState const & state, ref_ptr<AttributeProvider> params);
  IndicesRange InsertTriangleStrip(GLState const & state, ref_ptr<AttributeProvider> params,
                                   drape_ptr<OverlayHandle> && handle);

  void InsertTriangleFan(GLState const & state, ref_ptr<AttributeProvider> params);
  IndicesRange InsertTriangleFan(GLState const & state, ref_ptr<AttributeProvider> params,
                                 drape_ptr<OverlayHandle> && handle);

  void InsertListOfStrip(GLState const & state, ref_ptr<AttributeProvider> params, uint8_t vertexStride);
  IndicesRange InsertListOfStrip(GLState const & state, ref_ptr<AttributeProvider> params,
                                 drape_ptr<OverlayHandle> && handle, uint8_t vertexStride);

  void InsertLineStrip(GLState const & state, ref_ptr<AttributeProvider> params);
  IndicesRange InsertLineStrip(GLState const & state, ref_ptr<AttributeProvider> params,
                               drape_ptr<OverlayHandle> && handle);

  void InsertLineRaw(GLState const & state, ref_ptr<AttributeProvider> params,
                     vector<int> const & indices);
  IndicesRange InsertLineRaw(GLState const & state, ref_ptr<AttributeProvider> params,
                             vector<int> const & indices, drape_ptr<OverlayHandle> && handle);

  typedef function<void (GLState const &, drape_ptr<RenderBucket> &&)> TFlushFn;
  void StartSession(TFlushFn const & flusher);
  void EndSession();

  void SetFeatureMinZoom(int minZoom);

private:
  template<typename TBatcher, typename ... TArgs>
  IndicesRange InsertPrimitives(GLState const & state, ref_ptr<AttributeProvider> params,
                                drape_ptr<OverlayHandle> && transferHandle, uint8_t vertexStride,
                                TArgs ... batcherArgs);

  class CallbacksWrapper;
  void ChangeBuffer(ref_ptr<CallbacksWrapper> wrapper);
  ref_ptr<RenderBucket> GetBucket(GLState const & state);

  void FinalizeBucket(GLState const & state);
  void Flush();

  TFlushFn m_flushInterface;

  using TBuckets = map<GLState, drape_ptr<RenderBucket>>;
  TBuckets m_buckets;

  uint32_t m_indexBufferSize;
  uint32_t m_vertexBufferSize;

  int m_featureMinZoom = 0;
};

class BatcherFactory
{
public:
  Batcher * GetNew() const;
};

class SessionGuard
{
public:
  SessionGuard(Batcher & batcher, Batcher::TFlushFn const & flusher);
  ~SessionGuard();

  DISALLOW_COPY_AND_MOVE(SessionGuard);
private:
  Batcher & m_batcher;
};

} // namespace dp
