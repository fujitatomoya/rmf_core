/*
 * Copyright (C) 2019 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef RMF_TRAFFIC__CONFLICT_HPP
#define RMF_TRAFFIC__CONFLICT_HPP

#include <rmf_traffic/Trajectory.hpp>
#include <exception>

namespace rmf_traffic {

// Forward declaration
class ConflictResult;
class DetectConflict;

//==============================================================================
class ConflictData
{
public:

  /// Get the point in time that the conflict occurs.
  Time get_time() const;

  using Segments =
      std::pair<Trajectory::const_iterator, Trajectory::const_iterator>;

  /// Get iterators to the Segment of each Trajectory that is in conflict.
  Segments get_segments();

  /// Create uninitialized ConflictData.
  ///
  /// \warning When using this constructor, it is undefined behavior to call any
  /// of the object's member functions until a valid ConflictData instance is
  /// assigned to the object. This constructor should only be used to
  /// forward-declare a ConflictData instance that will be copied or moved into
  /// later. The safest practice is to only use the const-references provided by
  /// ConflictResult instances.
  ConflictData();

private:
  friend class ConflictResult;
  class Implementation;
  rmf_utils::impl_ptr<Implementation> _pimpl;
};

//==============================================================================
class bad_conflict_access : public std::exception
{
public:

  const char* what() const noexcept override;

  bad_conflict_access();

private:
  class Implementation;
  rmf_utils::impl_ptr<Implementation> _pimpl;
};

//==============================================================================
class ConflictResult
{
public:

  /// Returns true if a conflict was detected, otherwise returns false.
  bool has_conflict() const;

  /// Implicitly casts this object into a boolean, determined by has_conflict().
  operator bool() const;


  /// Dereference operator.
  ///
  /// Get a reference to the ConflictData for this result, if a conflict was
  /// detected.
  ///
  /// \warning If no conflict was detected, then a bad_conflict_access exception
  /// will be thrown.
  const ConflictData& operator*() const;

  /// Drill-down operator.
  ///
  /// Call a member function of the ConflictData for this result, if a conflict
  /// was detected.
  ///
  /// \warning If no conflict was detected, then a bad_conflict_access exception
  /// will be thrown.
  const ConflictData* operator->() const;

  /// Create an unitialized ConflictResult.
  ///
  /// This will be considered conflict-free until another ConflictResult is
  /// assigned to it.
  ConflictResult();

private:
  friend class DetectConflict;
  class Implementation;
  rmf_utils::impl_ptr<Implementation> _pimpl;
};

//==============================================================================
class DetectConflict
{
public:

  /// Checks if there are any conflicts between the two trajectories.
  ///
  /// First broad_phase will be run, and if there is an intersection in the
  /// broad_phase, then the result of narrow_phase will be returned.
  static ConflictResult between(
      const Trajectory& trajectory_a,
      const Trajectory& trajectory_b);

  /// Checks if there is any overlap in the map name and time of the two
  /// trajectories.
  static bool broad_phase(
      const Trajectory& trajectory_a,
      const Trajectory& trajectory_b);

  /// Assumes that:
  ///
  /// 1. the map names are the same and
  /// 2. that there is some overlap in the timing of the two Trajectories
  ///
  /// and then checks for conflicts between the Trajectories.
  ///
  /// If you are unsure about the stated assumptions, then use the between()
  /// function instead. It is undefined behavior to call this function on any
  /// pair of Trajectories that would fail the broad_phase() test. Segmentation
  /// faults or false positives may occur when this function is called on
  /// Trajectories that do not pass the broad_phase test.
  static ConflictResult narrow_phase(
      const Trajectory& trajectory_a,
      const Trajectory& trajectory_b);

  class Implementation;
};

} // namespace rmf_traffic

#endif // RMF_TRAFFIC__CONFLICT_HPP
