typedef uint64_t nn::os::Tick;

// forward declarations
struct nn::os::ThreadType;

struct nn::os::detail::InternalCriticalSectionImplByHorizon {
  uint32_t image;
};

struct nn::os::detail::InternalConditionVariableImplByHorizon {
  uint32_t image;
};

struct nn::os::detail::InternalCriticalSection {
  // constructor/destructor/methods seem to be all inlined
  nn::os::detail::InternalCriticalSectionImplByHorizon impl;
};

struct nn::os::detail::InternalConditionVariable {
  // constructor/destructor/methods seem to be all inlined
  nn::os::detail::InternalConditionVariableImplByHorizon impl;
};

struct nn::os::detail::InternalCriticalSectionStorage {
  // This is a typedef of nn::util::TypedStorage<nn::os::detail::InternalCriticalSection, 4, 4>, which is a typedef of std::aligned_storage<4, 4>.
  // Used with placement new for nn::os::detail::InternalCriticalSection.
  // For simplicitly, I include the InternalCriticalSection here directly.
  nn::os::detail::InternalCriticalSection storage;
};

struct nn::os::MutexType {
  uint8_t _state;
  bool _isRecursive;
  int _lockLevel;
  int _nestCount;
  nn::os::ThreadType *_ownerThread;
  union {
    int32_t _mutexImage[1];
    nn::os::detail::InternalCriticalSectionStorage _mutex;
  };
};

struct nn::os::ThreadType {
  __int64 field_0;
  __int64 field_8;
  __int64 field_10;
  __int64 field_18;
  char field_20[32];
  uint32_t thread_status;
  char field_41;
  __int16 field_42;
  uint32_t thread_prio_shift;
  uint64_t thread_stack_base_addr;
  uint64_t thread_stack_base_addr_mirror;
  uint64_t thread_stack_size;
  uint64_t thread_param;
  uint64_t thread_func;
  __int64 field_70;
  __int64 field_78;
  __int64 field_80;
  char field_88[0x100];
  char thread_name[0x20];
  const char *thread_name_addr;
  nn::os::detail::InternalCriticalSection crit;
  nn::os::detail::InternalConditionVariable condvar;
  nn::svc::Handle thread_handle;
  // some sdk versions have more fields?
};

struct nn::os::ConditionVariableType {
  char field_0;
  int internal_impl;
};

struct nn::os::EventType {
  nn::os::EventType *field_0;
  nn::os::EventType *field_8;
  bool is_signalled;
  bool is_initially_signaled;
  bool should_autoclear;
  bool is_initialized;
  uint32_t signal_counter;
  uint32_t signal_counter_2;
  nn::os::detail::InternalCriticalSectionImplByHorizon crit;
  nn::os::detail::InternalConditionVariableImplByHorizon condvar;
};

struct nn::os::detail::InterProcessEventType {
  nn::os::EventType *field_0;
  nn::os::EventType *field_8;
  bool unk2;
  bool is_initially_signaled;
  bool should_autoclear;
  bool unk3;
  uint32_t readable_handle;
  uint32_t writable_handle;
  nn::os::detail::InternalCriticalSectionImplByHorizon crit;
  nn::os::detail::InternalConditionVariableImplByHorizon condvar;
};

union nn::os::SystemEventType::u {
  nn::os::EventType event;
  nn::os::detail::InterProcessEventType ipe;
};

struct nn::os::SystemEventType {
  nn::os::SystemEventType::u nn::os::SystemEventType::u;
  uint8_t tag;
};
