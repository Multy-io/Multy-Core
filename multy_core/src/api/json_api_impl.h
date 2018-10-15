/* Copyright 2018 by Multy.io
 * Licensed under Multy.io license.
 *
 * See LICENSE for details
 */


#ifndef MULTY_CORE_SRC_JSON_API_IMPL_H
#define MULTY_CORE_SRC_JSON_API_IMPL_H

#include "multy_core/src/u_ptr.h"

#include <string>
#include <exception>

struct Error;

namespace multy_core
{
namespace internal
{

std::string make_transaction_from_json(const std::string&);

} // namespace internal
} // namespace multy_core


#endif // MULTY_CORE_SRC_JSON_API_IMPL_H
