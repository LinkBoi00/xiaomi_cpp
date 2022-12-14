// DO NOT EDIT. Autogenerated by Perfetto cppgen_plugin
#ifndef PERFETTO_PROTOS_PROTOS_PERFETTO_CONFIG_GPU_VULKAN_MEMORY_CONFIG_PROTO_CPP_H_
#define PERFETTO_PROTOS_PROTOS_PERFETTO_CONFIG_GPU_VULKAN_MEMORY_CONFIG_PROTO_CPP_H_

#include <stdint.h>
#include <bitset>
#include <vector>
#include <string>
#include <type_traits>

#include "perfetto/protozero/cpp_message_obj.h"
#include "perfetto/protozero/copyable_ptr.h"
#include "perfetto/base/export.h"

namespace perfetto {
namespace protos {
namespace gen {
class VulkanMemoryConfig;
}  // namespace perfetto
}  // namespace protos
}  // namespace gen

namespace protozero {
class Message;
}  // namespace protozero

namespace perfetto {
namespace protos {
namespace gen {

class PERFETTO_EXPORT VulkanMemoryConfig : public ::protozero::CppMessageObj {
 public:
  enum FieldNumbers {
    kTrackDriverMemoryUsageFieldNumber = 1,
    kTrackDeviceMemoryUsageFieldNumber = 2,
  };

  VulkanMemoryConfig();
  ~VulkanMemoryConfig() override;
  VulkanMemoryConfig(VulkanMemoryConfig&&) noexcept;
  VulkanMemoryConfig& operator=(VulkanMemoryConfig&&);
  VulkanMemoryConfig(const VulkanMemoryConfig&);
  VulkanMemoryConfig& operator=(const VulkanMemoryConfig&);
  bool operator==(const VulkanMemoryConfig&) const;
  bool operator!=(const VulkanMemoryConfig& other) const { return !(*this == other); }

  bool ParseFromArray(const void*, size_t) override;
  std::string SerializeAsString() const override;
  std::vector<uint8_t> SerializeAsArray() const override;
  void Serialize(::protozero::Message*) const;

  bool has_track_driver_memory_usage() const { return _has_field_[1]; }
  bool track_driver_memory_usage() const { return track_driver_memory_usage_; }
  void set_track_driver_memory_usage(bool value) { track_driver_memory_usage_ = value; _has_field_.set(1); }

  bool has_track_device_memory_usage() const { return _has_field_[2]; }
  bool track_device_memory_usage() const { return track_device_memory_usage_; }
  void set_track_device_memory_usage(bool value) { track_device_memory_usage_ = value; _has_field_.set(2); }

 private:
  bool track_driver_memory_usage_{};
  bool track_device_memory_usage_{};

  // Allows to preserve unknown protobuf fields for compatibility
  // with future versions of .proto files.
  std::string unknown_fields_;

  std::bitset<3> _has_field_{};
};

}  // namespace perfetto
}  // namespace protos
}  // namespace gen

#endif  // PERFETTO_PROTOS_PROTOS_PERFETTO_CONFIG_GPU_VULKAN_MEMORY_CONFIG_PROTO_CPP_H_