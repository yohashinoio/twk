/**
 * These codes are licensed under Apache-2.0 License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#include <maple/codegen/common.hpp>

namespace maple::codegen
{

[[nodiscard]] llvm::AllocaInst* createEntryAlloca(llvm::Function*    func,
                                                  const std::string& var_name,
                                                  llvm::Type*        type)
{
  return llvm::IRBuilder<>{&func->getEntryBlock(),
                           func->getEntryBlock().begin()}
    .CreateAlloca(type, nullptr, var_name);
}

[[nodiscard]] SignKind logicalOrSign(const Value& lhs, const Value& rhs)
{
  return lhs.isSigned() || rhs.isSigned() ? SignKind::signed_
                                          : SignKind::unsigned_;
}

// If either of them is signed, the signed type is returned. Otherwise,
// unsigned.
// Assuming the type is the same.
[[nodiscard]] std::shared_ptr<Type>
resultTypeOf(const std::shared_ptr<Type>& lhs_t,
             const std::shared_ptr<Type>& rhs_t)
{
  if (lhs_t->isSigned())
    return lhs_t;
  else if (rhs_t->isSigned())
    return rhs_t;

  // If unsigned.
  return lhs_t;
}

[[nodiscard]] Value
createAdd(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateAdd(lhs.getValue(), rhs.getValue()),
          resultTypeOf(lhs.getType(), rhs.getType())};
}

[[nodiscard]] Value
createSub(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateSub(lhs.getValue(), rhs.getValue()),
          resultTypeOf(lhs.getType(), rhs.getType())};
}

[[nodiscard]] Value
createMul(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateMul(lhs.getValue(), rhs.getValue()),
          resultTypeOf(lhs.getType(), rhs.getType())};
}

[[nodiscard]] Value
createDiv(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  const auto result_type = resultTypeOf(lhs.getType(), rhs.getType());

  if (result_type->isSigned()) {
    return {ctx.builder.CreateSDiv(lhs.getValue(), rhs.getValue()),
            result_type};
  }
  else {
    return {ctx.builder.CreateUDiv(lhs.getValue(), rhs.getValue()),
            result_type};
  }
}

[[nodiscard]] Value
createMod(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  const auto result_type = resultTypeOf(lhs.getType(), rhs.getType());

  if (result_type->isSigned()) {
    return {ctx.builder.CreateSRem(lhs.getValue(), rhs.getValue()),
            result_type};
  }
  else {
    return {ctx.builder.CreateURem(lhs.getValue(), rhs.getValue()),
            result_type};
  }
}

[[nodiscard]] Value
createEqual(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateICmp(llvm::ICmpInst::ICMP_EQ,
                                 lhs.getValue(),
                                 rhs.getValue()),
          std::make_shared<BuiltinType>(BuiltinTypeKind::bool_)};
}

[[nodiscard]] Value
createNotEqual(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateICmp(llvm::ICmpInst::ICMP_NE,
                                 lhs.getValue(),
                                 rhs.getValue()),
          std::make_shared<BuiltinType>(BuiltinTypeKind::bool_)};
}

[[nodiscard]] Value
createLessThan(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateICmp(isSigned(logicalOrSign(lhs, rhs))
                                   ? llvm::ICmpInst::ICMP_SLT
                                   : llvm::ICmpInst::ICMP_ULT,
                                 lhs.getValue(),
                                 rhs.getValue()),
          std::make_shared<BuiltinType>(BuiltinTypeKind::bool_)};
}

[[nodiscard]] Value
createGreaterThan(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateICmp(isSigned(logicalOrSign(lhs, rhs))
                                   ? llvm::ICmpInst::ICMP_SGT
                                   : llvm::ICmpInst::ICMP_UGT,
                                 lhs.getValue(),
                                 rhs.getValue()),
          std::make_shared<BuiltinType>(BuiltinTypeKind::bool_)};
}

[[nodiscard]] Value
createLessOrEqual(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateICmp(isSigned(logicalOrSign(lhs, rhs))
                                   ? llvm::ICmpInst::ICMP_SLE
                                   : llvm::ICmpInst::ICMP_ULE,
                                 lhs.getValue(),
                                 rhs.getValue()),
          std::make_shared<BuiltinType>(BuiltinTypeKind::bool_)};
}

[[nodiscard]] Value
createGreaterOrEqual(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateICmp(isSigned(logicalOrSign(lhs, rhs))
                                   ? llvm::ICmpInst::ICMP_SGE
                                   : llvm::ICmpInst::ICMP_UGE,
                                 lhs.getValue(),
                                 rhs.getValue()),
          std::make_shared<BuiltinType>(BuiltinTypeKind::bool_)};
}

[[nodiscard]] Value createLogicalNot(CGContext& ctx, const Value& value)
{
  return {
    ctx.builder.CreateICmp(llvm::ICmpInst::ICMP_EQ,
                           value.getValue(),
                           llvm::ConstantInt::get(value.getLLVMType(), 0)),
    std::make_shared<BuiltinType>(BuiltinTypeKind::bool_)};
}

[[nodiscard]] Value createSizeOf(CGContext& ctx, const Value& value)
{
  // Assuming a 64-bit environment.
  // FIXME: To work in different environments
  return {llvm::ConstantInt::get(
            ctx.builder.getInt64Ty(),
            ctx.module->getDataLayout().getTypeAllocSize(value.getLLVMType())),
          std::make_shared<BuiltinType>(BuiltinTypeKind::u64)};
}

[[nodiscard]] Value createAddInverse(CGContext& ctx, const Value& value)
{
  return {ctx.builder.CreateSub(llvm::ConstantInt::get(value.getLLVMType(), 0),
                                value.getValue()),
          value.getType()};
}

[[nodiscard]] Value
createLogicalAnd(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateLogicalAnd(lhs.getValue(), rhs.getValue()),
          std::make_shared<BuiltinType>(BuiltinTypeKind::bool_)};
}

[[nodiscard]] Value
createLogicalOr(CGContext& ctx, const Value& lhs, const Value& rhs)
{
  return {ctx.builder.CreateLogicalOr(lhs.getValue(), rhs.getValue()),
          std::make_shared<BuiltinType>(BuiltinTypeKind::bool_)};
}

// The code is based on https://gist.github.com/quantumsheep.
// Thank you!
[[nodiscard]] bool strictEquals(const llvm::Type* const left,
                                const llvm::Type* const right)
{
  auto left_ptr  = llvm::dyn_cast<llvm::PointerType>(left);
  auto right_ptr = llvm::dyn_cast<llvm::PointerType>(right);

  if (left != right)
    return false;

  if (left->getTypeID() != right->getTypeID())
    return false;

  switch (left->getTypeID()) {
  case llvm::Type::IntegerTyID:
    return llvm::cast<llvm::IntegerType>(left)->getBitWidth()
           == llvm::cast<llvm::IntegerType>(right)->getBitWidth();

  // left == right would have returned true earlier, because types are
  // uniqued.
  case llvm::Type::VoidTyID:
  case llvm::Type::FloatTyID:
  case llvm::Type::DoubleTyID:
  case llvm::Type::X86_FP80TyID:
  case llvm::Type::FP128TyID:
  case llvm::Type::PPC_FP128TyID:
  case llvm::Type::LabelTyID:
  case llvm::Type::MetadataTyID:
  case llvm::Type::TokenTyID:
    return true;

  case llvm::Type::PointerTyID:
    assert(left_ptr && right_ptr && "Both types must be pointers here.");
    return left_ptr->getAddressSpace() == right_ptr->getAddressSpace();

  case llvm::Type::StructTyID:
  {
    auto left_struct  = llvm::cast<llvm::StructType>(left);
    auto right_struct = llvm::cast<llvm::StructType>(right);

    if (left_struct->getNumElements() != right_struct->getNumElements())
      return false;

    if (left_struct->isPacked() != right_struct->isPacked())
      return false;

    for (unsigned i = 0, e = left_struct->getNumElements(); i != e; ++i) {
      if (!strictEquals(left_struct->getElementType(i),
                        right_struct->getElementType(i)))
        return false;
    }

    return true;
  }

  case llvm::Type::FunctionTyID:
  {
    auto left_function  = llvm::cast<llvm::FunctionType>(left);
    auto right_function = llvm::cast<llvm::FunctionType>(right);

    if (left_function->getNumParams() != right_function->getNumParams())
      return false;

    if (left_function->isVarArg() != right_function->isVarArg())
      return false;

    if (!strictEquals(left_function->getReturnType(),
                      right_function->getReturnType()))
      return false;

    for (unsigned i = 0, e = left_function->getNumParams(); i != e; ++i) {
      if (!strictEquals(left_function->getParamType(i),
                        right_function->getParamType(i)))
        return false;
    }

    return true;
  }

  case llvm::Type::ArrayTyID:
  {
    auto left_sequential  = llvm::cast<llvm::ArrayType>(left);
    auto right_sequential = llvm::cast<llvm::ArrayType>(right);

    if (left_sequential->getNumElements() != right_sequential->getNumElements())
      return false;

    return strictEquals(left_sequential->getElementType(),
                        right_sequential->getElementType());
  }

  // VectorType has not yet been used and is not yet implemented.
  case llvm::Type::FixedVectorTyID:
  case llvm::Type::ScalableVectorTyID:
    unreachable();

  default:
    return false;
  }

  unreachable();
}

} // namespace maple::codegen
