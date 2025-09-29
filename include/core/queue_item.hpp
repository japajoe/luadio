#ifndef LUADIO_QUEUE_ITEM_HPP
#define LUADIO_QUEUE_ITEM_HPP

#include <string>

namespace luadio
{
	enum item_type
	{
		item_type_log,
		item_type_audio
	};

	struct queue_item
	{
		item_type type;
		std::string message;

		queue_item() {}

		queue_item(item_type type, const std::string &message)
		{
			this->type = type;
			this->message = message;
		}
	};
}

#endif