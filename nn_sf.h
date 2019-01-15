struct nn::sf::cmif::server::CmifServerMessage;
struct nn::sf::cmif::CmifMessageMetaInfo; // TODO: fill out CmifMessageMetaInfo
struct nn::sf::cmif::CmifOutHeader; // TODO: fill out CmifOutHeader
struct nn::sf::IServiceObject;

struct nn::sf::detail::PointerAndSize {
  void *pointer;
  size_t size;
};

struct nn::sf::NativeHandle {
  nn::svc::Handle handle;
  char valid;
};

struct nn::sf::cmif::server::CmifServerObjectInfo {
  void *object;
  nn::Result (**s_Table)(nn::sf::IServiceObject *object, nn::sf::cmif::server::CmifServerMessage *message, nn::sf::detail::PointerAndSize const *raw_data);
};

struct nn::sf::cmif::server::CmifServerMessage::vtable {
  nn::Result (*PrepareForProcess)(nn::sf::cmif::server::CmifServerMessage *this, nn::sf::cmif::CmifMessageMetaInfo *info);
  nn::Result (*OverwriteClientProcessId)(nn::sf::cmif::server::CmifServerMessage *this, pid_t *pid);
  nn::Result (*GetBuffers)(nn::sf::cmif::server::CmifServerMessage *this, nn::sf::detail::PointerAndSize *pas);
  nn::Result (*GetInNativeHandles)(nn::sf::cmif::server::CmifServerMessage *this, nn::sf::NativeHandle *handles);
  nn::Result (*GetInObjects)(nn::sf::cmif::server::CmifServerMessage *this, nn::sf::cmif::server::CmifServerObjectInfo *info);
  nn::Result (*BeginPreparingForReply)(nn::sf::cmif::server::CmifServerMessage *this, nn::sf::detail::PointerAndSize *pas);
  nn::Result (*SetBuffers)(nn::sf::cmif::server::CmifServerMessage *this, nn::sf::detail::PointerAndSize *pas);
  nn::Result (*SetOutObjects)(nn::sf::cmif::server::CmifServerMessage *this, nn::sf::cmif::server::CmifServerObjectInfo *info);
  nn::Result (*SetOutNativeHandles)(nn::sf::cmif::server::CmifServerMessage *this, nn::sf::NativeHandle *handles);
  nn::Result (*BeginPreparingForErrorReply)(nn::sf::cmif::server::CmifServerMessage *this, nn::sf::detail::PointerAndSize *pas, uint64_t error_code);
  nn::Result (*EndPreparingForReply)(nn::sf::cmif::server::CmifServerMessage *this);
};

struct nn::sf::cmif::server::CmifServerMessage {
  nn::sf::cmif::server::CmifServerMessage::vtable *vtable;
  // TODO: fill out CmifServerMessage
};
