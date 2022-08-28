#pragma once
#include <game/attachments/object.hpp>
#include <tardigrade/tick_attachment.hpp>
#include <util/ptr.hpp>

namespace pew {
class Prop : public tg::TickAttachment {
  public:
	Ptr<Object> object{};

  protected:
	void setup() override { object = entity()->find_or_attach<Object>(); }
};
} // namespace pew
