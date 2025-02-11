/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: set ts=8 sts=2 et sw=2 tw=80:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef jit_CachetCompiler_h
#define jit_CachetCompiler_h

#include "mozilla/Maybe.h"

#include "builtin/DataViewObject.h"
#include "builtin/MapObject.h"
#include "builtin/Object.h"
#include "jit/CacheIR.h"
#include "jit/CacheIRCompiler.h"
#include "jit/CachetPrelude.h"
#include "jit/MacroAssembler.h"
#include "js/Class.h"
#include "js/RootingAPI.h"
#include "js/Value.h"
#include "vm/ArgumentsObject.h"
#include "vm/ArrayBufferObject.h"
#include "vm/ArrayBufferViewObject.h"
#include "vm/JSFunction.h"
#include "vm/JSObject.h"
#include "vm/NativeObject.h"
#include "vm/Opcodes.h"
#include "vm/PropertyInfo.h"
#include "vm/PropertyKey.h"
#include "vm/PropertyResult.h"
#include "vm/Shape.h"

namespace js {

namespace jit {

namespace cachet {

using namespace ::cachet::prelude;

struct CachetContext {
  CacheIRCompiler* compiler;
  JSContext* jsCx;
  // TODO(spinda): Temporary hack.
  CacheIRWriter* writer = nullptr;
  IRGenerator* generator = nullptr;
};

using Cachet_ContextRef = CachetContext;

namespace detail {

class CompilerInternals {
  CompilerInternals() = delete;
  CompilerInternals(const CompilerInternals&) = delete;
  void operator=(const CompilerInternals&) = delete;

 public:
  static MacroAssembler& masm(Cachet_ContextRef cx) {
    return cx.compiler->masm;
  }

  static mozilla::Maybe<TypedOrValueRegister> outputUnchecked(Cachet_ContextRef cx) {
    return cx.compiler->outputUnchecked_;
  }

  static LiveFloatRegisterSet& liveFloatRegs(Cachet_ContextRef cx) {
    return cx.compiler->liveFloatRegs_;
  }

  static CacheRegisterAllocator& allocator(Cachet_ContextRef cx) {
    return cx.compiler->allocator;
  }

  [[nodiscard]] static bool addFailurePath(Cachet_ContextRef cx, FailurePath** failure) {
    return cx.compiler->addFailurePath(failure);
  }

  static bool& addedFailurePath(Cachet_ContextRef cx) {
    return allocator(cx).cachetAddedFailurePath_;
  }

  static bool& hasAutoScratchFloatRegisterSpill(Cachet_ContextRef cx) {
    return allocator(cx).hasAutoScratchFloatRegisterSpill_;
  }

  static bool objectGuardNeedsSpectreMitigations(Cachet_ContextRef cx, ObjOperandId objId) {
    return cx.compiler->objectGuardNeedsSpectreMitigations(objId);
  }

  static void emitLoadStubField(Cachet_ContextRef cx, StubFieldOffset val, Register dest) {
    cx.compiler->emitLoadStubField(val, dest);
  }

  static void emitLoadValueStubField(Cachet_ContextRef cx, StubFieldOffset val, ValueOperand dest) {
    cx.compiler->emitLoadValueStubField(val, dest);
  }

  static int32_t int32StubField(Cachet_ContextRef cx, uint32_t offset) {
    return cx.compiler->int32StubField(offset);
  }

  static Shape* shapeStubField(Cachet_ContextRef cx, uint32_t offset) {
    return cx.compiler->shapeStubField(offset);
  }

  static Value valueStubField(Cachet_ContextRef cx, uint32_t offset) {
    return cx.compiler->valueStubField(offset);
  }

  static JSString* stringStubField(Cachet_ContextRef cx, uint32_t offset) {
    return cx.compiler->stringStubField(offset);
  }

  static JS::Symbol* symbolStubField(Cachet_ContextRef cx, uint32_t offset) {
    return cx.compiler->symbolStubField(offset);
  }

  static const JSClass* classStubField(Cachet_ContextRef cx, uint32_t offset) {
    return cx.compiler->classStubField(offset);
  }

#ifdef DEBUG
  static AllocatableRegisterSet& debugTrackedRegisters(Cachet_ContextRef cx) {
    return cx.compiler->masm.debugTrackedRegisters_;
  }
#endif
};

}  // namespace detail

#define Cachet_Assert(cond) MOZ_ASSERT(cond)

template <typename T>
struct GCType {
  using Val = T;
  using Local = js::Rooted<T>;
  using Ref = js::Handle<T>;
  using MutRef = js::MutableHandle<T>;

  static Val ToVal(const Local& local) {
    return local;
  }

  static Val ToVal(Ref ref) {
    return ref;
  }

  static Val ToVal(MutRef mutRef) {
    return mutRef;
  }

  static Local EmptyLocal(Cachet_ContextRef cx) {
    return Local(cx.jsCx);
  }

  static Local ToLocal(Cachet_ContextRef cx, Val&& val) {
    return Local(cx.jsCx, std::move(val));
  }

  static Local ToLocal(Cachet_ContextRef cx, Ref ref) {
    return Local(cx.jsCx, ref);
  }

  static Local ToLocal(Cachet_ContextRef cx, MutRef mutRef) {
    return Local(cx.jsCx, mutRef);
  }

  static Ref ToRef(const Local& local) {
    return local;
  }

  static Ref ToRef(MutRef mutRef) {
    return mutRef;
  }

  static MutRef ToMutRef(Local& local) {
    return &local;
  }

  static void SetMutRef(MutRef lhs, Val&& rhs) {
    lhs.set(std::move(rhs));
  }

  static void SetMutRef(MutRef lhs, const Local& rhs) {
    lhs.set(rhs);
  }

  static void SetMutRef(MutRef lhs, Ref rhs) {
    lhs.set(rhs);
  }

  static void SetMutRef(MutRef lhs, MutRef rhs) {
    lhs.set(rhs);
  }

  static bool Eq(Ref lhs, Ref rhs) {
    return lhs == rhs;
  }

  static bool Neq(Ref lhs, Ref rhs) {
    return lhs != rhs;
  }
};

using Type_Heap = Type_Unit;

enum class ABIFunction {
  EqualStringsHelperPure,
};
using Type_ABIFunction = PrimitiveType<ABIFunction>;

using Type_MIRType = PrimitiveType<MIRType>;
using Type_JSValueType = PrimitiveType<JSValueType>;
using Type_ValueType = PrimitiveType<JS::ValueType>;
using Type_Value = GCType<JS::Value>;
using Type_Float64 = cachet::Type_Double;
using Type_Object = GCType<JSObject*>;
using Type_String = GCType<JSString*>;
using Type_Atom = GCType<JSAtom*>;
using Type_Symbol = GCType<JS::Symbol*>;
using Type_BigInt = GCType<JS::BigInt*>;
using Type_NativeObject = GCType<NativeObject*>;
using Type_NativeObjectSlots = PrimitiveType<HeapSlot*>;
using Type_NativeObjectElements = PrimitiveType<HeapSlot*>;
using Type_JSFunction = GCType<JSFunction*>;
using Type_ArrayObject = GCType<js::ArrayObject*>;
using Type_PlainObject = GCType<js::PlainObject*>;
using Type_ArrayBufferObjectMaybeShared = GCType<js::ArrayBufferObjectMaybeShared*>;
using Type_ArrayBufferObject = GCType<js::ArrayBufferObject*>;
using Type_SharedArrayBufferObject = GCType<js::SharedArrayBufferObject*>;
using Type_ArrayBufferViewObject = GCType<js::ArrayBufferViewObject*>;
using Type_DataViewObject = GCType<js::DataViewObject*>;
using Type_ArgumentsObject = GCType<js::ArgumentsObject*>;
using Type_ArgumentsData = PrimitiveType<js::ArgumentsData*>;
using Type_MappedArgumentsObject = GCType<js::MappedArgumentsObject*>;
using Type_UnmappedArgumentsObject = GCType<js::UnmappedArgumentsObject*>;
using Type_SetObject = GCType<js::SetObject*>;
using Type_MapObject = GCType<js::MapObject*>;
using Type_Shape = GCType<js::Shape*>;
using Type_BaseShape = GCType<js::BaseShape*>;
using Type_Class = PrimitiveType<const JSClass*>;
using Type_TaggedProto = GCType<js::TaggedProto>;
using Type_JSOp = PrimitiveType<JSOp>;

//using Cast_Object_NativeObject = GCCast<Type_Object, Type_NativeObject>;

using Type_ValueReg = StructType<ValueOperand>;
using Type_Reg = PrimitiveType<Register>;
using Type_PhyFloatReg = PrimitiveType<X86Encoding::XMMRegisterID>;
using Type_FloatContentType = PrimitiveType<FloatRegisters::ContentType>;
using Type_FloatReg = PrimitiveType<FloatRegister>;
using Type_AnyReg = PrimitiveType<AnyRegister>;
using Type_TypedOrValueReg = PrimitiveType<TypedOrValueRegister>;
using Type_GeneralRegSet = PrimitiveType<GeneralRegisterSet>;
using Type_FloatRegSet = PrimitiveType<FloatRegisterSet>;
using Type_LiveRegSet = PrimitiveType<LiveRegisterSet>;
using Type_Condition = PrimitiveType<Assembler::Condition>;
using Type_Address = StructType<js::jit::Address>;
using Type_Scale = PrimitiveType<js::jit::Scale>;
using Type_BaseIndex = StructType<js::jit::BaseIndex>;
using Type_BaseValueIndex = StructType<js::jit::BaseValueIndex>;
using Type_BaseObjectElementIndex = StructType<js::jit::BaseObjectElementIndex>;
using Type_BaseObjectSlotIndex = StructType<js::jit::BaseObjectSlotIndex>;
// TODO(spinda): Figure this one out.
//using Type_AutoScratchFloatReg = PrimitiveType<AutoScratchFloatRegister>;

//using Cast_BaseIndex_BaseValueIndex = StructCast<BaseIndex, BaseValueIndex>;
//using Cast_BaseValueIndex_BaseObjectElementIndex = StructCast<BaseValueIndex, BaseObjectElementIndex>;
//using Cast_BaseValueIndex_BaseObjectSlotIndex = StructCast<BaseValueIndex, BaseObjectSlotIndex>;

using Type_FailurePath = PrimitiveType<js::jit::FailurePath*>;

using Type_OperandLocation = StructType<OperandLocation>;
using Type_OperandLocationKind = PrimitiveType<OperandLocation::Kind>;

using Type_OperandId = PrimitiveType<OperandId>;
using Type_ValueId = PrimitiveType<ValOperandId>;
using Type_ObjectId = PrimitiveType<ObjOperandId>;
using Type_StringId = PrimitiveType<StringOperandId>;
using Type_SymbolId = PrimitiveType<SymbolOperandId>;
using Type_BoolId = PrimitiveType<BooleanOperandId>;
using Type_Int32Id = PrimitiveType<Int32OperandId>;
using Type_NumberId = PrimitiveType<NumberOperandId>;
using Type_BigIntId = PrimitiveType<BigIntOperandId>;
using Type_ValueTagId = PrimitiveType<ValueTagOperandId>;
using Type_IntPtrId = PrimitiveType<IntPtrOperandId>;
using Type_TypedId = PrimitiveType<TypedOperandId>;

using Type_AttachDecision = PrimitiveType<AttachDecision>;

//using Cast_OperandId_ValueId = PrimitiveCast<OperandId, ValueId>;
//using Cast_OperandId_ObjectId = PrimitiveCast<OperandId, ObjectId>;
// ...

using StubFieldType = PrimitiveType<uint32_t>;
using Type_Int32Field = StubFieldType;
using Type_IntPtrField = StubFieldType;
using Type_ShapeField = StubFieldType;
using Type_ClassField = StubFieldType;
using Type_GetterSetterField = StubFieldType;
using Type_ObjectField = StubFieldType;
using Type_StringField = StubFieldType;
using Type_SymbolField = StubFieldType;
using Type_BaseScriptField = StubFieldType;
using Type_IdField = StubFieldType;
using Type_AllocSiteField = StubFieldType;
using Type_Int64Field = StubFieldType;
using Type_ValueField = StubFieldType;

using Type_GuardClassKind = PrimitiveType<GuardClassKind>;

using Type_NativeGetPropKind = PrimitiveType<NativeGetPropCacheability>;
using Type_PropertyFlags = PrimitiveType<PropertyFlags>;
using Type_PropertyInfo = PrimitiveType<PropertyInfo>;
using Type_PropertyKey = GCType<PropertyKey>;
using Type_PropertyResult = StructType<PropertyResult>;
using Type_PropertyResultKind = PrimitiveType<PropertyResult::Kind>;

using Type_MaybeNativeObject = GCType<mozilla::Maybe<NativeObject*>>;
using Type_MaybePropertyInfo = PrimitiveType<mozilla::Maybe<PropertyInfo>>;
using Type_MaybeReg = PrimitiveType<mozilla::Maybe<Register>>;

namespace IR_MASM {

using OpsRef = MacroAssembler&;

using LabelRef = Label*;
using LabelLocal = Label;
using LabelMutRef = Label*;

inline LabelLocal NewLabel(Cachet_ContextRef cx) {
  return Label();
}

inline LabelMutRef ToLabelMutRef(LabelLocal& label) {
  return &label;
}

inline LabelRef ToLabelRef(LabelLocal& label) {
  return &label;
}

inline void BindLabel(Cachet_ContextRef cx, OpsRef ops, LabelMutRef label) {
  ops.bind(label);
}

};  // namespace IR_MASM

namespace IR_CacheIR {

using OpsRef = CacheIRWriter&;

};  // namespace IR_CacheIR

#define CACHET_CacheIR_COMPILER
#define CACHET_CacheIR_EMIT
#define CACHET_MASM_EMIT

#include "jit/CachetGenerated.h"

#undef CACHET_CacheIR_COMPILER
#undef CACHET_MASM_EMIT

inline Type_Heap::MutRef Var_heap(Cachet_ContextRef cx) {
  static Type_Heap::Local heap;
  return heap;
}

namespace Impl_CacheIR {

inline Type_Bool::MutRef Var_addedFailurePath(Cachet_ContextRef cx) {
  return detail::CompilerInternals::addedFailurePath(cx);
}

inline Type_Bool::MutRef Var_hasAutoScratchFloatRegisterSpill(Cachet_ContextRef cx) {
  return detail::CompilerInternals::hasAutoScratchFloatRegisterSpill(cx);
}

inline Type_TypedOrValueReg::Ref Var_outputReg(Cachet_ContextRef cx) {
  return detail::CompilerInternals::outputUnchecked(cx).value();
}

inline Type_FloatRegSet::MutRef Var_liveFloatRegSet(Cachet_ContextRef cx) {
  return detail::CompilerInternals::liveFloatRegs(cx).set();
}

inline Type_UInt16::MutRef Var_nextOperandId(Cachet_ContextRef cx) {
  return cx.writer->nextOperandId_;
}

}  // namespace Impl_CacheIR

namespace Impl_ToPropertyKeyIRGenerator {

inline Type_Value::Ref Var_value(Cachet_ContextRef cx) {
  return static_cast<ToPropertyKeyIRGenerator*>(cx.generator)->val_;
}

};  // namespace Impl_ToPropertyKeyIRGenerator

namespace Impl_UnaryArithIRGenerator {

inline Type_JSOp::Ref Var_op_(Cachet_ContextRef cx) {
  return static_cast<UnaryArithIRGenerator*>(cx.generator)->op_;
}

inline Type_Value::Ref Var_value(Cachet_ContextRef cx) {
  return static_cast<UnaryArithIRGenerator*>(cx.generator)->val_;
}

inline Type_Value::Ref Var_resValue(Cachet_ContextRef cx) {
  return static_cast<UnaryArithIRGenerator*>(cx.generator)->res_;
}

};  // namespace Impl_UnaryArithIRGenerator

namespace Impl_BinaryArithIRGenerator {

inline Type_JSOp::Ref Var_op_(Cachet_ContextRef cx) {
  return static_cast<BinaryArithIRGenerator*>(cx.generator)->op_;
}

inline Type_Value::Ref Var_lhsValue(Cachet_ContextRef cx) {
  return static_cast<BinaryArithIRGenerator*>(cx.generator)->lhs_;
}

inline Type_Value::Ref Var_rhsValue(Cachet_ContextRef cx) {
  return static_cast<BinaryArithIRGenerator*>(cx.generator)->rhs_;
}

inline Type_Value::Ref Var_resValue(Cachet_ContextRef cx) {
  return static_cast<BinaryArithIRGenerator*>(cx.generator)->res_;
}

};  // namespace Impl_BinaryArithIRGenerator

namespace Impl_CompareIRGenerator {

inline Type_JSOp::Ref Var_op_(Cachet_ContextRef cx) {
  return static_cast<CompareIRGenerator*>(cx.generator)->op_;
}

inline Type_Value::Ref Var_lhsValue(Cachet_ContextRef cx) {
  return static_cast<CompareIRGenerator*>(cx.generator)->lhsVal_;
}

inline Type_Value::Ref Var_rhsValue(Cachet_ContextRef cx) {
  return static_cast<CompareIRGenerator*>(cx.generator)->rhsVal_;
}

};  // namespace Impl_CompareIRGenerator

namespace Impl_GetElemIRGenerator {

inline Type_Value::Ref Var_keyValue(Cachet_ContextRef cx) {
  return static_cast<GetPropIRGenerator*>(cx.generator)->idVal_;
}

inline Type_ValueId::Ref Var_keyValueId(Cachet_ContextRef cx) {
  return static_cast<GetPropIRGenerator*>(cx.generator)->getElemKeyValueId();
}

};  // namespace Impl_GetElemIRGenerator

namespace Impl_MIRType {

inline Type_MIRType::Ref Variant_Undefined(Cachet_ContextRef cx) {
  return MIRType::Undefined;
}

inline Type_MIRType::Ref Variant_Null(Cachet_ContextRef cx) {
  return MIRType::Null;
}

inline Type_MIRType::Ref Variant_Boolean(Cachet_ContextRef cx) {
  return MIRType::Boolean;
}

inline Type_MIRType::Ref Variant_Int32(Cachet_ContextRef cx) {
  return MIRType::Int32;
}

inline Type_MIRType::Ref Variant_Int64(Cachet_ContextRef cx) {
  return MIRType::Int64;
}

inline Type_MIRType::Ref Variant_IntPtr(Cachet_ContextRef cx) {
  return MIRType::IntPtr;
}

inline Type_MIRType::Ref Variant_Double(Cachet_ContextRef cx) {
  return MIRType::Double;
}

inline Type_MIRType::Ref Variant_Float32(Cachet_ContextRef cx) {
  return MIRType::Float32;
}

inline Type_MIRType::Ref Variant_String(Cachet_ContextRef cx) {
  return MIRType::String;
}

inline Type_MIRType::Ref Variant_Symbol(Cachet_ContextRef cx) {
  return MIRType::Symbol;
}

inline Type_MIRType::Ref Variant_BigInt(Cachet_ContextRef cx) {
  return MIRType::BigInt;
}

inline Type_MIRType::Ref Variant_Simd128(Cachet_ContextRef cx) {
  return MIRType::Simd128;
}

inline Type_MIRType::Ref Variant_Object(Cachet_ContextRef cx) {
  return MIRType::Object;
}

inline Type_MIRType::Ref Variant_MagicOptimizedOut(Cachet_ContextRef cx) {
  return MIRType::MagicOptimizedOut;
}

inline Type_MIRType::Ref Variant_MagicHole(Cachet_ContextRef cx) {
  return MIRType::MagicHole;
}

inline Type_MIRType::Ref Variant_MagicIsConstructing(Cachet_ContextRef cx) {
  return MIRType::MagicIsConstructing;
}

inline Type_MIRType::Ref Variant_MagicUninitializedLexical(Cachet_ContextRef cx) {
  return MIRType::MagicUninitializedLexical;
}

inline Type_MIRType::Ref Variant_Value(Cachet_ContextRef cx) {
  return MIRType::Value;
}

inline Type_MIRType::Ref Variant_None(Cachet_ContextRef cx) {
  return MIRType::None;
}

inline Type_MIRType::Ref Variant_Slots(Cachet_ContextRef cx) {
  return MIRType::Slots;
}

inline Type_MIRType::Ref Variant_Elements(Cachet_ContextRef cx) {
  return MIRType::Elements;
}

inline Type_MIRType::Ref Variant_Pointer(Cachet_ContextRef cx) {
  return MIRType::Pointer;
}

inline Type_MIRType::Ref Variant_RefOrNull(Cachet_ContextRef cx) {
  return MIRType::RefOrNull;
}

inline Type_MIRType::Ref Variant_StackResults(Cachet_ContextRef cx) {
  return MIRType::StackResults;
}

inline Type_MIRType::Ref Variant_Shape(Cachet_ContextRef cx) {
  return MIRType::Shape;
}

};  // namespace Impl_MIRType

namespace Impl_JSValueType {

inline Type_JSValueType::Ref Variant_Double(Cachet_ContextRef cx) {
  return JSVAL_TYPE_DOUBLE;
}

inline Type_JSValueType::Ref Variant_Int32(Cachet_ContextRef cx) {
  return JSVAL_TYPE_INT32;
}

inline Type_JSValueType::Ref Variant_Bool(Cachet_ContextRef cx) {
  return JSVAL_TYPE_BOOLEAN;
}

inline Type_JSValueType::Ref Variant_Undefined(Cachet_ContextRef cx) {
  return JSVAL_TYPE_UNDEFINED;
}

inline Type_JSValueType::Ref Variant_Null(Cachet_ContextRef cx) {
  return JSVAL_TYPE_NULL;
}

inline Type_JSValueType::Ref Variant_Magic(Cachet_ContextRef cx) {
  return JSVAL_TYPE_MAGIC;
}

inline Type_JSValueType::Ref Variant_String(Cachet_ContextRef cx) {
  return JSVAL_TYPE_STRING;
}

inline Type_JSValueType::Ref Variant_Symbol(Cachet_ContextRef cx) {
  return JSVAL_TYPE_SYMBOL;
}

inline Type_JSValueType::Ref Variant_PrivateGCThing(Cachet_ContextRef cx) {
  return JSVAL_TYPE_PRIVATE_GCTHING;
}

inline Type_JSValueType::Ref Variant_BigInt(Cachet_ContextRef cx) {
  return JSVAL_TYPE_BIGINT;
}

inline Type_JSValueType::Ref Variant_Object(Cachet_ContextRef cx) {
  return JSVAL_TYPE_OBJECT;
}

inline Type_JSValueType::Ref Variant_Unknown(Cachet_ContextRef cx) {
  return JSVAL_TYPE_UNKNOWN;
}

};  // namespace Impl_JSValueType

namespace Impl_ValueType {

inline Type_ValueType::Ref Variant_Double(Cachet_ContextRef cx) {
  return JS::ValueType::Double;
}

inline Type_ValueType::Ref Variant_Int32(Cachet_ContextRef cx) {
  return JS::ValueType::Int32;
}

inline Type_ValueType::Ref Variant_Bool(Cachet_ContextRef cx) {
  return JS::ValueType::Boolean;
}

inline Type_ValueType::Ref Variant_Undefined(Cachet_ContextRef cx) {
  return JS::ValueType::Undefined;
}

inline Type_ValueType::Ref Variant_Null(Cachet_ContextRef cx) {
  return JS::ValueType::Null;
}

inline Type_ValueType::Ref Variant_Magic(Cachet_ContextRef cx) {
  return JS::ValueType::Magic;
}

inline Type_ValueType::Ref Variant_String(Cachet_ContextRef cx) {
  return JS::ValueType::String;
}

inline Type_ValueType::Ref Variant_Symbol(Cachet_ContextRef cx) {
  return JS::ValueType::Symbol;
}

inline Type_ValueType::Ref Variant_PrivateGCThing(Cachet_ContextRef cx) {
  return JS::ValueType::PrivateGCThing;
}

inline Type_ValueType::Ref Variant_BigInt(Cachet_ContextRef cx) {
  return JS::ValueType::BigInt;
}

inline Type_ValueType::Ref Variant_Object(Cachet_ContextRef cx) {
  return JS::ValueType::Object;
}

};  // namespace Impl_ValueType

namespace Impl_JSOp {

inline Type_JSOp::Ref Variant_Eq(Cachet_ContextRef cx) {
  return JSOp::Eq;
}

inline Type_JSOp::Ref Variant_Ne(Cachet_ContextRef cx) {
  return JSOp::Ne;
}

inline Type_JSOp::Ref Variant_StrictEq(Cachet_ContextRef cx) {
  return JSOp::StrictEq;
}

inline Type_JSOp::Ref Variant_StrictNe(Cachet_ContextRef cx) {
  return JSOp::StrictNe;
}

inline Type_JSOp::Ref Variant_Lt(Cachet_ContextRef cx) {
  return JSOp::Lt;
}

inline Type_JSOp::Ref Variant_Gt(Cachet_ContextRef cx) {
  return JSOp::Gt;
}

inline Type_JSOp::Ref Variant_Le(Cachet_ContextRef cx) {
  return JSOp::Le;
}

inline Type_JSOp::Ref Variant_Ge(Cachet_ContextRef cx) {
  return JSOp::Ge;
}

inline Type_JSOp::Ref Variant_Pos(Cachet_ContextRef cx) {
  return JSOp::Pos;
}

inline Type_JSOp::Ref Variant_Neg(Cachet_ContextRef cx) {
  return JSOp::Neg;
}

inline Type_JSOp::Ref Variant_Inc(Cachet_ContextRef cx) {
  return JSOp::Inc;
}

inline Type_JSOp::Ref Variant_Dec(Cachet_ContextRef cx) {
  return JSOp::Dec;
}

inline Type_JSOp::Ref Variant_ToNumeric(Cachet_ContextRef cx) {
  return JSOp::ToNumeric;
}

inline Type_JSOp::Ref Variant_BitNot(Cachet_ContextRef cx) {
  return JSOp::BitNot;
}

inline Type_JSOp::Ref Variant_BitAnd(Cachet_ContextRef cx) {
  return JSOp::BitAnd;
}

inline Type_JSOp::Ref Variant_BitOr(Cachet_ContextRef cx) {
  return JSOp::BitOr;
}

inline Type_JSOp::Ref Variant_BitXor(Cachet_ContextRef cx) {
  return JSOp::BitXor;
}

inline Type_JSOp::Ref Variant_Lsh(Cachet_ContextRef cx) {
  return JSOp::Lsh;
}

inline Type_JSOp::Ref Variant_Rsh(Cachet_ContextRef cx) {
  return JSOp::Rsh;
}

inline Type_JSOp::Ref Variant_Ursh(Cachet_ContextRef cx) {
  return JSOp::Ursh;
}

inline Type_JSOp::Ref Variant_Add(Cachet_ContextRef cx) {
  return JSOp::Add;
}

inline Type_JSOp::Ref Variant_Sub(Cachet_ContextRef cx) {
  return JSOp::Sub;
}

inline Type_JSOp::Ref Variant_Mul(Cachet_ContextRef cx) {
  return JSOp::Mul;
}

inline Type_JSOp::Ref Variant_Div(Cachet_ContextRef cx) {
  return JSOp::Div;
}

inline Type_JSOp::Ref Variant_Mod(Cachet_ContextRef cx) {
  return JSOp::Mod;
}

};  // namespace Impl_JSOp

namespace Impl_NativeGetPropKind {

inline Type_NativeGetPropKind::Ref Variant_None(Cachet_ContextRef cx) {
  return CanAttachNone;
}

inline Type_NativeGetPropKind::Ref Variant_Slot(Cachet_ContextRef cx) {
  return CanAttachReadSlot;
}

inline Type_NativeGetPropKind::Ref Variant_NativeGetter(Cachet_ContextRef cx) {
  return CanAttachNativeGetter;
}

inline Type_NativeGetPropKind::Ref Variant_ScriptedGetter(Cachet_ContextRef cx) {
  return CanAttachScriptedGetter;
}

};  // namespace Impl_NativeGetPropKind

namespace Impl_PropertyResultKind {

inline Type_PropertyResultKind::Ref Variant_NotFound(Cachet_ContextRef cx) {
  return js::PropertyResult::Kind::NotFound;
}

inline Type_PropertyResultKind::Ref Variant_NativeProperty(Cachet_ContextRef cx) {
  return js::PropertyResult::Kind::NativeProperty;
}

inline Type_PropertyResultKind::Ref Variant_NonNativeProperty(Cachet_ContextRef cx) {
  return js::PropertyResult::Kind::NonNativeProperty;
}

inline Type_PropertyResultKind::Ref Variant_DenseElement(Cachet_ContextRef cx) {
  return js::PropertyResult::Kind::DenseElement;
}

inline Type_PropertyResultKind::Ref Variant_TypedArrayElement(Cachet_ContextRef cx) {
  return js::PropertyResult::Kind::TypedArrayElement;
}

};  // namespace Impl_PropertyResultKind

namespace Impl_Reg {

inline Type_Reg::Ref Variant_R11(Cachet_ContextRef cx) {
  return Register(X86Encoding::r11);
}

};  // namespace Impl_Reg

namespace Impl_PhyFloatReg {

inline Type_PhyFloatReg::Ref Variant_Xmm0(Cachet_ContextRef cx) {
  return X86Encoding::xmm0;
}

inline Type_PhyFloatReg::Ref Variant_Xmm15(Cachet_ContextRef cx) {
  return X86Encoding::xmm15;
}

};  // namespace Impl_PhyFloatReg

namespace Impl_FloatContentType {

inline Type_FloatContentType::Ref Variant_Single(Cachet_ContextRef cx) {
  return FloatRegisters::Single;
}

inline Type_FloatContentType::Ref Variant_Double(Cachet_ContextRef cx) {
  return FloatRegisters::Double;
}

inline Type_FloatContentType::Ref Variant_Simd128(Cachet_ContextRef cx) {
  return FloatRegisters::Simd128;
}

};  // namespace Impl_FloatContentType

namespace Impl_Condition {

inline Type_Condition::Ref Variant_Equal(Cachet_ContextRef cx) {
  return Assembler::Condition::Equal;
}

inline Type_Condition::Ref Variant_NotEqual(Cachet_ContextRef cx) {
  return Assembler::Condition::NotEqual;
}

inline Type_Condition::Ref Variant_Overflow(Cachet_ContextRef cx) {
  return Assembler::Condition::Overflow;
}

inline Type_Condition::Ref Variant_Zero(Cachet_ContextRef cx) {
  return Assembler::Condition::Zero;
}

inline Type_Condition::Ref Variant_NonZero(Cachet_ContextRef cx) {
  return Assembler::Condition::NonZero;
}

inline Type_Condition::Ref Variant_Signed(Cachet_ContextRef cx) {
  return Assembler::Condition::Signed;
}

inline Type_Condition::Ref Variant_NotSigned(Cachet_ContextRef cx) {
  return Assembler::Condition::NotSigned;
}

inline Type_Condition::Ref Variant_GreaterThan(Cachet_ContextRef cx) {
  return Assembler::Condition::GreaterThan;
}

inline Type_Condition::Ref Variant_LessThan(Cachet_ContextRef cx) {
  return Assembler::Condition::LessThan;
}

inline Type_Condition::Ref Variant_GreaterThanOrEqual(Cachet_ContextRef cx) {
  return Assembler::Condition::GreaterThanOrEqual;
}

inline Type_Condition::Ref Variant_LessThanOrEqual(Cachet_ContextRef cx) {
  return Assembler::Condition::LessThanOrEqual;
}

inline Type_Condition::Ref Variant_Above(Cachet_ContextRef cx) {
  return Assembler::Condition::Above;
}

inline Type_Condition::Ref Variant_Below(Cachet_ContextRef cx) {
  return Assembler::Condition::Below;
}

inline Type_Condition::Ref Variant_AboveOrEqual(Cachet_ContextRef cx) {
  return Assembler::Condition::AboveOrEqual;
}

inline Type_Condition::Ref Variant_BelowOrEqual(Cachet_ContextRef cx) {
  return Assembler::Condition::BelowOrEqual;
}

};  // namespace Impl_Condition

namespace Impl_Scale {

inline Type_Scale::Ref Variant_TimesOne(Cachet_ContextRef cx) {
  return TimesOne;
}

inline Type_Scale::Ref Variant_TimesTwo(Cachet_ContextRef cx) {
  return TimesTwo;
}

inline Type_Scale::Ref Variant_TimesFour(Cachet_ContextRef cx) {
  return TimesFour;
}

inline Type_Scale::Ref Variant_TimesEight(Cachet_ContextRef cx) {
  return TimesEight;
}

};  // namespace Impl_Scale

namespace Impl_OperandLocationKind {

inline Type_OperandLocationKind::Ref Variant_Uninitialized(Cachet_ContextRef cx) {
  return OperandLocation::Kind::Uninitialized;
}

inline Type_OperandLocationKind::Ref Variant_ValueReg(Cachet_ContextRef cx) {
  return OperandLocation::Kind::ValueReg;
}

inline Type_OperandLocationKind::Ref Variant_PayloadReg(Cachet_ContextRef cx) {
  return OperandLocation::Kind::PayloadReg;
}

inline Type_OperandLocationKind::Ref Variant_FloatReg(Cachet_ContextRef cx) {
  return OperandLocation::Kind::DoubleReg;
}

};  // namespace Impl_OperandLocationKind

namespace Impl_GuardClassKind {

inline Type_GuardClassKind::Ref Variant_Array(Cachet_ContextRef cx) {
  return GuardClassKind::Array;
}

inline Type_GuardClassKind::Ref Variant_ArrayBuffer(Cachet_ContextRef cx) {
  return GuardClassKind::ArrayBuffer;
}

inline Type_GuardClassKind::Ref Variant_SharedArrayBuffer(Cachet_ContextRef cx) {
  return GuardClassKind::SharedArrayBuffer;
}

inline Type_GuardClassKind::Ref Variant_DataView(Cachet_ContextRef cx) {
  return GuardClassKind::DataView;
}

inline Type_GuardClassKind::Ref Variant_MappedArguments(Cachet_ContextRef cx) {
  return GuardClassKind::MappedArguments;
}

inline Type_GuardClassKind::Ref Variant_UnmappedArguments(Cachet_ContextRef cx) {
  return GuardClassKind::UnmappedArguments;
}

inline Type_GuardClassKind::Ref Variant_WindowProxy(Cachet_ContextRef cx) {
  return GuardClassKind::WindowProxy;
}

inline Type_GuardClassKind::Ref Variant_JSFunction(Cachet_ContextRef cx) {
  return GuardClassKind::JSFunction;
}

inline Type_GuardClassKind::Ref Variant_Set(Cachet_ContextRef cx) {
  return GuardClassKind::Set;
}

inline Type_GuardClassKind::Ref Variant_Map(Cachet_ContextRef cx) {
  return GuardClassKind::Map;
}

};  // namespace Impl_GuardClassKind

namespace Impl_JSFunction {

inline Type_Class::Ref Var_rawClass(Cachet_ContextRef cx) {
  return &JSFunction::class_;
}

};  // namespace Impl_JSFunction

namespace Impl_ArrayObject {

inline Type_Class::Ref Var_rawClass(Cachet_ContextRef cx) {
  return &ArrayObject::class_;
}

};  // namespace Impl_ArrayObject

namespace Impl_PlainObject {

inline Type_Class::Ref Var_rawClass(Cachet_ContextRef cx) {
  return &PlainObject::class_;
}

};  // namespace Impl_PlainObject

namespace Impl_ArrayBufferObject {

inline Type_Class::Ref Var_rawClass(Cachet_ContextRef cx) {
  return &ArrayBufferObject::class_;
}

};  // namespace Impl_ArrayBufferObject

namespace Impl_SharedArrayBufferObject {

inline Type_Class::Ref Var_rawClass(Cachet_ContextRef cx) {
  return &SharedArrayBufferObject::class_;
}

};  // namespace Impl_SharedArrayBufferObject

namespace Impl_DataViewObject {

inline Type_Class::Ref Var_rawClass(Cachet_ContextRef cx) {
  return &DataViewObject::class_;
}

};  // namespace Impl_DataViewObject

namespace Impl_UnmappedArgumentsObject {

inline Type_Class::Ref Var_rawClass(Cachet_ContextRef cx) {
  return &UnmappedArgumentsObject::class_;
}

};  // namespace Impl_UnmappedArgumentsObject

namespace Impl_MappedArgumentsObject {

inline Type_Class::Ref Var_rawClass(Cachet_ContextRef cx) {
  return &MappedArgumentsObject::class_;
}

};  // namespace Impl_MappedArgumentsObject

namespace Impl_SetObject {

inline Type_Class::Ref Var_rawClass(Cachet_ContextRef cx) {
  return &SetObject::class_;
}

};  // namespace Impl_SetObject

namespace Impl_MapObject {

inline Type_Class::Ref Var_rawClass(Cachet_ContextRef cx) {
  return &MapObject::class_;
}

};  // namespace Impl_MapObject

namespace Impl_Object {

inline Type_NativeObject::Val To_NativeObject_Val(Type_Object::Val&& param_in) {
  return &param_in->as<NativeObject>();
}

inline Type_NativeObject::Ref To_NativeObject_Ref(Type_Object::Ref param_in) {
  return param_in.as<NativeObject>();
}

};  // namespace Impl_Object

namespace Impl_NativeObject {

inline Type_Object::Val To_Object_Val(Type_NativeObject::Val&& param_in) {
  return param_in;
}

inline Type_Object::Ref To_Object_Ref(Type_NativeObject::Ref param_in) {
  return Handle<JSObject*>::fromMarkedLocation(
      reinterpret_cast<JSObject* const*>(param_in.address()));
}

inline Type_ArrayObject::Val To_ArrayObject_Val(Type_NativeObject::Val&& param_in) {
  return &param_in->as<ArrayObject>();
}

inline Type_ArrayObject::Ref To_ArrayObject_Ref(Type_NativeObject::Ref param_in) {
  return Handle<ArrayObject*>::fromMarkedLocation(
      reinterpret_cast<ArrayObject* const*>(param_in.address()));
}

inline Type_ArgumentsObject::Val To_ArgumentsObject_Val(Type_NativeObject::Val&& param_in) {
  return &param_in->as<ArgumentsObject>();
}

inline Type_ArgumentsObject::Ref To_ArgumentsObject_Ref(Type_NativeObject::Ref param_in) {
  return Handle<ArgumentsObject*>::fromMarkedLocation(
      reinterpret_cast<ArgumentsObject* const*>(param_in.address()));
}

};  // namespace Impl_NativeObject

namespace Impl_BaseValueIndex {

inline Type_BaseIndex::Val To_BaseIndex_Val(Type_BaseValueIndex::Val&& in) {
  return static_cast<BaseIndex>(in);
}

inline Type_BaseIndex::Ref To_BaseIndex_Ref(Type_BaseValueIndex::Ref in) {
  return static_cast<const BaseIndex&>(in);
}

};  // namespace Impl_BaseValueIndex

namespace Impl_ArgumentsObject {

inline Type_NativeObject::Val To_NativeObject_Val(Type_ArgumentsObject::Val&& param_in) {
  return param_in;
}

inline Type_NativeObject::Ref To_NativeObject_Ref(Type_ArgumentsObject::Ref param_in) {
  return Handle<NativeObject*>::fromMarkedLocation(
      reinterpret_cast<NativeObject* const*>(param_in.address()));
}

};  // namespace Impl_ArgumentsObject

namespace Impl_BaseObjectElementIndex {

inline Type_BaseValueIndex::Val To_BaseValueIndex_Val(Type_BaseObjectElementIndex::Val&& in) {
  return static_cast<BaseValueIndex>(in);
}

inline Type_BaseValueIndex::Ref To_BaseValueIndex_Ref(Type_BaseObjectElementIndex::Ref in) {
  return static_cast<const BaseValueIndex&>(in);
}

};  // namespace Impl_BaseObjectElementIndex

namespace Impl_BaseObjectSlotIndex {

inline Type_BaseValueIndex::Val To_BaseValueIndex_Val(Type_BaseObjectSlotIndex::Val&& in) {
  return static_cast<BaseValueIndex>(in);
}

inline Type_BaseValueIndex::Ref To_BaseValueIndex_Ref(Type_BaseObjectSlotIndex::Ref in) {
  return static_cast<const BaseValueIndex&>(in);
}

};  // namespace Impl_BaseObjectSlotIndex

namespace Impl_Atom {

inline Type_String::Val To_String_Val(Type_Atom::Val&& in) {
  return in;
}

inline Type_String::Ref To_String_Ref(Type_Atom::Ref in) {
  return in;
}

};  // namespace Impl_Atom

namespace Impl_AtomState {

inline Type_Atom::Ref Var_length(Cachet_ContextRef cx) {
  return cx.jsCx->names().length.toHandle();
}

inline Type_Atom::Ref Var_null(Cachet_ContextRef cx) {
  return cx.jsCx->names().null.toHandle();
}

inline Type_Atom::Ref Var_undefined(Cachet_ContextRef cx) {
  return cx.jsCx->names().undefined.toHandle();
}

};  // namespace Impl_AtomState

namespace Impl_ValueId {

inline Type_OperandId::Val To_OperandId_Val(Type_ValueId::Val&& in) {
  return static_cast<OperandId>(in);
}

inline Type_OperandId::Ref To_OperandId_Ref(Type_ValueId::Ref in) {
  return static_cast<OperandId>(in);
}

};  // namespace Impl_ValueId

namespace Impl_ObjectId {

inline Type_OperandId::Val To_OperandId_Val(Type_ObjectId::Val&& in) {
  return static_cast<OperandId>(in);
}

inline Type_OperandId::Ref To_OperandId_Ref(Type_ObjectId::Ref in) {
  return static_cast<OperandId>(in);
}

};  // namespace Impl_ObjectId

namespace Impl_StringId {

inline Type_OperandId::Val To_OperandId_Val(Type_StringId::Val&& in) {
  return static_cast<OperandId>(in);
}

inline Type_OperandId::Ref To_OperandId_Ref(Type_StringId::Ref in) {
  return static_cast<OperandId>(in);
}

};  // namespace Impl_StringId

namespace Impl_SymbolId {

inline Type_OperandId::Val To_OperandId_Val(Type_SymbolId::Val&& in) {
  return static_cast<OperandId>(in);
}

inline Type_OperandId::Ref To_OperandId_Ref(Type_SymbolId::Ref in) {
  return static_cast<OperandId>(in);
}

};  // namespace Impl_SymbolId

namespace Impl_BoolId {

inline Type_OperandId::Val To_OperandId_Val(Type_BoolId::Val&& in) {
  return static_cast<OperandId>(in);
}

inline Type_OperandId::Ref To_OperandId_Ref(Type_BoolId::Ref in) {
  return static_cast<OperandId>(in);
}

};  // namespace Impl_BoolId

namespace Impl_Int32Id {

inline Type_OperandId::Val To_OperandId_Val(Type_Int32Id::Val&& in) {
  return static_cast<OperandId>(in);
}

inline Type_OperandId::Ref To_OperandId_Ref(Type_Int32Id::Ref in) {
  return static_cast<OperandId>(in);
}

};  // namespace Impl_Int32Id

namespace Impl_NumberId {

inline Type_ValueId::Val To_ValueId_Val(Type_NumberId::Val&& in) {
  return static_cast<ValOperandId>(in);
}

inline Type_ValueId::Ref To_ValueId_Ref(Type_NumberId::Ref in) {
  return static_cast<ValOperandId>(in);
}

};  // namespace Impl_NumberId

namespace Impl_BigIntId {

inline Type_OperandId::Val To_OperandId_Val(Type_BigIntId::Val&& in) {
  return static_cast<OperandId>(in);
}

inline Type_OperandId::Ref To_OperandId_Ref(Type_BigIntId::Ref in) {
  return static_cast<OperandId>(in);
}

};  // namespace Impl_BigIntId

namespace Impl_ValueTagId {

inline Type_OperandId::Val To_OperandId_Val(Type_ValueTagId::Val&& in) {
  return static_cast<OperandId>(in);
}

inline Type_OperandId::Ref To_OperandId_Ref(Type_ValueTagId::Ref in) {
  return static_cast<OperandId>(in);
}

};  // namespace Impl_ValueTagId

namespace Impl_IntPtrId {

inline Type_OperandId::Val To_OperandId_Val(Type_IntPtrId::Val&& in) {
  return static_cast<OperandId>(in);
}

inline Type_OperandId::Ref To_OperandId_Ref(Type_IntPtrId::Ref in) {
  return static_cast<OperandId>(in);
}

};  // namespace Impl_IntPtrId

namespace Impl_TypedId {

inline Type_OperandId::Val To_OperandId_Val(Type_TypedId::Val&& in) {
  return static_cast<OperandId>(in);
}

inline Type_OperandId::Ref To_OperandId_Ref(Type_TypedId::Ref in) {
  return static_cast<OperandId>(in);
}

};  // namespace Impl_TypedId

namespace Impl_AttachDecision {

inline Type_AttachDecision::Ref Variant_NoAction(Cachet_ContextRef cx) {
  return AttachDecision::NoAction;
}

inline Type_AttachDecision::Ref Variant_Attach(Cachet_ContextRef cx) {
  return AttachDecision::Attach;
}

inline Type_AttachDecision::Ref Variant_TemporarilyUnoptimizable(Cachet_ContextRef cx) {
  return AttachDecision::TemporarilyUnoptimizable;
}

inline Type_AttachDecision::Ref Variant_Deferred(Cachet_ContextRef cx) {
  return AttachDecision::Deferred;
}

};  // namespace Impl_AttachDecision

};  // namespace cachet

};  // namespace jit

};  // namespace js

#endif /* jit_CachetCompiler_h */
