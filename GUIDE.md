This is a quick start guide for the specifics reverse-engineering a sysmodule on the Nintendo
Switch. It is intended for people with a bit of experience using IDA, a strong understanding
of C, and a moderate understanding of C++. This is not a beginners guide to reverse-engineering.

# Basic Setup
1. Open your NSO file ([reswitched/loaders](https://github.com/reswitched/loaders))
2. Parse header file nn.h ([misson20000/nn-types](https://github.com/misson20000/nn-types))
3. Load FLIRT signature file ([misson20000/nn-types](https://github.com/misson20000/nn-types))
4. Open Names window, Database Notepad, & Local Types

# Reverse-Engineering a Service

## Finding s_Tables

On firmware versions before 4.0.0(?), each sysmodule included a very convenient symbol for each interface it implemented.
These are the `s_Table`s. Search for `s_Table` in the **Names window** and you'll see one for every interface this sysmodule
implements. These are static variables that contain function pointers.

## The ProcessServerMessage Function

Once you've located the `s_Table`, it should be a static variable containing a function pointer. Jump to this function.
Typically, these don't have names. These functions are called `ProcessServerMessage` and are static functions on the
`nn::sf::cmif::server::detail::CmifProcessFunctionTableGetter<typename Interface>` template. You can use the following
signature. Just hit Y on the function definition line and copy/paste:

```cpp
nn::Result ProcessServerMessage(nn::sf::IServiceObject *object, nn::sf::cmif::server::CmifServerMessage *message, nn::sf::detail::PointerAndSize const *pas)
```

The function pointers in the `nn::sf::cmif::server::CmifServerMessage::vtable` struct are likely more correct than whatever
Hex-Rays guesses here, so you can right-click on the function pointer casts and select "Force call type" to clean them up
a little bit.

At about this point, you'll probably want to create a struct to represent this interface's service objects. For now,
all it needs is an 8-byte `vtable` field.

The `ProcessServerMessage` function will do some basic sanity checks on the message (checking minimum size, checking for `SFCI`
, etc.), then calls `DispatchServerMessage`, casting the `IServiceObject*` to a pointer to the correct object and passing
`((uint32_t*) pas->pointer)[2]` as the command id to dispatch on. This function is usually inlined.

`DispatchServerMessage` dispatches based on the passed command-id. Usually this is a switch-case, but sometimes Hex-Rays doesn't
pick up the control flow quite right and it might look a little uglier. It should branch to a different function call for each
command, returning `0x1BA0A` in the default case.

## The Process_CommandName Functions

Each of the functions that `DispatchServerMessage` branches to is another static function on the `CmifProcessFunctionTableGetterImpl`
template. These are named as `Process_CommandName`, with `CommandName` replaced with the name of the command. While they
can sometimes be inlined, they are usually not. You can use the following signature, replacing `Interface *` with a pointer to
your struct.

```cpp
nn::Result Process_Command(nn::sf::cmif::CmifOutHeader **, Interface *, nn::sf::cmif::server::CmifServerMessage *, nn::sf::detail::PointerAndSize *)
```

This is the function that actually unpacks the message, invokes the correct function on the interface object, and packs the response.
It has a lot of calls to virtual functions on `CmifServerMessage`, which you can again "Force call type" on to clean up.

Near the middle of the function (after `PrepareForProcess` and before `BeginPreparingForReply`/`BeginPreparingForErrorReply`), you
should see the call to the actual implementation function. This is always a vtable call. At this point, it's probably about time
to start working on the vtable for your interface.

## Interface Object Vtables

Create a new struct `Interface::vtable` (replacing `Interface` with the name of your interface, of course). Fortunately,
these vtables are always very predictable. They always start with the same four virtual functions. Usually, I just create four
8-byte fields with the following names.

```cpp
__int64 AddReference;
__int64 Release;
__int64 GetProxyInfo;
__int64 GetInterfaceTypeInfo; // always inherited from `nn::sf::IServiceObject`, which returns nullptr
```

After these common four functions, there is typically one entry for each command. I find it helpful to read off SwIPC when
creating these fields, mostly just so I know how many I need to make.

Next, you need to find the vtable. If you're luckly, there will be a symbol for it that you can find in the names window
called something like this:
```
vtable for'nn::sf::UnmanagedServiceObject<nn::nsd::detail::IManager, nn::nsd::detail::server::ManagerImpl>
```
The symbol will point to two 8-byte zero fields (offset to derived class and typeinfo), then there will be an array of
function pointers. The first four are the common ones mentioned earlier. After that, each one corresponds to an actual
implementation of an IPC command. The signatures vary depending on the command. Usually you can guess what each argument
means, but it can be helpful to look at the `Process_CommandName` function for more complex commands with lots of arguments.

## Data Sources & Actual Signatures
Most of this information is inferred from the sdk nso distributed with The Legend of Zelda: Breath of the Wild version 1.0.0.
Some of these signatures I modified a little bit to work with IDA (references -> pointers). These are closer to the original
signatures.

```cpp
template<typename Interface>
class nn::sf::cmif::server::detail::CmifProcessFunctionTableGetterImplBase {
    ...
    nn::Result ProcessServerMessage(nn::sf::IServiceObject *, nn::sf::cmif::server::CmifServerMessage *, nn::sf::detail::PointerAndSize const&)
    ...
};

template<typename Interface>
class nn::sf::cmif::server::detail::CmifProcessFunctionTableGetterImpl : CmifProcessFunctionTableGetterImplBase<Interface> {
    ...
    nn::Result DispatchServerMessage(nn::sf::cmif::CmifOutHeader **, Interface *, nn::sf::cmif::server::CmifServerMessage *, unsigned int, nn::sf::detail::PointerAndSize &&);
    nn::Result Process_CommandName(nn::sf::cmif::CmifOutHeader **, nn::nsd::detail::IManager*, nn::sf::cmif::server::CmifServerMessage *, nn::sf::detail::PointerAndSize &&);
    ...
};
```
