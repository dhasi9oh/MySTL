#ifndef MYSTL_EXCEPTDEF_H
#define MYSTL_EXCEPTDEF_H

#include <stdexcept>
#include <cassert>


namespace mystl {

#define MYSTL_DEBUG(expr) assert(expr)

#define THROW_LENGTH_ERROR_IF(expr, what) if ((expr)) throw std::length_error(what)

#define THROW_OUT_RANGE_IF(expr, what) if ((expr)) throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr, what) if ((expr)) throw std::runtime_error(what)

}


#endif // !MYSTL_EXCEPTDEF_H
