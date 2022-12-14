/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef TENSORFLOW_CORE_PROFILER_UTILS_XPLANE_BUILDER_H_
#define TENSORFLOW_CORE_PROFILER_UTILS_XPLANE_BUILDER_H_

#include <stddef.h>

#include <string>
#include <utility>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/numbers.h"
#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include "tensorflow/core/platform/macros.h"
#include "tensorflow/core/platform/protobuf.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/profiler/protobuf/xplane.pb.h"
#include "tensorflow/core/profiler/utils/time_utils.h"
#include "tensorflow/core/profiler/utils/timespan.h"

namespace tensorflow {
namespace profiler {

class XPlaneBuilder;

template <typename T>
class XStatsBuilder {
 public:
  explicit XStatsBuilder(T* stats_owner, XPlaneBuilder* stats_metadata_owner)
      : stats_owner_(stats_owner),
        stats_metadata_owner_(stats_metadata_owner) {}

  // NOTE: A stat shouldn't have existed for the given metadata.
  // Adds a stat for the given metadata and sets its value.
  template <typename ValueT>
  void AddStatValue(const XStatMetadata& metadata, ValueT value) {
    SetStatValue(value, AddStat(metadata));
  }

  // Adds or finds a stat for the given metadata and sets its value.
  template <typename ValueT>
  void SetOrAddStatValue(const XStatMetadata& metadata, ValueT value) {
    SetStatValue(value, FindOrAddStat(metadata));
  }

  // Adds a stat by copying a stat from another XPlane. Does not check if a stat
  // with the same metadata already exists in the event. To avoid duplicated
  // stats, use the variant below.
  void AddStat(const XStatMetadata& metadata, const XStat& src_stat,
               const XPlane& src_plane) {
    CopyStatValue(src_stat, src_plane, AddStat(metadata));
  }
  // Same as above but overrides an existing stat with the same metadata.
  void SetOrAddStat(const XStatMetadata& metadata, const XStat& src_stat,
                    const XPlane& src_plane) {
    CopyStatValue(src_stat, src_plane, FindOrAddStat(metadata));
  }

  void ParseAndAddStatValue(const XStatMetadata& metadata,
                            absl::string_view value) {
    int64_t int_value;
    uint64 uint_value;
    double double_value;
    if (absl::SimpleAtoi(value, &int_value)) {
      AddStatValue(metadata, int_value);
    } else if (absl::SimpleAtoi(value, &uint_value)) {
      AddStatValue(metadata, uint_value);
    } else if (absl::SimpleAtod(value, &double_value)) {
      AddStatValue(metadata, double_value);
    } else {
      AddStatValue(metadata, GetOrCreateStatMetadata(value));
    }
  }

  void ReserveStats(size_t num_stats) {
    stats_owner_->mutable_stats()->Reserve(num_stats);
  }

  template <typename ForEachStatFunc>
  void ForEachStat(ForEachStatFunc&& for_each_stat) {
    for (XStat& stat : *stats_owner_->mutable_stats()) {
      for_each_stat(&stat);
    }
  }

 private:
  XStat* AddStat(const XStatMetadata& metadata) {
    XStat* stat = stats_owner_->add_stats();
    stat->set_metadata_id(metadata.id());
    return stat;
  }

  XStat* FindOrAddStat(const XStatMetadata& metadata) {
    for (auto& stat : *stats_owner_->mutable_stats()) {
      if (stat.metadata_id() == metadata.id()) {
        return &stat;
      }
    }
    return AddStat(metadata);
  }

  static void SetStatValue(uint32 value, XStat* stat) {
    stat->set_uint64_value(value);
  }
  static void SetStatValue(unsigned long value, XStat* stat) {  // NOLINT
    stat->set_uint64_value(value);
  }
  static void SetStatValue(unsigned long long value, XStat* stat) {  // NOLINT
    stat->set_uint64_value(value);
  }
  static void SetStatValue(int32_t value, XStat* stat) {
    stat->set_int64_value(value);
  }
  static void SetStatValue(long value, XStat* stat) {  // NOLINT
    stat->set_int64_value(value);
  }
  static void SetStatValue(long long value, XStat* stat) {  // NOLINT
    stat->set_int64_value(value);
  }
  static void SetStatValue(double value, XStat* stat) {
    stat->set_double_value(value);
  }
  static void SetStatValue(absl::string_view value, XStat* stat) {
    stat->set_str_value(std::string(value));
  }
  static void SetStatValue(std::string&& value, XStat* stat) {
    stat->set_str_value(std::move(value));
  }
  static void SetStatValue(const XStatMetadata& value, XStat* stat) {
    stat->set_ref_value(value.id());
  }
  static void SetStatValue(const protobuf::MessageLite& proto, XStat* stat) {
    auto* bytes = stat->mutable_bytes_value();
    proto.SerializeToString(bytes);
  }

  void CopyStatValue(const XStat& src_stat, const XPlane& src_plane,
                     XStat* dst_stat) {
    switch (src_stat.value_case()) {
      case XStat::VALUE_NOT_SET:
        break;
      case XStat::kInt64Value:
        dst_stat->set_int64_value(src_stat.int64_value());
        break;
      case XStat::kUint64Value:
        dst_stat->set_uint64_value(src_stat.uint64_value());
        break;
      case XStat::kDoubleValue:
        dst_stat->set_double_value(src_stat.double_value());
        break;
      case XStat::kStrValue:
        dst_stat->set_str_value(src_stat.str_value());
        break;
      case XStat::kRefValue: {
        const auto& stat_metadata_by_id = src_plane.stat_metadata();
        const auto it = stat_metadata_by_id.find(src_stat.ref_value());
        if (TF_PREDICT_TRUE(it != stat_metadata_by_id.end())) {
          absl::string_view value = it->second.name();
          dst_stat->set_ref_value(GetOrCreateStatMetadata(value).id());
        }
        break;
      }
      case XStat::kBytesValue:
        dst_stat->set_bytes_value(src_stat.bytes_value());
        break;
    }
  }

  const XStatMetadata& GetOrCreateStatMetadata(absl::string_view value);

  T* stats_owner_;
  XPlaneBuilder* stats_metadata_owner_;
};

class XEventBuilder : public XStatsBuilder<XEvent> {
 public:
  XEventBuilder(const XLine* line, XPlaneBuilder* plane, XEvent* event)
      : XStatsBuilder<XEvent>(event, plane), line_(line), event_(event) {}

  int64 OffsetPs() const { return event_->offset_ps(); }
  int64 MetadataId() const { return event_->metadata_id(); }

  void SetOffsetPs(int64_t offset_ps) { event_->set_offset_ps(offset_ps); }

  void SetOffsetNs(int64_t offset_ns) { SetOffsetPs(NanosToPicos(offset_ns)); }

  void SetTimestampNs(int64_t timestamp_ns) {
    SetOffsetPs(NanosToPicos(timestamp_ns - line_->timestamp_ns()));
  }

  void SetNumOccurrences(int64_t num_occurrences) {
    event_->set_num_occurrences(num_occurrences);
  }

  void SetDurationPs(int64_t duration_ps) {
    event_->set_duration_ps(duration_ps);
  }
  void SetDurationNs(int64_t duration_ns) {
    SetDurationPs(NanosToPicos(duration_ns));
  }

  void SetEndTimestampPs(int64_t end_timestamp_ps) {
    SetDurationPs(end_timestamp_ps - PicosToNanos(line_->timestamp_ns()) -
                  event_->offset_ps());
  }
  void SetEndTimestampNs(int64_t end_timestamp_ns) {
    SetDurationPs(NanosToPicos(end_timestamp_ns - line_->timestamp_ns()) -
                  event_->offset_ps());
  }

  Timespan GetTimespan() const {
    return Timespan(NanosToPicos(line_->timestamp_ns()) + event_->offset_ps(),
                    event_->duration_ps());
  }

 private:
  const XLine* line_;
  XEvent* event_;
};

class XLineBuilder {
 public:
  explicit XLineBuilder(XLine* line, XPlaneBuilder* plane)
      : line_(line), plane_(plane) {}

  // Returns the owner plane.
  XPlaneBuilder* Plane() const { return plane_; }

  int64 Id() const { return line_->id(); }
  void SetId(int64_t id) { line_->set_id(id); }

  int64 NumEvents() const { return line_->events_size(); }

  absl::string_view Name() const { return line_->name(); }
  void SetName(absl::string_view name) { line_->set_name(std::string(name)); }

  void SetNameIfEmpty(absl::string_view name) {
    if (line_->name().empty()) SetName(name);
  }

  int64 TimestampNs() const { return line_->timestamp_ns(); }
  // This will set the line start timestamp.
  // WARNING: The offset_ps of existing events will not be altered.
  void SetTimestampNs(int64_t timestamp_ns) {
    line_->set_timestamp_ns(timestamp_ns);
  }
  // This will set the line start timestamp to specific time, and adjust
  // the offset_ps of all existing events.
  void SetTimestampNsAndAdjustEventOffsets(int64_t timestamp_ns);

  void SetDurationPs(int64_t duration_ps) {
    line_->set_duration_ps(duration_ps);
  }

  void ReserveEvents(size_t num_events) {
    line_->mutable_events()->Reserve(num_events);
  }

  void SetDisplayNameIfEmpty(absl::string_view display_name) {
    if (line_->display_name().empty()) {
      line_->set_display_name(std::string(display_name));
    }
  }

  XEventBuilder AddEvent(const XEventMetadata& metadata);
  XEventBuilder AddEvent(const XEvent& event);

  template <typename ForEachEventFunc>
  void ForEachEvent(ForEachEventFunc&& for_each_event) {
    for (XEvent& event : *line_->mutable_events()) {
      for_each_event(XEventBuilder(line_, plane_, &event));
    }
  }

 private:
  XLine* line_;
  XPlaneBuilder* plane_;
};

// Provides methods to build an XPlane.
// NOTE: avoid to use two builders to wrap the same XPlane.
class XPlaneBuilder : public XStatsBuilder<XPlane> {
 public:
  explicit XPlaneBuilder(XPlane* plane);

  int64 Id() const { return plane_->id(); }
  void SetId(int64_t id) { plane_->set_id(id); }

  absl::string_view Name() const { return plane_->name(); }
  void SetName(absl::string_view name) { plane_->set_name(std::string(name)); }

  void ReserveLines(size_t num_lines) {
    plane_->mutable_lines()->Reserve(num_lines);
  }

  template <typename ForEachLineFunc>
  void ForEachLine(ForEachLineFunc&& for_each_line) {
    for (XLine& line : *plane_->mutable_lines()) {
      for_each_line(XLineBuilder(&line, this));
    }
  }

  // Returns a builder for the line with the given id. Creates a new line if the
  // id was unused, otherwise the builder will add events to an existing line.
  XLineBuilder GetOrCreateLine(int64_t line_id);

  // Returns a new event metadata with an automatically generated metadata_id.
  // WARNING: If calling this function, don't call GetOrCreateEventMetadata.
  XEventMetadata* CreateEventMetadata();

  // Returns event metadata with the given id. Creates a new metadata if the id
  // was unused.
  // WARNING: If calling this function, don't call the string overloads below
  // on the same instance.
  XEventMetadata* GetOrCreateEventMetadata(int64_t metadata_id);

  // Returns event metadata with the given name. The id is internally assigned.
  // Creates a new metadata if the name was unused.
  // Using these overloads guarantees names are unique.
  // WARNING: If calling any of these overloads, do not call the integer one
  // above on the same instance.
  XEventMetadata* GetOrCreateEventMetadata(absl::string_view name);
  XEventMetadata* GetOrCreateEventMetadata(std::string&& name);
  XEventMetadata* GetOrCreateEventMetadata(const char* name) {
    return GetOrCreateEventMetadata(absl::string_view(name));
  }

  // Returns event metadata with the given name. Returns nullptr if not found.
  XEventMetadata* GetEventMetadata(absl::string_view name) const;

  // Returns stat metadata with the given name. Returns nullptr if not found.
  XStatMetadata* GetStatMetadata(absl::string_view name) const;

  // Returns a new stat metadata with an automatically generated metadata_id.
  // WARNING: If calling this function, don't call GetOrCreateEventMetadata.
  XStatMetadata* CreateStatMetadata();

  // Returns stat metadata with the given id. Creates a new metadata if the id
  // was unused.
  // WARNING: If calling this function, don't call the string overloads below
  // on the same instance.
  XStatMetadata* GetOrCreateStatMetadata(int64_t metadata_id);

  // Returns stat metadata with the given name. The id is internally assigned.
  // Creates a new metadata if the name was unused.
  // Using these overloads guarantees names are unique.
  // WARNING: If calling any of these overloads, do not call the integer one
  // above on the same instance.
  XStatMetadata* GetOrCreateStatMetadata(absl::string_view name);
  XStatMetadata* GetOrCreateStatMetadata(std::string&& name);
  XStatMetadata* GetOrCreateStatMetadata(const char* name) {
    return GetOrCreateStatMetadata(absl::string_view(name));
  }

 private:
  XPlane* plane_;

  // Artifacts to accelerate the builders.
  int64 last_event_metadata_id_ = 0LL;
  int64 last_stat_metadata_id_ = 0LL;
  absl::flat_hash_map<std::string, XEventMetadata*> event_metadata_by_name_;
  absl::flat_hash_map<std::string, XStatMetadata*> stat_metadata_by_name_;
  absl::flat_hash_map<int64, XLine*> lines_by_id_;
};

template <typename T>
const XStatMetadata& XStatsBuilder<T>::GetOrCreateStatMetadata(
    absl::string_view value) {
  return *stats_metadata_owner_->GetOrCreateStatMetadata(value);
}

}  // namespace profiler
}  // namespace tensorflow

#endif  // TENSORFLOW_CORE_PROFILER_UTILS_XPLANE_BUILDER_H_