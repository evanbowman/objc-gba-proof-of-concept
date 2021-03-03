#include <stdint.h>


/**
 * Structure used to store selectors in the list.
 */
struct objc_selector
{
	union
	{
		/**
		 * The name of this selector.  Used for unregistered selectors.
		 */
		const char *name;
		/**
		 * The index of this selector in the selector table.  When a selector
		 * is registered with the runtime, its name is replaced by an index
		 * uniquely identifying this selector.  The index is used for dispatch.
		 */
		uintptr_t index;
	};
	/**
	 * The Objective-C type encoding of the message identified by this selector.
	 */
	const char * types;
};
