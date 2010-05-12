#pragma once

#include "Registry.h"
#include "SimpleSerializer.h"
#include "ContainerSerializer.h"

namespace Reflect
{
  class REFLECT_API ArraySerializer : public ContainerSerializer
  {
  public:
    REFLECT_DECLARE_ABSTRACT( ArraySerializer, ContainerSerializer );

    virtual void SetSize(size_t size) = 0;

    virtual i32 GetItemType() const = 0;
    virtual SerializerPtr GetItem(size_t at) = 0;
    virtual ConstSerializerPtr GetItem(size_t at) const = 0;
    virtual void SetItem(size_t at, const Serializer* value) = 0;
  };

  template <class T>
  class REFLECT_API SimpleArraySerializer : public ArraySerializer
  {
  public:
    typedef std::vector<T> DataType;
    Serializer::DataPtr<DataType> m_Data;

    typedef SimpleArraySerializer<T> ArraySerializerT;
    REFLECT_DECLARE_CLASS( ArraySerializerT, ArraySerializer )

    SimpleArraySerializer();
    virtual ~SimpleArraySerializer();

    virtual void ConnectData(Nocturnal::HybridPtr<void> data) NOC_OVERRIDE;

    virtual size_t GetSize() const NOC_OVERRIDE;
    virtual void SetSize(size_t size) NOC_OVERRIDE;
    virtual void Clear() NOC_OVERRIDE;

    virtual i32 GetItemType() const NOC_OVERRIDE;
    virtual SerializerPtr GetItem(size_t at) NOC_OVERRIDE;
    virtual ConstSerializerPtr GetItem(size_t at) const NOC_OVERRIDE;
    virtual void SetItem(size_t at, const Serializer* value) NOC_OVERRIDE;

    virtual bool Set(const Serializer* src, u32 flags = 0) NOC_OVERRIDE;
    virtual bool Equals(const Serializer* s) const NOC_OVERRIDE;

    virtual void Serialize(Archive& archive) const NOC_OVERRIDE;
    virtual void Deserialize(Archive& archive) NOC_OVERRIDE;

    virtual std::ostream& operator >> (std::ostream& stream) const NOC_OVERRIDE;
    virtual std::istream& operator << (std::istream& stream) NOC_OVERRIDE;
  };

  typedef SimpleArraySerializer<std::string> StringArraySerializer;                       REFLECT_SPECIALIZE_SERIALIZER( StringArraySerializer );
  typedef SimpleArraySerializer<bool> BoolArraySerializer;                                REFLECT_SPECIALIZE_SERIALIZER( BoolArraySerializer );
  typedef SimpleArraySerializer<u8> U8ArraySerializer;                                    REFLECT_SPECIALIZE_SERIALIZER( U8ArraySerializer );
  typedef SimpleArraySerializer<i8> I8ArraySerializer;                                    REFLECT_SPECIALIZE_SERIALIZER( I8ArraySerializer );
  typedef SimpleArraySerializer<u16> U16ArraySerializer;                                  REFLECT_SPECIALIZE_SERIALIZER( U16ArraySerializer );
  typedef SimpleArraySerializer<i16> I16ArraySerializer;                                  REFLECT_SPECIALIZE_SERIALIZER( I16ArraySerializer );
  typedef SimpleArraySerializer<u32> U32ArraySerializer;                                  REFLECT_SPECIALIZE_SERIALIZER( U32ArraySerializer );
  typedef SimpleArraySerializer<i32> I32ArraySerializer;                                  REFLECT_SPECIALIZE_SERIALIZER( I32ArraySerializer );
  typedef SimpleArraySerializer<u64> U64ArraySerializer;                                  REFLECT_SPECIALIZE_SERIALIZER( U64ArraySerializer );
  typedef SimpleArraySerializer<i64> I64ArraySerializer;                                  REFLECT_SPECIALIZE_SERIALIZER( I64ArraySerializer );
  typedef SimpleArraySerializer<f32> F32ArraySerializer;                                  REFLECT_SPECIALIZE_SERIALIZER( F32ArraySerializer );
  typedef SimpleArraySerializer<f64> F64ArraySerializer;                                  REFLECT_SPECIALIZE_SERIALIZER( F64ArraySerializer );
  typedef SimpleArraySerializer<UniqueID::GUID> GUIDArraySerializer;                      REFLECT_SPECIALIZE_SERIALIZER( GUIDArraySerializer );
  typedef SimpleArraySerializer<UniqueID::TUID> TUIDArraySerializer;                      REFLECT_SPECIALIZE_SERIALIZER( TUIDArraySerializer );

  typedef SimpleArraySerializer<::Math::Vector2> Vector2ArraySerializer;                  REFLECT_SPECIALIZE_SERIALIZER( Vector2ArraySerializer );
  typedef SimpleArraySerializer<::Math::Vector3> Vector3ArraySerializer;                  REFLECT_SPECIALIZE_SERIALIZER( Vector3ArraySerializer );
  typedef SimpleArraySerializer<::Math::Vector4> Vector4ArraySerializer;                  REFLECT_SPECIALIZE_SERIALIZER( Vector4ArraySerializer );
  typedef SimpleArraySerializer<::Math::Matrix3> Matrix3ArraySerializer;                  REFLECT_SPECIALIZE_SERIALIZER( Matrix3ArraySerializer );
  typedef SimpleArraySerializer<::Math::Matrix4> Matrix4ArraySerializer;                  REFLECT_SPECIALIZE_SERIALIZER( Matrix4ArraySerializer );
  typedef SimpleArraySerializer<::Math::Quaternion> QuaternionArraySerializer;            REFLECT_SPECIALIZE_SERIALIZER( QuaternionArraySerializer );

  typedef SimpleArraySerializer<::Math::Color3> Color3ArraySerializer;                    REFLECT_SPECIALIZE_SERIALIZER( Color3ArraySerializer );
  typedef SimpleArraySerializer<::Math::Color4> Color4ArraySerializer;                    REFLECT_SPECIALIZE_SERIALIZER( Color4ArraySerializer );
  typedef SimpleArraySerializer<::Math::HDRColor3> HDRColor3ArraySerializer;              REFLECT_SPECIALIZE_SERIALIZER( HDRColor3ArraySerializer );
  typedef SimpleArraySerializer<::Math::HDRColor4> HDRColor4ArraySerializer;              REFLECT_SPECIALIZE_SERIALIZER( HDRColor4ArraySerializer );
}