#pragma once

namespace echo { namespace execution_context {

////////////////////////
// matrix_operation_t //
////////////////////////

enum class matrix_operation_t { none, transpose, conjugate_transpose };

///////////////////
// matrix_side_t //
///////////////////

enum class matrix_side_t { left, right };

////////////////////////////
// matrix_diagonal_fill_t //
////////////////////////////

enum class matrix_diagonal_fill_t { general, unity, null };

}}
