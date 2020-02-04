// Distributed under the MIT License.
// See LICENSE.txt for details.

/// \file
/// Defines action UpdateU

#pragma once

#include <tuple>
#include <utility>  // IWYU pragma: keep // for std::move

#include "DataStructures/DataBox/DataBox.hpp"
#include "DataStructures/DataBox/DataBoxTag.hpp"
#include "DataStructures/DataBox/Prefixes.hpp"
#include "Parallel/ConstGlobalCache.hpp"
#include "Time/Tags.hpp"
#include "Utilities/Gsl.hpp"
#include "Utilities/TaggedTuple.hpp"

// IWYU pragma: no_include "Time/Time.hpp" // for TimeDelta

/// \cond
// IWYU pragma: no_forward_declare TimeDelta
// IWYU pragma: no_forward_declare db::DataBox
/// \endcond

namespace Actions {
/// \ingroup ActionsGroup
/// \ingroup TimeGroup
/// \brief Perform variable updates for one substep
///
/// With `dt_variables_tag = db::add_tag_prefix<Tags::dt, variables_tag>`:
///
/// Uses:
/// - ConstGlobalCache: Tags::TimeStepperBase
/// - DataBox:
///   - variables_tag (either the provided `VariablesTag` or the
///   `system::variables_tag` if none is provided)
///   - Tags::HistoryEvolvedVariables<variables_tag, dt_variables_tag>
///   - Tags::TimeStep
///
/// DataBox changes:
/// - Adds: nothing
/// - Removes: nothing
/// - Modifies:
///   - variables_tag
///   - Tags::HistoryEvolvedVariables<variables_tag, dt_variables_tag>
template <typename VariablesTag = NoSuchType>
struct UpdateU {
  template <typename DbTags, typename... InboxTags, typename Metavariables,
            typename ArrayIndex, typename ActionList,
            typename ParallelComponent>
  static std::tuple<db::DataBox<DbTags>&&> apply(
      db::DataBox<DbTags>& box, tuples::TaggedTuple<InboxTags...>& /*inboxes*/,
      const Parallel::ConstGlobalCache<Metavariables>& cache,
      const ArrayIndex& /*array_index*/, ActionList /*meta*/,
      const ParallelComponent* const /*meta*/) noexcept {  // NOLINT const
    using variables_tag =
        tmpl::conditional_t<cpp17::is_same_v<VariablesTag, NoSuchType>,
                            typename Metavariables::system::variables_tag,
                            VariablesTag>;
    using dt_variables_tag = db::add_tag_prefix<Tags::dt, variables_tag>;
    using history_tag =
        Tags::HistoryEvolvedVariables<variables_tag, dt_variables_tag>;

    db::mutate<variables_tag, history_tag>(
        make_not_null(&box),
        [&cache](
            const gsl::not_null<db::item_type<variables_tag>*> vars,
            const gsl::not_null<db::item_type<history_tag>*> history,
            const db::const_item_type<Tags::TimeStep>& time_step) noexcept {
          const auto& time_stepper =
              Parallel::get<Tags::TimeStepperBase>(cache);
          time_stepper.update_u(vars, history, time_step);
        },
        db::get<Tags::TimeStep>(box));

    return std::forward_as_tuple(std::move(box));
  }
};
}  // namespace Actions
