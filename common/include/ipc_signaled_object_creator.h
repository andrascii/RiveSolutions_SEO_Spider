#pragma once

namespace Common
{

class IIpcSignaledObject;

using SignaledObjectCreator = std::function<std::unique_ptr<IIpcSignaledObject>()>;

SignaledObjectCreator creator();

}