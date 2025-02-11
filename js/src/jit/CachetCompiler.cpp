/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: set ts=8 sts=2 et sw=2 tw=80:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "jit/CachetCompiler.h"

#include "mozilla/FloatingPoint.h"

#include "jit/CacheIRCompiler.h"
#include "vm/JSObject.h"

using namespace mozilla;
using namespace js::jit::cachet;

namespace js {

namespace jit {

namespace cachet {

namespace Impl_Float64 {

Type_Bool::Val Fn_isNegativeZero(Cachet_ContextRef cx, Type_Float64::Ref param_float64) {
  return IsNegativeZero(param_float64);
}

Type_Bool::Val Fn_isInfinite(Cachet_ContextRef cx, Type_Float64::Ref param_float64) {
  return IsInfinite(param_float64);
}

Type_Bool::Val Fn_isNaN(Cachet_ContextRef cx, Type_Float64::Ref param_float64) {
  return IsNaN(param_float64);
}

Type_Bool::Val Fn_equalsInt32Unchecked(Cachet_ContextRef cx, Type_Float64::Ref param_float64) {
  int32_t unused;
  return NumberEqualsInt32(param_float64, &unused);
}

Type_Float64::Val Fn_fromInt32Unchecked(Cachet_ContextRef cx, Type_Int32::Ref param_int32) {
  return (double)param_int32;
}

Type_Int32::Val Fn_toInt32Unchecked(Cachet_ContextRef cx, Type_Float64::Ref param_float64) {
  return (int32_t)param_float64;
}

Type_Float64::Val Fn_fromUInt32Unchecked(Cachet_ContextRef cx, Type_UInt32::Ref param_uint32) {
  return (double)param_uint32;
}

};  // namespace Impl_Float64

namespace Impl_JSValueType {

Type_JSValueType::Val Fn_fromValueType(Cachet_ContextRef cx, Type_ValueType::Ref param_valTy) {
  return JSValueType(param_valTy);
}

};  // namespace Impl_JSValueType

namespace Impl_Value {

Type_ValueType::Val Fn_typeOf(Cachet_ContextRef cx,
                                          Type_Value::Ref param_value) {
  return param_value.type();
}

Type_Value::Val Fn_fromDoubleUnchecked(Cachet_ContextRef cx, Type_Double::Ref param_value) {
  return DoubleValue(param_value);
}

Type_Double::Val Fn_toDoubleUnchecked(Cachet_ContextRef cx, Type_Value::Ref param_value) {
  return param_value.toDouble();
}

Type_Value::Val Fn_fromInt32Unchecked(Cachet_ContextRef cx, Type_Int32::Ref param_value) {
  return Int32Value(param_value);
}

Type_Int32::Val Fn_toInt32Unchecked(Cachet_ContextRef cx, Type_Value::Ref param_value) {
  return param_value.toInt32();
}

Type_Value::Val Fn_fromBoolUnchecked(Cachet_ContextRef cx, Type_Bool::Ref param_value) {
  return BooleanValue(param_value);
}

Type_Bool::Val Fn_toBoolUnchecked(Cachet_ContextRef cx,
                                  Type_Value::Ref param_value) {
  return param_value.toBoolean();
}

Type_Value::Val Fn_getUndefinedUnchecked(Cachet_ContextRef cx) {
  return JS::UndefinedValue();
}

Type_Value::Val Fn_fromObjectUnchecked(Cachet_ContextRef cx,
                                       Type_Object::Ref param_value) {
  return ObjectValue(*param_value);
}

Type_Object::Val Fn_toObjectUnchecked(Cachet_ContextRef cx,
                                      Type_Value::Ref param_value) {
  return &param_value.toObject();
}

Type_Value::Val Fn_fromStringUnchecked(Cachet_ContextRef cx,
                                       Type_String::Ref param_value) {
  return StringValue(param_value);
}

Type_String::Val Fn_toStringUnchecked(Cachet_ContextRef cx,
                                      Type_Value::Ref param_value) {
  return param_value.toString();
}

Type_Value::Val Fn_fromSymbolUnchecked(Cachet_ContextRef cx,
                                       Type_Symbol::Ref param_value) {
  return SymbolValue(param_value);
}

Type_Symbol::Val Fn_toSymbolUnchecked(Cachet_ContextRef cx,
                                      Type_Value::Ref param_value) {
  return param_value.toSymbol();
}

Type_Value::Val Fn_fromBigIntUnchecked(Cachet_ContextRef cx,
                                       Type_BigInt::Ref param_value) {
  return BigIntValue(param_value);
}

Type_BigInt::Val Fn_toBigIntUnchecked(Cachet_ContextRef cx,
                                      Type_Value::Ref param_value) {
  return param_value.toBigInt();
}

};  // namespace Impl_Value

namespace Impl_Object {

Type_Shape::Val Fn_shapeOfUnchecked(Cachet_ContextRef cx,
                                    Type_Heap::Ref param_heap,
                                    Type_Object::Ref param_object) {
  return param_object->shape();
}

Type_Bool::Val Fn_lookupPropertyPureUnchecked(
    Cachet_ContextRef cx, Type_Object::Ref param_object,
    Type_PropertyKey::Ref param_key,
    Type_MaybeNativeObject::MutRef param_holder,
    Type_PropertyResult::MutRef param_propInfo) {
  RootedNativeObject holder(cx.jsCx);
  const bool result = LookupPropertyPure(cx.jsCx, param_object, param_key,
                                         holder.address(), &param_propInfo);
  param_holder.set(holder ? Some(holder.get()) : Nothing());
  return result;
}

};  // namespace Impl_Object

namespace Impl_NativeObject {

Type_Value::Val Fn_getFixedSlotUnchecked(Cachet_ContextRef cx,
                                         Type_Heap::Ref param_heap,
                                         Type_NativeObject::Ref param_nativeObject,
                                         Type_UInt32::Ref param_slot) {
  return param_nativeObject->getFixedSlot(param_slot);
}

Type_NativeObjectSlots::Val Fn_getSlotsUnchecked(Cachet_ContextRef cx,
                                                 Type_Heap::Ref param_heap,
                                                 Type_NativeObject::Ref param_nativeObject) {
  return param_nativeObject->getSlotsHeader()->slots();
}

Type_NativeObjectElements::Val Fn_getElementsHeaderUnchecked(Cachet_ContextRef cx,
                                                             Type_Heap::Ref param_heap,
                                                             Type_NativeObject::Ref param_nativeObject) {
  return param_nativeObject->getElementsHeader()->elements();
}

Type_Bool::Val Fn_containsDenseElement(Cachet_ContextRef cx, Type_NativeObject::Ref param_nativeObject, Type_UInt32::Ref param_index) {
  return param_nativeObject->containsDenseElement(param_index);
}

};  // namespace Impl_NativeObject

namespace Impl_NativeObjectSlots {

// TODO(abhishekc-sharma): need to rethink how to implement this
Type_UInt32::Val Fn_length(Cachet_ContextRef cx, Type_NativeObjectSlots::Ref param_slots) {
  return 0;
}

Type_Value::Val Fn_getDynamicSlotUnchecked(Cachet_ContextRef cx,
                                           Type_Heap::Ref param_heap,
                                           Type_NativeObjectSlots::Ref param_slots,
                                           Type_UInt32::Ref param_slot) {
  return param_slots[param_slot];
}

};  // namespace Impl_NativeObjectSlots

namespace Impl_NativeObjectElements {

Type_UInt32::Val Fn_getLengthUnchecked(Cachet_ContextRef cx,
                                       Type_Heap::Ref param_heap,
                                       Type_NativeObjectElements::Ref param_elements) {
  return *(Type_UInt32::Val *)((char *)param_elements + Impl_NativeObjectElements::Var_offsetOfLength(cx));
}

Type_UInt32::Val Fn_getInitializedLengthUnchecked(Cachet_ContextRef cx,
                                                  Type_Heap::Ref param_heap,
                                                  Type_NativeObjectElements::Ref param_elements) {
  return *(Type_UInt32::Val *)((char *)param_elements + Impl_NativeObjectElements::Var_offsetOfInitializedLength(cx));
}

Type_Value::Val Fn_getElementUnchecked(Cachet_ContextRef cx,
                                       Type_Heap::Ref param_heap,
                                       Type_NativeObjectElements::Ref param_elements,
                                       Type_UInt64::Ref param_index) {
  return param_elements[param_index];
}

};  // namespace Impl_NativeObjectElements

namespace Impl_ArrayObject {

Type_UInt32::Val Fn_lengthUnchecked(Cachet_ContextRef cx, Type_Heap::Ref param_heap, Type_ArrayObject::Ref param_arrayObject) {
  return param_arrayObject->length();
}

};  // namespace Impl_ArrayObject

namespace Impl_ArgumentsObject {

Type_UInt32::Val Fn_getInitialLengthSlotUnchecked(Cachet_ContextRef cx, Type_Heap::Ref param_heap, Type_ArgumentsObject::Ref param_obj) {
  return uint32_t(param_obj->getFixedSlot(ArgumentsObject::INITIAL_LENGTH_SLOT).toInt32()); 
}

Type_ArgumentsData::Val Fn_getArgumentsDataUnchecked(Cachet_ContextRef cx, Type_Heap::Ref param_heap, Type_ArgumentsObject::Ref param_obj) {
  return reinterpret_cast<ArgumentsData*>(
      param_obj->getFixedSlot(ArgumentsObject::DATA_SLOT).toPrivate()
  );
}

Type_Bool::Val Fn_argIsForwarded(Cachet_ContextRef cx, Type_ArgumentsObject::Ref param_obj, Type_UInt32::Ref param_index) {
  return param_obj->argIsForwarded(param_index);
}

};  // namespace Impl_ArgumentsObject

namespace Impl_ArgumentsData {

Type_UInt32::Ref Field_numArgs(Cachet_ContextRef cx, Type_ArgumentsData::Ref in) {
  return in->numArgs;
}

Type_Value::Val Fn_getArgUnchecked(Cachet_ContextRef cx, Type_Heap::Ref param_heap, Type_ArgumentsData::Ref param_argsData, Type_UInt32::Ref param_index) {
  return param_argsData->args[param_index];
}

};  // namespace Impl_ArgumentsData

namespace Impl_MaybeNativeObject {

Type_Bool::Val Fn_isNativeObject(
    Cachet_ContextRef cx, Type_MaybeNativeObject::Ref param_maybeObject) {
  return param_maybeObject.isSome();
}

Type_MaybeNativeObject::Val Fn_noneUnchecked(Cachet_ContextRef cx) {
  return Nothing();
}

Type_MaybeNativeObject::Val Fn_fromNativeObjectUnchecked(
    Cachet_ContextRef cx, Type_NativeObject::Ref param_object) {
  return Some(param_object);
}

Type_NativeObject::Val Fn_toNativeObjectUnchecked(
    Cachet_ContextRef cx, Type_MaybeNativeObject::Ref param_maybeObject) {
  return param_maybeObject.value();
}

};  // namespace Impl_MaybeNativeObject

namespace Impl_MaybePropertyInfo {

Type_Bool::Val Fn_isPropertyInfo(Cachet_ContextRef cx, Type_MaybePropertyInfo::Ref param_maybeInfo) {
  return param_maybeInfo.isSome();
}

Type_MaybePropertyInfo::Val Fn_noneUnchecked(Cachet_ContextRef cx) {
  return Nothing();
}

Type_MaybePropertyInfo::Val Fn_fromPropertyInfoUnchecked(Cachet_ContextRef cx, Type_PropertyInfo::Ref param_info) {
  return Some(param_info);
}

Type_PropertyInfo::Val Fn_toPropertyInfoUnchecked(Cachet_ContextRef cx, Type_MaybePropertyInfo::Ref param_maybeInfo) {
  return param_maybeInfo.value();
}

};  // namespace Impl_MaybePropertyInfo

namespace Impl_MaybeReg {

Type_Bool::Val Fn_isReg(Cachet_ContextRef cx, Type_MaybeReg::Ref param_maybeReg) {
  return param_maybeReg.isSome();
}

Type_MaybeReg::Val Fn_noneUnchecked(Cachet_ContextRef cx) {
  return Nothing();
}

Type_MaybeReg::Val Fn_fromRegUnchecked(Cachet_ContextRef cx, Type_Reg::Ref param_reg) {
  return Some(param_reg);
}

Type_Reg::Val Fn_toRegUnchecked(Cachet_ContextRef cx, Type_MaybeReg::Ref param_maybeReg) {
  return param_maybeReg.value();
}

};  // namespace Impl_MaybeReg

namespace Impl_PropertyFlags {

Type_Bool::Val Fn_isAccessorProperty(Cachet_ContextRef cx, Type_PropertyFlags::Ref param_flags) {
  return param_flags.isAccessorProperty();
}

Type_Bool::Val Fn_isCustomDataProperty(Cachet_ContextRef cx, Type_PropertyFlags::Ref param_flags) {
  return param_flags.isCustomDataProperty();
}

};  // namespace Impl_PropertyFlags

namespace Impl_PropertyInfo {

Type_PropertyFlags::Val Fn_flags(Cachet_ContextRef cx, Type_PropertyInfo::Ref param_info) {
  return param_info.flags();
}

Type_UInt32::Val Fn_maybeSlotUnchecked(Cachet_ContextRef cx, Type_PropertyInfo::Ref param_info) {
  return param_info.maybeSlot();
}

};  // namespace Impl_PropertyInfo

namespace Impl_PropertyKey {

Type_Bool::Val Fn_isString(Cachet_ContextRef cx, Type_PropertyKey::Ref param_key) {
  return param_key.isString();
}

Type_Bool::Val Fn_isSymbol(Cachet_ContextRef cx, Type_PropertyKey::Ref param_key) {
  return param_key.isSymbol();
}

Type_Bool::Val Fn_isAtom(Cachet_ContextRef cx, Type_PropertyKey::Ref param_key, Type_Atom::Ref param_atom) {
  return param_key.isAtom(param_atom);
}

Type_Symbol::Val Fn_toSymbolUnchecked(Cachet_ContextRef cx, Type_PropertyKey::Ref param_key) {
  return param_key.toSymbol();
}

Type_Atom::Val Fn_toAtomUnchecked(Cachet_ContextRef cx, Type_PropertyKey::Ref param_key) {
  return param_key.toAtom();
}

Type_Bool::Val Fn_nameOrSymbolFromValueUnchecked(
    Cachet_ContextRef cx, Type_Value::Ref param_value, Type_PropertyKey::MutRef
    param_key, Type_Bool::MutRef param_nameOrSymbol) {
  return ValueToNameOrSymbolId(cx.jsCx, param_value, param_key,
                               &param_nameOrSymbol);
}

};  // namespace Impl_PropertyKey

namespace Impl_PropertyResult {

Type_PropertyResultKind::Ref Field_kind(Cachet_ContextRef cx,
                                        Type_PropertyResult::Ref parent) {
  return parent.kind();
}

Type_PropertyInfo::Val Fn_propertyInfoUnchecked(
    Cachet_ContextRef cx, Type_PropertyResult::Ref param_result) {
  return param_result.propertyInfo();
}

};  // namespace Impl_PropertyResult


namespace Impl_BaseShape {

Type_Class::Val Fn_classOf(Cachet_ContextRef cx, Type_BaseShape::Ref param_baseShape) {
  return param_baseShape->clasp();
}

Type_TaggedProto::Val Fn_protoOfUnchecked(Cachet_ContextRef cx, Type_Heap::Ref param_heap, Type_BaseShape::Ref param_baseShape) {
  return param_baseShape->proto();
}

};  // namespace Impl_BaseShape

namespace Impl_Shape {

Type_BaseShape::Val Fn_baseShapeOf(Cachet_ContextRef cx, Type_Shape::Ref param_shape) {
  return param_shape->base();
}

Type_UInt32::Val Fn_numFixedSlotsUnchecked(Cachet_ContextRef cx, Type_Shape::Ref param_shape) {
  return param_shape->numFixedSlots();
}

Type_UInt32::Val Fn_slotSpanUnchecked(Cachet_ContextRef cx, Type_Shape::Ref param_shape) {
  return param_shape->slotSpan();
}

}; // namespace Impl_Shape

namespace Impl_Class {

Type_Class::Val Fn_windowProxyClass(Cachet_ContextRef cx) {
  return cx.jsCx->runtime()->maybeWindowProxyClass();
}

Type_Bool::Val Fn_emulatesUndefined(Cachet_ContextRef cx, Type_Class::Ref param_class) {
  return param_class->emulatesUndefined();
}

Type_Bool::Val Fn_isNativeObject(Cachet_ContextRef cx, Type_Class::Ref param_class) {
  return param_class->isNativeObject();
}

Type_Bool::Val Fn_isProxyObject(Cachet_ContextRef cx, Type_Class::Ref param_class) {
  return param_class->isProxyObject();
}

};  // namespace Impl_Class

namespace Impl_String {

Type_Bool::Val Fn_isAtom(Cachet_ContextRef cx, Type_String::Ref param_string) {
  return param_string->isAtom();
}

//FIXME(abhishekc-sharma): returns a pointer to a local variable
Type_Atom::Val Fn_asAtom(Cachet_ContextRef cx, Type_String::Ref param_string) {
  return &param_string->asAtom();
}

Type_UInt64::Val Fn_length(Cachet_ContextRef cx, Type_String::Ref param_string) {
  return param_string->length();
}

};  // namespace Impl_String

namespace Impl_TaggedProto {

Type_Bool::Val Fn_isNull(Cachet_ContextRef cx, Type_TaggedProto::Ref param_proto) {
  return !param_proto.raw();
}

Type_TaggedProto::Val Fn_nullUnchecked(Cachet_ContextRef cx) {
  return TaggedProto();
}

Type_Bool::Val Fn_isLazy(Cachet_ContextRef cx, Type_TaggedProto::Ref param_proto) {
  return param_proto.isDynamic();
}

Type_TaggedProto::Val Fn_lazyUnchecked(Cachet_ContextRef cx) {
  return TaggedProto(TaggedProto::LazyProto);
}

Type_Bool::Val Fn_isObject(Cachet_ContextRef cx, Type_TaggedProto::Ref param_proto) {
  return param_proto.isObject();
}

Type_TaggedProto::Val Fn_fromObjectUnchecked(Cachet_ContextRef cx, Type_Object::Ref param_object) {
  return TaggedProto(param_object);
}

Type_Object::Val Fn_toObjectUnchecked(Cachet_ContextRef cx, Type_TaggedProto::Ref param_proto) {
  return param_proto.toObject();
}

};  // namespace Impl_TaggedProto

namespace Impl_ValueReg {

Type_Reg::Val Fn_scratchReg(Cachet_ContextRef cx, Type_ValueReg::Ref param_valueReg) {
  return param_valueReg.scratchReg();
}

};  // namespace Impl_ValueReg

namespace Impl_FloatReg {

Type_PhyFloatReg::Ref Field_reg(Cachet_ContextRef cx, Type_FloatReg::Ref in) {
  return in.encoding();
}

Type_FloatContentType::Ref Field_type(Cachet_ContextRef cx, Type_FloatReg::Ref in) {
  if(in.isSingle()) {
    return Impl_FloatContentType::Variant_Single(cx);
  } else if(in.isDouble()) {
    return Impl_FloatContentType::Variant_Double(cx);
  } else {
    MOZ_ASSERT(in.isSimd128());
    return Impl_FloatContentType::Variant_Simd128(cx);
  }
}

Type_FloatReg::Val Fn_newUnchecked(Cachet_ContextRef cx, Type_PhyFloatReg::Ref param_reg, Type_FloatContentType::Ref param_type) {
  return FloatRegister(param_reg, param_type);
}

};  // namespace Impl_FloatReg

namespace Impl_AnyReg {

Type_Bool::Val Fn_isFloat(Cachet_ContextRef cx, Type_AnyReg::Ref param_anyReg) {
  return param_anyReg.isFloat();
}

Type_AnyReg::Val Fn_fromRegUnchecked(Cachet_ContextRef cx, Type_Reg::Ref param_reg) {
  return AnyRegister(param_reg);
}

Type_Reg::Val Fn_toRegUnchecked(Cachet_ContextRef cx, Type_AnyReg::Ref param_anyReg) {
  return param_anyReg.gpr();
}

Type_AnyReg::Val Fn_fromFloatRegUnchecked(Cachet_ContextRef cx, Type_FloatReg::Ref param_floatReg) {
  return AnyRegister(param_floatReg);
}

Type_FloatReg::Val Fn_toFloatRegUnchecked(Cachet_ContextRef cx, Type_AnyReg::Ref param_anyReg) {
  return param_anyReg.fpu();
}

};  // namespace Impl_AnyReg

namespace Impl_TypedOrValueReg {

Type_MIRType::Val Fn_type(Cachet_ContextRef cx, Type_TypedOrValueReg::Ref param_reg) {
  return param_reg.type();
}

Type_TypedOrValueReg::Val Fn_fromValueRegUnchecked(Cachet_ContextRef cx, Type_ValueReg::Ref param_valueReg) {
  return TypedOrValueRegister(param_valueReg);
}

Type_ValueReg::Val Fn_toValueRegUnchecked(Cachet_ContextRef cx, Type_TypedOrValueReg::Ref param_reg) {
  return param_reg.valueReg();
}

Type_TypedOrValueReg::Val Fn_fromTypedRegUnchecked(Cachet_ContextRef cx, Type_MIRType::Ref param_type, Type_AnyReg::Ref param_reg) {
  return TypedOrValueRegister(param_type, param_reg);
}

Type_AnyReg::Val Fn_toTypedRegUnchecked(Cachet_ContextRef cx, Type_TypedOrValueReg::Ref param_reg) {
  return param_reg.typedReg();
}

};  // namespace Impl_TypedOrValueReg

namespace Impl_GeneralRegSet {

Type_GeneralRegSet::Val Fn_empty(Cachet_ContextRef cx) {
  return TypedRegisterSet<Register>();
}

Type_GeneralRegSet::Val Fn_volatile(Cachet_ContextRef cx) {
  return TypedRegisterSet<Register>::Volatile();
}

Type_GeneralRegSet::Val Fn_intersect(Cachet_ContextRef cx, Type_GeneralRegSet::Ref param_lhs, Type_GeneralRegSet::Ref param_rhs) {
  return TypedRegisterSet<Register>::Intersect(param_lhs, param_rhs);
}

Type_GeneralRegSet::Val Fn_difference(Cachet_ContextRef cx, Type_GeneralRegSet::Ref param_lhs, Type_GeneralRegSet::Ref param_rhs) {
  return TypedRegisterSet<Register>::Subtract(param_lhs, param_rhs);
}

Type_Bool::Val Fn_isEmpty(Cachet_ContextRef cx, Type_GeneralRegSet::Ref param_set) {
  return param_set.empty();
}

Type_Bool::Val Fn_contains(Cachet_ContextRef cx, Type_GeneralRegSet::Ref param_set, Type_Reg::Ref param_reg) {
  return param_set.hasRegisterIndex(param_reg);
}

void Fn_add(Cachet_ContextRef cx, Type_GeneralRegSet::MutRef param_newSet, Type_GeneralRegSet::Ref param_set, Type_Reg::Ref param_reg) {
  param_newSet.addRegisterIndex(param_reg);  
}

void Fn_take(Cachet_ContextRef cx, Type_GeneralRegSet::MutRef param_newSet, Type_GeneralRegSet::Ref param_set, Type_Reg::Ref param_reg) {
  param_newSet.takeRegisterIndex(param_reg);
}

};  // namespace Impl_GeneralRegSet

namespace Impl_FloatRegSet {

Type_FloatRegSet::Val Fn_empty(Cachet_ContextRef cx) {
  return TypedRegisterSet<FloatRegister>();
}

Type_FloatRegSet::Val Fn_volatile(Cachet_ContextRef cx) {
  return TypedRegisterSet<FloatRegister>::Volatile();
}

Type_FloatRegSet::Val Fn_intersect(Cachet_ContextRef cx, Type_FloatRegSet::Ref param_lhs, Type_FloatRegSet::Ref param_rhs) {
  return TypedRegisterSet<FloatRegister>::Intersect(param_lhs, param_rhs);
}

Type_FloatRegSet::Val Fn_difference(Cachet_ContextRef cx, Type_FloatRegSet::Ref param_lhs, Type_FloatRegSet::Ref param_rhs) {
  return TypedRegisterSet<FloatRegister>::Subtract(param_lhs, param_rhs);
}

Type_Bool::Val Fn_contains(Cachet_ContextRef cx, Type_FloatRegSet::Ref param_set, Type_FloatReg::Ref param_floatReg) {
  return param_set.hasRegisterIndex(param_floatReg);
}

void Fn_add(Cachet_ContextRef cx, Type_FloatRegSet::MutRef param_newSet, Type_FloatRegSet::Ref param_set, Type_FloatReg::Ref param_floatReg) {
  param_newSet.addRegisterIndex(param_floatReg);
}

void Fn_take(Cachet_ContextRef cx, Type_FloatRegSet::MutRef param_newSet, Type_FloatRegSet::Ref param_set, Type_FloatReg::Ref param_floatReg) {
  param_newSet.takeRegisterIndex(param_floatReg);
}

Type_Bool::Val Fn_isEmpty(Cachet_ContextRef cx, Type_FloatRegSet::Ref param_set) {
  return param_set.empty();
}

Type_FloatRegSet::Val Fn_reducedForPush(Cachet_ContextRef cx, Type_FloatRegSet::Ref param_set) {
  return param_set.reduceSetForPush();
}

};  // namespace Impl_FloatRegSet

namespace Impl_LiveRegSet {

Type_GeneralRegSet::Val Fn_gprs(Cachet_ContextRef cx, Type_LiveRegSet::Ref param_set) {
  return param_set.gprs();
}

Type_FloatRegSet::Val Fn_fpus(Cachet_ContextRef cx, Type_LiveRegSet::Ref param_set) {
  return param_set.fpus();
}

Type_LiveRegSet::Val Fn_newUnchecked(Cachet_ContextRef cx, Type_GeneralRegSet::Ref param_gprs, Type_FloatRegSet::Ref param_fpus) {
  return LiveRegisterSet(param_gprs, param_fpus);
}

};  // namespace Impl_LiveRegSet

namespace Impl_Address {

Type_Reg::Ref Field_base(Cachet_ContextRef cx, Type_Address::Ref in) {
  return in.base;
}

Type_Int32::Ref Field_offset(Cachet_ContextRef cx, Type_Address::Ref in) {
  return in.offset;
}

Type_Address::Val Fn_newUnchecked(Cachet_ContextRef cx, Type_Reg::Ref param_base, Type_Int32::Ref param_offset) {
  return Address(param_base, param_offset);
}

};

namespace Impl_BaseIndex {

Type_Reg::Ref Field_base(Cachet_ContextRef cx, Type_BaseIndex::Ref in) {
  return in.base;
}

Type_Reg::Ref Field_index(Cachet_ContextRef cx, Type_BaseIndex::Ref in) {
  return in.index;
}

Type_Scale::Ref Field_scale(Cachet_ContextRef cx, Type_BaseIndex::Ref in) {
  return in.scale;
}

Type_UInt32::Ref Field_offset(Cachet_ContextRef cx, Type_BaseIndex::Ref in) {
  return in.offset;
}

Type_BaseIndex::Val Fn_newUnchecked(Cachet_ContextRef cx, Type_Reg::Ref param_base, Type_Reg::Ref param_index, Type_Scale::Ref param_scale, Type_UInt32::Ref param_offset) {
  return BaseIndex(param_base, param_index, param_scale, param_offset);
}

};  // namespace Impl_BaseIndex

namespace Impl_BaseValueIndex {

Type_BaseValueIndex::Val Fn_newUnchecked(Cachet_ContextRef cx, Type_Reg::Ref param_base, Type_Reg::Ref param_index, Type_UInt32::Ref param_offset) {
  return BaseValueIndex(param_base, param_index, param_offset);
}

};  // namespace Impl_BaseValueIndex

namespace Impl_BaseObjectElementIndex {

Type_BaseObjectElementIndex::Val Fn_newUnchecked(Cachet_ContextRef cx, Type_Reg::Ref param_base, Type_Reg::Ref param_index, Type_UInt32::Ref param_offset) {
  return BaseObjectElementIndex(param_base, param_index, param_offset);
}

};  // namespace Impl_BaseObjectElementIndex

namespace Impl_BaseObjectSlotIndex {

Type_BaseObjectSlotIndex::Val Fn_newUnchecked(Cachet_ContextRef cx, Type_Reg::Ref param_base, Type_Reg::Ref param_index) {
  return BaseObjectSlotIndex(param_base, param_index);
}

};  // namespace Impl_BaseObjectSlotIndex

namespace Impl_OperandLocation {

Type_OperandLocationKind::Val Fn_kind(Cachet_ContextRef cx, Type_OperandLocation::Ref param_loc) {
  return Type_OperandLocationKind::Val(param_loc.kind());
}

Type_OperandLocation::Val Fn_newUninitializedUnchecked(Cachet_ContextRef cx) {
  return OperandLocation();
}

void Fn_setValueRegUnchecked(Cachet_ContextRef cx, Type_OperandLocation::MutRef param_loc, Type_ValueReg::Ref param_valueReg) {
  param_loc.setValueReg(param_valueReg);
}

Type_ValueReg::Val Fn_getValueRegUnchecked(Cachet_ContextRef cx, Type_OperandLocation::Ref param_loc) {
  return param_loc.valueReg();
}

void Fn_setPayloadRegUnchecked(Cachet_ContextRef cx, Type_OperandLocation::MutRef param_loc, Type_Reg::Ref param_reg, Type_JSValueType::Ref param_type) {
  param_loc.setPayloadReg(param_reg, param_type);
}

Type_Reg::Val Fn_getPayloadRegUnchecked(Cachet_ContextRef cx, Type_OperandLocation::Ref param_loc) {
  return param_loc.payloadReg();
}

Type_JSValueType::Val Fn_getPayloadTypeUnchecked(Cachet_ContextRef cx, Type_OperandLocation::Ref param_loc) {
  return param_loc.payloadType();
}

void Fn_setFloatRegUnchecked(Cachet_ContextRef cx, Type_OperandLocation::MutRef param_loc, Type_FloatReg::Ref param_floatReg) {
  param_loc.setDoubleReg(param_floatReg);
}

Type_FloatReg::Val Fn_getFloatRegUnchecked(Cachet_ContextRef cx, Type_OperandLocation::Ref param_loc) {
  return param_loc.doubleReg();
}

}

namespace Impl_OperandId {

Type_UInt16::Val Fn_id(Cachet_ContextRef cx, Type_OperandId::Ref param_operandId) {
  return param_operandId.id();
}

Type_OperandId::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return OperandId(param_id);
}

};  // namespace Impl_OperandId

namespace Impl_ValueId {

Type_ValueId::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return ValOperandId(param_id);
}

};  // namespace Impl_ValueId

namespace Impl_ObjectId {

Type_ObjectId::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return ObjOperandId(param_id);
}

};  // namespace Impl_ObjectId

namespace Impl_StringId {

Type_StringId::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return StringOperandId(param_id);
}

};  // namespace Impl_StringId

namespace Impl_SymbolId {

Type_SymbolId::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return SymbolOperandId(param_id);
}

};  // namespace Impl_SymbolId

namespace Impl_BoolId {

Type_BoolId::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return BooleanOperandId(param_id);
}

};  // namespace Impl_BoolId

namespace Impl_Int32Id {

Type_Int32Id::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return Int32OperandId(param_id);
}

};  // namespace Impl_Int32Id

namespace Impl_NumberId {

Type_NumberId::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return NumberOperandId(param_id);
}

};  // namespace Impl_NumberId

namespace Impl_BigIntId {

Type_BigIntId::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return BigIntOperandId(param_id);
}

};  // namespace Impl_BigIntId

namespace Impl_ValueTagId {

Type_ValueTagId::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return ValueTagOperandId(param_id);
}

};  // namespace Impl_ValueTagId

namespace Impl_IntPtrId {

Type_IntPtrId::Val Fn_fromIdUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return IntPtrOperandId(param_id);
}

};  // namespace Impl_IntPtrId

namespace Impl_TypedId {

Type_JSValueType::Val Fn_type(Cachet_ContextRef cx, Type_TypedId::Ref param_typedId) {
  return param_typedId.type();
}

Type_TypedId::Val Fn_newUnchecked(Cachet_ContextRef cx, Type_UInt16::Ref param_id, Type_JSValueType::Ref param_type) {
  return TypedOperandId(ValOperandId(param_id), param_type);
}

};  // namespace Impl_TypedId

namespace Impl_FailurePath {

IR_MASM::LabelRef Field_label_(Cachet_ContextRef cx, Type_FailurePath::Ref in) {
  return in->label();
}

void Fn_setHasAutoScratchFloatRegister(Cachet_ContextRef cx, Type_FailurePath::Ref param_failure) {
  param_failure->setHasAutoScratchFloatRegister();
}

void Fn_clearHasAutoScratchFloatRegister(Cachet_ContextRef cx, Type_FailurePath::Ref param_failure) {
  param_failure->clearHasAutoScratchFloatRegister();
}

};  // namespace Impl_FailurePath

namespace Impl_MASM {

void EmitOp_AssumeUnreachable(Cachet_ContextRef cx, IR_MASM::OpsRef ops) {
  ops.assumeUnreachable("Cachet MASM::AssumeUnreachable");
}

void EmitOp_Assert(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_Bool::Ref param_cond) {
  if (!param_cond) {
    ops.assumeUnreachable("Cachet MASM::Assert(false)");
  }
}

void EmitOp_AssertEqValue(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                          Type_ValueReg::Ref param_valueReg,
                          Type_Value::Ref param_value,
                          Type_ValueReg::Ref param_scratchValueReg) {
#ifdef DEBUG
  Label ok;
  ops.moveValue(param_value, param_scratchValueReg);
  ops.branch64(Assembler::Equal, param_valueReg.toRegister64(),
               param_scratchValueReg.toRegister64(), &ok);
  ops.assumeUnreachable("Cachet MASM::AssertEqValue not equal");
  ops.bind(&ok);
#endif
}

void EmitOp_Move(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                 Type_Reg::Ref param_srcReg, Type_Reg::Ref param_dstReg) {
  ops.mov(param_srcReg, param_dstReg);
}

void EmitOp_Move32Bool(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                       Type_Reg::Ref param_srcReg, Type_Reg::Ref param_dstReg) {
  ops.move32(param_srcReg, param_dstReg);
}

void EmitOp_Move32Int32(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                        Type_Reg::Ref param_srcReg, Type_Reg::Ref param_dstReg)
{
  ops.move32(param_srcReg, param_dstReg);
}

void EmitOp_Move32Int32Imm(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                           Type_Int32::Ref param_srcInt32,
                           Type_Reg::Ref param_dstReg) {
  ops.move32(Imm32(param_srcInt32), param_dstReg);
}

void EmitOp_MoveValue(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                      Type_ValueReg::Ref param_srcValueReg,
                      Type_ValueReg::Ref param_dstValueReg) {
  ops.moveValue(param_srcValueReg, param_dstValueReg);
}

void EmitOp_MovePtrBoolImmWord(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                Type_Bool::Ref param_b, Type_Reg::Ref param_dstReg) {
  ops.movePtr(ImmWord(param_b), param_dstReg);
}

void EmitOp_MoveValueImm(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                          Type_Value::Ref param_value, Type_ValueReg::Ref param_dstReg) {
  ops.moveValue(param_value, param_dstReg);
}

void EmitOp_Cmp32Move32(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                    Type_Condition::Ref param_condition,
                    Type_Reg::Ref param_lhsReg,
                    Type_Reg::Ref param_rhsReg,
                    Type_Reg::Ref param_srcReg,
                    Type_Reg::Ref param_dstReg) {

  ops.cmp32Move32(param_condition, param_lhsReg, param_rhsReg, param_srcReg, param_dstReg);
}

void EmitOp_Jump(Cachet_ContextRef cx, IR_MASM::OpsRef ops, IR_MASM::LabelRef param_target) {
  ops.jump(param_target);
}

void EmitOp_Load32Address(Cachet_ContextRef cx,
                          IR_MASM::OpsRef ops,
                          Type_Address::Ref param_address,
                          Type_Reg::Ref param_dstReg) {
  ops.load32(param_address, param_dstReg);
}

void EmitOp_LoadTypedOrValueAddress(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                    Type_Address::Ref param_address,
                                    Type_TypedOrValueReg::Ref param_dstReg) {
  ops.loadTypedOrValue(param_address, param_dstReg);
}

void EmitOp_LoadTypedOrValueBaseObjectElementIndex(Cachet_ContextRef cx,
                                                   IR_MASM::OpsRef ops,
                                                   Type_BaseObjectElementIndex::Ref param_index,
                                                   Type_TypedOrValueReg::Ref param_dstReg) {
  ops.loadTypedOrValue(param_index, param_dstReg);
}

void EmitOp_LoadValueBaseObjectElementIndex(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                            Type_BaseObjectElementIndex::Ref param_index,
                                            Type_ValueReg::Ref param_dstReg) {
  ops.loadValue(param_index, param_dstReg);
}

void EmitOp_LoadPtrAddress(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                           Type_Address::Ref param_address,
                           Type_Reg::Ref param_dstReg) {
  ops.loadPtr(param_address, param_dstReg);
}

void EmitOp_PushFloatReg(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                         Type_FloatReg::Ref param_floatReg) {
  ops.push(param_floatReg);
}

void EmitOp_PopFloatReg(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                        Type_FloatReg::Ref param_floatReg) {
  ops.pop(param_floatReg);
}

void EmitOp_GuardSpecificAtom(Cachet_ContextRef cx,
                              IR_MASM::OpsRef ops,
                              Type_Reg::Ref param_strReg,
                              Type_Atom::Ref param_atom,
                              Type_Reg::Ref param_scratchReg,
                              Type_LiveRegSet::Ref param_volatileRegs,
                              IR_MASM::LabelRef param_fail) {
  ops.guardSpecificAtom(param_strReg, param_atom, param_scratchReg, param_volatileRegs, param_fail);
}

void EmitOp_TagValue(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_JSValueType::Ref param_valTy,
                      Type_Reg::Ref param_payload, Type_ValueReg::Ref param_dest) {
  ops.tagValue(param_valTy, param_payload, param_dest);
}

void EmitOp_BoxDouble(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                      Type_FloatReg::Ref param_srcReg,
                      Type_ValueReg::Ref param_valueReg,
                      Type_FloatReg::Ref param_scratchReg) {
  ops.boxDouble(param_srcReg, param_valueReg, param_scratchReg);
}

void EmitOp_UnboxInt32(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                          Type_ValueReg::Ref param_valueReg,
                          Type_Reg::Ref param_int32Reg) {
  ops.unboxInt32(param_valueReg, param_int32Reg);
}

void EmitOp_UnboxObject(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                               Type_ValueReg::Ref param_valueReg,
                               Type_Reg::Ref param_objectReg) {
  ops.unboxObject(param_valueReg, param_objectReg);
}

void EmitOp_FallibleUnboxObject(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                Type_ValueReg::Ref param_valueReg,
                                Type_Reg::Ref param_objectReg,
                                IR_MASM::LabelRef param_failure) {
  ops.fallibleUnboxObject(param_valueReg, param_objectReg, param_failure);
}

void EmitOp_FallibleUnboxBoolean(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                        Type_ValueReg::Ref param_valueReg,
                                        Type_Reg::Ref param_boolReg,
                                        IR_MASM::LabelRef param_failure) {
  ops.fallibleUnboxBoolean(param_valueReg, param_boolReg, param_failure);
}

void EmitOp_UnboxDouble(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                        Type_ValueReg::Ref param_valueReg,
                        Type_FloatReg::Ref param_floatReg) {
  ops.unboxDouble(param_valueReg, param_floatReg);
}

void EmitOp_CastBoolToInt32(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_Reg::Ref param_int32Reg) {
  // Phantom op
}

void EmitOp_BranchSymbolImmGCPtr(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_Condition::Ref param_condition, Type_Reg::Ref param_lhsReg, Type_Symbol::Ref param_rhs, IR_MASM::LabelRef param_branch) {
  ops.branchPtr(param_condition, param_lhsReg, ImmGCPtr(param_rhs), param_branch);
}

void EmitOp_ConvertInt32ValueToDouble(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                        Type_ValueReg::Ref param_valueReg) {
  ops.convertInt32ValueToDouble(param_valueReg);
}

void EmitOp_ConvertDoubleToInt32(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                 Type_FloatReg::Ref param_srcReg,
                                 Type_Reg::Ref param_destReg,
                                 IR_MASM::LabelRef param_failure,
                                 Type_Bool::Ref param_negativeZeroCheck) {
  ops.convertDoubleToInt32(param_srcReg, param_destReg, param_failure,
                           param_negativeZeroCheck);
}

void EmitOp_ConvertUInt32ToDouble(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                  Type_Reg::Ref param_srcReg,
                                  Type_FloatReg::Ref param_destReg) {
  ops.convertUInt32ToDouble(param_srcReg, param_destReg);
}

void EmitOp_SpectreBoundsCheck32Address(Cachet_ContextRef cx,
                                        IR_MASM::OpsRef ops,
                                        Type_Reg::Ref param_indexReg,
                                        Type_Address::Ref param_length,
                                        Type_Reg::Ref param_maybeScratch,
                                        IR_MASM::LabelRef param_failure) {
  ops.spectreBoundsCheck32(param_indexReg, param_length, param_maybeScratch, param_failure);
}

void EmitOp_BranchTestNumber(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                    Type_Condition::Ref param_condition,
                                    Type_ValueReg::Ref param_valueReg,
                                    IR_MASM::LabelRef param_branch) {
  ops.branchTestNumber(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestNumberTag(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                            Type_Condition::Ref param_condition,
                            Type_Reg::Ref param_tagReg,
                            IR_MASM::LabelRef param_branch) {
  ops.branchTestNumber(param_condition, param_tagReg, param_branch);
}

void EmitOp_BranchTestDouble(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                             Type_Condition::Ref param_condition,
                             Type_ValueReg::Ref param_valueReg,
                             IR_MASM::LabelRef param_branch) {
  ops.branchTestDouble(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestDoubleTag(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                Type_Condition::Ref param_condition,
                                Type_Reg::Ref param_tagReg,
                                IR_MASM::LabelRef param_branch) {
  ops.branchTestDouble(param_condition, param_tagReg, param_branch);
}

void EmitOp_BranchObject(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                         Type_Condition::Ref param_condition,
                         Type_Reg::Ref param_lhsReg,
                         Type_Reg::Ref param_rhsReg,
                         IR_MASM::LabelRef param_branch) {
  ops.branchPtr(param_condition, param_lhsReg, param_rhsReg, param_branch);
}

void EmitOp_BranchObjectProto(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                              Type_Condition::Ref param_condition,
                              Type_Reg::Ref param_protoReg,
                              Type_Reg::Ref param_objectReg,
                              IR_MASM::LabelRef param_branch) {
  ops.branchPtr(param_condition, param_protoReg, param_objectReg, param_branch);
}

void EmitOp_ConvertInt32ToDouble(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                 Type_Reg::Ref param_srcReg,
                                 Type_FloatReg::Ref param_destReg) {
  ops.convertInt32ToDouble(param_srcReg, param_destReg);
}

void EmitOp_BranchTestInt32(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                    Type_Condition::Ref param_condition,
                                    Type_ValueReg::Ref param_valueReg,
                                    IR_MASM::LabelRef param_branch) {
  ops.branchTestInt32(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestInt32Tag(Cachet_ContextRef cx,
                               IR_MASM::OpsRef ops,
                               Type_Condition::Ref param_condition,
                               Type_Reg::Ref param_tagReg,
                               IR_MASM::LabelRef param_branch) {
  ops.branchTestInt32(param_condition, param_tagReg, param_branch);
}

void EmitOp_BranchTestInt32Truthy(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                    Type_Bool::Ref param_truthy,
                                    Type_ValueReg::Ref param_valueReg,
                                    IR_MASM::LabelRef param_branch) {
  ops.branchTestInt32Truthy(param_truthy, param_valueReg, param_branch);
}

void EmitOp_BranchTestBoolean(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_Condition::Ref param_condition, Type_ValueReg::Ref param_valueReg, IR_MASM::LabelRef param_branch) {
  ops.branchTestBoolean(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestString(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_Condition::Ref param_condition, Type_ValueReg::Ref param_valueReg, IR_MASM::LabelRef param_branch) {
  ops.branchTestString(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestSymbol(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_Condition::Ref param_condition, Type_ValueReg::Ref param_valueReg, IR_MASM::LabelRef param_branch) {
  ops.branchTestSymbol(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestBigInt(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_Condition::Ref param_condition, Type_ValueReg::Ref param_valueReg, IR_MASM::LabelRef param_branch) {
  ops.branchTestBigInt(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestObject(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                    Type_Condition::Ref param_condition,
                                    Type_ValueReg::Ref param_valueReg,
                                    IR_MASM::LabelRef param_branch) {
  ops.branchTestObject(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestObjectTag(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                Type_Condition::Ref param_condition,
                                Type_Reg::Ref param_tagReg,
                                IR_MASM::LabelRef param_branch) {
  ops.branchTestObject(param_condition, param_tagReg, param_branch);
}

void EmitOp_BranchTestValue(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                    Type_Condition::Ref param_cond,
                                    Type_BaseIndex::Ref param_lhs,
                                    Type_ValueReg::Ref param_rhs,
                                    IR_MASM::LabelRef param_branch) {
  ops.branchTestValue(param_cond, param_lhs, param_rhs, param_branch);
}

void EmitOp_BranchTestNullProto(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                Type_Reg::Ref param_reg,
                                IR_MASM::LabelRef param_branch) {
  ops.branchTestPtr(Assembler::Zero, param_reg, param_reg, param_branch);
}

void EmitOp_BranchTestLazyProto(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                Type_Reg::Ref param_reg,
                                IR_MASM::LabelRef param_branch) {
  MOZ_ASSERT(uintptr_t(TaggedProto::LazyProto) == 1);
  ops.branchPtr(Assembler::Equal, param_reg, ImmWord(1), param_branch);
}

void EmitOp_BranchTestObjectProto(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                  Type_Reg::Ref param_reg,
                                  IR_MASM::LabelRef param_branch) {
  MOZ_ASSERT(uintptr_t(TaggedProto::LazyProto) == 1);
  ops.branchPtr(Assembler::Above, param_reg, ImmWord(1), param_branch);
}

void EmitOp_BranchTestNull(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_Condition::Ref param_condition, Type_ValueReg::Ref param_valueReg, IR_MASM::LabelRef param_branch) {
  ops.branchTestNull(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestNullTag(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_Condition::Ref param_condition, Type_Reg::Ref param_tagReg, IR_MASM::LabelRef param_branch) {
  ops.branchTestNull(param_condition, param_tagReg, param_branch);
}

void EmitOp_TestNullSet(Cachet_ContextRef cx, IR_MASM::OpsRef ops, Type_Condition::Ref param_condition, Type_ValueReg::Ref param_valueReg, Type_Reg::Ref param_destReg) {
  ops.testNullSet(param_condition, param_valueReg, param_destReg);
}

void EmitOp_BranchTestUndefined(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                Type_Condition::Ref param_condition,
                                Type_ValueReg::Ref param_valueReg,
                                IR_MASM::LabelRef param_branch) {
  ops.branchTestUndefined(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestUndefinedTag(Cachet_ContextRef cx,
                                   IR_MASM::OpsRef ops,
                                   Type_Condition::Ref param_condition,
                                   Type_Reg::Ref param_tagReg,
                                   IR_MASM::LabelRef param_branch) {
  ops.branchTestUndefined(param_condition, param_tagReg, param_branch);
}

void EmitOp_BranchIfObjectEmulatesUndefined(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                            Type_Reg::Ref param_objReg,
                                            Type_Reg::Ref param_scratchReg,
                                            IR_MASM::LabelRef param_slowCheck,
                                            IR_MASM::LabelRef param_branch) {
  ops.branchIfObjectEmulatesUndefined(param_objReg, param_scratchReg, param_slowCheck, param_branch);
}

void EmitOp_TestUndefinedSet(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                             Type_Condition::Ref param_condition,
                             Type_ValueReg::Ref param_valueReg,
                             Type_Reg::Ref param_destReg) {
  ops.testUndefinedSet(param_condition, param_valueReg, param_destReg);
}

void EmitOp_BranchTestMagic(Cachet_ContextRef cx,
                            IR_MASM::OpsRef ops,
                            Type_Condition::Ref param_condition,
                            Type_ValueReg::Ref param_valueReg,
                            IR_MASM::LabelRef param_branch) {
  ops.branchTestMagic(param_condition, param_valueReg, param_branch);
}

void EmitOp_BranchTestMagicBaseObjectElementIndex(Cachet_ContextRef cx,
                                                  IR_MASM::OpsRef ops,
                                                  Type_Condition::Ref param_condition,
                                                  Type_BaseObjectElementIndex::Ref param_index,
                                                  IR_MASM::LabelRef param_branch) {
  ops.branchTestMagic(param_condition, param_index, param_branch);
}

void EmitOp_BranchTestObjectShape(Cachet_ContextRef cx,
                                         IR_MASM::OpsRef ops,
                                         Type_Condition::Ref param_condition,
                                         Type_Reg::Ref param_objectReg,
                                         Type_Shape::Ref param_shape,
                                         Type_Reg::Ref param_scratchReg,
                                         Type_Reg::Ref param_spectreRegToZero,
                                         IR_MASM::LabelRef param_branch) {
  ops.branchTestObjShape(param_condition, param_objectReg, param_shape,
                         param_scratchReg, param_spectreRegToZero,
                         param_branch);
}

void EmitOp_BranchTestObjectShapeNoSpectreMitigations(
    Cachet_ContextRef cx, IR_MASM::OpsRef ops,
    Type_Condition::Ref param_condition, Type_Reg::Ref param_objectReg,
    Type_Shape::Ref param_shape, IR_MASM::LabelRef param_branch) {
  ops.branchTestObjShapeNoSpectreMitigations(param_condition, param_objectReg,
                                             param_shape, param_branch);
}

void EmitOp_BranchTestObjClass(Cachet_ContextRef cx,
                                    IR_MASM::OpsRef ops,
                                    Type_Condition::Ref param_condition,
                                    Type_Reg::Ref param_objectReg,
                                    Type_Class::Ref param_class,
                                    Type_Reg::Ref param_scratchReg,
                                    Type_Reg::Ref param_spectreRegToZero,
                                    IR_MASM::LabelRef param_branch) {
  ops.branchTestObjClass(param_condition, param_objectReg, param_class,
                         param_scratchReg, param_spectreRegToZero,
                         param_branch);
}

void EmitOp_BranchTestObjClassNoSpectreMitigations(Cachet_ContextRef cx,
                                    IR_MASM::OpsRef ops,
                                    Type_Condition::Ref param_condition,
                                    Type_Reg::Ref param_objectReg,
                                    Type_Class::Ref param_class,
                                    Type_Reg::Ref param_scratchReg,
                                    IR_MASM::LabelRef param_branch) {
  ops.branchTestObjClassNoSpectreMitigations(param_condition, param_objectReg,
                                             param_class, param_scratchReg,
                                             param_branch);
}

void EmitOp_BranchIfNonNativeObj(Cachet_ContextRef cx,
                                  IR_MASM::OpsRef ops,
                                  Type_Reg::Ref param_objectReg,
                                  Type_Reg::Ref param_scratchReg,
                                  IR_MASM::LabelRef param_branch) {
  ops.branchIfNonNativeObj(param_objectReg, param_scratchReg, param_branch);
}

void EmitOp_BranchTestObjectIsProxy(Cachet_ContextRef cx,
                                    IR_MASM::OpsRef ops,
                                    Type_Bool::Ref param_proxy,
                                    Type_Reg::Ref param_objectReg,
                                    Type_Reg::Ref param_scratchReg,
                                    IR_MASM::LabelRef param_branch) {
  ops.branchTestObjectIsProxy(param_proxy, param_objectReg, param_scratchReg, param_branch);
}

void EmitOp_Branch32(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                     Type_Condition::Ref param_condition,
                     Type_Reg::Ref param_lhsReg,
                     Type_Reg::Ref param_rhsReg,
                     IR_MASM::LabelRef param_branch) {
  ops.branch32(param_condition, param_lhsReg, param_rhsReg, param_branch);
}

void EmitOp_Branch32Tag(Cachet_ContextRef cx,
                        IR_MASM::OpsRef ops,
                        Type_Condition::Ref param_condition,
                        Type_Reg::Ref param_lhsReg,
                        Type_Reg::Ref param_rhsReg,
                        IR_MASM::LabelRef param_branch) {
  ops.branch32(param_condition, param_lhsReg, param_rhsReg, param_branch);
}

void EmitOp_Branch32Imm(Cachet_ContextRef cx,
                        IR_MASM::OpsRef ops,
                        Type_Condition::Ref param_condition,
                        Type_Reg::Ref param_lhsReg,
                        Type_Int32::Ref param_rhsInt32,
                        IR_MASM::LabelRef param_branch) {
  ops.branch32(param_condition, param_lhsReg, Imm32(param_rhsInt32), param_branch);
}

void EmitOp_Branch32AddressImm32(Cachet_ContextRef cx,
                                 IR_MASM::OpsRef ops,
                                 Type_Condition::Ref param_condition,
                                 Type_Address::Ref param_address,
                                 Type_Int32::Ref param_rhsInt32,
                                 IR_MASM::LabelRef param_branch) {
  ops.branch32(param_condition, param_address, Imm32(param_rhsInt32), param_branch);
}

void EmitOp_BranchTest32(Cachet_ContextRef cx,
                            IR_MASM::OpsRef ops,
                            Type_Condition::Ref param_condition,
                            Type_Reg::Ref param_lhsReg,
                            Type_Reg::Ref param_rhsReg,
                            IR_MASM::LabelRef param_branch) {
  ops.branchTest32(param_condition, param_lhsReg, param_rhsReg, param_branch);
}

void EmitOp_BranchTest32Imm(Cachet_ContextRef cx,
                            IR_MASM::OpsRef ops,
                            Type_Condition::Ref param_condition,
                            Type_Reg::Ref param_lhsReg,
                            Type_Int32::Ref param_rhsInt32,
                            IR_MASM::LabelRef param_branch) {
  ops.branchTest32(param_condition, param_lhsReg, Imm32(param_rhsInt32), param_branch);
}

void EmitOp_BranchAdd32(Cachet_ContextRef cx,
                                         IR_MASM::OpsRef ops,
                                         Type_Condition::Ref param_condition,
                                         Type_Reg::Ref param_srcReg,
                                         Type_Reg::Ref param_dstReg,
                                         IR_MASM::LabelRef param_branch) {
  ops.branchAdd32(param_condition, param_srcReg, param_dstReg,
                         param_branch);
}

void EmitOp_BranchAdd32Imm(Cachet_ContextRef cx,
                           IR_MASM::OpsRef ops,
                           Type_Condition::Ref param_condition,
                           Type_Int32::Ref param_lhsInt32,
                           Type_Reg::Ref param_dstReg,
                           IR_MASM::LabelRef param_branch) {
  ops.branchAdd32(param_condition, Imm32(param_lhsInt32), param_dstReg, param_branch);
}

void EmitOp_BranchSub32(Cachet_ContextRef cx,
                                         IR_MASM::OpsRef ops,
                                         Type_Condition::Ref param_condition,
                                         Type_Reg::Ref param_srcReg,
                                         Type_Reg::Ref param_dstReg,
                                         IR_MASM::LabelRef param_branch) {
  ops.branchSub32(param_condition, param_srcReg, param_dstReg,
                         param_branch);
}

void EmitOp_BranchSub32Imm(Cachet_ContextRef cx,
                           IR_MASM::OpsRef ops,
                           Type_Condition::Ref param_condition,
                           Type_Int32::Ref param_lhsInt32,
                           Type_Reg::Ref param_dstReg,
                           IR_MASM::LabelRef param_branch) {
  ops.branchSub32(param_condition, Imm32(param_lhsInt32), param_dstReg, param_branch);
}

void EmitOp_BranchMul32(Cachet_ContextRef cx,
                                         IR_MASM::OpsRef ops,
                                         Type_Condition::Ref param_condition,
                                         Type_Reg::Ref param_srcReg,
                                         Type_Reg::Ref param_dstReg,
                                         IR_MASM::LabelRef param_branch) {
  ops.branchMul32(param_condition, param_srcReg, param_dstReg,
                         param_branch);
}

void EmitOp_FlexibleDivMod32(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                             Type_Reg::Ref param_rhsReg,
                             Type_Reg::Ref param_lhsOutputReg,
                             Type_Reg::Ref param_remOutputReg,
                             Type_Bool::Ref param_isUnsigned,
                             Type_LiveRegSet::Ref param_volatileRegs) {
  ops.flexibleDivMod32(param_rhsReg, param_lhsOutputReg, param_remOutputReg, param_isUnsigned, param_volatileRegs);
}

void EmitOp_FlexibleRemainder32(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                Type_Reg::Ref param_rhsReg,
                                Type_Reg::Ref param_lhsOutputReg,
                                Type_Bool::Ref param_isUnsigned,
                                Type_LiveRegSet::Ref param_volatileLiveRegs) {
  ops.flexibleRemainder32(param_rhsReg, param_lhsOutputReg, param_isUnsigned, param_volatileLiveRegs);
}


void EmitOp_Neg32(Cachet_ContextRef cx,
                  IR_MASM::OpsRef ops,
                  Type_Reg::Ref param_valueReg) {
  ops.neg32(param_valueReg);
}

void EmitOp_Not32(Cachet_ContextRef cx,
                  IR_MASM::OpsRef ops,
                  Type_Reg::Ref param_valueReg) {
  ops.not32(param_valueReg);
}

void EmitOp_And32(Cachet_ContextRef cx,
                  IR_MASM::OpsRef ops,
                  Type_Reg::Ref param_srcReg,
                  Type_Reg::Ref param_dstReg) {
  ops.and32(param_srcReg, param_dstReg);
}

void EmitOp_Or32(Cachet_ContextRef cx,
                                         IR_MASM::OpsRef ops,
                                         Type_Reg::Ref param_srcReg,
                                         Type_Reg::Ref param_dstReg) {
  ops.or32(param_srcReg, param_dstReg);
}

void EmitOp_Xor32(Cachet_ContextRef cx,
                  IR_MASM::OpsRef ops,
                  Type_Reg::Ref param_srcReg,
                  Type_Reg::Ref param_dstReg) {
  ops.xor32(param_srcReg, param_dstReg);
}

void EmitOp_FlexibleLshift32(Cachet_ContextRef cx,
                             IR_MASM::OpsRef ops,
                             Type_Reg::Ref param_shiftReg,
                             Type_Reg::Ref param_srcDestReg) {
  ops.flexibleLshift32(param_shiftReg, param_srcDestReg);
}

void EmitOp_FlexibleRshift32Arithmetic(Cachet_ContextRef cx,
                                       IR_MASM::OpsRef ops,
                                       Type_Reg::Ref param_shiftReg,
                                       Type_Reg::Ref param_srcDestReg) {
  ops.flexibleRshift32Arithmetic(param_shiftReg, param_srcDestReg);
}

void EmitOp_FlexibleRshift32(Cachet_ContextRef cx,
                             IR_MASM::OpsRef ops,
                             Type_Reg::Ref param_shiftReg,
                             Type_Reg::Ref param_srcDestReg) {
  ops.flexibleRshift32(param_shiftReg, param_srcDestReg);
}

void EmitOp_TestObjectSet(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                          Type_Condition::Ref param_condition,
                          Type_ValueReg::Ref param_valueReg,
                          Type_Reg::Ref param_dstReg) {
  ops.testObjectSet(param_condition, param_valueReg, param_dstReg);
}

void EmitOp_LoadArgumentsObjectLength(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                      Type_Reg::Ref param_objectReg,
                                      Type_Reg::Ref param_outputReg,
                                      IR_MASM::LabelRef param_failure) {
  ops.loadArgumentsObjectLength(param_objectReg, param_outputReg, param_failure);
}

void EmitOp_LoadArgumentsObjectElement(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                       Type_Reg::Ref param_objReg,
                                       Type_Reg::Ref param_indexReg,
                                       Type_ValueReg::Ref param_outputReg,
                                       Type_Reg::Ref param_tempReg,
                                       IR_MASM::LabelRef param_failure) {
  ops.loadArgumentsObjectElement(param_objReg, param_indexReg, param_outputReg, param_tempReg, param_failure);
}

void EmitOp_LoadObjectProto(Cachet_ContextRef cx,
                            IR_MASM::OpsRef ops,
                            Type_Reg::Ref param_objectReg,
                            Type_Reg::Ref param_protoReg) {
  ops.loadObjProto(param_objectReg, param_protoReg);
}

void EmitOp_UnboxObjectProto(Cachet_ContextRef cx,
                             IR_MASM::OpsRef ops,
                             Type_Reg::Ref param_protoReg) {
  // Phantom op
}

void EmitOp_SplitTagForTest(Cachet_ContextRef cx,
                            IR_MASM::OpsRef ops,
                            Type_ValueReg::Ref param_valueReg,
                            Type_Reg::Ref param_tagReg) {
  // skipping conversion from ScratchTagScope
  ops.splitTag(param_valueReg, param_tagReg);
}

};  // namespace Impl_MASM

namespace Impl_CacheIR {

void EmitOp_ReturnFromIC(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops) {
  ops.returnFromIC();
}

void EmitOp_GuardToInt32(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ValueId::Ref param_inputId) {
  ops.guardToInt32(param_inputId);
}

void EmitOp_GuardToInt32Index(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ValueId::Ref param_inputId, Type_Int32Id::Ref param_resultId) {
  ops.writeOp(CacheOp::GuardToInt32Index);
  ops.writeOperandId(param_inputId);
  ops.writeOperandId(param_resultId);
  ops.assertLengthMatches();
}

void EmitOp_GuardToString(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ValueId::Ref param_inputId) {
  ops.guardToString(param_inputId);
}

void EmitOp_GuardToSymbol(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ValueId::Ref param_inputId) {
  ops.guardToSymbol(param_inputId);
}

void EmitOp_GuardIsNull(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ValueId::Ref param_inputId) {
  ops.guardIsNull(param_inputId);
}

void EmitOp_GuardIsUndefined(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ValueId::Ref param_inputId) {
  ops.guardIsUndefined(param_inputId);
}

void EmitOp_GuardIsNullOrUndefined(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ValueId::Ref param_inputId) {
  ops.guardIsNullOrUndefined(param_inputId);
}

void EmitOp_GuardBooleanToInt32(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ValueId::Ref param_inputId, Type_Int32Id::Ref param_resultId) {
  ops.writeOp(CacheOp::GuardBooleanToInt32);
  ops.writeOperandId(param_inputId);
  ops.writeOperandId(param_resultId);
  ops.assertLengthMatches();
}

void EmitOp_GuardTagNotEqual(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ValueTagId::Ref param_lhsId, Type_ValueTagId::Ref param_rhsId) {
  ops.guardTagNotEqual(param_lhsId, param_rhsId);
}

void EmitOp_GuardShape(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ObjectId::Ref param_objectId, Type_ShapeField::Ref param_shapeField) {
  ops.writeOp(CacheOp::GuardShape);
  ops.writeOperandId(param_objectId);
  ops.writeStubField(param_shapeField);
  ops.assertLengthMatches();
}

void EmitOp_GuardClass(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ObjectId::Ref param_objId, Type_GuardClassKind::Ref param_kind) {
  ops.guardClass(param_objId, param_kind);
}

void EmitOp_GuardSpecificSymbol(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_SymbolId::Ref param_symbolId, Type_SymbolField::Ref param_expectedSymbolField) {
  ops.writeOp(CacheOp::GuardSpecificSymbol);
  ops.writeOperandId(param_symbolId);
  ops.writeStubField(param_expectedSymbolField);
  ops.assertLengthMatches();
}

void EmitOp_GuardSpecificAtom(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_StringId::Ref param_strId, Type_StringField::Ref param_stringField) {
  ops.writeOp(CacheOp::GuardSpecificAtom);
  ops.writeOperandId(param_strId);
  ops.writeStubField(param_stringField);
  ops.assertLengthMatches();
}

void EmitOp_LoadValueTag(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ValueId::Ref param_valId, Type_ValueTagId::Ref param_resultId) {
  ops.writeOp(CacheOp::LoadValueTag);
  ops.writeOperandId(param_valId);
  ops.writeOperandId(param_resultId);
  ops.assertLengthMatches();
}

void EmitOp_LoadBooleanResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Bool::Ref param_bool) {
  ops.loadBooleanResult(param_bool);
}

void EmitOp_LoadInt32Result(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_int32Id) {
  ops.loadInt32Result(param_int32Id);
}

void EmitOp_LoadStringResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_StringId::Ref param_stringId) {
  ops.loadStringResult(param_stringId);
}

void EmitOp_LoadSymbolResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_SymbolId::Ref param_symbolId) {
  ops.loadSymbolResult(param_symbolId);
}

void EmitOp_LoadInt32Constant(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Field::Ref param_valField, Type_Int32Id::Ref param_resultId) {
  ops.writeOp(CacheOp::LoadInt32Constant);
  ops.writeStubField(param_valField);
  ops.writeOperandId(param_resultId);
  ops.assertLengthMatches();
}

void EmitOp_LoadDenseElementResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ObjectId::Ref param_objectId, Type_Int32Id::Ref param_indexId) {
  ops.loadDenseElementResult(param_objectId, param_indexId);
}

void EmitOp_LoadInt32ArrayLengthResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ObjectId::Ref param_objectId) {
  ops.loadInt32ArrayLengthResult(param_objectId);
}

void EmitOp_LoadArgumentsObjectLengthResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ObjectId::Ref param_objectId) {
  ops.loadArgumentsObjectLengthResult(param_objectId);
}

void EmitOp_LoadArgumentsObjectArgResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ObjectId::Ref param_objId, Type_Int32Id::Ref param_indexId) {
  ops.loadArgumentsObjectArgResult(param_objId, param_indexId);
}

void EmitOp_LoadFixedSlotResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ObjectId::Ref param_objectId, Type_Int32Field::Ref param_slotField) {
  ops.writeOp(CacheOp::LoadFixedSlotResult);
  ops.writeOperandId(param_objectId);
  ops.writeStubField(param_slotField);
  ops.assertLengthMatches();
}

void EmitOp_LoadDynamicSlotResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_ObjectId::Ref param_objectId, Type_Int32Field::Ref param_slotField) {
  ops.writeOp(CacheOp::LoadDynamicSlotResult);
  ops.writeOperandId(param_objectId);
  ops.writeStubField(param_slotField);
  ops.assertLengthMatches();
}

void EmitOp_Int32NegationResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_inputId) {
  ops.int32NegationResult(param_inputId);
}

void EmitOp_Int32IncResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_inputId) {
  ops.int32IncResult(param_inputId);
}

void EmitOp_Int32DecResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_inputId) {
  ops.int32DecResult(param_inputId);
}

void EmitOp_Int32NotResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_inputId) {
  ops.int32NotResult(param_inputId);
}

void EmitOp_Int32AddResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.int32AddResult(param_lhsId, param_rhsId);
}

void EmitOp_Int32SubResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.int32SubResult(param_lhsId, param_rhsId);
}

void EmitOp_Int32MulResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.int32MulResult(param_lhsId, param_rhsId);
}

void EmitOp_Int32DivResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.int32DivResult(param_lhsId, param_rhsId);
}

void EmitOp_Int32ModResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.int32ModResult(param_lhsId, param_rhsId);
}

void EmitOp_Int32BitOrResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.int32BitOrResult(param_lhsId, param_rhsId);
}

void EmitOp_Int32BitXorResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.int32BitXorResult(param_lhsId, param_rhsId);
}

void EmitOp_Int32BitAndResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.int32BitAndResult(param_lhsId, param_rhsId);
}

void EmitOp_Int32LeftShiftResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.int32LeftShiftResult(param_lhsId, param_rhsId);
}

void EmitOp_Int32RightShiftResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.int32RightShiftResult(param_lhsId, param_rhsId);
}

void EmitOp_Int32URightShiftResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId, Type_Bool::Ref param_forceDouble) {
  ops.int32URightShiftResult(param_lhsId, param_rhsId, param_forceDouble);
}

void EmitOp_CompareNullUndefinedResult(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_JSOp::Ref param_jsop, Type_Bool::Ref param_isUndefined, Type_ValueId::Ref param_inputId) {
  ops.compareNullUndefinedResult(param_jsop, param_isUndefined, param_inputId);
}

void EmitOp_CompareInt32Result(Cachet_ContextRef cx, IR_CacheIR::OpsRef ops, Type_JSOp::Ref param_jsop, Type_Int32Id::Ref param_lhsId, Type_Int32Id::Ref param_rhsId) {
  ops.compareInt32Result(param_jsop, param_lhsId, param_rhsId);
}

Type_FailurePath::Val Fn_addFailurePathUnchecked(Cachet_ContextRef cx) {
  FailurePath* failurePath;
  // FIXME: use this result in some way
  (void)detail::CompilerInternals::addFailurePath(cx, &failurePath);
  return failurePath;
}

Type_ValueReg::Val Fn_allocateValueReg(Cachet_ContextRef cx) {
  return detail::CompilerInternals::allocator(cx).allocateValueRegister(detail::CompilerInternals::masm(cx));
}

void Fn_releaseValueReg(Cachet_ContextRef cx, Type_ValueReg::Ref param_valueReg) {
  return detail::CompilerInternals::allocator(cx).releaseValueRegister(param_valueReg);
}

Type_Reg::Val Fn_allocateReg(Cachet_ContextRef cx) {
  return detail::CompilerInternals::allocator(cx).allocateRegister(detail::CompilerInternals::masm(cx));
}

void Fn_releaseReg(Cachet_ContextRef cx, Type_Reg::Ref param_reg) {
  return detail::CompilerInternals::allocator(cx).releaseRegister(param_reg);
}

Type_Reg::Val Fn_allocateScratchReg(Cachet_ContextRef cx) {
#ifdef DEBUG
  detail::CompilerInternals::debugTrackedRegisters(cx).add(ScratchReg);
#endif
  return ScratchReg;
}

void Fn_releaseScratchReg(Cachet_ContextRef cx) {
#ifdef DEBUG
  detail::CompilerInternals::debugTrackedRegisters(cx).take(ScratchReg);
#endif
}

void Fn_allocateKnownReg(Cachet_ContextRef cx, Type_Reg::Ref param_reg) {
  detail::CompilerInternals::allocator(cx).allocateFixedRegister(
    detail::CompilerInternals::masm(cx), param_reg);
}

void Fn_allocateKnownValueReg(Cachet_ContextRef cx, Type_ValueReg::Ref param_valueReg) {
  detail::CompilerInternals::allocator(cx).allocateFixedValueRegister(
    detail::CompilerInternals::masm(cx), param_valueReg);
}

Type_FloatReg::Val Fn_allocateDoubleScratchReg(Cachet_ContextRef cx) {
#ifdef DEBUG
  detail::CompilerInternals::debugTrackedRegisters(cx).add(ScratchDoubleReg_);
#endif
  return ScratchDoubleReg_;
}

void Fn_releaseDoubleScratchReg(Cachet_ContextRef cx) {
#ifdef DEBUG
  detail::CompilerInternals::debugTrackedRegisters(cx).take(ScratchDoubleReg_);
#endif
}

Type_Reg::Val Fn_defineObjectId(Cachet_ContextRef cx, Type_ObjectId::Ref param_objectId) {
  return detail::CompilerInternals::allocator(cx).defineRegister(detail::CompilerInternals::masm(cx), param_objectId);
}

Type_Reg::Val Fn_defineInt32Id(Cachet_ContextRef cx, Type_Int32Id::Ref param_int32Id) {
  return detail::CompilerInternals::allocator(cx).defineRegister(detail::CompilerInternals::masm(cx), param_int32Id);
}

Type_ValueReg::Val Fn_defineNumberId(Cachet_ContextRef cx, Type_NumberId::Ref param_numberId) {
  return detail::CompilerInternals::allocator(cx).defineValueRegister(detail::CompilerInternals::masm(cx), param_numberId);
}

Type_Reg::Val Fn_defineValueTagId(Cachet_ContextRef cx,
                                            Type_ValueTagId::Ref param_valueTagId) {
  return detail::CompilerInternals::allocator(cx).defineRegister(detail::CompilerInternals::masm(cx), param_valueTagId);
}

Type_JSValueType::Ref Fn_knownType(Cachet_ContextRef cx, Type_ValueId::Ref param_valueId) {
  return detail::CompilerInternals::allocator(cx).knownType(param_valueId);
}

Type_ValueReg::Val Fn_useValueId(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                 Type_ValueId::Ref param_valueId) {
  return detail::CompilerInternals::allocator(cx).useValueRegister(ops, param_valueId);
}

Type_Reg::Val Fn_useTypedId(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                            Type_TypedId::Ref param_typedId) {
  return detail::CompilerInternals::allocator(cx).useRegister(ops, param_typedId);
}

Type_Reg::Val Fn_useObjectId(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                             Type_ObjectId::Ref param_objectId) {
  return detail::CompilerInternals::allocator(cx).useRegister(ops, param_objectId);
}

Type_Reg::Val Fn_useInt32Id(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                            Type_Int32Id::Ref param_int32Id) {
  return detail::CompilerInternals::allocator(cx).useRegister(ops, param_int32Id);
}

Type_ValueReg::Val Fn_useNumberId(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                                  Type_NumberId::Ref param_numberId) {
  return detail::CompilerInternals::allocator(cx).useValueRegister(ops, param_numberId);
}

Type_Reg::Val Fn_useBoolId(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                           Type_BoolId::Ref param_boolId) {
  return detail::CompilerInternals::allocator(cx).useRegister(ops, param_boolId);
}

Type_Reg::Val Fn_useStringId(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                             Type_StringId::Ref param_stringId) {
  return detail::CompilerInternals::allocator(cx).useRegister(ops, param_stringId);
}

Type_Reg::Val Fn_useSymbolId(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                             Type_SymbolId::Ref param_symbolId) {
  return detail::CompilerInternals::allocator(cx).useRegister(ops, param_symbolId);
}

Type_Reg::Val Fn_useBigIntId(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                             Type_BigIntId::Ref param_bigIntId) {
  return detail::CompilerInternals::allocator(cx).useRegister(ops, param_bigIntId);
}

Type_Reg::Val Fn_useValueTagId(Cachet_ContextRef cx, IR_MASM::OpsRef ops,
                               Type_ValueTagId::Ref param_valueTagId) {
  return detail::CompilerInternals::allocator(cx).useRegister(ops, param_valueTagId);
}

void Fn_emitLoadInt32StubField(
    Cachet_ContextRef cx, IR_MASM::OpsRef ops,
    Type_Int32Field::Ref param_int32Field, Type_Reg::Ref param_dstReg) {
  StubFieldOffset val(param_int32Field, StubField::Type::RawInt32);
  detail::CompilerInternals::emitLoadStubField(cx, val, param_dstReg);
}

void Fn_emitLoadObjectStubField(
    Cachet_ContextRef cx, IR_MASM::OpsRef ops,
    Type_ObjectField::Ref param_objectField,
    Type_Reg::Ref param_dstReg) {
  StubFieldOffset val(param_objectField, StubField::Type::Object);
  detail::CompilerInternals::emitLoadStubField(cx, val, param_dstReg);
}

void Fn_emitLoadValueStubField(
    Cachet_ContextRef cx, IR_MASM::OpsRef ops,
    Type_ValueField::Ref param_valueField,
    Type_ValueReg::Ref param_dstReg) {
  StubFieldOffset val(param_valueField, StubField::Type::Value);
  detail::CompilerInternals::emitLoadValueStubField(cx, val, param_dstReg);
}

Type_Int32::Val Fn_readInt32Field(
    Cachet_ContextRef cx,
    Type_Int32Field::Ref param_int32Field) {
  return detail::CompilerInternals::int32StubField(cx, param_int32Field);
}

Type_String::Val Fn_readStringField(
    Cachet_ContextRef cx,
    Type_StringField::Ref param_stringField) {
  return detail::CompilerInternals::stringStubField(cx, param_stringField);
}

Type_Symbol::Val Fn_readSymbolField(
    Cachet_ContextRef cx,
    Type_SymbolField::Ref param_symbolField) {
  return detail::CompilerInternals::symbolStubField(cx, param_symbolField);
}

Type_Shape::Val Fn_readShapeField(
    Cachet_ContextRef cx,
    Type_ShapeField::Ref param_shapeField) {
  return detail::CompilerInternals::shapeStubField(cx, param_shapeField);
}

Type_Class::Val Fn_readClassField(
    Cachet_ContextRef cx,
    Type_ClassField::Ref param_classField) {
  return detail::CompilerInternals::classStubField(cx, param_classField);
}

Type_Value::Val Fn_readValueField(
    Cachet_ContextRef cx,
    Type_ValueField::Ref param_valueField) {
  return detail::CompilerInternals::valueStubField(cx, param_valueField);
}

Type_Bool::Val Fn_objectGuardNeedsSpectreMitigations(
    Cachet_ContextRef cx,
    Type_ObjectId::Ref param_objectId) {
  return detail::CompilerInternals::objectGuardNeedsSpectreMitigations(cx, param_objectId);
}

Type_ValueId::Val Fn_defineInputValueId(Cachet_ContextRef cx, Type_UInt16::Ref param_id) {
  return Type_ValueId::Val(cx.writer->setInputOperandId(param_id));
}

Type_Int32Field::Val Fn_writeInt32Field(Cachet_ContextRef cx, Type_Int32::Ref param_int32) {
  return (Type_Int32Field::Val)cx.writer->addStubField(param_int32, StubField::Type::RawInt32);
}

Type_StringField::Val Fn_writeStringField(Cachet_ContextRef cx, Type_String::Ref param_string) {
  return (Type_StringField::Val)cx.writer->addStubField(
      uintptr_t(param_string.get()), StubField::Type::String);
}

Type_SymbolField::Val Fn_writeSymbolField(Cachet_ContextRef cx, Type_Symbol::Ref param_symbol) {
  return (Type_SymbolField::Val)cx.writer->addStubField(
      uintptr_t(param_symbol.get()), StubField::Type::Symbol);
}

Type_ShapeField::Val Fn_writeShapeField(Cachet_ContextRef cx, Type_Shape::Ref param_shape) {
  return (Type_ShapeField::Val)cx.writer->addStubField(
      uintptr_t(param_shape.get()), StubField::Type::Shape);
}

};  // namespace Impl_CacheIR

Type_Bool::Val Fn_isCacheableProtoChain(Cachet_ContextRef cx,
                                        Type_NativeObject::Ref param_object,
                                        Type_NativeObject::Ref param_holder) {
  return IsCacheableProtoChain(param_object, param_holder);
}

#define CACHET_CacheIR_COMPILER
#define CACHET_MASM_EMIT

// TODO(spinda): Figure out whether we really need this.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-qualification"

#include "jit/CachetGenerated.inc"

#pragma clang diagnostic pop

#undef CACHET_CacheIR_COMPILER
#undef CACHET_MASM_EMIT

};  // namespace cachet

};  // namespace jit

};  // namespace js
