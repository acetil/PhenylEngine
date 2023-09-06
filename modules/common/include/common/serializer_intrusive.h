#pragma once

#define PHENYL_SERIALIZE_INTRUSIVE(T) friend struct phenyl_##T##_SerializerType;