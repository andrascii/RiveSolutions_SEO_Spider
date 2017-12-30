#pragma once

namespace Common
{

class IIpcSignaledObject;

std::shared_ptr<IIpcSignaledObject> createSignaledObjectInstance();

}